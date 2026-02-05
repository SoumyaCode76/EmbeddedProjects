#!/usr/bin/env python3
"""
Multi-threaded UART Analyzer Usage Examples

This file demonstrates various ways to use the UART analyzer
in multi-threaded applications for embedded systems debugging.
"""

import threading
import time
import queue
import serial
from collections import deque
from datetime import datetime


# Example 1: Basic Multi-threaded UART Capture with Data Processing
# ====================================================================

class ThreadedUARTCapture:
    """Capture UART data in a separate thread and process it"""
    
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.running = False
        self.data_queue = queue.Queue()
        self.capture_thread = None
        self.ser = None
        
    def start(self):
        """Start the capture thread"""
        self.running = True
        self.capture_thread = threading.Thread(target=self._capture_loop, daemon=True)
        self.capture_thread.start()
        print(f"UART capture started on {self.port}")
        
    def stop(self):
        """Stop the capture thread"""
        self.running = False
        if self.capture_thread:
            self.capture_thread.join(timeout=2.0)
        if self.ser and self.ser.is_open:
            self.ser.close()
        print("UART capture stopped")
        
    def _capture_loop(self):
        """Internal capture loop running in separate thread"""
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=0.1)
            
            while self.running:
                if self.ser.in_waiting > 0:
                    data = self.ser.read(self.ser.in_waiting)
                    timestamp = time.time()
                    # Put data in queue for processing
                    self.data_queue.put((timestamp, data))
                time.sleep(0.01)
                
        except serial.SerialException as e:
            print(f"Serial error: {e}")
        finally:
            if self.ser and self.ser.is_open:
                self.ser.close()
    
    def get_data(self, timeout=0.1):
        """Get captured data from queue"""
        try:
            return self.data_queue.get(timeout=timeout)
        except queue.Empty:
            return None


# Example 2: UART Monitor with Signal Analysis
# ==============================================

class UARTSignalMonitor:
    """Monitor UART for specific signal patterns in real-time"""
    
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.running = False
        self.callbacks = []
        self.statistics = {
            'total_bytes': 0,
            'pattern_matches': 0,
            'errors': 0
        }
        self.lock = threading.Lock()
        
    def add_callback(self, pattern, callback_func):
        """Add a callback for when pattern is detected"""
        self.callbacks.append((pattern.encode() if isinstance(pattern, str) else pattern, callback_func))
        
    def start(self):
        """Start monitoring in background thread"""
        self.running = True
        self.thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.thread.start()
        
    def stop(self):
        """Stop monitoring"""
        self.running = False
        if hasattr(self, 'thread'):
            self.thread.join(timeout=2.0)
            
    def _monitor_loop(self):
        """Internal monitoring loop"""
        try:
            ser = serial.Serial(self.port, self.baudrate, timeout=0.1)
            buffer = b''
            
            while self.running:
                if ser.in_waiting > 0:
                    data = ser.read(ser.in_waiting)
                    
                    with self.lock:
                        self.statistics['total_bytes'] += len(data)
                    
                    buffer += data
                    
                    # Check for patterns
                    for pattern, callback in self.callbacks:
                        if pattern in buffer:
                            with self.lock:
                                self.statistics['pattern_matches'] += 1
                            # Execute callback in separate thread to avoid blocking
                            threading.Thread(target=callback, args=(data,), daemon=True).start()
                    
                    # Keep buffer manageable
                    if len(buffer) > 1024:
                        buffer = buffer[-512:]
                
                time.sleep(0.01)
                
        except Exception as e:
            with self.lock:
                self.statistics['errors'] += 1
            print(f"Monitor error: {e}")
        finally:
            if ser.is_open:
                ser.close()
    
    def get_statistics(self):
        """Get current statistics"""
        with self.lock:
            return self.statistics.copy()


# Example 3: Dual UART Logger (Monitor two ports simultaneously)
# ================================================================

class DualUARTLogger:
    """Log data from two UART ports simultaneously"""
    
    def __init__(self, port1, port2, baudrate=115200, log_file="dual_uart.log"):
        self.port1 = port1
        self.port2 = port2
        self.baudrate = baudrate
        self.log_file = log_file
        self.running = False
        self.threads = []
        self.log_queue = queue.Queue()
        self.log_handle = None
        
    def start(self):
        """Start capturing from both ports"""
        self.running = True
        
        # Start capture threads for both ports
        t1 = threading.Thread(target=self._capture, args=(self.port1, "PORT1"), daemon=True)
        t2 = threading.Thread(target=self._capture, args=(self.port2, "PORT2"), daemon=True)
        
        # Start logging thread
        t_log = threading.Thread(target=self._logger, daemon=True)
        
        self.threads = [t1, t2, t_log]
        
        for t in self.threads:
            t.start()
            
        print(f"Dual UART logging started: {self.port1}, {self.port2}")
        
    def stop(self):
        """Stop all threads"""
        self.running = False
        for t in self.threads:
            t.join(timeout=2.0)
        if self.log_handle:
            self.log_handle.close()
        print("Dual UART logging stopped")
        
    def _capture(self, port, port_name):
        """Capture from a single port"""
        try:
            ser = serial.Serial(port, self.baudrate, timeout=0.1)
            
            while self.running:
                if ser.in_waiting > 0:
                    data = ser.read(ser.in_waiting)
                    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
                    self.log_queue.put((timestamp, port_name, data))
                time.sleep(0.01)
                
        except Exception as e:
            print(f"Error on {port_name}: {e}")
        finally:
            if ser.is_open:
                ser.close()
    
    def _logger(self):
        """Log queue data to file"""
        try:
            self.log_handle = open(self.log_file, 'a', encoding='utf-8')
            
            while self.running:
                try:
                    timestamp, port_name, data = self.log_queue.get(timeout=0.1)
                    log_line = f"[{timestamp}] {port_name}: {data.decode('utf-8', errors='replace')}\n"
                    self.log_handle.write(log_line)
                    self.log_handle.flush()
                    print(log_line, end='')
                except queue.Empty:
                    continue
                    
        except Exception as e:
            print(f"Logger error: {e}")


# Example 4: UART Command Sender with Response Capture
# ======================================================

class UARTCommandInterface:
    """Send commands and capture responses in separate threads"""
    
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.response_queue = queue.Queue()
        self.running = False
        self.rx_thread = None
        
    def connect(self):
        """Connect to UART"""
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=0.1)
            self.running = True
            # Start receive thread
            self.rx_thread = threading.Thread(target=self._rx_loop, daemon=True)
            self.rx_thread.start()
            print(f"Connected to {self.port}")
            return True
        except serial.SerialException as e:
            print(f"Connection failed: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from UART"""
        self.running = False
        if self.rx_thread:
            self.rx_thread.join(timeout=2.0)
        if self.ser and self.ser.is_open:
            self.ser.close()
        print("Disconnected")
    
    def _rx_loop(self):
        """Receive loop running in background"""
        while self.running:
            if self.ser.in_waiting > 0:
                data = self.ser.readline()
                self.response_queue.put(data)
            time.sleep(0.01)
    
    def send_command(self, command, wait_response=True, timeout=1.0):
        """Send command and optionally wait for response"""
        if not self.ser or not self.ser.is_open:
            print("Not connected")
            return None
        
        # Clear response queue
        while not self.response_queue.empty():
            try:
                self.response_queue.get_nowait()
            except queue.Empty:
                break
        
        # Send command
        cmd_bytes = command.encode() if isinstance(command, str) else command
        if not cmd_bytes.endswith(b'\n'):
            cmd_bytes += b'\n'
        
        self.ser.write(cmd_bytes)
        print(f"Sent: {cmd_bytes.decode('utf-8', errors='replace').strip()}")
        
        if wait_response:
            try:
                response = self.response_queue.get(timeout=timeout)
                print(f"Response: {response.decode('utf-8', errors='replace').strip()}")
                return response
            except queue.Empty:
                print("No response received")
                return None
        
        return None


# =============================================================================
# USAGE EXAMPLES
# =============================================================================

def example_1_basic_capture():
    """Example 1: Basic threaded capture with processing"""
    print("\n=== Example 1: Basic Threaded Capture ===\n")
    
    # Create capture instance
    capture = ThreadedUARTCapture('COM3', 115200)
    capture.start()
    
    try:
        # Process data in main thread
        for i in range(10):
            data = capture.get_data(timeout=1.0)
            if data:
                timestamp, raw_data = data
                print(f"[{timestamp:.3f}] Received: {raw_data.decode('utf-8', errors='replace')}")
            time.sleep(0.1)
    
    except KeyboardInterrupt:
        print("\nInterrupted")
    finally:
        capture.stop()


def example_2_pattern_monitoring():
    """Example 2: Monitor for specific patterns"""
    print("\n=== Example 2: Pattern Monitoring ===\n")
    
    monitor = UARTSignalMonitor('COM3', 115200)
    
    # Define callbacks for different patterns
    def on_error_detected(data):
        print(f"⚠️  ERROR DETECTED: {data.decode('utf-8', errors='replace')}")
    
    def on_ready_detected(data):
        print(f"✅ SYSTEM READY")
    
    def on_toggle_detected(data):
        print(f"🔄 TOGGLE EVENT")
    
    # Register pattern callbacks
    monitor.add_callback("ERROR", on_error_detected)
    monitor.add_callback("READY", on_ready_detected)
    monitor.add_callback("TOGGLE", on_toggle_detected)
    
    monitor.start()
    
    try:
        # Monitor for 30 seconds
        for i in range(30):
            time.sleep(1)
            stats = monitor.get_statistics()
            print(f"Stats: {stats['total_bytes']} bytes, {stats['pattern_matches']} matches")
    
    except KeyboardInterrupt:
        print("\nInterrupted")
    finally:
        monitor.stop()


def example_3_dual_port_logging():
    """Example 3: Log two UART ports simultaneously"""
    print("\n=== Example 3: Dual Port Logging ===\n")
    
    logger = DualUARTLogger('COM3', 'COM4', baudrate=115200, log_file='dual_uart.log')
    logger.start()
    
    try:
        # Let it run
        while True:
            time.sleep(1)
    
    except KeyboardInterrupt:
        print("\nStopping...")
    finally:
        logger.stop()


def example_4_command_interface():
    """Example 4: Interactive command interface"""
    print("\n=== Example 4: Command Interface ===\n")
    
    interface = UARTCommandInterface('COM3', 115200)
    
    if interface.connect():
        try:
            # Send test commands
            interface.send_command("help", wait_response=True, timeout=2.0)
            time.sleep(0.5)
            
            interface.send_command("version", wait_response=True, timeout=2.0)
            time.sleep(0.5)
            
            # Toggle GPIO example
            for i in range(5):
                interface.send_command(f"gpio toggle", wait_response=True, timeout=1.0)
                time.sleep(0.5)
        
        except KeyboardInterrupt:
            print("\nInterrupted")
        finally:
            interface.disconnect()


def example_5_combined_monitoring_and_logging():
    """Example 5: Combine monitoring with data logging"""
    print("\n=== Example 5: Combined Monitoring and Logging ===\n")
    
    # Create capture and monitor instances
    capture = ThreadedUARTCapture('COM3', 115200)
    log_file = open('uart_detailed.log', 'a', encoding='utf-8')
    
    capture.start()
    
    # Statistics
    total_bytes = 0
    start_time = time.time()
    
    try:
        while True:
            data = capture.get_data(timeout=0.1)
            
            if data:
                timestamp, raw_data = data
                total_bytes += len(raw_data)
                
                # Decode and log
                text = raw_data.decode('utf-8', errors='replace')
                log_entry = f"[{timestamp:.6f}] {text}"
                
                # Write to file
                log_file.write(log_entry)
                log_file.flush()
                
                # Print to console
                print(log_entry, end='')
                
                # Check for specific conditions
                if "ERROR" in text:
                    print("\n⚠️  ERROR CONDITION DETECTED!\n")
                elif "READY" in text:
                    print("\n✅ Device is ready\n")
            
            # Periodic statistics
            elapsed = time.time() - start_time
            if elapsed > 0 and int(elapsed) % 10 == 0:
                rate = total_bytes / elapsed
                print(f"\n--- Stats: {total_bytes} bytes, {rate:.1f} B/s ---\n")
    
    except KeyboardInterrupt:
        print("\n\nStopped by user")
    finally:
        capture.stop()
        log_file.close()
        
        # Final statistics
        elapsed = time.time() - start_time
        print(f"\nFinal Statistics:")
        print(f"  Duration: {elapsed:.2f}s")
        print(f"  Total bytes: {total_bytes}")
        print(f"  Average rate: {total_bytes/elapsed:.2f} B/s")


# =============================================================================
# MAIN ENTRY POINT
# =============================================================================

if __name__ == '__main__':
    import sys
    
    print("Multi-threaded UART Analyzer Examples")
    print("=" * 50)
    print("\nAvailable examples:")
    print("  1. Basic threaded capture with processing")
    print("  2. Pattern monitoring with callbacks")
    print("  3. Dual port logging")
    print("  4. Command interface with responses")
    print("  5. Combined monitoring and logging")
    print("\nUsage: python uart_analyzer_example.py [1-5]")
    print("\nNOTE: Update COM port numbers in the code before running!")
    
    if len(sys.argv) > 1:
        choice = sys.argv[1]
        
        if choice == '1':
            example_1_basic_capture()
        elif choice == '2':
            example_2_pattern_monitoring()
        elif choice == '3':
            example_3_dual_port_logging()
        elif choice == '4':
            example_4_command_interface()
        elif choice == '5':
            example_5_combined_monitoring_and_logging()
        else:
            print("\nInvalid choice. Please select 1-5")
    else:
        print("\nPlease specify an example number (1-5)")
        print("Example: python uart_analyzer_example.py 1")

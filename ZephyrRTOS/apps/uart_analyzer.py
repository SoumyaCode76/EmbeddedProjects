#!/usr/bin/env python3
"""
UART Logic Analyzer and Tracer
A Python-based tool for capturing, analyzing, and logging UART communication.
Includes live waveform visualization (GUI and ASCII terminal modes).
"""

import serial
import serial.tools.list_ports
import time
import argparse
import sys
from datetime import datetime
from collections import deque
import threading
import os
import re

# Optional imports for plotting
try:
    import matplotlib
    matplotlib.use('TkAgg')  # Use TkAgg backend for better Windows compatibility
    import matplotlib.pyplot as plt
    import matplotlib.animation as animation
    from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False


class WaveformPlotter:
    """Real-time waveform plotter using matplotlib"""
    
    def __init__(self, window_size=100, update_interval=50):
        """
        Initialize waveform plotter
        
        Args:
            window_size: Number of samples to display
            update_interval: Update interval in milliseconds
        """
        if not MATPLOTLIB_AVAILABLE:
            raise ImportError("matplotlib is required for waveform plotting")
        
        self.window_size = window_size
        self.update_interval = update_interval
        
        # Data storage
        self.timestamps = deque(maxlen=window_size)
        self.values = deque(maxlen=window_size)
        self.lock = threading.Lock()
        
        # Setup plot
        self.fig, self.ax = plt.subplots(figsize=(12, 5))
        self.line, = self.ax.plot([], [], 'b-', linewidth=1.5, drawstyle='steps-post')
        
        self.ax.set_xlabel('Time (s)')
        self.ax.set_ylabel('Signal Level')
        self.ax.set_title('UART Signal Waveform (Live)')
        self.ax.grid(True, alpha=0.3)
        self.ax.set_ylim(-0.2, 1.2)
        
        # Animation
        self.ani = None
        
    def add_sample(self, timestamp, value):
        """Add a new sample to the plot"""
        with self.lock:
            self.timestamps.append(timestamp)
            self.values.append(value)
    
    def update_plot(self, frame):
        """Update plot callback"""
        with self.lock:
            if len(self.timestamps) > 0:
                self.line.set_data(list(self.timestamps), list(self.values))
                
                # Auto-scale x-axis
                if len(self.timestamps) >= 2:
                    self.ax.set_xlim(self.timestamps[0], self.timestamps[-1])
                
        return self.line,
    
    def start(self):
        """Start the animation"""
        self.ani = animation.FuncAnimation(
            self.fig, self.update_plot, 
            interval=self.update_interval, 
            blit=True, cache_frame_data=False
        )
        plt.show(block=False)
        plt.pause(0.001)
    
    def stop(self):
        """Stop the animation"""
        if self.ani:
            self.ani.event_source.stop()
        plt.close(self.fig)


class ASCIIWaveform:
    """Terminal-based ASCII waveform renderer"""
    
    def __init__(self, width=80, height=5):
        """
        Initialize ASCII waveform renderer
        
        Args:
            width: Character width of waveform
            height: Character height of waveform
        """
        self.width = width
        self.height = height
        self.samples = deque(maxlen=width)
    
    def add_sample(self, value):
        """Add a sample (0 or 1)"""
        self.samples.append(int(value))
    
    def render(self):
        """Render the waveform as ASCII art"""
        if len(self.samples) < 2:
            return ""
        
        # Create waveform string
        output = []
        
        # Top line (HIGH level)
        line_high = []
        for i, val in enumerate(self.samples):
            if val == 1:
                if i > 0 and self.samples[i-1] == 0:
                    line_high.append('┌')
                elif i < len(self.samples) - 1 and self.samples[i+1] == 0:
                    line_high.append('┐')
                else:
                    line_high.append('─')
            else:
                line_high.append(' ')
        
        # Middle line (transitions)
        line_mid = []
        for i, val in enumerate(self.samples):
            if val == 1:
                if i > 0 and self.samples[i-1] == 0:
                    line_mid.append('│')
                elif i < len(self.samples) - 1 and self.samples[i+1] == 0:
                    line_mid.append('│')
                else:
                    line_mid.append(' ')
            else:
                if i > 0 and self.samples[i-1] == 1:
                    line_mid.append('│')
                elif i < len(self.samples) - 1 and self.samples[i+1] == 1:
                    line_mid.append('│')
                else:
                    line_mid.append(' ')
        
        # Bottom line (LOW level)
        line_low = []
        for i, val in enumerate(self.samples):
            if val == 0:
                if i > 0 and self.samples[i-1] == 1:
                    line_low.append('└')
                elif i < len(self.samples) - 1 and self.samples[i+1] == 1:
                    line_low.append('┘')
                else:
                    line_low.append('─')
            else:
                line_low.append(' ')
        
        output.append(''.join(line_high))
        output.append(''.join(line_mid))
        output.append(''.join(line_low))
        
        return '\n'.join(output)
    
    def clear(self):
        """Clear the sample buffer"""
        self.samples.clear()


class UARTAnalyzer:
    def __init__(self, port, baudrate=115200, bytesize=8, parity='N', stopbits=1, 
                 log_file=None, display_hex=False, display_ascii=True, 
                 timestamp=True, buffer_size=1000, waveform_mode=None, 
                 waveform_pattern=None):
        """
        Initialize UART Analyzer
        
        Args:
            port: Serial port name (e.g., 'COM3' or '/dev/ttyUSB0')
            baudrate: Baud rate (default: 115200)
            bytesize: Number of data bits (5, 6, 7, 8)
            parity: Parity ('N', 'E', 'O', 'M', 'S')
            stopbits: Stop bits (1, 1.5, 2)
            log_file: Path to log file (None = no logging)
            display_hex: Display data in hexadecimal
            display_ascii: Display data in ASCII
            timestamp: Add timestamps to output
            buffer_size: Size of internal buffer for statistics
            waveform_mode: Waveform display mode ('gui', 'ascii', 'both', None)
            waveform_pattern: Regex pattern to extract signal values
        """
        self.port = port
        self.baudrate = baudrate
        self.bytesize = bytesize
        self.parity = parity
        self.stopbits = stopbits
        self.log_file = log_file
        self.display_hex = display_hex
        self.display_ascii = display_ascii
        self.timestamp = timestamp
        self.buffer_size = buffer_size
        self.waveform_mode = waveform_mode
        self.waveform_pattern = waveform_pattern
        
        # Statistics
        self.bytes_received = 0
        self.start_time = None
        self.data_buffer = deque(maxlen=buffer_size)
        self.running = False
        
        # Serial connection
        self.ser = None
        self.log_handle = None
        
        # Waveform visualization
        self.gui_plotter = None
        self.ascii_waveform = None
        self.last_waveform_update = 0
        self.waveform_update_interval = 0.5  # Update ASCII waveform every 0.5s
        
        # Setup waveform displays
        if waveform_mode in ['gui', 'both']:
            if MATPLOTLIB_AVAILABLE:
                try:
                    self.gui_plotter = WaveformPlotter(window_size=500, update_interval=10)
                except Exception as e:
                    print(f"Warning: Could not initialize GUI plotter: {e}")
            else:
                print("Warning: matplotlib not available, GUI plotting disabled")
        
        if waveform_mode in ['ascii', 'both']:
            self.ascii_waveform = ASCIIWaveform(width=80, height=3)
        
    def open(self):
        """Open serial port and log file"""
        try:
            # Convert parity
            parity_map = {'N': serial.PARITY_NONE, 'E': serial.PARITY_EVEN, 
                         'O': serial.PARITY_ODD, 'M': serial.PARITY_MARK, 
                         'S': serial.PARITY_SPACE}
            
            # Convert stopbits
            stopbits_map = {1: serial.STOPBITS_ONE, 1.5: serial.STOPBITS_ONE_POINT_FIVE, 
                          2: serial.STOPBITS_TWO}
            
            self.ser = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                bytesize=self.bytesize,
                parity=parity_map.get(self.parity, serial.PARITY_NONE),
                stopbits=stopbits_map.get(self.stopbits, serial.STOPBITS_ONE),
                timeout=0.1
            )
            
            print(f"Connected to {self.port} at {self.baudrate} baud")
            print(f"Config: {self.bytesize}{self.parity}{self.stopbits}")
            
            # Open log file if specified
            if self.log_file:
                self.log_handle = open(self.log_file, 'a', encoding='utf-8')
                print(f"Logging to: {self.log_file}")
            
            self.start_time = time.time()
            return True
            
        except serial.SerialException as e:
            print(f"Error opening port: {e}")
            return False
    
    def close(self):
        """Close serial port and log file"""
        if self.ser and self.ser.is_open:
            self.ser.close()
        if self.log_handle:
            self.log_handle.close()
        if self.gui_plotter:
            self.gui_plotter.stop()
    
    def format_output(self, data, timestamp_val=None):
        """Format output string based on display settings"""
        output_parts = []
        
        # Add timestamp
        if self.timestamp:
            if timestamp_val is None:
                timestamp_val = time.time() - self.start_time
            ts_str = f"[{timestamp_val:>10.6f}s]"
            output_parts.append(ts_str)
        
        # Add hex representation
        if self.display_hex:
            hex_str = ' '.join(f"{b:02X}" for b in data)
            output_parts.append(f"HEX: {hex_str}")
        
        # Add ASCII representation
        if self.display_ascii:
            ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in data)
            output_parts.append(f"ASCII: {ascii_str}")
        
        return ' | '.join(output_parts)
    
    def display_statistics(self):
        """Display reception statistics"""
        if self.start_time:
            elapsed = time.time() - self.start_time
            rate = self.bytes_received / elapsed if elapsed > 0 else 0
            print(f"\n--- Statistics ---")
            print(f"Bytes received: {self.bytes_received}")
            print(f"Elapsed time: {elapsed:.2f}s")
            print(f"Average rate: {rate:.2f} bytes/s")
            print(f"Effective baud: {rate * 10:.0f} baud (approx)")
    
    def find_pattern(self, pattern):
        """Search for a pattern in the buffer"""
        pattern_bytes = pattern.encode('utf-8') if isinstance(pattern, str) else pattern
        buffer_data = bytes(self.data_buffer)
        
        positions = []
        start = 0
        while True:
            pos = buffer_data.find(pattern_bytes, start)
            if pos == -1:
                break
            positions.append(pos)
            start = pos + 1
        
        return positions
    
    def analyze_line_protocol(self):
        """Analyze data for common line protocols (CR, LF, CRLF)"""
        buffer_data = bytes(self.data_buffer)
        cr_count = buffer_data.count(b'\r')
        lf_count = buffer_data.count(b'\n')
        crlf_count = buffer_data.count(b'\r\n')
        
        print(f"\n--- Line Protocol Analysis ---")
        print(f"CR (\\r) count: {cr_count}")
        print(f"LF (\\n) count: {lf_count}")
        print(f"CRLF (\\r\\n) count: {crlf_count}")
    
    def parse_signal_value(self, data_str):
        """
        Parse signal value from data string
        Returns 1 for HIGH, 0 for LOW, None if cannot parse
        """
        # Try pattern matching if specified
        if self.waveform_pattern:
            match = re.search(self.waveform_pattern, data_str)
            if match:
                try:
                    value = match.group(1)
                    # Convert to binary
                    if value.lower() in ['1', 'high', 'on', 'true']:
                        return 1
                    elif value.lower() in ['0', 'low', 'off', 'false']:
                        return 0
                    else:
                        return int(value) != 0
                except (ValueError, IndexError):
                    pass
        
        # Default patterns
        data_lower = data_str.lower().strip()
        
        # Check for binary values
        if data_lower in ['1', 'high', 'on', 'true']:
            return 1
        elif data_lower in ['0', 'low', 'off', 'false']:
            return 0
        
        # Check for numeric values
        try:
            val = float(data_lower)
            return 1 if val != 0 else 0
        except ValueError:
            pass
        
        return None
    
    def update_waveforms(self, data_str, timestamp_val):
        """Update waveform displays with new data"""
        signal_value = self.parse_signal_value(data_str)
        
        if signal_value is not None:
            # Update GUI plotter
            if self.gui_plotter:
                self.gui_plotter.add_sample(timestamp_val, signal_value)
            
            # Update ASCII waveform
            if self.ascii_waveform:
                self.ascii_waveform.add_sample(signal_value)
                
                # Render periodically
                current_time = time.time()
                if current_time - self.last_waveform_update > self.waveform_update_interval:
                    waveform_str = self.ascii_waveform.render()
                    if waveform_str:
                        print("\n" + "="*80)
                        print("Waveform:")
                        print(waveform_str)
                        print("="*80 + "\n")
                    self.last_waveform_update = current_time
    
    def run(self):
        """Main capture loop"""
        if not self.open():
            return
        
        # Start GUI plotter if enabled
        if self.gui_plotter:
            self.gui_plotter.start()
            print("GUI waveform plotter started")
        
        self.running = True
        print("\nCapturing data... (Press Ctrl+C to stop)\n")
        
        if self.waveform_mode:
            print("Waveform mode enabled - parsing signal values")
            if self.waveform_pattern:
                print(f"Using pattern: {self.waveform_pattern}\n")
        
        try:
            while self.running:
                if self.ser.in_waiting > 0:
                    # Read available data
                    data = self.ser.read(self.ser.in_waiting)
                    self.bytes_received += len(data)
                    
                    # Store in buffer for analysis
                    self.data_buffer.extend(data)
                    
                    # Format and display (suppress output in GUI-only mode for performance)
                    timestamp_val = time.time() - self.start_time
                    if self.waveform_mode != 'gui' or self.display_hex:
                        output = self.format_output(data, timestamp_val)
                        print(output)
                    
                    # Update waveforms if enabled
                    if self.waveform_mode:
                        try:
                            data_str = data.decode('utf-8', errors='ignore')
                            self.update_waveforms(data_str, timestamp_val)
                        except Exception as e:
                            pass  # Silently ignore waveform update errors
                    
                    # Log to file
                    if self.log_handle:
                        self.log_handle.write(output + '\n')
                        self.log_handle.flush()
                
                # Keep GUI responsive when data received
                if self.gui_plotter:
                    plt.pause(0.001)
                
                # Small delay to prevent CPU spinning (shorter when GUI active)
                time.sleep(0.001 if self.gui_plotter else 0.01)
                
        except KeyboardInterrupt:
            print("\n\nStopped by user")
        finally:
            self.display_statistics()
            self.close()


def list_ports():
    """List available serial ports"""
    ports = serial.tools.list_ports.comports()
    print("\nAvailable serial ports:")
    for i, port in enumerate(ports):
        print(f"  [{i}] {port.device} - {port.description}")
    return ports


def main():
    parser = argparse.ArgumentParser(
        description='UART Logic Analyzer and Tracer',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Basic usage
  python uart_analyzer.py -p COM3
  
  # With hex display and logging
  python uart_analyzer.py -p COM3 -x -l uart_log.txt
  
  # Different baud rate and parity
  python uart_analyzer.py -p COM3 -b 9600 -P E
  
  # List available ports
  python uart_analyzer.py --list-ports
        """)
    
    parser.add_argument('-p', '--port', type=str, 
                       help='Serial port name (e.g., COM3, /dev/ttyUSB0)')
    parser.add_argument('-b', '--baudrate', type=int, default=115200,
                       help='Baud rate (default: 115200)')
    parser.add_argument('-d', '--bytesize', type=int, default=8, choices=[5, 6, 7, 8],
                       help='Number of data bits (default: 8)')
    parser.add_argument('-P', '--parity', type=str, default='N', 
                       choices=['N', 'E', 'O', 'M', 'S'],
                       help='Parity: N=None, E=Even, O=Odd, M=Mark, S=Space (default: N)')
    parser.add_argument('-s', '--stopbits', type=float, default=1, 
                       choices=[1, 1.5, 2],
                       help='Stop bits (default: 1)')
    parser.add_argument('-l', '--log-file', type=str,
                       help='Log file path (optional)')
    parser.add_argument('-x', '--hex', action='store_true',
                       help='Display data in hexadecimal')
    parser.add_argument('--no-ascii', action='store_true',
                       help='Disable ASCII display')
    parser.add_argument('--no-timestamp', action='store_true',
                       help='Disable timestamps')
    parser.add_argument('--buffer-size', type=int, default=1000,
                       help='Internal buffer size for analysis (default: 1000)')
    parser.add_argument('--list-ports', action='store_true',
                       help='List available serial ports and exit')
    
    # Waveform options
    parser.add_argument('-w', '--waveform', type=str, choices=['gui', 'ascii', 'both'],
                       help='Enable waveform visualization (gui=plot, ascii=terminal, both)')
    parser.add_argument('--waveform-pattern', type=str,
                       help='Regex pattern to extract signal value (use group 1 for value)')
    
    args = parser.parse_args()
    
    # List ports if requested
    if args.list_ports:
        list_ports()
        return
    
    # Check if port is specified
    if not args.port:
        print("Error: Port must be specified with -p/--port")
        print("\nUse --list-ports to see available ports")
        list_ports()
        sys.exit(1)
    
    # Check waveform dependencies
    if args.waveform in ['gui', 'both'] and not MATPLOTLIB_AVAILABLE:
        print("Warning: matplotlib not installed. GUI waveform disabled.")
        print("Install with: pip install matplotlib")
        if args.waveform == 'gui':
            args.waveform = None
        elif args.waveform == 'both':
            args.waveform = 'ascii'
    
    # Create and run analyzer
    analyzer = UARTAnalyzer(
        port=args.port,
        baudrate=args.baudrate,
        bytesize=args.bytesize,
        parity=args.parity,
        stopbits=args.stopbits,
        log_file=args.log_file,
        display_hex=args.hex,
        display_ascii=not args.no_ascii,
        timestamp=not args.no_timestamp,
        buffer_size=args.buffer_size,
        waveform_mode=args.waveform,
        waveform_pattern=args.waveform_pattern
    )
    
    analyzer.run()


if __name__ == '__main__':
    main()

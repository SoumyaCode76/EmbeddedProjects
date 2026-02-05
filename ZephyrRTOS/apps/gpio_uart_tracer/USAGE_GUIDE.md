# Complete Usage Guide: GPIO UART Tracer + Python Analyzer

This guide shows you how to set up and use the complete GPIO monitoring and waveform visualization system.

## Quick Start (5 Steps)

### Step 1: Build and Flash Embedded Code

```bash
cd apps/gpio_uart_tracer
west build -p auto -b stm32f429i_disc1 .
west flash
```

### Step 2: Find Your COM Port

```bash
# On Windows PowerShell
python ../uart_analyzer.py --list-ports
```

Example output:
```
Available serial ports:
  [0] COM3 - STMicroelectronics STLink Virtual COM Port
  [1] COM8 - USB Serial Device
```

### Step 3: Run Python Analyzer (ASCII Waveform)

```bash
python ../uart_analyzer.py -p COM3 -b 115200 -w ascii
```

### Step 4: Press Button and Watch Waveform

Press the user button on your STM32 board and watch the terminal display:

```
[  12.345678s] ASCII: GPIO=0
[  12.445123s] ASCII: GPIO=0
[  12.545890s] ASCII: GPIO=1  ← Button pressed
[  12.645234s] ASCII: GPIO=1
[  12.745567s] ASCII: GPIO=0  ← Button released

================================================================================
Waveform:
     ┌───────┐        ┌──┐     
     │       │        │  │     
─────┘       └────────┘  └─────
================================================================================
```

### Step 5: Try GUI Waveform (Optional)

```bash
# Install matplotlib first
pip install matplotlib

# Run with GUI
python ../uart_analyzer.py -p COM3 -b 115200 -w both
```

---

## Detailed Usage Scenarios

### Scenario 1: Debug Button Debouncing

Watch for bounce behavior when pressing/releasing buttons:

```bash
# High-speed sampling
python ../uart_analyzer.py -p COM3 -b 115200 -w ascii -l button_debug.log
```

In embedded code, reduce `SAMPLE_INTERVAL_MS` to 10ms for faster sampling.

### Scenario 2: Monitor Toggle Signal

Generate a square wave by toggling GPIO in code:

```c
// In main.c, replace the button read with:
static int toggle = 0;
toggle = !toggle;
printk("GPIO=%d\n", toggle);
k_msleep(500);  // 500ms → 1 Hz square wave
```

Then visualize:
```bash
python ../uart_analyzer.py -p COM3 -b 115200 -w gui
```

### Scenario 3: Multi-threaded Monitoring

Use the example script for advanced monitoring:

```bash
# Pattern detection with callbacks
python ../uart_analyzer_example.py 2
```

This will alert you when specific patterns are detected.

### Scenario 4: Log Multiple Test Sessions

```bash
# Create timestamped logs
python ../uart_analyzer.py -p COM3 -b 115200 -w ascii -l test_session_$(date +%Y%m%d_%H%M%S).log
```

### Scenario 5: Compare Two Signals (Dual Port)

If you have two boards:

```python
from uart_analyzer_example import DualUARTLogger

logger = DualUARTLogger('COM3', 'COM4', baudrate=115200)
logger.start()
# Press Ctrl+C to stop
```

---

## Output Format Comparison

### Format 1: Simple (Best for clean waveforms)
```c
#define OUTPUT_FORMAT OUTPUT_FORMAT_SIMPLE
```
Output: `0` or `1`

**Python command:**
```bash
python ../uart_analyzer.py -p COM3 -w ascii
```

### Format 2: Labeled (Best for pattern matching)
```c
#define OUTPUT_FORMAT OUTPUT_FORMAT_LABELED
```
Output: `GPIO=0` or `GPIO=1`

**Python command:**
```bash
python ../uart_analyzer.py -p COM3 -w ascii --waveform-pattern "GPIO=(\d+)"
```

### Format 3: Verbose (Human readable)
```c
#define OUTPUT_FORMAT OUTPUT_FORMAT_VERBOSE
```
Output: `Button: LOW` or `Button: HIGH`

**Python command:**
```bash
python ../uart_analyzer.py -p COM3 -w ascii --waveform-pattern "Button: (HIGH|LOW)"
```

---

## Performance Tuning

### Maximize Sample Rate

**Embedded side:**
```c
#define SAMPLE_INTERVAL_MS  1  // 1ms = 1000 samples/sec
```

**Python side:**
```bash
python ../uart_analyzer.py -p COM3 -b 921600 -w gui  # Higher baud rate
```

### Minimize UART Traffic (State-change only)

In `main.c`, uncomment:
```c
if (pin_state != prev_state) {
    printk("GPIO=%d (changed)\n", pin_state);
    prev_state = pin_state;
}
```

This only transmits on transitions.

---

## Waveform Analysis Tips

### Measure Signal Frequency

The Python analyzer shows timestamps. Calculate frequency from period:

```
GPIO=1 at t=10.000s
GPIO=0 at t=10.250s
GPIO=1 at t=10.500s
GPIO=0 at t=10.750s

Period = 0.500s → Frequency = 2 Hz
```

### Detect Glitches

Use GUI mode with high sample rate to catch short glitches:

```bash
python ../uart_analyzer.py -p COM3 -b 921600 -w gui
```

### Pattern Analysis

Use the statistics feature:
```python
from uart_analyzer_example import UARTSignalMonitor

monitor = UARTSignalMonitor('COM3', 115200)

def on_pattern(data):
    print(f"Pattern detected: {data}")

monitor.add_callback("GPIO=1", on_pattern)
monitor.start()

# Monitor for 60 seconds
time.sleep(60)
print(monitor.get_statistics())
monitor.stop()
```

---

## Troubleshooting

### Problem: No waveform displayed

**Solution 1:** Check output format matches pattern
```bash
# If using labeled format
python ../uart_analyzer.py -p COM3 -w ascii --waveform-pattern "GPIO=(\d+)"
```

**Solution 2:** Verify data is binary (0/1)
```bash
# View raw data first
python ../uart_analyzer.py -p COM3 --no-timestamp
```

### Problem: Waveform choppy or delayed

**Solution:** Reduce sample interval or increase baud rate
```c
#define SAMPLE_INTERVAL_MS  50  // Reduce from 100
```

### Problem: Matplotlib errors

**Solution:** Install/reinstall matplotlib
```bash
pip uninstall matplotlib
pip install matplotlib
```

Or use ASCII-only mode:
```bash
python ../uart_analyzer.py -p COM3 -w ascii
```

---

## Advanced: Custom GPIO Pins

### Using Different Pins

Create `app.overlay` in your project:

```dts
/ {
    aliases {
        sw0 = &custom_button;
        led0 = &custom_led;
    };
};

&gpiob {
    custom_button: custom-button {
        gpios = <&gpiob 5 GPIO_ACTIVE_HIGH>;  // PB5
    };
};

&gpioc {
    custom_led: custom-led {
        gpios = <&gpioc 13 GPIO_ACTIVE_HIGH>;  // PC13
    };
};
```

Rebuild and flash.

---

## Example Session

```bash
# Terminal 1: Flash device
cd apps/gpio_uart_tracer
west build -p auto -b stm32f429i_disc1 .
west flash

# Terminal 2: Run analyzer with logging and both waveforms
python ../uart_analyzer.py -p COM3 -b 115200 -w both -l gpio_session.log

# Now press the button and watch:
# - Terminal shows ASCII waveform
# - GUI window shows scrolling plot
# - gpio_session.log records everything
```

Press Ctrl+C to stop and see statistics:

```
Stopped by user

--- Statistics ---
Bytes received: 4582
Elapsed time: 45.23s
Average rate: 101.29 bytes/s
Effective baud: 1013 baud (approx)
```

---

## Integration with Test Framework

Use the multi-threaded example for automated testing:

```python
from uart_analyzer_example import ThreadedUARTCapture
import time

# Start capture
capture = ThreadedUARTCapture('COM3', 115200)
capture.start()

# Run test sequence
test_results = []
for i in range(10):
    data = capture.get_data(timeout=1.0)
    if data:
        timestamp, raw = data
        text = raw.decode('utf-8', errors='ignore')
        if 'GPIO=1' in text:
            test_results.append('PASS')
        else:
            test_results.append('FAIL')
    time.sleep(0.5)

capture.stop()

# Analyze results
print(f"Test Results: {test_results.count('PASS')}/{len(test_results)} passed")
```

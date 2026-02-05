# GPIO UART Tracer Example

Bare-metal embedded C code that reads a GPIO pin and transmits its state over UART for real-time visualization using the Python UART analyzer.

## Hardware Setup

### STM32F429I-DISC1
- **Button (Input)**: User button (PA0) - `sw0` alias
- **LED (Output)**: LED3 (PG13) - `led0` alias (visual feedback)
- **UART**: Console UART (typically USART1)

### Other Boards
The code uses Zephyr device tree aliases, so it's portable across boards:
- `sw0` - Button/switch input
- `led0` - LED output (optional)

## Build Instructions

```bash
# Navigate to the project directory
cd apps/gpio_uart_tracer

# Build for STM32F429I-DISC1
west build -p auto -b stm32f429i_disc1 .

# Flash to board
west flash
```

## Python UART Analyzer Usage

### Option 1: Basic ASCII Waveform (Terminal)
```bash
python ../uart_analyzer.py -p COM3 -b 115200 -w ascii
```

### Option 2: GUI Waveform (Matplotlib)
```bash
python ../uart_analyzer.py -p COM3 -b 115200 -w gui
```

### Option 3: Both ASCII and GUI
```bash
python ../uart_analyzer.py -p COM3 -b 115200 -w both
```

### Option 4: With custom pattern extraction
If using `OUTPUT_FORMAT_LABELED` (GPIO=0 or GPIO=1):
```bash
python ../uart_analyzer.py -p COM3 -b 115200 -w both --waveform-pattern "GPIO=(\d+)"
```

### Option 5: With logging
```bash
python ../uart_analyzer.py -p COM3 -b 115200 -w both -l gpio_trace.log
```

## Output Formats

The embedded code supports 4 output formats (change `OUTPUT_FORMAT` in main.c):

1. **SIMPLE**: `0` or `1`
2. **LABELED**: `GPIO=0` or `GPIO=1` (recommended for pattern matching)
3. **VERBOSE**: `Button: LOW` or `Button: HIGH`
4. **NUMERIC**: `Value: 0` or `Value: 100`

## Customization

### Change Sample Rate
In `main.c`, modify:
```c
#define SAMPLE_INTERVAL_MS  100  // Change to desired interval (ms)
```

### Output Only on State Change
Uncomment the conditional output section in `main.c` to reduce UART traffic:
```c
if (pin_state != prev_state) {
    printk("GPIO=%d (changed)\n", pin_state);
    prev_state = pin_state;
}
```

### Use Different GPIO Pin
Modify device tree overlay (`app.overlay`) to use different pins.

## Expected Behavior

1. **Embedded Device**: 
   - Reads button state every 100ms
   - Sends state over UART
   - LED mirrors button state (visual feedback)

2. **Python Analyzer**:
   - Captures UART data in real-time
   - Displays ASCII waveform in terminal (if `-w ascii` or `-w both`)
   - Shows GUI plot window (if `-w gui` or `-w both`)
   - Logs to file (if `-l` specified)

## Example Output

### Terminal (ASCII Waveform)
```
================================================================================
Waveform:
┌─┐   ┌─┐   ┌─┐   
│ │   │ │   │ │   
└─┘   └─┘   └─┘   
================================================================================
```

### GUI Window
Real-time scrolling waveform showing button press/release patterns.

## Troubleshooting

### No Data in Python Analyzer
- Check COM port number (`python ../uart_analyzer.py --list-ports`)
- Verify baud rate matches (115200)
- Ensure board is flashed and running
- Check USB cable connection

### Waveform Not Displaying
- Ensure output format produces recognizable values (0/1)
- Use `--waveform-pattern` if using labeled format
- Check that matplotlib is installed (`pip install matplotlib`)

### Build Errors
- Ensure Zephyr SDK is properly installed
- Check that board has `sw0` and `led0` aliases defined
- Run `west update` to sync dependencies

## Integration with Custom Hardware

To use with custom GPIO pins, create `app.overlay`:

```dts
/ {
    aliases {
        custom-gpio = &gpiob 5;  // PB5 as example
    };
};
```

Then modify code to use `DT_ALIAS(custom_gpio)`.

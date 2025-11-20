# STM32 Blinky Application

A simple Zephyr RTOS application that blinks an LED on STM32F429I Discovery board.

## Files Structure

```
stm32_blinky/
├── src/
│   └── main.c              # Application source code
├── boards/
│   └── stm32f429i_disc1.overlay  # Board-specific overlays
├── prj.conf                # Project configuration
├── CMakeLists.txt          # Build configuration
└── README.md              # This file
```

## Building

```bash
west build -b stm32f429i_disc1
```

## Flashing

```bash
west flash
```

## Features

- Toggles LED every 1 second
- Prints LED status to console
- Uses Zephyr GPIO driver
- Configured for STM32F429I Discovery board
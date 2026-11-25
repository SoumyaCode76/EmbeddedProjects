# ARM Firmware Compilation and STM32 QEMU Guide

This guide shows how to compile bare-metal ARM firmware and run it with QEMU in Linux.

## 1. Install the ARM GNU Toolchain

Download the Arm GNU Toolchain 15.2 package:

```bash
cd /tmp
wget https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi.tar.xz
```

Extract and install it under `/opt`:

```bash
tar -xf arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi.tar.xz
sudo mv arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi /opt/
```

Add the toolchain to the shell's `PATH`:

```bash
echo 'export PATH="/opt/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

Verify the compiler:

```bash
arm-none-eabi-gcc --version
```

Ubuntu package repositories do not provide packages named `arm-none-eabi-gcc`, `arm-none-eabi-g++`, or `arm-none-eabi-gdb`, so installing them with `apt` is not the recommended installation method.

## 2. Install QEMU

```bash
sudo apt update
sudo apt install -y qemu-system-arm
qemu-system-arm --version
```

List the available ARM machines:

```bash
qemu-system-arm -machine help
```

This guide uses machines available in QEMU 8.2:

| QEMU machine | Board | Core |
|---|---|---|
| `stm32vldiscovery` | ST STM32VLDISCOVERY | Cortex-M3 |
| `olimex-stm32-h405` | Olimex STM32-H405 | Cortex-M4 |

Support for a board in QEMU does not guarantee that every peripheral on the physical board is implemented.

## 3. Create a Linker Script

Create `stm32vldiscovery.ld`:

```ld
ENTRY(Reset_Handler)

_estack = ORIGIN(RAM) + LENGTH(RAM);

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 128K
    RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 8K
}

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    .text :
    {
        *(.text*)
        *(.rodata*)
        KEEP(*(.init))
        KEEP(*(.fini))
    } > FLASH

    .data :
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > RAM AT > FLASH

    _sidata = LOADADDR(.data);

    .bss (NOLOAD):
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM
}
```

The linker script must match the target MCU's memory map. The STM32VLDISCOVERY values above are for a small STM32F100-based device. Adjust the flash, RAM, and vector table addresses for a different MCU.

## 4. Compile Firmware

Set the toolchain path if a new shell does not have it:

```bash
export PATH="/opt/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi/bin:$PATH"
```

Compile for the Cortex-M3 board:

```bash
arm-none-eabi-gcc \
    -mcpu=cortex-m3 \
    -mthumb \
    -ffreestanding \
    -fno-builtin \
    -nostdlib \
    -T stm32vldiscovery.ld \
    -Wl,-Map=firmware.map \
    -o firmware.elf \
    startup.c main.c
```

Generate an image, map file information, and ELF size information:

```bash
arm-none-eabi-size firmware.elf
arm-none-eabi-objdump -h firmware.elf
arm-none-eabi-objcopy -O binary firmware.elf firmware.bin
```

For an Olimex STM32-H405 Cortex-M4 build, use:

```bash
arm-none-eabi-gcc \
    -mcpu=cortex-m4 \
    -mthumb \
    -mfpu=fpv4-sp-d16 \
    -mfloat-abi=hard \
    -ffreestanding \
    -fno-builtin \
    -nostdlib \
    -T stm32h405.ld \
    -o firmware.elf \
    startup.c main.c
```

## 5. Run the Firmware

Run the ELF directly on the STM32VLDISCOVERY QEMU machine:

```bash
qemu-system-arm \
    -M stm32vldiscovery \
    -nographic \
    -semihosting-config enable=on,target=native \
    -kernel firmware.elf
```

Run it on the Olimex STM32-H405 machine:

```bash
qemu-system-arm \
    -M olimex-stm32-h405 \
    -nographic \
    -semihosting-config enable=on,target=native \
    -kernel firmware.elf
```

To exit QEMU's `-nographic` console, press `Ctrl+A`, then `X`.

## 6. Using Semihosting

Semihosting lets firmware call host operations, such as writing test output or terminating the program. This is useful for automated QEMU analysis.

The following example requests an ARM semihosting exit:

```c
#include <stdint.h>

static inline void semihost_exit(void)
{
    register uint32_t r0 __asm__("r0") = 0x18;
    register uint32_t r1 __asm__("r1") = 0x20026;

    __asm__ volatile ("bkpt 0xAB"
                      :
                      : "r"(r0), "r"(r1)
                      : "memory");
}
```

QEMU must be started with `-semihosting-config enable=on,target=native`.

## 7. Debug with GDB

Install GDB and start QEMU paused:

```bash
sudo apt install -y gdb-multiarch

qemu-system-arm \
    -M stm32vldiscovery \
    -nographic \
    -S \
    -gdb tcp::1234 \
    -kernel firmware.elf
```

In another terminal:

```bash
arm-none-eabi-gdb firmware.elf
```

Inside GDB:

```text
(gdb) target remote localhost:1234
(gdb) break Reset_Handler
(gdb) continue
(gdb) stepi
```

## 8. Minimal Startup Expectations

A basic bare-metal firmware must provide:

- A vector table in `.isr_vector`
- A `Reset_Handler`
- Stack initialization
- `.data` copying from flash to RAM
- `.bss` clearing
- A call to `main()`

The startup assembly and linker script must agree on the flash origin and RAM origin. QEMU starts Cortex-M firmware using the initial stack pointer and reset vector.

## 9. QEMU Troubleshooting

Check whether the ELF contains a valid Cortex-M reset vector:

```bash
arm-none-eabi-readelf -h firmware.elf
arm-none-eabi-nm -n firmware.elf
arm-none-eabi-objdump -d firmware.elf
```

If QEMU does not start the firmware:

- Confirm that `Reset_Handler` is the reset vector.
- Confirm that the vector table is linked at `0x08000000`.
- Confirm that the vector table is in the output ELF.
- Confirm that the Cortex-M CPU option matches the board.
- Use `-d int,cpu_reset -D qemu.log` to capture CPU diagnostics.

Example diagnostic command:

```bash
qemu-system-arm \
    -M stm32vldiscovery \
    -nographic \
    -kernel firmware.elf \
    -d int,cpu_reset \
    -D qemu.log
```

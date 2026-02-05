# STM32 vs Zephyr DMA Terminology Guide

## Overview

There's a common confusion when working with DMA in Zephyr RTOS on STM32 platforms because **STM32 hardware terminology** differs from **Zephyr API terminology**.

This guide clarifies the mapping between them.

---

## STM32 DMA Hardware Terminology

STM32F4 series has a hierarchical DMA structure:

```
DMA Controller (DMA1 or DMA2)
  └── Stream (0-7)          ← 8 streams per controller
       └── Channel (0-7)    ← Channel selection within stream (hardware mux)
```

### Example from STM32F429 Reference Manual (Table 28)

| Peripheral | DMA Controller | Stream | Channel |
|------------|---------------|--------|---------|
| USART1_TX  | DMA2          | 7      | 4       |
| USART1_RX  | DMA2          | 5      | 4       |
| USART1_RX  | DMA2          | 2      | 4       |
| SPI1_TX    | DMA2          | 3      | 3       |
| SPI1_RX    | DMA2          | 0 or 2 | 3       |
| I2C1_TX    | DMA1          | 6 or 7 | 1       |
| I2C1_RX    | DMA1          | 0 or 5 | 1       |
| ADC1       | DMA2          | 0 or 4 | 0       |

### STM32 Hardware Components

1. **DMA Controller**: Either DMA1 or DMA2
   - Each controller has 8 streams
   - Different peripherals are routed to specific controllers

2. **Stream (0-7)**: The physical DMA channel/engine
   - Performs the actual data transfer
   - Can be configured for different transfer types (M2M, M2P, P2M, P2P)
   - Has its own configuration register (DMA_SxCR)

3. **Channel (0-7)**: Request mapping/multiplexer
   - Selects which peripheral request triggers the stream
   - Configured in DMA_SxCR register bits 25-27
   - Multiple peripherals can share the same channel number on different streams

---

## Zephyr DMA API Terminology

Zephyr uses a hardware-agnostic API that must work across different architectures (STM32, NXP, Nordic, TI, etc.).

### Zephyr API Structure

```c
int dma_config(const struct device *dma_dev, 
               uint32_t channel,           // ← This is the STREAM number
               struct dma_config *config)
```

```c
struct dma_config {
    uint32_t dma_slot;      // ← This is the CHANNEL (hardware mux/request)
    uint32_t channel_direction;
    uint32_t source_data_size;
    uint32_t dest_data_size;
    // ... other fields
};
```

### Zephyr Terminology Components

1. **`dma_dev`**: Pointer to DMA controller device (DMA1 or DMA2)
2. **`channel`**: Stream number (0-7) - the physical DMA engine
3. **`dma_slot`**: Channel selection (0-7) - the peripheral request mapping

---

## Critical Mapping Table

| STM32 Hardware Term | STM32 Range | Zephyr API Term | Zephyr Location | Description |
|---------------------|-------------|-----------------|-----------------|-------------|
| **Controller** | DMA1, DMA2 | `dma_dev` | Function parameter | Device pointer |
| **Stream** | 0-7 | `channel` | `dma_config()` parameter | Physical DMA stream |
| **Channel** | 0-7 | `dma_slot` | `struct dma_config` field | Request mapping |

---

## Practical Example: USART1 on STM32F429

### From STM32F429 Reference Manual

- **USART1_TX**: DMA2, Stream 7, Channel 4
- **USART1_RX**: DMA2, Stream 5, Channel 4

### In Zephyr Code

```c
#include <zephyr/drivers/dma.h>

/* STM32 terminology → Zephyr defines */
#define DMA_STREAM_TX    7    // STM32 Stream 7 → Zephyr channel
#define DMA_STREAM_RX    5    // STM32 Stream 5 → Zephyr channel
#define DMA_CHANNEL_USART1 4  // STM32 Channel 4 → Zephyr dma_slot

/* Get DMA2 device */
static const struct device *dma_dev = DEVICE_DT_GET(DT_NODELABEL(dma2));

int setup_dma_uart_tx(void)
{
    struct dma_config dma_cfg = {0};
    struct dma_block_config dma_block_cfg = {0};

    /* Get USART1 data register address */
    uint32_t usart1_base = DT_REG_ADDR(DT_NODELABEL(usart1));
    uint32_t usart1_dr = usart1_base + 0x04;  // DR offset

    /* Configure DMA */
    dma_cfg.channel_direction = MEMORY_TO_PERIPHERAL;
    dma_cfg.source_data_size = 1;      // 8-bit
    dma_cfg.dest_data_size = 1;        // 8-bit
    dma_cfg.source_burst_length = 1;
    dma_cfg.dest_burst_length = 1;
    
    /* STM32 "Channel 4" → Zephyr "dma_slot" */
    dma_cfg.dma_slot = DMA_CHANNEL_USART1;  // ← Channel selection (4)
    
    dma_cfg.dma_callback = dma_callback;
    dma_cfg.user_data = NULL;
    dma_cfg.head_block = &dma_block_cfg;

    /* Configure transfer block */
    dma_block_cfg.block_size = 32;
    dma_block_cfg.source_address = (uint32_t)tx_buffer;
    dma_block_cfg.dest_address = usart1_dr;

    /* STM32 "Stream 7" → Zephyr "channel" parameter */
    return dma_config(dma_dev, DMA_STREAM_TX, &dma_cfg);  // ← Stream (7)
    //                         ^^^^^^^^^^^^^^^
    //                         Stream number
}

int setup_dma_uart_rx(void)
{
    struct dma_config dma_cfg = {0};
    struct dma_block_config dma_block_cfg = {0};

    uint32_t usart1_base = DT_REG_ADDR(DT_NODELABEL(usart1));
    uint32_t usart1_dr = usart1_base + 0x04;

    dma_cfg.channel_direction = PERIPHERAL_TO_MEMORY;
    dma_cfg.source_data_size = 1;
    dma_cfg.dest_data_size = 1;
    dma_cfg.source_burst_length = 1;
    dma_cfg.dest_burst_length = 1;
    
    /* STM32 "Channel 4" → Zephyr "dma_slot" */
    dma_cfg.dma_slot = DMA_CHANNEL_USART1;  // ← Channel selection (4)
    
    dma_cfg.dma_callback = dma_callback;
    dma_cfg.user_data = NULL;
    dma_cfg.head_block = &dma_block_cfg;

    dma_block_cfg.block_size = 32;
    dma_block_cfg.source_address = usart1_dr;
    dma_block_cfg.dest_address = (uint32_t)rx_buffer;

    /* STM32 "Stream 5" → Zephyr "channel" parameter */
    int ret = dma_config(dma_dev, DMA_STREAM_RX, &dma_cfg);  // ← Stream (5)
    if (ret == 0) {
        ret = dma_start(dma_dev, DMA_STREAM_RX);
    }
    return ret;
}
```

---

## Visual Comparison

### STM32 Reference Manual Says:

```
USART1_TX → DMA2, Stream 7, Channel 4
            ^^^^  ^^^^^^^^  ^^^^^^^^^
            |     |         |
            |     |         └─ Hardware channel select (request mux)
            |     └─────────── Physical DMA stream (transfer engine)
            └───────────────── DMA controller
```

### In Zephyr Code:

```c
const struct device *dma_dev = DEVICE_DT_GET(DT_NODELABEL(dma2));
                                                          ^^^^
                                                          DMA controller
                                                          
dma_config(dma_dev,        DMA_STREAM_TX,         &dma_cfg);
           ^^^^^^^         ^^^^^^^^^^^^^^         
           DMA2 device     Stream 7 (channel)     

dma_cfg.dma_slot = DMA_CHANNEL_USART1;  // = 4
                   ^^^^^^^^^^^^^^^^^^
                   Channel 4 (dma_slot)
```

---

## Why the Difference?

Zephyr RTOS is a **hardware-agnostic** real-time operating system that runs on many different architectures:

- **STM32** (ARM Cortex-M): Uses Stream + Channel
- **NXP i.MX RT**: Uses different DMA terminology
- **Nordic nRF**: Has EasyDMA with different structure
- **TI CC13xx**: Has μDMA with different architecture
- **RISC-V platforms**: Various DMA implementations

Zephyr abstracts these differences with generic terms:
- `channel` = the physical DMA transfer engine
- `dma_slot` = the peripheral request/trigger selection

---

## Complete STM32F429 DMA Mapping Reference

### DMA1 Streams and Channels

| Stream | Ch 0 | Ch 1 | Ch 2 | Ch 3 | Ch 4 | Ch 5 | Ch 6 | Ch 7 |
|--------|------|------|------|------|------|------|------|------|
| 0 | - | I2C1_RX | TIM4_CH1 | - | I2C3_RX | TIM5_CH3 | TIM5_UP | - |
| 1 | - | I2C3_RX | TIM2_UP | - | UART5_RX | TIM5_CH4 | TIM5_TRIG | TIM6_UP |
| 2 | - | TIM7_UP | I2C3_RX | - | UART4_RX | TIM3_CH4 | TIM5_CH1 | I2C3_RX |
| 3 | - | TIM4_CH2 | I2C3_RX | - | UART4_TX | TIM5_CH4 | TIM5_UP | I2C2_RX |
| 4 | - | TIM4_CH3 | I2C3_TX | - | UART4_TX | TIM5_CH2 | - | I2C2_TX |
| 5 | - | I2C1_RX | TIM2_CH1 | - | UART5_RX | TIM5_CH1 | TIM5_UP | DAC1 |
| 6 | - | I2C1_TX | TIM2_CH2 | - | UART5_TX | TIM5_UP | TIM5_TRIG | DAC2 |
| 7 | - | I2C1_TX | TIM2_UP | TIM4_UP | - | TIM5_CH3 | I2C2_TX | - |

### DMA2 Streams and Channels

| Stream | Ch 0 | Ch 1 | Ch 2 | Ch 3 | Ch 4 | Ch 5 | Ch 6 | Ch 7 |
|--------|------|------|------|------|------|------|------|------|
| 0 | ADC1 | - | TIM8_CH1 | SPI1_RX | SPI4_RX | - | TIM1_TRIG | - |
| 1 | - | SAI1_A | TIM8_CH2 | - | - | SAI1_B | TIM1_CH1 | DCMI |
| 2 | TIM8_CH1 | - | TIM8_CH2 | SPI1_RX | USART1_RX | TIM8_UP | TIM1_CH2 | DCMI |
| 3 | SAI1_A | TIM8_CH1 | - | SPI1_TX | SDIO | SAI1_B | TIM1_CH1 | - |
| 4 | ADC1 | - | TIM8_CH3 | - | - | SAI1_B | TIM1_CH4 | - |
| 5 | SAI1_A | - | TIM8_CH2 | SPI1_TX | USART1_RX | - | TIM1_UP | DCMI |
| 6 | TIM1_CH1 | - | TIM8_CH3 | - | SDIO | USART6_TX | TIM1_CH3 | - |
| 7 | - | TIM8_CH4 | TIM8_CH3 | - | USART1_TX | USART6_TX | - | DCMI |

---

## Quick Reference for Common Peripherals

```c
/* USART1 on STM32F429 */
#define USART1_TX_STREAM  7   // DMA2 Stream 7
#define USART1_RX_STREAM  5   // DMA2 Stream 5 (or Stream 2)
#define USART1_CHANNEL    4   // Channel 4

/* SPI1 on STM32F429 */
#define SPI1_TX_STREAM    3   // DMA2 Stream 3 (or Stream 5)
#define SPI1_RX_STREAM    0   // DMA2 Stream 0 (or Stream 2)
#define SPI1_CHANNEL      3   // Channel 3

/* I2C1 on STM32F429 */
#define I2C1_TX_STREAM    6   // DMA1 Stream 6 (or Stream 7)
#define I2C1_RX_STREAM    0   // DMA1 Stream 0 (or Stream 5)
#define I2C1_CHANNEL      1   // Channel 1

/* ADC1 on STM32F429 */
#define ADC1_STREAM       0   // DMA2 Stream 0 (or Stream 4)
#define ADC1_CHANNEL      0   // Channel 0
```

---

## Common Mistakes to Avoid

### ❌ Wrong - Using STM32 terminology directly in Zephyr

```c
// DON'T DO THIS
struct dma_config cfg = {
    .channel = 7,        // ← Trying to use stream number here
    .dma_slot = 4        // ← This is correct
};
```

### ✅ Correct - Using Zephyr terminology

```c
// DO THIS
dma_config(dma_dev, 7, &cfg);  // ← Stream 7 goes in the parameter
cfg.dma_slot = 4;              // ← Channel 4 goes in the struct
```

---

## Device Tree Configuration

For STM32 in Zephyr, you typically just enable the DMA controller:

```dts
&usart1 {
    status = "okay";
    pinctrl-0 = <&usart1_tx_pa9 &usart1_rx_pa10>;
    pinctrl-names = "default";
    current-speed = <115200>;
};

&dma2 {
    status = "okay";
};

/ {
    aliases {
        my-dma = &dma2;
    };
};
```

The DMA configuration (stream and channel) is done **in C code** using the DMA API, not in the device tree.

---

## Summary

| When you see in STM32 docs... | Use in Zephyr code... | Example Value |
|-------------------------------|----------------------|---------------|
| DMA1 or DMA2 | `DEVICE_DT_GET(DT_NODELABEL(dma1))` | `dma_dev` |
| Stream 0-7 | `dma_config()` 2nd parameter | `7` |
| Channel 0-7 | `dma_cfg.dma_slot` | `4` |

**Remember**: 
- Zephyr `channel` = STM32 `Stream`
- Zephyr `dma_slot` = STM32 `Channel`

---

## Additional Resources

- [STM32F429 Reference Manual RM0090](https://www.st.com/resource/en/reference_manual/dm00031020.pdf) - See Table 28 for DMA request mapping
- [Zephyr DMA API Documentation](https://docs.zephyrproject.org/latest/hardware/peripherals/dma.html)
- [Zephyr STM32 DMA Driver](https://github.com/zephyrproject-rtos/zephyr/tree/main/drivers/dma)

---

*Document created for STM32F429ZIT6 with Zephyr RTOS*

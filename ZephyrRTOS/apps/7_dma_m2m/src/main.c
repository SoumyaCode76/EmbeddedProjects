#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/dma.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/devicetree.h>
#include <string.h>

#define UART_NODE DT_NODELABEL(usart1)
#define UART_BASE_ADDR DT_REG_ADDR(UART_NODE)

#define DMA_STREAM_TX    7    // USART1_TX uses Stream 7
#define DMA_STREAM_RX    5    // USART1_RX uses Stream 5
#define DMA_CHANNEL_USART1 4  // Both use Channel 4

static const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);
static const struct device *dma_dev = DEVICE_DT_GET(DT_ALIAS(my_dma));

/* Separate buffers for TX and RX */
static char tx_buf[32] __aligned(32);
static char rx_buf[32] __aligned(32);

/* Separate completion flags */
static volatile bool tx_complete = false;
static volatile bool rx_complete = false;

void dma_callback(const struct device *dev, void *user_data, 
                  uint32_t channel, int status)
{
    if (status == DMA_STATUS_COMPLETE) {
        if (channel == DMA_STREAM_TX) {
            tx_complete = true;
            printk("TX DMA complete\n");
        } else if (channel == DMA_STREAM_RX) {
            rx_complete = true;
            printk("RX DMA complete\n");
        }
    } else {
        printk("DMA error on channel %u: %d\n", channel, status);
    }
}

int configure_uart_dma_both_directions(void)
{
    int ret;
    uint32_t usart1_dr = UART_BASE_ADDR + 0x04;

    /* ===== Configure TX DMA (Memory to Peripheral) ===== */
    struct dma_config tx_cfg = {0};
    struct dma_block_config tx_block = {0};

    tx_cfg.channel_direction = MEMORY_TO_PERIPHERAL;
    tx_cfg.source_data_size = 1;
    tx_cfg.dest_data_size = 1;
    tx_cfg.source_burst_length = 1;
    tx_cfg.dest_burst_length = 1;
    tx_cfg.dma_slot = DMA_CHANNEL_USART1;  // Channel 4
    tx_cfg.dma_callback = dma_callback;
    tx_cfg.user_data = (void *)"TX";
    tx_cfg.channel_priority = 0;
    tx_cfg.head_block = &tx_block;

    tx_block.block_size = sizeof(tx_buf);
    tx_block.source_address = (uint32_t)tx_buf;
    tx_block.dest_address = usart1_dr;

    ret = dma_config(dma_dev, DMA_STREAM_TX, &tx_cfg);  // Stream 7
    if (ret < 0) {
        printk("TX DMA config failed: %d\n", ret);
        return ret;
    }

    /* ===== Configure RX DMA (Peripheral to Memory) ===== */
    struct dma_config rx_cfg = {0};
    struct dma_block_config rx_block = {0};

    rx_cfg.channel_direction = PERIPHERAL_TO_MEMORY;
    rx_cfg.source_data_size = 1;
    rx_cfg.dest_data_size = 1;
    rx_cfg.source_burst_length = 1;
    rx_cfg.dest_burst_length = 1;
    rx_cfg.dma_slot = DMA_CHANNEL_USART1;  // Same Channel 4
    rx_cfg.dma_callback = dma_callback;
    rx_cfg.user_data = (void *)"RX";
    rx_cfg.channel_priority = 1;  // Higher priority for RX
    

    rx_cfg.block_count = 1;          // Single block in circular mode
    
    rx_cfg.head_block = &rx_block;

    rx_block.block_size = 2;//sizeof(rx_buf);
    rx_block.source_address = usart1_dr;
    rx_block.dest_address = (uint32_t)rx_buf;

    ret = dma_config(dma_dev, DMA_STREAM_RX, &rx_cfg);  // Stream 5
    if (ret < 0) {
        printk("RX DMA config failed: %d\n", ret);
        return ret;
    }

    printk("Both TX and RX DMA configured successfully\n");
    return 0;
}

int main(void)
{
    int ret;

    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready\n");
        return 0;
    }

    uart_irq_rx_enable(uart_dev);

    if (!device_is_ready(dma_dev)) {
        printk("DMA not ready\n");
        return 0;
    }

    /* Configure both directions */
    ret = configure_uart_dma_both_directions();
    if (ret < 0) {
        return 0;
    }

    /* Start RX DMA first (always listening) */
    ret = dma_start(dma_dev, DMA_STREAM_RX);
    if (ret < 0) {
        printk("RX DMA start failed: %d\n", ret);
        return 0;
    }

    printk("Both TX and RX DMA are active!\n");

    /* Both channels can now operate simultaneously */
    while (1) {
		if(rx_complete == true)
		{
			rx_complete = false;
			/* Restart RX DMA to keep it always listening */
            int ret = dma_start(dma_dev, DMA_STREAM_RX);
            if (ret < 0) {
                printk("Failed to restart RX DMA: %d\n", ret);
            }
			ret = dma_stop(dma_dev, DMA_STREAM_TX);
			if(ret != 0 && ret != -EBUSY)
			{
				printk("TX DMA stop failed: %d\n", ret);
			}
			memcpy((void *)tx_buf, (const void *)rx_buf, sizeof(tx_buf));
			tx_complete = false;
			ret = dma_start(dma_dev, DMA_STREAM_TX);
			if (ret < 0) {
				printk("TX DMA start failed: %d\n", ret);
				return 0;
			}
		}
		else if(tx_complete == true)
		{
			tx_complete = false;
			printk("TX DMA complete\n");
		}
        k_msleep(50);
    }

    return 0;
}
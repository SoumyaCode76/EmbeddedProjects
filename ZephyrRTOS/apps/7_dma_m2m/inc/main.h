#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/device.h>
#include <zephyr/irq.h>

#define UART1     DT_ALIAS(stlink_uart)
#define LEDG_NODE DT_NODELABEL(green_led_3)
#define LEDR_NODE DT_NODELABEL(red_led_4)

typedef struct
{
    uint8_t data;
}UART_RX_t;

#define UART_MSGQ_SIZE      sizeof(UART_RX_t)
#define UART_MSGQ_MAX       (10)

extern struct k_msgq uart_input_msgq;
extern const struct device * uart_dev;

void uart_rx_isr(const struct device *, void *);



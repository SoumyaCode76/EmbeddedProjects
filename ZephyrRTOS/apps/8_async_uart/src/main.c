/*
 * Copyright (c) 2022 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <errno.h>

#include <string.h>

/* change this to any other UART peripheral if desired */
#define UART_DEVICE_NODE  DT_NODELABEL(usart1)//DT_CHOSEN(zephyr_shell_uart)

#define MSG_SIZE 32

/* queue to store up to 10 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 10, 4);

static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* receive buffer used in UART ISR callback */
static char rx_buf[MSG_SIZE];
static char tx_buf[MSG_SIZE];


/*
 * Read characters from UART until line end is detected. Afterwards push the
 * data to the message queue.
 */
void serial_cb_async(const struct device *dev, struct uart_event *evt, void *user_data);

/*
 * Print a null-terminated string character by character to the UART interface
 */

void print_uart(char *buf);

int main(void)
{
	volatile int ret = 0;

	ret = device_is_ready(uart_dev);
	if (ret < 0) {
		printk("UART device not found!: %d\n", ret);
		return 0;
	}
	ret = 0;
	ret = uart_callback_set(uart_dev, serial_cb_async, NULL);
	if(ret != 0)
	{
		printk("Failed to set UART callback: %d\n", ret);
		return -1;
	}
	ret = 0;
	ret = uart_rx_enable(uart_dev, rx_buf, sizeof(rx_buf), SYS_FOREVER_US);
	if(ret != 0)
	{
		if (ret < 0) {
			if (ret == -ENOTSUP) {
				printk("Interrupt-driven UART API support not enabled\n");
			} else if (ret == -ENOSYS) {
				printk("UART device does not support interrupt-driven API\n");
			} else {
				printk("Error setting UART callback: %d\n", ret);
			}
		}		
	}

	print_uart("Hello! I'm your echo bot.\n");
	print_uart("Tell me something and press enter:\n");

	/* indefinitely wait for input from the user */
	while (k_msgq_get(&uart_msgq, &tx_buf[0], K_FOREVER) == 0) {
		print_uart("Echo: ");
		print_uart(tx_buf);
		print_uart("\r\n");
	}
	return 0;
}

void print_uart(char *buf)
{
	int msg_len = strlen(buf);
	uart_tx(uart_dev, buf, msg_len, SYS_FOREVER_US);
}

void serial_cb_async(const struct device *dev, struct uart_event *evt, void *user_data)
{
	/* Program the callback handler to perform line buffering based on \n or \r
	*/
	switch(evt->type)
	{
		case UART_RX_RDY:
		{
			uint8_t rx_len = evt->data.rx.len;
			uint8_t rx_offset = evt->data.rx.offset;
			for(uint8_t idx = rx_offset; idx < (rx_offset + rx_len); idx++)
			{
				if(evt->data.rx.buf[idx] == '\r' || evt->data.rx.buf[idx] == '\n')
				{
					evt->data.rx.buf[idx] = '\0';
					k_msgq_put(&uart_msgq, evt->data.rx.buf, K_FOREVER);
					break;
				}
			}
		}
		break;
		case UART_TX_DONE:
		{
			/* Transmission is complete */
			printk("UART TX Done\n");
		}
		break;
		case UART_RX_STOPPED:
		{
			/* Reception stopped */
			printk("UART RX Stopped\n");
			uart_rx_enable(uart_dev, rx_buf, sizeof(rx_buf), SYS_FOREVER_US);
		}
		break;
	}
}
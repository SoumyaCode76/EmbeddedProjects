#include "bsp.h"

void uart_init(void)
{
	// Initialize UART peripheral
	// This function should configure the UART settings such as baud rate, data bits, stop bits, parity, etc.
	// The actual implementation will depend on the specific microcontroller and its UART peripheral.

	/*
	 * 1. Enable the UART clock in the microcontroller's clock control register.
	 * 2. Configure the UART pins for TX and RX functionality.
	 * 3. Set the desired baud rate by calculating the appropriate value for the UART's baud rate register.
	 * 4. Configure the UART control registers for data bits, stop bits, and parity settings.
	 * 5. Enable the UART transmitter and receiver.
	 */
	UART_PERIPH->CR1 &= ~USART_CR1_UE; // Disable UART before configuration
	RCC_UART_CLK_ENABLE(); // Enable UART clock
	UART_PIN_MUX_TX(); // Configure TX pin for UART alternate function
	UART_PIN_MUX_RX(); // Configure RX pin for UART alternate function
	UART_SET_BAUDRATE(); // Set baud rate
	UART_PERIPH->CR1 |= USART_CR1_TE | USART_CR1_RE; // Enable transmitter and receiver
	UART_PERIPH->CR1 |= USART_CR1_UE; // Enable UART
}

void uart_deinit(void)
{
	UART_PERIPH->CR1 = 0x0; // Reset UART control registers
	UART_PERIPH->CR2 = 0x0;
	UART_PERIPH->CR3 = 0x0;
	UART_PERIPH->BRR = 0x0;	// Reset Baud Rate divider value
	RCC_UART_CLK_DISABLE(); // Disable UART clock
}

void uart_send(uint8_t *data, uint16_t length)
{
	// Send data over UART
	for (uint16_t i = 0; i < length; i++)
	{
		while (!(UART_PERIPH->SR & USART_SR_TXE)); // Wait until transmit data register is empty
		UART_PERIPH->DR = data[i]; // Send data byte
	}
}

void uart_string_print(char * data)
{
	for (uint16_t i = 0 ;; i++)
	{
		if(data[i] == '\0')
		{
			break;
		}
		while (!(UART_PERIPH->SR & USART_SR_TXE)); // Wait until transmit data register is empty
		UART_PERIPH->DR = data[i]; // Send data byte
	}
}

void uart_receive(uint8_t *data, uint16_t length)
{
	// Receive data over UART
	for (uint16_t i = 0; i < length; i++)
	{
		while (!(UART_PERIPH->SR & USART_SR_RXNE)); // Wait until receive data register is not empty
		data[i] = UART_PERIPH->DR; // Read received data byte
	}
}

int __io_putchar(int ch)
{
	uart_send((uint8_t *)&ch, 1);
	return ch;
}

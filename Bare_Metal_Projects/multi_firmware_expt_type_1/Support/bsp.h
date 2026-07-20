#ifndef __BSP_H__
#define __BSP_H__

#include "common.h"
#include "stm32f4xx.h"
#include "led.h"
#include "uart.h"

/*
 * Board configuration
 * Board: STM32F401RE Nucleo / STM32F407VG Discovery / STM32F429ZI Discovery
 */

#ifdef STM32F401xE
	#define BOARD_NAME 															("STM32F401RE Nucleo")
	#define HSI_CLOCK_FREQUENCY 												(16000000) // 16 MHz for HSI clock
	#define LED_PIN 															(5)
	#define LED_PORT 															(GPIOA)
	#define UART_PERIPH															(USART2)
	#define UART_BAUDRATE 														(115200)
	#define UART_BUFFER_SIZE 													(256)
	#define RCC_UART_CLK_ENABLE()												(RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
	#define UART_TX_PIN 														(2)
	#define UART_RX_PIN 														(3)
	#define UART_TX_PORT 														(GPIOA)
	#define UART_RX_PORT 														(GPIOA)
	#define UART_TX_AF 															(0x7) // Alternate function for TX pin
	#define UART_RX_AF 															(0x7) // Alternate function for RX pin
	#define UART_PIN_MUX_TX() 													(UART_TX_PORT->AFR[UART_TX_PIN >> 3] |= (UART_TX_AF << ((UART_TX_PIN & 0x7) * 4))) \
																				UART_TX_PORT->MODER &= ~(0x3 << (UART_TX_PIN * 2)); \
																				UART_TX_PORT->MODER |= (0x2 << (UART_TX_PIN * 2));
	#define UART_PIN_MUX_RX()	 												(UART_RX_PORT->AFR[UART_RX_PIN >> 3] |= (UART_RX_AF << ((UART_RX_PIN & 0x7) * 4))) \
																				UART_RX_PORT->MODER &= ~(0x3 << (UART_RX_PIN * 2)); \
																				UART_RX_PORT->MODER |= (0x2 << (UART_RX_PIN * 2));
	#define UART_SET_BAUDRATE()													UART_PERIPH->BRR = (uint16_t)(HSI_CLOCK_FREQUENCY / UART_BAUDRATE);
#elif defined(STM32F407xx)
	#define BOARD_NAME 															"STM32F407VG Discovery"
	#define HSI_CLOCK_FREQUENCY 												(16000000) // 16 MHz for HSI clock
	#define LD3_PIN 															(13)
	#define LD3_PORT 															(GPIOD)
	#define LD4_PIN 															(12)
	#define LD4_PORT 															(GPIOD)
	#define LD5_PIN 															(14)
	#define LD5_PORT 															(GPIOD)
	#define LD6_PIN 															(15)
	#define LD6_PORT 															(GPIOD)
	enum Leds{
		LD3 = 0,
		LD4,
		LD5,
		LD6
	};
	#define UART_PERIPH															(USART2)
	#define UART_BAUDRATE 														(115200)
	#define UART_BUFFER_SIZE 													(256)
	#define UART_TX_PIN 														(2)
	#define UART_RX_PIN 														(3)
	#define UART_TX_PORT 														(GPIOA)
	#define UART_RX_PORT 														(GPIOA)
	#define RCC_UART_CLK_ENABLE()												(RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN); \
																				(RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
	#define RCC_UART_CLK_DISABLE() 												(RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN)
	#define UART_TX_AF 															(0x7) // Alternate function for TX pin
	#define UART_RX_AF 															(0x7) // Alternate function for RX pin
	#define UART_PIN_MUX_TX() 													(UART_TX_PORT->AFR[UART_TX_PIN >> 3] |= (UART_TX_AF << ((UART_TX_PIN & 0x7) * 4))); \
																				UART_TX_PORT->MODER &= ~(0x3 << (UART_TX_PIN * 2)); \
																				UART_TX_PORT->MODER |= (0x2 << (UART_TX_PIN * 2));
	#define UART_PIN_MUX_RX()	 												(UART_RX_PORT->AFR[UART_RX_PIN >> 3] |= (UART_RX_AF << ((UART_RX_PIN & 0x7) * 4))); \
																				UART_RX_PORT->MODER &= ~(0x3 << (UART_RX_PIN * 2)); \
																				UART_RX_PORT->MODER |= (0x2 << (UART_RX_PIN * 2));
	#define UART_SET_BAUDRATE()													UART_PERIPH->BRR = (uint16_t)(HSI_CLOCK_FREQUENCY / UART_BAUDRATE);

#elif defined(STM32F429xx)
	#define BOARD_NAME 															("STM32F429ZI Discovery")
	#define HSI_CLOCK_FREQUENCY 												(16000000) // 16 MHz for HSI clock
	#define LED1_PIN 															(13)
	#define LED1_PORT 															(GPIOG)
	#define LED2_PIN 															(14)
	#define LED2_PORT 															(GPIOG)
	#define UART_PERIPH															(USART1)
	#define UART_BAUDRATE 														(115200)
	#define UART_BUFFER_SIZE 													(256)
	#define RCC_UART_CLK_ENABLE()												(RCC->APB2ENR |= RCC_APB2ENR_USART1EN)
	#define UART_TX_PIN 														(9)
	#define UART_RX_PIN 														(10)
	#define UART_TX_PORT 														(GPIOA)
	#define UART_RX_PORT 														(GPIOA)
	#define UART_TX_AF 															(0x7) // Alternate function for TX pin
	#define UART_RX_AF 															(0x7) // Alternate function for RX pin
	#define UART_PIN_MUX_TX() 													\ (UART_TX_PORT->AFR[UART_TX_PIN >> 3] |= (UART_TX_AF << ((UART_TX_PIN & 0x7) * 4))) \
																				UART_TX_PORT->MODER &= ~(0x3 << (UART_TX_PIN * 2)); \
																				UART_TX_PORT->MODER |= (0x2 << (UART_TX_PIN * 2));
	#define UART_PIN_MUX_RX()	 												\ (UART_RX_PORT->AFR[UART_RX_PIN >> 3] |= (UART_RX_AF << ((UART_RX_PIN & 0x7) * 4))) \
																				UART_RX_PORT->MODER &= ~(0x3 << (UART_RX_PIN * 2)); \
																				UART_RX_PORT->MODER |= (0x2 << (UART_RX_PIN * 2));
	#define UART_SET_BAUDRATE()													\ UART_PERIPH->BRR = (uint16_t)(HSI_CLOCK_FREQUENCY / UART_BAUDRATE);
#endif

#endif // __BSP_H__

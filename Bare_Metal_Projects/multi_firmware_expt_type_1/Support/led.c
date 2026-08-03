#include "led.h"

void led_init(void)
{
	// Initialize LED GPIO pin
	// This function should configure the GPIO pin connected to the LED as an output pin.
	// The actual implementation will depend on the specific microcontroller and its GPIO peripheral.

	/*
	 * 1. Enable the GPIO clock in the microcontroller's clock control register.
	 * 2. Configure the GPIO pin mode to output.
	 * 3. Set the output type, speed, and pull-up/pull-down resistors as needed.
	 */
#ifdef STM32F401xE
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable GPIOA clock (example for GPIOA)
	GPIOA->MODER |= (0x1 << (LED_PIN * 2)); // Set LED pin to output mode
	GPIOA->OTYPER &= ~(0x1 << LED_PIN); // Set output type to push-pull
	GPIOA->OSPEEDR |= (0x3 << (LED_PIN * 2)); // Set output speed to high
	GPIOA->PUPDR &= ~(0x3 << (LED_PIN * 2)); // No pull-up, no pull-down
#elif defined(STM32F407xx)
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable GPIOD clock (example for GPIOD)

	LD3_PORT->MODER |= (0x1 << (LD3_PIN * 2)); // Set LD3 pin to output mode
	LD3_PORT->OTYPER &= ~(0x1 << LD3_PIN); // Set output type to push-pull
	LD3_PORT->OSPEEDR |= (0x3 << (LD3_PIN * 2)); // Set output speed to high
	LD3_PORT->PUPDR &= ~(0x3 << (LD3_PIN * 2)); // No pull-up, no pull-down

	LD4_PORT->MODER |= (0x1 << (LD4_PIN * 2)); // Set LED1 pin to output mode
	LD4_PORT->OTYPER &= ~(0x1 << LD4_PIN); // Set output type to push-pull
	LD4_PORT->OSPEEDR |= (0x3 << (LD4_PIN * 2)); // Set output speed to high
	LD4_PORT->PUPDR &= ~(0x3 << (LD4_PIN * 2)); // No pull-up, no pull-down

	LD5_PORT->MODER |= (0x1 << (LD5_PIN * 2)); // Set LED1 pin to output mode
	LD5_PORT->OTYPER &= ~(0x1 << LD5_PIN); // Set output type to push-pull
	LD5_PORT->OSPEEDR |= (0x3 << (LD5_PIN * 2)); // Set output speed to high
	LD5_PORT->PUPDR &= ~(0x3 << (LD5_PIN * 2)); // No pull-up, no pull-down

	LD6_PORT->MODER |= (0x1 << (LD6_PIN * 2)); // Set LED1 pin to output mode
	LD6_PORT->OTYPER &= ~(0x1 << LD6_PIN); // Set output type to push-pull
	LD6_PORT->OSPEEDR |= (0x3 << (LD6_PIN * 2)); // Set output speed to high
	LD6_PORT->PUPDR &= ~(0x3 << (LD6_PIN * 2)); // No pull-up, no pull-down
#elif defined(STM32F429xx)
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN; // Enable GPIOG clock (example for GPIOG)
	GPIOG->MODER |= (0x1 << (LED_PIN * 2)); // Set LED pin to output mode
	GPIOG->OTYPER &= ~(0x1 << LED_PIN); // Set output type to push-pull
	GPIOG->OSPEEDR |= (0x3 << (LED_PIN * 2)); // Set output speed to high
	GPIOG->PUPDR &= ~(0x3 << (LED_PIN * 2)); // No pull-up, no pull-down
#endif
}

void led_on(uint8_t led_number)
{
	// Turn on the LED
#ifdef STM32F401xE
	GPIOA->BSRR |= (0x1 << led_number); // Set the LED pin high
#elif defined(STM32F407xx)
	switch(led_number) {
		case 0:
			LD3_PORT->BSRR |= (0x1 << LD3_PIN); // Set the LD3 pin high
			break;
		case 1:
			LD4_PORT->BSRR |= (0x1 << LD4_PIN); // Set the LD4 pin high
			break;
		case 2:
			LD5_PORT->BSRR |= (0x1 << LD5_PIN); // Set the LD5 pin high
			break;
		case 3:
			LD6_PORT->BSRR |= (0x1 << LD6_PIN); // Set the LD6 pin high
			break;
		default:
			break; // Invalid LED number, do nothing
	}
#elif defined(STM32F429xx)
	LED_PORT->BSRR |= (0x1 << led_number); // Set the LED pin high

#endif
}

void led_off(uint8_t led_number)
{
	// Turn off the LED
#ifdef STM32F401xE
	GPIOA->BSRR &= ~(0x1 << (LED_PIN)); // Reset the Bit set bit to turn off the LED
	GPIOA->BSRR |= (0x1 << (LED_PIN + 16)); // Set the LED pin low
#elif defined(STM32F407xx)
	switch(led_number) {
		case 0:
			LD3_PORT->BSRR &= ~(0x1 << (LD3_PIN)); // Reset the Bit set bit to turn off the LED
			LD3_PORT->BSRR |= (0x1 << (LD3_PIN + 16)); // Set the LED1 pin low
			break;
		case 1:
			LD4_PORT->BSRR &= ~(0x1 << (LD4_PIN)); // Reset the Bit set bit to turn off the LED
			LD4_PORT->BSRR |= (0x1 << (LD4_PIN + 16)); // Set the LD4 pin low
			break;
		case 2:
			LD5_PORT->BSRR &= ~(0x1 << (LD5_PIN)); // Reset the Bit set bit to turn off the LED
			LD5_PORT->BSRR |= (0x1 << (LD5_PIN + 16)); // Set the LD5 pin low
			break;
		case 3:
			LD6_PORT->BSRR &= ~(0x1 << (LD6_PIN)); // Reset the Bit set bit to turn off the LED
			LD6_PORT->BSRR |= (0x1 << (LD6_PIN + 16)); // Set the LD6 pin low
			break;
		default:
			break; // Invalid LED number, do nothing
	}
#elif defined(STM32F429xx)
	LED_PORT->BSRR &= ~(0x1 << (LED_PIN)); // Reset the Bit set bit to turn off the LED
	LED_PORT->BSRR |= (0x1 << (LED_PIN + 16)); // Set the LED pin low
#endif
}

void led_toggle(uint8_t led_number)
{
	// Turn off the LED
#ifdef STM32F401xE
	GPIOA->BSRR &= ~(0x1 << (LED_PIN)); // Reset the Bit set bit to turn off the LED
	GPIOA->BSRR |= (0x1 << (LED_PIN + 16)); // Set the LED pin low
#elif defined(STM32F407xx)
	switch(led_number) {
		case LD3:
			LD3_PORT->ODR ^= (0x1 << LD3_PIN);
			break;
		case LD4:
			LD4_PORT->ODR ^= (0x1 << LD4_PIN);
			break;
		case LD5:
			LD5_PORT->ODR ^= (0x1 << LD5_PIN);
			break;
		case LD6:
			LD6_PORT->ODR ^= (0x1 << LD6_PIN);
			break;
		default:
			break; // Invalid LED number, do nothing
	}
#elif defined(STM32F429xx)
	LED_PORT->BSRR &= ~(0x1 << (LED_PIN)); // Reset the Bit set bit to turn off the LED
	LED_PORT->BSRR |= (0x1 << (LED_PIN + 16)); // Set the LED pin low
#endif
}



#include "stm32f4xx.h"

void SystemInit(void) {}

int main(void) {
    // Enable GPIOD clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    // Set PD12 (LED) as output
    GPIOD->MODER |= (1 << (12 * 2));
    while (1) {
        // Toggle PD12
        GPIOD->ODR ^= (1 << 12);
        for (volatile int i = 0; i < 100000; ++i); // Delay
    }
}

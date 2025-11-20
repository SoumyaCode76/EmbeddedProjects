#include "mock_stm32f4xx_hal.h"

/* Mock peripheral instances */
GPIO_TypeDef mock_GPIOA = {0};
GPIO_TypeDef mock_GPIOD = {0};
GPIO_TypeDef mock_GPIOG = {0};
RCC_TypeDef mock_RCC = {0};

void mock_reset_all_peripherals(void) {
    /* Reset all mock peripherals to zero */
    mock_GPIOA = (GPIO_TypeDef){0};
    mock_GPIOD = (GPIO_TypeDef){0};
    mock_GPIOG = (GPIO_TypeDef){0};
    mock_RCC = (RCC_TypeDef){0};
}

void mock_init_peripherals(void) {
    /* Initialize peripherals with default reset values if needed */
    mock_reset_all_peripherals();
    
    /* Set any default values that peripherals would have after reset */
    /* For now, keeping them as zeros which is typical reset state */
}
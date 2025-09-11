#include "Unity/unity.h"
#include "usart.h"
#include "stm32f1xx_hal.h"

// Mocked variables to check initialization
extern UART_HandleTypeDef huart1;

void test_uart_init_should_set_baudrate(void) {
    MX_USART1_UART_Init();
    TEST_ASSERT_EQUAL(115200, huart1.Init.BaudRate); // Adjust baudrate as configured
}

void test_uart_transmit_should_return_ok(void) {
    uint8_t data[] = "Hello";
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, data, sizeof(data), 100);
    // This test assumes HAL_UART_Transmit returns HAL_OK in your setup.
    TEST_ASSERT_EQUAL(HAL_OK, status);
}
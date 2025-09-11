#include "Unity/unity.h"

// Declare test functions
void test_uart_init_should_set_baudrate(void);
void test_uart_transmit_should_return_ok(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_uart_init_should_set_baudrate);
    RUN_TEST(test_uart_transmit_should_return_ok);
    return UNITY_END();
}
#include <gtest/gtest.h>

extern "C" {
    // Include our mocks
    #include "mocks/mock_stm32f4xx_hal.h"
    #define main_h // Prevent inclusion of real main.h
    #include "mocks/mock_main.h"
    
    // Include the source file under test
    #include "../Core/Src/gpio.c"
}

class GPIOTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset all mock peripherals before each test
        mock_init_peripherals();
    }

    void TearDown() override {
        // Clean up after each test
        mock_reset_all_peripherals();
    }
};

TEST_F(GPIOTest, MX_GPIO_Init_EnablesClocks) {
    // Arrange
    // Clocks should start disabled (0)
    EXPECT_EQ(0U, RCC->AHB1ENR & RCC_AHB1ENR_GPIOAEN);
    EXPECT_EQ(0U, RCC->AHB1ENR & RCC_AHB1ENR_GPIODEN);
    EXPECT_EQ(0U, RCC->AHB1ENR & RCC_AHB1ENR_GPIOGEN);

    // Act
    MX_GPIO_Init();

    // Assert - Check that GPIO clocks are enabled
    EXPECT_NE(0U, RCC->AHB1ENR & RCC_AHB1ENR_GPIOAEN);
    EXPECT_NE(0U, RCC->AHB1ENR & RCC_AHB1ENR_GPIODEN);
    EXPECT_NE(0U, RCC->AHB1ENR & RCC_AHB1ENR_GPIOGEN);
}

TEST_F(GPIOTest, MX_GPIO_Init_ConfiguresPD11_AsOutput) {
    // Act
    MX_GPIO_Init();

    // Assert - Check PD11 configuration
    // MODER should be set to 01 (output mode) for pin 11
    uint32_t pd11_moder = (GPIOD->MODER >> (11 * 2)) & 0x3;
    EXPECT_EQ(0x1U, pd11_moder); // 01 = Output mode

    // OTYPER should be 0 (push-pull) for pin 11
    uint32_t pd11_otyper = (GPIOD->OTYPER >> 11) & 0x1;
    EXPECT_EQ(0x0U, pd11_otyper); // 0 = Push-pull

    // OSPEEDR should be 11 (high speed) for pin 11
    uint32_t pd11_ospeedr = (GPIOD->OSPEEDR >> (11 * 2)) & 0x3;
    EXPECT_EQ(0x3U, pd11_ospeedr); // 11 = High speed

    // PUPDR should be 00 (no pull-up/down) for pin 11
    uint32_t pd11_pupdr = (GPIOD->PUPDR >> (11 * 2)) & 0x3;
    EXPECT_EQ(0x0U, pd11_pupdr); // 00 = No pull-up, no pull-down
}

TEST_F(GPIOTest, MX_GPIO_Init_ConfiguresPG13_AsOutput) {
    // Act
    MX_GPIO_Init();

    // Assert - Check PG13 configuration
    // MODER should be set to 01 (output mode) for pin 13
    uint32_t pg13_moder = (GPIOG->MODER >> (13 * 2)) & 0x3;
    EXPECT_EQ(0x1U, pg13_moder); // 01 = Output mode

    // OTYPER should be 0 (push-pull) for pin 13
    uint32_t pg13_otyper = (GPIOG->OTYPER >> 13) & 0x1;
    EXPECT_EQ(0x0U, pg13_otyper); // 0 = Push-pull

    // OSPEEDR should be 11 (high speed) for pin 13
    uint32_t pg13_ospeedr = (GPIOG->OSPEEDR >> (13 * 2)) & 0x3;
    EXPECT_EQ(0x3U, pg13_ospeedr); // 11 = High speed

    // PUPDR should be 00 (no pull-up/down) for pin 13
    uint32_t pg13_pupdr = (GPIOG->PUPDR >> (13 * 2)) & 0x3;
    EXPECT_EQ(0x0U, pg13_pupdr); // 00 = No pull-up, no pull-down
}

TEST_F(GPIOTest, MX_GPIO_Init_ConfiguresPG2_AsOutput) {
    // Act
    MX_GPIO_Init();

    // Assert - Check PG2 configuration (note: code says PG14 but actually configures PG2)
    // MODER should be set to 01 (output mode) for pin 2
    uint32_t pg2_moder = (GPIOG->MODER >> (2 * 2)) & 0x3;
    EXPECT_EQ(0x1U, pg2_moder); // 01 = Output mode

    // OTYPER should be 0 (push-pull) for pin 2
    uint32_t pg2_otyper = (GPIOG->OTYPER >> 2) & 0x1;
    EXPECT_EQ(0x0U, pg2_otyper); // 0 = Push-pull

    // OSPEEDR should be 11 (high speed) for pin 2
    uint32_t pg2_ospeedr = (GPIOG->OSPEEDR >> (2 * 2)) & 0x3;
    EXPECT_EQ(0x3U, pg2_ospeedr); // 11 = High speed

    // PUPDR should be 00 (no pull-up/down) for pin 2
    uint32_t pg2_pupdr = (GPIOG->PUPDR >> (2 * 2)) & 0x3;
    EXPECT_EQ(0x0U, pg2_pupdr); // 00 = No pull-up, no pull-down
}

TEST_F(GPIOTest, MX_GPIO_Init_DoesNotAffectOtherPins) {
    // Arrange - Set some other pins to different values
    GPIOD->MODER = 0xFFFFFFFF;  // Set all pins to some other mode
    GPIOG->MODER = 0xFFFFFFFF;  // Set all pins to some other mode
    
    // Store expected values for pins that should NOT be modified
    uint32_t expected_gpiod_moder = GPIOD->MODER;
    uint32_t expected_gpiog_moder = GPIOG->MODER;
    
    // Clear the bits that SHOULD be modified (PD11, PG13, PG2)
    expected_gpiod_moder &= ~(0x3 << (11 * 2)); // Clear PD11
    expected_gpiod_moder |= (0x1 << (11 * 2));  // Set PD11 to output
    
    expected_gpiog_moder &= ~(0x3 << (13 * 2)); // Clear PG13
    expected_gpiog_moder |= (0x1 << (13 * 2));  // Set PG13 to output
    
    expected_gpiog_moder &= ~(0x3 << (2 * 2));  // Clear PG2
    expected_gpiog_moder |= (0x1 << (2 * 2));   // Set PG2 to output

    // Act
    MX_GPIO_Init();

    // Assert - Check that only the expected pins were modified
    EXPECT_EQ(expected_gpiod_moder, GPIOD->MODER);
    EXPECT_EQ(expected_gpiog_moder, GPIOG->MODER);
}

TEST_F(GPIOTest, MX_GPIO_Init_InitialStateZero) {
    // Arrange - Start with clean state
    EXPECT_EQ(0U, GPIOD->MODER);
    EXPECT_EQ(0U, GPIOG->MODER);

    // Act
    MX_GPIO_Init();

    // Assert - Verify that initialization happened correctly from zero state
    // This test ensures the function works correctly when starting from reset state
    uint32_t pd11_moder = (GPIOD->MODER >> (11 * 2)) & 0x3;
    uint32_t pg13_moder = (GPIOG->MODER >> (13 * 2)) & 0x3;
    uint32_t pg2_moder = (GPIOG->MODER >> (2 * 2)) & 0x3;
    
    EXPECT_EQ(0x1U, pd11_moder);
    EXPECT_EQ(0x1U, pg13_moder);
    EXPECT_EQ(0x1U, pg2_moder);
}
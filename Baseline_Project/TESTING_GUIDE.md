# Unit Testing with GTest for STM32 C Code

This project demonstrates how to use Google Test (GTest) to unit test C code written for STM32 microcontrollers.

## Prerequisites

- CMake (version 3.14 or higher)
- A C++ compiler (MSVC, GCC, or Clang)
- Git (to download GoogleTest)

## 1. What is a Mock? How to Know When You Need One? How to Write a Mock?

### What is a Mock?
A **mock** is a fake implementation of a dependency that simulates the behavior of real code in controlled ways. Mocks are used to:
- Replace external dependencies during testing
- Control the behavior of functions you don't own (like HAL functions)
- Verify that your code calls the right functions with the right parameters
- Test error conditions that are hard to reproduce with real hardware

### When Do You Need a Mock?
You need a mock when your code depends on:
- **Hardware abstraction layer (HAL) functions** - like `HAL_GPIO_Init()`, `HAL_UART_Transmit()`
- **System calls** - like file I/O, network operations
- **External libraries** - third-party code you can't easily control
- **Hardware-specific functions** - that won't work in a test environment

Example: If your function calls `HAL_GPIO_WritePin()`, you need a mock because:
- The real HAL function requires actual hardware
- You want to verify your code calls it with correct parameters
- You want to test different scenarios (success/failure)

### How to Write a Mock?

#### Step 1: Create Mock Header
```c
// mocks/mock_stm32f4xx_hal.h
#ifndef MOCK_STM32F4XX_HAL_H
#define MOCK_STM32F4XX_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

// Mock the HAL types and functions
typedef enum {
    HAL_OK = 0x00U,
    HAL_ERROR = 0x01U
} HAL_StatusTypeDef;

typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    // ... other registers
} GPIO_TypeDef;

typedef struct {
    uint32_t Pin;
    uint32_t Mode;
    uint32_t Pull;
    uint32_t Speed;
} GPIO_InitTypeDef;

// Mock function declarations
HAL_StatusTypeDef HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_Init);
void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t PinState);

#ifdef __cplusplus
}
#endif

#endif
```

#### Step 2: Create Mock Implementation
```cpp
// mocks/mock_stm32f4xx_hal.cpp
#include "mock_stm32f4xx_hal.h"
#include <gtest/gtest.h>
#include <vector>
#include <tuple>

// Global variables to track mock calls
std::vector<std::tuple<GPIO_TypeDef*, GPIO_InitTypeDef>> gpio_init_calls;
std::vector<std::tuple<GPIO_TypeDef*, uint16_t, uint32_t>> gpio_write_calls;

// Mock implementations
HAL_StatusTypeDef HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_Init) {
    gpio_init_calls.push_back(std::make_tuple(GPIOx, *GPIO_Init));
    return HAL_OK;  // Always return success for simple mock
}

void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t PinState) {
    gpio_write_calls.push_back(std::make_tuple(GPIOx, GPIO_Pin, PinState));
}

// Helper functions for tests
void clear_mock_calls() {
    gpio_init_calls.clear();
    gpio_write_calls.clear();
}

size_t get_gpio_init_call_count() {
    return gpio_init_calls.size();
}
```

## 2. What is a Stub? How to Write a Stub?

### What is a Stub?
A **stub** is a minimal implementation of a function that:
- Provides a simple, predictable response
- Doesn't perform the actual work of the real function
- Returns fixed values or performs minimal logic
- Is used when you just need the function to exist, but don't care about its behavior

**Key Difference**: Mocks are about *verification* (did my code call this correctly?), stubs are about *isolation* (replace this dependency so my code runs).

### When to Use Stubs?
Use stubs when:
- You need a function to exist but don't care what it does
- You want to return specific values to test different code paths
- You want to simulate error conditions
- The real function has side effects you want to avoid

### How to Write a Stub?

#### Simple Stub Example:
```c
// In your test file or mock implementation
HAL_StatusTypeDef HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_Init) {
    // Stub: Just return success, do nothing else
    return HAL_OK;
}
```

#### Configurable Stub:
```cpp
// Global variable to control stub behavior
static HAL_StatusTypeDef stub_return_value = HAL_OK;

// Stub implementation
HAL_StatusTypeDef HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_Init) {
    return stub_return_value;
}

// Helper function to configure stub
void set_hal_gpio_init_return_value(HAL_StatusTypeDef value) {
    stub_return_value = value;
}
```

#### Stub with Simple Logic:
```cpp
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, 
                                   uint16_t Size, uint32_t Timeout) {
    // Stub: Simulate timeout if Size is too large
    if (Size > 100) {
        return HAL_TIMEOUT;
    }
    return HAL_OK;
}
```

## 3. How to Write Unit Tests? Are Stubs Included in Unit Tests?

### What is a Unit Test?
A unit test is a piece of code that:
- Tests a single "unit" of code (usually one function)
- Runs in isolation from other code
- Verifies that the function behaves correctly for various inputs
- Can be run automatically and repeatedly

### How to Write Unit Tests?

#### Basic Test Structure:
```cpp
#include <gtest/gtest.h>

extern "C" {
    #include "gpio.h"  // Your C code
    #include "mock_stm32f4xx_hal.h"  // Your mocks
}

class GpioTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset mocks before each test
        clear_mock_calls();
    }
    
    void TearDown() override {
        // Cleanup after each test if needed
    }
};

TEST_F(GpioTest, MX_GPIO_Init_CallsHAL_Functions) {
    // Arrange: Set up test conditions
    
    // Act: Call the function under test
    MX_GPIO_Init();
    
    // Assert: Verify the results
    EXPECT_EQ(get_gpio_init_call_count(), 2);  // Expect 2 GPIO pins initialized
}
```

#### Testing Different Scenarios:
```cpp
TEST_F(GpioTest, MX_GPIO_Init_ConfiguresLED_Correctly) {
    // Act
    MX_GPIO_Init();
    
    // Assert - Check that LED pin was configured correctly
    auto calls = get_gpio_init_calls();
    bool found_led_config = false;
    
    for (const auto& call : calls) {
        GPIO_TypeDef* port = std::get<0>(call);
        GPIO_InitTypeDef config = std::get<1>(call);
        
        if (port == GPIOG && config.Pin == GPIO_PIN_13) {
            EXPECT_EQ(config.Mode, GPIO_MODE_OUTPUT_PP);
            EXPECT_EQ(config.Pull, GPIO_NOPULL);
            found_led_config = true;
        }
    }
    
    EXPECT_TRUE(found_led_config) << "LED configuration not found";
}

TEST_F(GpioTest, GPIO_Toggle_LED_CallsCorrectHAL_Function) {
    // Arrange: Create a function that toggles LED
    // (You would add this function to your gpio.c)
    
    // Act
    // toggle_led();  // Your custom function
    
    // Assert
    // EXPECT_EQ(get_gpio_write_call_count(), 1);
}
```

#### Testing Error Conditions:
```cpp
TEST_F(GpioTest, HandleHAL_GPIO_Init_Failure) {
    // Arrange: Configure stub to return error
    set_hal_gpio_init_return_value(HAL_ERROR);
    
    // Act & Assert: If your function handles errors
    // EXPECT_EQ(MX_GPIO_Init(), INIT_FAILED);
}
```

### Are Stubs Included in Unit Tests?

**Yes, stubs are typically included in unit tests**, but they serve different purposes:

1. **Stubs as Dependencies**: Your unit tests will use stubs to replace real HAL functions
   ```cpp
   // This test uses stubs for HAL functions
   TEST_F(GpioTest, TestMyFunction) {
       // The HAL functions called by MX_GPIO_Init() are stubbed
       MX_GPIO_Init();  
       // Test passes because stubs provide fake HAL functions
   }
   ```

2. **Stubs for Test Setup**: You might create stubs to set up test conditions
   ```cpp
   // Stub to simulate different hardware states
   uint32_t get_system_clock_stub() {
       return 168000000;  // Always return 168 MHz
   }
   ```

3. **Mocks vs Stubs in Tests**:
   - Use **mocks** when you want to verify function calls
   - Use **stubs** when you just need functions to exist and return values

### Complete Test Example:
```cpp
// test_gpio.cpp
#include <gtest/gtest.h>

extern "C" {
    #include "gpio.h"
}

class GpioTest : public ::testing::Test {
protected:
    void SetUp() override {
        clear_mock_calls();
    }
};

TEST_F(GpioTest, MX_GPIO_Init_InitializesAllPins) {
    // Act
    MX_GPIO_Init();
    
    // Assert
    EXPECT_GT(get_gpio_init_call_count(), 0) << "No GPIO pins were initialized";
}

TEST_F(GpioTest, MX_GPIO_Init_ConfiguresUserLED) {
    // Act
    MX_GPIO_Init();
    
    // Assert - Verify LED pin configuration
    auto calls = get_gpio_init_calls();
    bool led_configured = false;
    
    for (const auto& call : calls) {
        if (is_led_pin_config(call)) {
            led_configured = true;
            break;
        }
    }
    
    EXPECT_TRUE(led_configured) << "User LED was not properly configured";
}
```

## Running Tests

### Build and Run Tests:
```powershell
# Navigate to project directory
cd d:\Users\soumy\GitHub\Learn_embedded_software_dev\EmbeddedProjects\Baseline_Project

# Build tests
cmake -B build_tests -DBUILD_TESTS=ON
cmake --build build_tests

# Run tests
cd build_tests
ctest --verbose
```

### Directory Structure

```
tests/
├── CMakeLists.txt          # Test build configuration
├── test_main.cpp           # Main test runner
├── test_gpio.cpp           # GPIO function tests
└── mocks/                  # Mock implementations
    ├── mock_stm32f4xx_hal.h
    ├── mock_stm32f4xx_hal.cpp
    └── mock_main.h
```

# Configure with tests enabled
cmake .. -DBUILD_TESTS=ON

# Build
cmake --build . --config Debug

# Run tests
ctest --output-on-failure --verbose
```

## Writing New Tests

### 1. Create Test Files

Test files should be named `test_<module>.cpp` and placed in the `tests/` directory.

Example structure:
```cpp
#include <gtest/gtest.h>

extern "C" {
    #include "mocks/mock_stm32f4xx_hal.h"
    #include "mocks/mock_main.h"
    
    // Include your C source file
    #include "../Core/Src/your_module.c"
}

class YourModuleTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_init_peripherals();
    }
    
    void TearDown() override {
        mock_reset_all_peripherals();
    }
};

TEST_F(YourModuleTest, YourFunction_DoesExpectedBehavior) {
    // Arrange
    
    // Act
    
    // Assert
}
```

### 2. Create Mocks

For hardware-dependent code, create mock implementations:

- **Headers**: Define the same interface as the real hardware
- **Implementation**: Provide controllable behavior for testing
- **Reset functions**: Clean state between tests

### 3. Test Categories

#### **State Testing**
Verify that functions correctly modify register states:
```cpp
TEST_F(GPIOTest, ConfiguresPin_SetsCorrectMode) {
    configure_pin_as_output(GPIOA, 5);
    
    uint32_t mode = (GPIOA->MODER >> (5 * 2)) & 0x3;
    EXPECT_EQ(0x1U, mode); // 01 = Output mode
}
```

#### **Clock Testing**
Verify that clock enables are called:
```cpp
TEST_F(GPIOTest, Init_EnablesRequiredClocks) {
    MX_GPIO_Init();
    
    EXPECT_NE(0U, RCC->AHB1ENR & RCC_AHB1ENR_GPIOAEN);
}
```

#### **Isolation Testing**
Verify functions don't affect unintended registers:
```cpp
TEST_F(GPIOTest, ConfigurePin_DoesNotAffectOtherPins) {
    GPIOA->MODER = 0xFFFFFFFF;  // Set all pins
    
    configure_pin_as_output(GPIOA, 5);
    
    // Check that only pin 5 was modified
    // ... verification logic
}
```

## Best Practices

### Testing C Code with GTest

1. **Use `extern "C"`** blocks to properly link C code with C++ test framework
2. **Mock hardware dependencies** - Don't test the actual hardware
3. **Test one function at a time** - Keep tests focused
4. **Use descriptive test names** - `FunctionName_Scenario_ExpectedResult`
5. **Reset state between tests** - Use SetUp() and TearDown()

### Mock Design

1. **Match the real interface** - Same function signatures and register layouts
2. **Provide reset functions** - Allow clean state for each test
3. **Keep it simple** - Only implement what you need to test
4. **Make it observable** - Allow tests to verify behavior

### Test Structure

1. **Arrange** - Set up test conditions
2. **Act** - Call the function under test
3. **Assert** - Verify the expected behavior

## Example: GPIO Tests

The provided `test_gpio.cpp` demonstrates testing the `MX_GPIO_Init()` function:

- **Clock enable verification** - Ensures GPIO clocks are enabled
- **Pin configuration testing** - Verifies each pin is configured correctly
- **Register isolation** - Ensures only intended registers are modified
- **Initial state testing** - Verifies behavior from reset state

## Extending the Framework

To add tests for other modules:

1. Create `test_<module>.cpp`
2. Add any needed mocks to `mocks/` directory
3. Update `tests/CMakeLists.txt` to include new test file
4. Run tests to verify everything works

The framework is designed to be extensible and maintainable as your project grows.
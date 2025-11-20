# Mock & Stub Testing Tips and Tricks Guide

This guide provides practical tips and tricks for quickly creating mocks, stubs, and achieving comprehensive test coverage for embedded C code.

## 🚀 Quick Mock & Stub Preparation Tips

### 1. **Dependencies Analysis - Find What Needs Mocking**

Use these commands to quickly identify dependencies:

```bash
# Find all HAL function calls
grep -r "HAL_\|__HAL_" Core/Src/your_file.c

# Find all STM32 includes  
grep -r "#include.*stm32" Core/Src/your_file.c

# Find peripheral register accesses
grep -r "GPIO\|RCC\|USART\|TIM" Core/Src/your_file.c
```

**Rule of Thumb:** If it starts with `HAL_`, `__HAL_`, or accesses hardware registers, you need a mock/stub!

### 2. **Copy-Paste-Modify Strategy**

Don't write mocks from scratch! Follow this progression:

#### Phase 1: Make It Compile (Simplest Stub)
```c
// Just return success for everything
#define HAL_GPIO_Init(x,y) HAL_OK
#define HAL_UART_Transmit(a,b,c,d) HAL_OK
```

#### Phase 2: Add Basic Tracking
```cpp
// Track function calls
static int gpio_init_call_count = 0;
HAL_StatusTypeDef HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init) {
    gpio_init_call_count++;
    return HAL_OK;
}

// Helper for tests
int get_gpio_init_call_count(void) { return gpio_init_call_count; }
void reset_gpio_init_call_count(void) { gpio_init_call_count = 0; }
```

#### Phase 3: Add Full Register Simulation (Only If Needed)
```cpp
// Full register manipulation
HAL_StatusTypeDef HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init) {
    // Simulate real register operations
    GPIOx->MODER   |= (GPIO_Init->Mode << (GPIO_Init->Pin * 2));
    GPIOx->OSPEEDR |= (GPIO_Init->Speed << (GPIO_Init->Pin * 2));
    return HAL_OK;
}
```

### 3. **Register Mock Template**

Use this template for any 32-bit peripheral:

```cpp
// Generic peripheral template
typedef struct {
    volatile uint32_t REG1;      // Add registers as you discover them
    volatile uint32_t REG2;      
    volatile uint32_t REG3;
    // Add more as needed
} PERIPHERAL_TypeDef;

// Create mock instance
PERIPHERAL_TypeDef mock_PERIPHERAL = {0};

// Redirect hardware pointer to mock
#define PERIPHERAL (&mock_PERIPHERAL)

// Reset function for clean tests
void reset_PERIPHERAL_mock(void) {
    mock_PERIPHERAL = (PERIPHERAL_TypeDef){0};
}

// Bit manipulation helpers
#define SET_BIT(reg, bit)     ((reg) |= (bit))
#define CLEAR_BIT(reg, bit)   ((reg) &= ~(bit))
#define READ_BIT(reg, bit)    ((reg) & (bit))
```

### 4. **Incremental Mocking Workflow**

**Start minimal and expand as needed:**

```cpp
// Day 1: Just make tests compile
#define HAL_GPIO_WritePin(port, pin, state) /* do nothing */

// Day 2: Add return values  
HAL_StatusTypeDef HAL_GPIO_WritePin(GPIO_TypeDef* port, uint16_t pin, uint32_t state) {
    return HAL_OK;
}

// Day 3: Add call verification
static struct { GPIO_TypeDef* port; uint16_t pin; uint32_t state; } last_write;
HAL_StatusTypeDef HAL_GPIO_WritePin(GPIO_TypeDef* port, uint16_t pin, uint32_t state) {
    last_write.port = port; last_write.pin = pin; last_write.state = state;
    return HAL_OK;
}

// Day 4: Add full register simulation (if needed for your tests)
HAL_StatusTypeDef HAL_GPIO_WritePin(GPIO_TypeDef* port, uint16_t pin, uint32_t state) {
    if (state) {
        port->BSRR = pin;        // Set bit
        port->ODR |= pin;
    } else {
        port->BSRR = pin << 16;  // Reset bit  
        port->ODR &= ~pin;
    }
    return HAL_OK;
}
```

### 5. **Mock Generation Script**

Create a PowerShell script to generate boilerplate:

```powershell
# create_mock.ps1
param(
    [string]$PeripheralName,
    [string[]]$RegisterList = @("REG1", "REG2", "REG3")
)

$headerContent = @"
#ifndef MOCK_${PeripheralName}_H  
#define MOCK_${PeripheralName}_H

#include <stdint.h>

typedef struct {
$(foreach($reg in $RegisterList) { "    volatile uint32_t $reg;" })
} ${PeripheralName}_TypeDef;

extern ${PeripheralName}_TypeDef mock_$PeripheralName;
#define $PeripheralName (&mock_$PeripheralName)

void reset_${PeripheralName}_mock(void);

#endif
"@

$sourceContent = @"
#include "mock_$PeripheralName.h"

${PeripheralName}_TypeDef mock_$PeripheralName = {0};

void reset_${PeripheralName}_mock(void) {
    mock_$PeripheralName = (${PeripheralName}_TypeDef){0};
}
"@

$headerContent | Out-File "mock_$PeripheralName.h" -Encoding UTF8
$sourceContent | Out-File "mock_$PeripheralName.c" -Encoding UTF8

Write-Host "Generated mock_$PeripheralName.h and mock_$PeripheralName.c"
```

Usage:
```powershell
.\create_mock.ps1 -PeripheralName "TIMER" -RegisterList @("CNT", "ARR", "CCR1", "CCR2")
```

## 📊 Statement & Decision Coverage Tips

### **Statement Coverage** - Every Line Must Execute

#### Tip 1: Use Coverage Tools

```cmake
# Add to your test CMakeLists.txt
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(gpio_tests PRIVATE --coverage)
    target_link_libraries(gpio_tests --coverage)
endif()
```

```powershell
# Generate coverage report
cmake --build build --config Debug
.\build\gpio_tests.exe
gcov Core/Src/gpio.c
# or for HTML report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

#### Tip 2: Follow the Code Flow Systematically

For any function like this:
```c
void configure_led(int state, int brightness) {
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);  // Line 1
    
    if (state > 0) {                                        // Line 2 - Decision
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);// Line 3
        
        if (brightness > 50) {                              // Line 4 - Nested decision
            set_pwm_duty(brightness);                       // Line 5
        }
    }
    
    HAL_Delay(100);                                         // Line 6
}
```

**Statement coverage test matrix:**
```cpp
class LEDTest : public ::testing::Test {
protected:
    void SetUp() override { reset_all_mocks(); }
};

TEST_F(LEDTest, StatementCoverage_AllPaths) {
    // Path 1: state > 0, brightness > 50 (covers lines 1,2,3,4,5,6)
    configure_led(1, 75);   
    
    // Path 2: state > 0, brightness <= 50 (covers lines 1,2,3,4,6)  
    configure_led(1, 25);   
    
    // Path 3: state <= 0 (covers lines 1,2,6)
    configure_led(0, 75);   
    
    // All lines now covered!
}
```

### **Decision Coverage** - Every Branch Must Be Taken

#### Tip 3: Truth Table Approach

For complex conditions, create a systematic truth table:

```c
if (sensor_active && (temperature > threshold || emergency_mode)) {
    activate_cooling();
} else {
    deactivate_cooling();  
}
```

**Decision coverage truth table:**
| sensor_active | temp > threshold | emergency_mode | Result | Test Case |
|---------------|------------------|----------------|---------|-----------|
| true          | true             | true           | TRUE    | Test1     |
| true          | true             | false          | TRUE    | Test2     |
| true          | false            | true           | TRUE    | Test3     |
| true          | false            | false          | FALSE   | Test4     |
| false         | true             | true           | FALSE   | Test5     |
| false         | true             | false          | FALSE   | Test6     |
| false         | false            | true           | FALSE   | Test7     |
| false         | false            | false          | FALSE   | Test8     |

```cpp
TEST_F(CoolingTest, DecisionCoverage_ComplexCondition) {
    // TRUE cases
    test_cooling(true, 85, true);    // T && (T || T) = T
    test_cooling(true, 85, false);   // T && (T || F) = T  
    test_cooling(true, 15, true);    // T && (F || T) = T
    
    // FALSE cases  
    test_cooling(true, 15, false);   // T && (F || F) = F
    test_cooling(false, 85, true);   // F && (T || T) = F
    test_cooling(false, 85, false);  // F && (T || F) = F
    test_cooling(false, 15, true);   // F && (F || T) = F
    test_cooling(false, 15, false);  // F && (F || F) = F
}
```

#### Tip 4: Boundary Value Testing

Always test boundary conditions:

```c
if (value > 100) {
    return HIGH_RANGE;
} else if (value > 50) {
    return MEDIUM_RANGE;  
} else if (value >= 0) {
    return LOW_RANGE;
} else {
    return ERROR_RANGE;
}
```

**Boundary test cases:**
```cpp
TEST_F(RangeTest, BoundaryValues) {
    // Test boundaries and just inside/outside
    EXPECT_EQ(HIGH_RANGE, classify_value(101));    // > 100
    EXPECT_EQ(MEDIUM_RANGE, classify_value(100));  // = 100  
    EXPECT_EQ(MEDIUM_RANGE, classify_value(51));   // > 50
    EXPECT_EQ(LOW_RANGE, classify_value(50));      // = 50
    EXPECT_EQ(LOW_RANGE, classify_value(1));       // > 0
    EXPECT_EQ(LOW_RANGE, classify_value(0));       // = 0
    EXPECT_EQ(ERROR_RANGE, classify_value(-1));    // < 0
}
```

### **Modified Condition/Decision Coverage (MC/DC)**

For safety-critical code, test that each condition independently affects the outcome:

```c
if (a && b || c) {
    return true;
} else {
    return false;
}
```

**MC/DC test pairs:**
```cpp
TEST_F(MCDCTest, ConditionA_Independence) {
    // Show 'a' independently affects outcome (b=T, c=F)
    EXPECT_TRUE(test_function(true, true, false));   // a=T causes TRUE
    EXPECT_FALSE(test_function(false, true, false)); // a=F causes FALSE
}

TEST_F(MCDCTest, ConditionB_Independence) { 
    // Show 'b' independently affects outcome (a=T, c=F)
    EXPECT_TRUE(test_function(true, true, false));   // b=T causes TRUE  
    EXPECT_FALSE(test_function(true, false, false)); // b=F causes FALSE
}

TEST_F(MCDCTest, ConditionC_Independence) {
    // Show 'c' independently affects outcome (a=F, b=F) 
    EXPECT_TRUE(test_function(false, false, true));  // c=T causes TRUE
    EXPECT_FALSE(test_function(false, false, false));// c=F causes FALSE
}
```

## 🎯 Advanced Testing Techniques

### **Error Path Coverage**

Don't forget to test error conditions:

```cpp
TEST_F(ErrorTest, HAL_Function_Failures) {
    // Configure mocks to return errors
    set_hal_gpio_init_return_value(HAL_ERROR);
    set_hal_uart_transmit_return_value(HAL_TIMEOUT);
    
    // Test that your code handles these errors correctly
    EXPECT_EQ(INIT_FAILED, initialize_system());
    EXPECT_EQ(COMM_ERROR, send_message("test"));
}

TEST_F(ErrorTest, OutOfMemory_Conditions) {
    // Simulate resource exhaustion
    set_malloc_to_fail_after_n_calls(5);
    
    EXPECT_EQ(MEMORY_ERROR, allocate_buffers());
}
```

### **State Machine Testing**

For state machines, test all transitions:

```cpp
TEST_F(StateMachineTest, AllStateTransitions) {
    // Test each valid transition
    EXPECT_EQ(IDLE, get_state());
    
    send_event(START_EVENT);
    EXPECT_EQ(RUNNING, get_state());
    
    send_event(STOP_EVENT);  
    EXPECT_EQ(IDLE, get_state());
    
    // Test invalid transitions
    send_event(STOP_EVENT); // Already in IDLE
    EXPECT_EQ(IDLE, get_state()); // Should stay in IDLE
}
```

### **Timing-Dependent Code Testing**

Mock time-related functions:

```cpp
// Mock HAL_GetTick()
static uint32_t mock_tick_value = 0;
uint32_t HAL_GetTick(void) {
    return mock_tick_value;
}

void set_mock_tick(uint32_t value) {
    mock_tick_value = value;
}

TEST_F(TimingTest, Timeout_Behavior) {
    set_mock_tick(0);
    start_operation();
    
    set_mock_tick(999);  // Just before timeout
    EXPECT_EQ(RUNNING, get_operation_status());
    
    set_mock_tick(1000); // At timeout  
    EXPECT_EQ(TIMEOUT, get_operation_status());
}
```

## 🛠️ Practical Workflow & Automation

### **Step-by-Step Coverage Process**

1. **Write basic smoke test** (just call the function)
2. **Run with coverage** to see what's missed  
3. **Add tests for missed branches**
4. **Repeat until target coverage reached**

```cpp
// Step 1: Smoke test (maybe 40% coverage)
TEST_F(GPIOTest, SmokeTest) {
    EXPECT_NO_THROW(MX_GPIO_Init());
}

// Step 2: Check coverage, add missing paths (70% coverage)
TEST_F(GPIOTest, ErrorHandling) {
    set_hal_gpio_init_return_value(HAL_ERROR);
    EXPECT_EQ(GPIO_INIT_FAILED, MX_GPIO_Init());
}

// Step 3: Add edge cases (90% coverage) 
TEST_F(GPIOTest, EdgeCases) {
    // Test any remaining uncovered lines
    test_gpio_with_null_pointer();
    test_gpio_with_invalid_pin();
}

// Step 4: Reach 100% with corner cases
TEST_F(GPIOTest, CornerCases) {
    // Test the last few uncovered lines
}
```

### **Automated Coverage Checking**

Add to your CMakeLists.txt:

```cmake
# Coverage target
find_program(GCOV_PATH gcov)
find_program(LCOV_PATH lcov)  
find_program(GENHTML_PATH genhtml)

if(GCOV_PATH AND LCOV_PATH AND GENHTML_PATH)
    add_custom_target(coverage
        # Run tests
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/gpio_tests
        
        # Generate coverage data
        COMMAND ${LCOV_PATH} --capture --directory . --output-file coverage.info
        COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' --output-file coverage.info
        COMMAND ${LCOV_PATH} --remove coverage.info '*/external/*' --output-file coverage.info
        
        # Generate HTML report
        COMMAND ${GENHTML_PATH} coverage.info --output-directory coverage_html
        
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating coverage report"
    )
endif()
```

### **Continuous Integration Setup**

```yaml
# .github/workflows/test.yml
name: Unit Tests
on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
      with:
        submodules: recursive
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake gcc-arm-none-eabi lcov
    
    - name: Build and test
      run: |
        cmake -B build -DBUILD_TESTS=ON
        cmake --build build
        cd build && ctest --verbose
        
    - name: Generate coverage
      run: |
        cd build
        make coverage
        
    - name: Upload coverage  
      uses: codecov/codecov-action@v1
      with:
        file: ./build/coverage.info
```

## 🔍 Debugging Coverage Issues

### **Finding Uncovered Code**

```bash
# Show uncovered lines with line numbers
gcov -b -c gpio.c | grep "#####"

# Generate detailed HTML report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html --show-details
```

### **Common Coverage Gaps & Solutions**

| Gap Type | Example | Solution |
|----------|---------|----------|
| Error paths | `if (ptr == NULL) return ERROR;` | Test with NULL pointer |
| Default cases | `default: break;` | Add test for unexpected enum value |
| Early returns | `if (ready) return OK;` | Test both ready and not-ready |
| Cleanup code | `cleanup: free(buffer);` | Test error paths that jump to cleanup |
| Unreachable code | Code after `while(1)` | Remove or add break condition |

### **Coverage Quality Checks**

```cpp
// Good: Actually tests the logic
TEST_F(GPIOTest, LED_TurnsOn_When_Requested) {
    turn_on_led();
    EXPECT_TRUE(is_led_on());
}

// Bad: Just executes code without verification
TEST_F(GPIOTest, LED_Function_Executes) {
    turn_on_led(); // No assertions!
}

// Better: Test behavior, not just execution
TEST_F(GPIOTest, LED_GPIO_Pin_Set_When_TurnedOn) {
    turn_on_led();
    EXPECT_EQ(GPIO_PIN_SET, get_last_gpio_write_state());
}
```

## 📋 Quick Reference Checklist

### **Mock Creation Checklist**
- [ ] Identify all HAL/hardware dependencies
- [ ] Create minimal stubs first  
- [ ] Add call tracking if needed
- [ ] Add register simulation if testing register values
- [ ] Create reset/cleanup functions
- [ ] Add error injection capabilities

### **Coverage Checklist**  
- [ ] Statement coverage: All lines execute
- [ ] Decision coverage: All branches taken
- [ ] Condition coverage: All boolean sub-expressions tested
- [ ] Error path coverage: All error conditions tested
- [ ] Boundary value coverage: All edge cases tested
- [ ] State transition coverage: All state changes tested

### **Test Quality Checklist**
- [ ] Each test has clear Arrange-Act-Assert structure
- [ ] Tests are independent (can run in any order)
- [ ] Mock state is reset between tests
- [ ] Tests verify behavior, not just execution
- [ ] Error cases are tested
- [ ] Boundary conditions are tested
- [ ] Tests have descriptive names

Remember: **Start simple, iterate quickly, and focus on testing behavior rather than just achieving coverage numbers!**
cmake_minimum_required(VERSION 3.14)

# Set up GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/googletest
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include directories for tests
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

# Create mock library
add_library(mocks
    mocks/mock_stm32f4xx_hal.cpp
)

target_include_directories(mocks PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

# Define test executable
add_executable(
    gpio_tests
    test_main.cpp
    test_gpio.cpp
)

# Link test executable with GoogleTest and mocks
target_link_libraries(
    gpio_tests
    gtest_main
    gtest
    mocks
)

# Add compiler definitions to avoid conflicts with STM32 HAL
target_compile_definitions(gpio_tests PRIVATE
    -DUNIT_TESTING=1
)

# Enable C++14 for GoogleTest
target_compile_features(gpio_tests PRIVATE cxx_std_14)

# Add test discovery
include(GoogleTest)
gtest_discover_tests(gpio_tests)

# Add coverage target
add_custom_target(coverage
    COMMAND gcov ${CMAKE_SOURCE_DIR}/Core/Src/*.c
    COMMAND lcov --capture --directory . --output-file coverage.info
    COMMAND genhtml coverage.info --output-directory coverage_report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
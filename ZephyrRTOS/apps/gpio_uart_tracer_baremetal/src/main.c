/*
 * GPIO UART Tracer - Bare-Metal STM32 HAL Example
 * 
 * Pure bare-metal implementation without Zephyr RTOS.
 * Reads GPIO pin and transmits state over UART for visualization.
 * 
 * Target: STM32F429 (adaptable to other STM32 families)
 */

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

/* GPIO Configuration */
#define BUTTON_PIN       GPIO_PIN_0
#define BUTTON_PORT      GPIOA
#define BUTTON_CLK()     __HAL_RCC_GPIOA_CLK_ENABLE()

#define LED_PIN          GPIO_PIN_13
#define LED_PORT         GPIOG
#define LED_CLK()        __HAL_RCC_GPIOG_CLK_ENABLE()

/* UART Configuration */
#define UART_INSTANCE    USART1
#define UART_BAUDRATE    115200
#define UART_TX_PIN      GPIO_PIN_9
#define UART_RX_PIN      GPIO_PIN_10
#define UART_GPIO_PORT   GPIOA
#define UART_GPIO_AF     GPIO_AF7_USART1
#define UART_CLK()       __HAL_RCC_USART1_CLK_ENABLE()

/* Sampling configuration */
#define SAMPLE_INTERVAL_MS  100

/* Output format selection */
#define OUTPUT_FORMAT_SIMPLE    1
#define OUTPUT_FORMAT_LABELED   2
#define OUTPUT_FORMAT_VERBOSE   3
#define OUTPUT_FORMAT OUTPUT_FORMAT_LABELED

/* Global handles */
UART_HandleTypeDef huart;

/* Function prototypes */
void SystemClock_Config(void);
void GPIO_Init(void);
void UART_Init(void);
void UART_Print(const char* str);
void delay_ms(uint32_t ms);

int main(void)
{
    GPIO_PinState pin_state;
    uint32_t sample_count = 0;
    char buffer[64];

    /* Initialize HAL */
    HAL_Init();

    /* Configure system clock */
    SystemClock_Config();

    /* Initialize peripherals */
    GPIO_Init();
    UART_Init();

    /* Startup message */
    UART_Print("\r\n");
    UART_Print("========================================\r\n");
    UART_Print("  GPIO UART Tracer - Bare-Metal STM32\r\n");
    UART_Print("========================================\r\n");
    sprintf(buffer, "Sample Rate: %d ms\r\n", SAMPLE_INTERVAL_MS);
    UART_Print(buffer);
    UART_Print("\r\n");
    UART_Print("Starting GPIO monitoring...\r\n");
    UART_Print("Press button to see state changes\r\n\r\n");

    /* Main loop */
    while (1)
    {
        /* Read button state */
        pin_state = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN);

        /* Mirror state to LED (visual feedback) */
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, pin_state);

        /* Format and transmit state */
        #if OUTPUT_FORMAT == OUTPUT_FORMAT_SIMPLE
            /* Simple format: "0\n" or "1\n" */
            sprintf(buffer, "%d\r\n", pin_state);
            UART_Print(buffer);

        #elif OUTPUT_FORMAT == OUTPUT_FORMAT_LABELED
            /* Labeled format: "GPIO=1\n" */
            sprintf(buffer, "GPIO=%d\r\n", pin_state);
            UART_Print(buffer);

        #elif OUTPUT_FORMAT == OUTPUT_FORMAT_VERBOSE
            /* Verbose format */
            sprintf(buffer, "Button: %s\r\n", pin_state ? "HIGH" : "LOW");
            UART_Print(buffer);
        #endif

        sample_count++;

        /* Periodic statistics */
        if (sample_count % 50 == 0) {
            sprintf(buffer, "--- Sample #%lu, State=%d ---\r\n", sample_count, pin_state);
            UART_Print(buffer);
        }

        /* Wait before next sample */
        HAL_Delay(SAMPLE_INTERVAL_MS);
    }
}

/**
 * @brief Initialize GPIO pins
 */
void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIO clocks */
    BUTTON_CLK();
    LED_CLK();

    /* Configure button pin as input */
    GPIO_InitStruct.Pin = BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;  // or GPIO_PULLDOWN if needed
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);

    /* Configure LED pin as output */
    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

    /* Set LED to initial state (OFF) */
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
}

/**
 * @brief Initialize UART
 */
void UART_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    UART_CLK();

    /* Configure UART GPIO pins */
    GPIO_InitStruct.Pin = UART_TX_PIN | UART_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = UART_GPIO_AF;
    HAL_GPIO_Init(UART_GPIO_PORT, &GPIO_InitStruct);

    /* Configure UART parameters */
    huart.Instance = UART_INSTANCE;
    huart.Init.BaudRate = UART_BAUDRATE;
    huart.Init.WordLength = UART_WORDLENGTH_8B;
    huart.Init.StopBits = UART_STOPBITS_1;
    huart.Init.Parity = UART_PARITY_NONE;
    huart.Init.Mode = UART_MODE_TX_RX;
    huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart) != HAL_OK) {
        /* Initialization Error */
        Error_Handler();
    }
}

/**
 * @brief Transmit string over UART
 */
void UART_Print(const char* str)
{
    HAL_UART_Transmit(&huart, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

/**
 * @brief System Clock Configuration (180 MHz for STM32F429)
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Configure the main internal regulator output voltage */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Initialize RCC Oscillators */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 360;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /* Activate Over-Drive mode */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
        Error_Handler();
    }

    /* Initialize CPU, AHB and APB clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief Error handler
 */
void Error_Handler(void)
{
    /* Infinite loop on error */
    while (1) {
        /* Blink LED rapidly to indicate error */
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        HAL_Delay(100);
    }
}

/* Required for HAL */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

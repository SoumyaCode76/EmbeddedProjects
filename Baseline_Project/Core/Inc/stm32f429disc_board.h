#ifndef __STM32F429DISC_BOARD_H
#define __STM32F429DISC_BOARD_H

#include <stdint.h>
#include <stm32f4xx.h>
#include "usart.h"
#include "gpio.h"

#define USER_VECT_TAB_ADDRESS
#ifdef __FPU_PRESENT
#undef __FPU_PRESENT
#endif
#ifdef __FPU_USED
#undef __FPU_USED
#endif
#define __FPU_PRESENT               (1)
#define __FPU_USED                  (1)

typedef struct GPIO_Config
{
    GPIO_TypeDef* GPIO_Port;
    uint16_t GPIO_Pin;
    uint32_t GPIO_Mode;
    uint32_t GPIO_Pull;
    uint32_t GPIO_Speed;
} GPIO_Config_t;

typedef struct UART_Config
{
    USART_TypeDef* USART_Instance;
    uint32_t BaudRate;
    uint32_t WordLength;
    uint32_t StopBits;
    uint32_t Parity;
    uint32_t Mode;
} UART_Config_t;

typedef struct Board_Config
{
    GPIO_Config_t *GREEN_LED;
    GPIO_Config_t *RED_LED;
    UART_Config_t *STLINK_UART_CHANNEL;
    struct Board_Config (*Board_Init)(struct Board_Config* self);
    struct Board_Config (*Board_DeInit)(struct Board_Config* self);
    struct Board_Config (*Board_ConfigureLED)(struct Board_Config* self, GPIO_TypeDef* port, uint16_t pin);
    struct Board_Config (*Board_ConfigureUART)(struct Board_Config* self, USART_TypeDef* instance, uint32_t baudrate);
} Board_Config_t;

typedef struct Board_Config_Builder
{
    Board_Config_t board_config;
} Board_Config_Builder_t;

Board_Config_Builder_t Board_Config_Builder_Init(Board_Config_Builder_t * self);
Board_Config_Builder_t Board_Config_Builder_SetLED(Board_Config_Builder_t * self, GPIO_TypeDef* port, uint16_t pin);
/* void Board_Config_Builder_SetLED2(Board_Config_Builder_t* builder, GPIO_TypeDef* port, uint16_t pin);
void Board_Config_Builder_SetLED3(Board_Config_Builder_t* builder, GPIO_TypeDef* port, uint16_t pin);
void Board_Config_Builder_SetUART(Board_Config_Builder_t* builder, USART_TypeDef* instance, uint32_t baudrate); */
Board_Config_t Board_Config_Builder_Build(Board_Config_Builder_t* builder);

extern uint32_t _sisr_vector;

#endif /* __STM32F429DISC_BOARD_H */
#ifndef MOCK_STM32F4XX_HAL_H
#define MOCK_STM32F4XX_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Mock GPIO Register Structure */
typedef struct {
    volatile uint32_t MODER;    /*!< GPIO port mode register               */
    volatile uint32_t OTYPER;   /*!< GPIO port output type register        */
    volatile uint32_t OSPEEDR;  /*!< GPIO port output speed register       */
    volatile uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register  */
    volatile uint32_t IDR;      /*!< GPIO port input data register         */
    volatile uint32_t ODR;      /*!< GPIO port output data register        */
    volatile uint32_t BSRR;     /*!< GPIO port bit set/reset register      */
    volatile uint32_t LCKR;     /*!< GPIO port configuration lock register */
    volatile uint32_t AFR[2];   /*!< GPIO alternate function registers     */
} GPIO_TypeDef;

/* Mock RCC Register Structure */
typedef struct {
    volatile uint32_t CR;            /*!< RCC clock control register */
    volatile uint32_t PLLCFGR;       /*!< RCC PLL configuration register */
    volatile uint32_t CFGR;          /*!< RCC clock configuration register */
    volatile uint32_t CIR;           /*!< RCC clock interrupt register */
    volatile uint32_t AHB1RSTR;      /*!< RCC AHB1 peripheral reset register */
    volatile uint32_t AHB2RSTR;      /*!< RCC AHB2 peripheral reset register */
    volatile uint32_t AHB3RSTR;      /*!< RCC AHB3 peripheral reset register */
    uint32_t      RESERVED0;         /*!< Reserved, 0x1C */
    volatile uint32_t APB1RSTR;      /*!< RCC APB1 peripheral reset register */
    volatile uint32_t APB2RSTR;      /*!< RCC APB2 peripheral reset register */
    uint32_t      RESERVED1[2];      /*!< Reserved, 0x28-0x2C */
    volatile uint32_t AHB1ENR;       /*!< RCC AHB1 peripheral clock register */
    volatile uint32_t AHB2ENR;       /*!< RCC AHB2 peripheral clock enable register */
    volatile uint32_t AHB3ENR;       /*!< RCC AHB3 peripheral clock enable register */
    uint32_t      RESERVED2;         /*!< Reserved, 0x3C */
    volatile uint32_t APB1ENR;       /*!< RCC APB1 peripheral clock enable register */
    volatile uint32_t APB2ENR;       /*!< RCC APB2 peripheral clock enable register */
} RCC_TypeDef;

/* Mock peripheral instances */
extern GPIO_TypeDef mock_GPIOA;
extern GPIO_TypeDef mock_GPIOD;
extern GPIO_TypeDef mock_GPIOG;
extern RCC_TypeDef mock_RCC;

/* Redefine peripheral pointers for testing */
#define GPIOA (&mock_GPIOA)
#define GPIOD (&mock_GPIOD)
#define GPIOG (&mock_GPIOG)
#define RCC   (&mock_RCC)

/* RCC AHB1 peripheral clock enable register bit definitions */
#define RCC_AHB1ENR_GPIOAEN_Pos          (0U)
#define RCC_AHB1ENR_GPIOAEN_Msk          (0x1UL << RCC_AHB1ENR_GPIOAEN_Pos)
#define RCC_AHB1ENR_GPIOAEN              RCC_AHB1ENR_GPIOAEN_Msk
#define RCC_AHB1ENR_GPIODEN_Pos          (3U)
#define RCC_AHB1ENR_GPIODEN_Msk          (0x1UL << RCC_AHB1ENR_GPIODEN_Pos)
#define RCC_AHB1ENR_GPIODEN              RCC_AHB1ENR_GPIODEN_Msk
#define RCC_AHB1ENR_GPIOGEN_Pos          (6U)
#define RCC_AHB1ENR_GPIOGEN_Msk          (0x1UL << RCC_AHB1ENR_GPIOGEN_Pos)
#define RCC_AHB1ENR_GPIOGEN              RCC_AHB1ENR_GPIOGEN_Msk

/* Mock HAL macros */
#define __HAL_RCC_GPIOA_CLK_ENABLE()   do { \
                                        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; \
                                        } while(0U)

#define __HAL_RCC_GPIOD_CLK_ENABLE()   do { \
                                        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; \
                                        } while(0U)

#define __HAL_RCC_GPIOG_CLK_ENABLE()   do { \
                                        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN; \
                                        } while(0U)

/* Test helper functions */
void mock_reset_all_peripherals(void);
void mock_init_peripherals(void);

#ifdef __cplusplus
}
#endif

#endif /* MOCK_STM32F4XX_HAL_H */
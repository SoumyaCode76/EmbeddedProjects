#ifndef __STM32F429DISC_BOARD_H
#define __STM32F429DISC_BOARD_H

#include <stdint.h>
#include <stm32f4xx.h>

#define USER_VECT_TAB_ADDRESS
#ifdef __FPU_PRESENT
#undef __FPU_PRESENT
#endif
#ifdef __FPU_USED
#undef __FPU_USED
#endif
#define __FPU_PRESENT               (1)
#define __FPU_USED                  (1)


extern uint32_t _sisr_vector;

#endif /* __STM32F429DISC_BOARD_H */
#ifndef MOCK_MAIN_H
#define MOCK_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include our mock HAL instead of the real one */
#include "mock_stm32f4xx_hal.h"

/* Standard library includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* Mock any other defines from main.h if needed */

#ifdef __cplusplus
}
#endif

#endif /* MOCK_MAIN_H */
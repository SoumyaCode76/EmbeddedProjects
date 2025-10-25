/*
 * log_uart.h
 * Non-blocking UART logger API (ring buffer + IT)
 */
#ifndef __LOG_UART_H
#define __LOG_UART_H

#include <stdint.h>
#include <stddef.h>

void LOG_UART_Init(void);
// Enqueue data to logger; returns number of bytes enqueued
size_t LOG_UART_Enqueue(const uint8_t *data, size_t len);
// Enqueue a null-terminated string convenience
size_t LOG_UART_EnqueueString(const char *s);
// Called by HAL when UART TX completes
void LOG_UART_TxCpltCallback(void);

#endif // __LOG_UART_H

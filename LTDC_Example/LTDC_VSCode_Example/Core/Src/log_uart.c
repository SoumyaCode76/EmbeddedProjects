/* log_uart.c
 * Simple ring-buffered, interrupt-driven UART logger.
 * Uses HAL_UART_Transmit_IT to send bytes from buffer.
 */

#include "log_uart.h"
#include "usart.h" // for huart1
#include "string.h"
#include "stm32f4xx_hal.h"

/* Buffer size: power-of-two recommended. */
#define LOG_UART_BUF_SIZE 512

static uint8_t log_buf[LOG_UART_BUF_SIZE];
static volatile uint32_t log_head = 0; // next write
static volatile uint32_t log_tail = 0; // next read
static volatile uint8_t tx_busy = 0;
static volatile uint16_t last_tx_size = 0; // track last transmit size

static inline uint32_t buf_mask(void) { return LOG_UART_BUF_SIZE - 1; }

void LOG_UART_Init(void)
{
  log_head = log_tail = 0;
  tx_busy = 0;
  last_tx_size = 0;
}

// Internal: start transfer if not busy
static void start_tx_if_needed(void)
{
  if (tx_busy) return;
  uint32_t head = log_head;
  uint32_t tail = log_tail;
  if (head == tail) return; // empty

  // Determine contiguous chunk size
  uint32_t size = (head > tail) ? (head - tail) : (LOG_UART_BUF_SIZE - tail);

  tx_busy = 1;
  last_tx_size = (uint16_t)size; // store size for TxCplt callback
  HAL_StatusTypeDef st = HAL_UART_Transmit_IT(&huart1, (uint8_t *)&log_buf[tail], (uint16_t)size);
  if (st != HAL_OK)
  {
    // If transmit cannot be started, clear busy flag to retry later
    tx_busy = 0;
    last_tx_size = 0;
  }
}

size_t LOG_UART_Enqueue(const uint8_t *data, size_t len)
{
  if (data == NULL || len == 0) return 0;
  uint32_t space;
  uint32_t head = log_head;
  uint32_t tail = log_tail;

  if (tail <= head)
    space = LOG_UART_BUF_SIZE - (head - tail) - 1;
  else
    space = (tail - head) - 1;

  if (space == 0) return 0; // buffer full

  if (len > space) len = space;

  for (size_t i = 0; i < len; ++i)
  {
    log_buf[head] = data[i];
    head = (head + 1) & (LOG_UART_BUF_SIZE - 1);
  }

  log_head = head;

  // attempt to start TX
  start_tx_if_needed();

  return len;
}

size_t LOG_UART_EnqueueString(const char *s)
{
  if (s == NULL) return 0;
  return LOG_UART_Enqueue((const uint8_t *)s, strlen(s));
}

// This must be called from HAL UART Tx Complete callback
void LOG_UART_TxCpltCallback(void)
{
  // Use the stored transmission size to advance tail correctly
  uint16_t sent = last_tx_size;
  
  if (sent > 0) {
    log_tail = (log_tail + sent) & (LOG_UART_BUF_SIZE - 1);
  }
  
  tx_busy = 0;
  last_tx_size = 0;
  
  // Start next chunk if data present
  start_tx_if_needed();
}

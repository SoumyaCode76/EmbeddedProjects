#include "bsp.h"
#include "queue.h"

void uart_init(void);
void uart_deinit(void);
void uart_send(uint8_t *data, uint16_t length);
void uart_string_print(char * data);
void uart_receive(uint8_t *data, uint16_t length);



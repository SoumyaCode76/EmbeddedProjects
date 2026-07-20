#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "common.h"

#define QUEUE_SIZE											(256)

typedef struct
{
	uint8_t buffer[QUEUE_SIZE];
	uint16_t count_elements;
} queue_t;

void queue_init(void);
void queue_push(queue_t * pLogQueue, const uint8_t * data, const uint32_t length);
void queue_pop(queue_t * pLogQueue, uint8_t *data, uint32_t length);

#endif

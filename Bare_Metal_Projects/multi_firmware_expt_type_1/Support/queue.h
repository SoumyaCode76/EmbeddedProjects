#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "common.h"

#define QUEUE_SIZE											(256)

typedef struct
{
	uint16_t push_index;
	uint16_t pop_index;
	uint16_t number_of_elements_inserted;
	uint8_t storage[QUEUE_SIZE];
}queue_t;

queue_t * queue_init(queue_t * const pLogQueue);
void queue_push(queue_t * const pLogQueue, const uint8_t * const data, const uint32_t length);
void queue_pop(queue_t * const pLogQueue, uint8_t * const data, const uint32_t length);

#endif

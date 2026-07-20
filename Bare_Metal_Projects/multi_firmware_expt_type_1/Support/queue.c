#include "queue.h"

queue_t LogQueue = {ZERO};

void queue_init(void)
{
	LogQueue.count_elements = ZERO;
	memset((void *)LogQueue.buffer, ZERO, QUEUE_SIZE);
}

void queue_push(queue_t * pLogQueue, const uint8_t * data, const uint32_t length)
{
	if (pLogQueue->count_elements + length <= QUEUE_SIZE)
	{
		memcpy((void *)&pLogQueue->buffer[pLogQueue->count_elements], (const void *)data, length);
		pLogQueue->count_elements += length;
	}
}

void queue_pop(queue_t * pLogQueue, uint8_t *data, uint32_t length)
{
	if (pLogQueue->count_elements >= length)
	{
		memcpy((void *)data, (const void *)pLogQueue->buffer, length);
		memmove((void *)pLogQueue->buffer, (const void *)&pLogQueue->buffer[length], pLogQueue->count_elements - length);
		pLogQueue->count_elements -= length;
	}
}

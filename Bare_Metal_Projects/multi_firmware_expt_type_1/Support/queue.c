#include "queue.h"


queue_t * queue_init(queue_t * const pLogQueue)
{
	if(pLogQueue != NULL)
	{
		pLogQueue->number_of_elements_inserted = ZERO;
		pLogQueue->push_index = ZERO;
		pLogQueue->pop_index = ZERO;
		memset((void *)pLogQueue->storage, ZERO, sizeof(pLogQueue->storage));
	}
	return pLogQueue;
}

/**
 * @brief Adds data to the back of the queue
 *
 * Appends the specified data to the queue buffer if there is sufficient space.
 * Data is copied to the end of existing queue contents without removing old data.
 * The number of used rooms is incremented by the length of data added.
 *
 * @param[in,out] pLogQueue Pointer to the queue structure to push data into
 * @param[in] data Pointer to the data buffer to be copied into the queue
 * @param[in] length Number of bytes to copy from data into the queue
 *
 * @note Operation only proceeds if total used rooms plus new length does not exceed available space
 * @note Does not modify queue if insufficient space is available
 */

void queue_push(queue_t * const pLogQueue, const uint8_t * const data, const uint32_t length)
{
	if(pLogQueue == NULL || data == NULL || length == 0U)
	{
		return;
	}
	else
	{
		if (length <= sizeof(pLogQueue->storage))
		{
			memcpy((void *)&pLogQueue->storage[pLogQueue->push_index], (const void *)data, length);
			pLogQueue->number_of_elements_inserted += length;
		}
		else
		{
			uint16_t space_available = sizeof(pLogQueue->storage) - pLogQueue->push_index;
			memcpy((void *)&pLogQueue->storage[pLogQueue->push_index], (const void *)data, space_available);
			memcpy((void *)pLogQueue->storage, (const void *)&data[space_available], length - space_available);
			pLogQueue->number_of_elements_inserted += length;
		}
		pLogQueue->push_index = (pLogQueue->push_index + length) % sizeof(pLogQueue->storage);
	}
}



void queue_pop(queue_t * const pLogQueue, uint8_t * const data, const uint32_t length)
{
	if(pLogQueue == NULL || data == NULL || length == 0U)
	{
		return;
	}
	else
	{
		if (length <= sizeof(pLogQueue->storage))
		{
			memcpy((void *)data, (const void *)&pLogQueue->storage[pLogQueue->pop_index], length);
			pLogQueue->number_of_elements_inserted -= length;
		}
		else
		{
			uint16_t number_of_forward_requested_cells = sizeof(pLogQueue->storage) - pLogQueue->pop_index;
			memcpy((void *)data, (const void *)&pLogQueue->storage[pLogQueue->pop_index], number_of_forward_requested_cells);
			memcpy((void *)&data[number_of_forward_requested_cells], (const void *)pLogQueue->storage, length - number_of_forward_requested_cells);
			pLogQueue->number_of_elements_inserted -= length;
		}
		pLogQueue->pop_index = (pLogQueue->pop_index + length) % sizeof(pLogQueue->storage);
	}
}

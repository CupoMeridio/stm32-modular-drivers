#include "queue.h"
#include "main.h"
#include "string.h"

static inline void increase_head(queue_t* queue)
{
	queue->head = (queue->head+1)%queue->qsize;
	queue->length--;
}

static inline void extract_data(queue_t* queue, void* item)
{
	size_t offset = queue->head * queue->isize;
	memcpy(item, queue->items+offset, queue->isize);
}

uint8_t queue_init(queue_t* queue, uint8_t* queue_buffer, uint16_t items_size, uint16_t queue_size)
{
	if(!queue || !queue_buffer || items_size == 0 || queue_size == 0)
	{
		return QUEUE_ERR;
	}

	queue->qsize = queue_size;
	queue->isize = items_size;
	queue->length = 0;
	queue->head = 0;
	queue->tail = 0;
	queue->items = queue_buffer;
	queue->mode = QUEUE_MODE_REJECT; // Default operating mode

	return QUEUE_OK;
}

uint8_t queue_set_mode(queue_t* queue, queue_mode_t mode)
{
	if(!queue)
	{
		return QUEUE_ERR;
	}

	__disable_irq();
	queue->mode = mode;
	__enable_irq();

	return QUEUE_OK;
}

uint8_t queue_enqueue(queue_t* queue, const void* item)
{
	if(!queue || !item || !queue->items)
	{
		return QUEUE_ERR;
	}

	//Entering critical section
	__disable_irq();

	if(queue->length >= queue->qsize)
	{
		if(queue->mode == QUEUE_MODE_REJECT)
		{
			__enable_irq();
			return QUEUE_FULL;
		}
		else // QUEUE_MODE_OVERWRITE
		{
			// Move head to discard the oldest item since we are overwriting it
			queue->head = (queue->head+1)%queue->qsize;
			// Temporarily decrement length to balance the increment below
			queue->length--;
		}
	}

	//Insert in the current free slot
	size_t offset = queue->tail * queue->isize;
	memcpy(queue->items+offset, item, queue->isize);

	//Evaluate next free slot
	queue->tail = (queue->tail+1)%queue->qsize;

	queue->length++;

	//Exiting critical section
	__enable_irq();

	return QUEUE_OK;
}

uint8_t queue_head(queue_t* queue, void* item)
{
	uint8_t ret = QUEUE_ERR;

	if(!queue || !item || !queue->items)
	{
		return QUEUE_ERR;
	}

	__disable_irq();

	if(queue->length > 0)
	{
		//Getting head item by copy
		extract_data(queue, item);

		ret = QUEUE_OK;
	}
	else
	{
		ret = QUEUE_EMPTY;
	}
	__enable_irq();

	return ret;

}

uint8_t queue_extract(queue_t* queue, void* item){

	uint8_t ret = QUEUE_ERR;

	if(!queue || !item || !queue->items)
	{
		return QUEUE_ERR;
	}

	__disable_irq();

	if(queue->length > 0)
	{
		//Getting head item by copy
		extract_data(queue, item);

		//Moving to next item
		increase_head(queue);
		ret = QUEUE_OK;
	}
	else
	{
		ret = QUEUE_EMPTY;
	}
	__enable_irq();

	return ret;

}

uint8_t queue_dequeue(queue_t* queue){

	uint8_t ret = QUEUE_ERR;

	if(!queue || !queue->items)
	{
		return QUEUE_ERR;
	}

	__disable_irq();

	if(queue->length > 0)
	{
		//Moving to next item
		increase_head(queue);
		ret = QUEUE_OK;
	}
	else
	{
		ret = QUEUE_EMPTY;
	}
	__enable_irq();

	return ret;

}

uint8_t queue_get_length(const queue_t* queue, uint16_t* length)
{
	if(!queue || !length || !queue->items)
	{
		return QUEUE_ERR;
	}

	*length = queue->length;
	return QUEUE_OK;
}

uint8_t queue_get_size(const queue_t* queue, uint16_t* size)
{
	if(!queue || !size || !queue->items)
	{
		return QUEUE_ERR;
	}

	*size = queue->qsize;
	return QUEUE_OK;
}

uint8_t queue_get_item_size(const queue_t* queue, uint16_t* size)
{
	if(!queue || !size || !queue->items)
	{
		return QUEUE_ERR;
	}

	*size = queue->isize;
	return QUEUE_OK;
}

uint8_t queue_is_full(const queue_t* queue)
{
	if(!queue || !queue->items)
	{
		return QUEUE_ERR;
	}

	if(queue->length >= queue->qsize)
	{
		return QUEUE_FULL;
	}
	return QUEUE_OK;
}

uint8_t queue_is_empty(const queue_t* queue)
{
	if(!queue || !queue->items)
	{
		return QUEUE_ERR;
	}

	if(queue->length == 0)
	{
		return QUEUE_EMPTY;
	}
	return QUEUE_OK;
}

/*
 * ring_buf.c
 *
 *  Created on: Feb 12, 2025
 *      Author: mikop
 */
#include "ring_buf.h"

HAL_StatusTypeDef RB_read(ringBuffer_t *ringBuf, uint8_t *val) {
	if (ringBuf->head == ringBuf->tail) {
		return HAL_ERROR;
	}

	*val = ringBuf->buffer[ringBuf->tail];
	ringBuf->tail = (ringBuf->tail + 1) % RING_BUFFER_SIZE;
	return HAL_OK;
}

HAL_StatusTypeDef RB_write(ringBuffer_t *ringBuf, uint8_t val) {

	uint16_t tempHead = (ringBuf->head + 1) % RING_BUFFER_SIZE;
	if (tempHead == ringBuf->tail) {
		return HAL_ERROR;
	}

	ringBuf->buffer[ringBuf->head] = val;
	ringBuf->head = tempHead;

	return HAL_OK;
}

HAL_StatusTypeDef RB_flush(ringBuffer_t *ringBuf) {
	ringBuf->head = 0;
	ringBuf->tail = 0;
	return HAL_OK;
}

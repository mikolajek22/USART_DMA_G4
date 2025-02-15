/*
 * ring_buf.h
 *
 *  Created on: Feb 12, 2025
 *      Author: mikop
 */

#ifndef INC_RING_BUF_H_
#define INC_RING_BUF_H_

#include "main.h"
#define RING_BUFFER_SIZE	512
typedef struct {
	uint16_t head;
	uint16_t tail;
	uint8_t buffer[RING_BUFFER_SIZE];
}ringBuffer_t;
HAL_StatusTypeDef RB_read(ringBuffer_t *ringBuf, uint8_t *val);
HAL_StatusTypeDef RB_write(ringBuffer_t *ringBuf, uint8_t val);
#endif /* INC_RING_BUF_H_ */

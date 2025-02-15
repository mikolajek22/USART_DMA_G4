/*
 * frameInterpret.h
 *
 *  Created on: Feb 13, 2025
 *      Author: mikop
 */

#ifndef INC_FRAME_PARSER_H_
#define INC_FRAME_PARSER_H_
#include "stdint.h"
#include "main.h"

#define MAX_FRAME_SIZE		167
#define MAX_PAYLOAD_SIZE	160
#define EOF_CHAR			191
#define SOF_CHAR			190
typedef struct {
	uint8_t addr;
	uint8_t cmd;
	uint8_t arg;
	uint8_t payload[160];
	uint8_t len;
} usartMessage_t;

uint8_t frame_parser_create(uint8_t *rawFrame, usartMessage_t *message);
HAL_StatusTypeDef frame_parser_analyze(uint8_t *rawFrame, uint8_t size, usartMessage_t *message);
#endif /* INC_FRAME_PARSER_H_ */

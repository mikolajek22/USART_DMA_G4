/*
 * frameInterpret.c
 *
 *  Created on: Feb 13, 2025
 *      Author: mikop
 */

#include <frame_parser.h>
#include "crc8.h"

#define MSG_PREFIX		4
#define MSG_SUBFIX		3

/* Returns number of frame bytes */
uint8_t frame_parser_create(uint8_t *rawFrame, usartMessage_t *message) {
	uint8_t crc;
	uint8_t idx = 0;
	if (message->len > MAX_PAYLOAD_SIZE) {
		// payload out of score
		return 0;
	}
	rawFrame[idx++] = SOF_CHAR;
	rawFrame[idx++] = message->addr;
	rawFrame[idx++] = message->cmd;
	rawFrame[idx++] = message->arg;
	for (uint8_t i = 0; i < message->len; i++) {
		rawFrame[idx + i] = message->payload[i];
	}
	idx = idx + message->len;
	rawFrame[idx++] = message->len;
	crc = crc8ccitt(rawFrame + 1, message->len + 4); // addr + 1 (start after SOF, size +4 (LEN,ADR,CMD,ARG)
	rawFrame[idx++] = crc;
	rawFrame[idx++] = EOF_CHAR;
	return idx;
}

HAL_StatusTypeDef frame_parser_analyze(uint8_t *rawFrame, uint8_t size, usartMessage_t *message) {

	/*	the frame:		|SOF|ADDR|CMD|ARG|PAYLOAD|LEN|CRC|EOF| 		*/
	/*	size in bytes:	| 1 | 1 | 1 | 1 | 0 - 160| 1 | 1 | 1 |		*/
	uint8_t payloadLen = size - MSG_PREFIX - MSG_SUBFIX;
	// check payload size
	uint8_t readSize = rawFrame[MSG_PREFIX + payloadLen];
	if (payloadLen != readSize) {
		//message is corrupted: payload size differs from expected.
		return HAL_ERROR;
	}

	// get payload from raw buffer
	for (uint8_t i = 0; i < payloadLen; i++) {
		message->payload[i] = rawFrame[MSG_PREFIX + i];
	}

	// check if crc is ok
	uint8_t crcCode = crc8ccitt(rawFrame + 1, size - 3);	//addr +1 (data after sof) // -3 (no CRC, EOF, SOF)
	if (rawFrame[MSG_PREFIX + payloadLen + 1] != crcCode) {
//		crc is not equal
		return HAL_ERROR;
	}

//	everything is ok.

	message->addr 	= rawFrame[1];
	message->cmd 	= rawFrame[2];
	message->arg 	= rawFrame[3];
	message->len	= rawFrame[MSG_PREFIX + payloadLen];
	return HAL_OK;
}

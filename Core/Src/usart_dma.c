/*
 * usart_dma.c
 *
 *  Created on: Feb 14, 2025
 *      Author: mikop
 */
#include "usart_dma.h"
#include <string.h>
#define MAX_RCV_SIZE	255

ringBuffer_t ringBufRx;
ringBuffer_t ringBufTx;

uint8_t rcvBuffer[MAX_RCV_SIZE];
volatile uint8_t isDmaSend;				// check if DMA is busy
volatile uint8_t frameEndCnt;			// counter for frames in rcv_RB
UART_HandleTypeDef *huartHandler;

HAL_StatusTypeDef uart_Send();

// Receiving done - CB
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	if(huart->Instance == LPUART1) {

		static uint8_t start = 0;
		// assuming that data can be send with delays - CB from IDLE
		static uint8_t tempBuf[MAX_FRAME_SIZE];
		static uint8_t tempPos = 0;

		for (uint8_t i = 0; i < Size; i++) {
			// do not save trash data to RB
			if (rcvBuffer[i] == SOF_CHAR || start == 1) {
				start = 1;
				// save in temp buffer - prevent RB from corrupted frames
				tempBuf[tempPos++] = rcvBuffer[i];
				if (rcvBuffer[i] == EOF_CHAR) {
					for (uint8_t j = 0; j < tempPos; j++) {
						RB_write(&ringBufRx, tempBuf[j]);
					}
					// ready to parse
					frameEndCnt++;
					tempPos = 0;
				}
				else if (tempPos > MAX_FRAME_SIZE) {
					/* ERROR - frame is bigger than expected */
					start = 0;
					tempPos = 0;
				}
			}
		}
	}

	HAL_UARTEx_ReceiveToIdle_DMA(huartHandler, rcvBuffer, MAX_RCV_SIZE);
	__HAL_DMA_DISABLE_IT(huartHandler->hdmarx, DMA_IT_HT);
}

// Sending done - CB
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if(huart->Instance == LPUART1) {
		// If there is still sth in RB - continue sending.
		if (ringBufTx.head != ringBufTx.tail) {
			uart_Send();
		}
		else {
			// DMA free
			isDmaSend = 0;
		}
	}
}


void usart_dma_init(UART_HandleTypeDef *huart) {
	huartHandler = huart;

	/* Disable callback from receiving half of the expected data*/
	HAL_UARTEx_ReceiveToIdle_DMA(huartHandler, rcvBuffer, MAX_RCV_SIZE);
	__HAL_DMA_DISABLE_IT(huartHandler->hdmarx, DMA_IT_HT);
}


HAL_StatusTypeDef usart_dma_SendFrame(uint8_t *pData, size_t size) {

	for (int i = 0; i < size; i++) {
		RB_write(&ringBufTx, pData[i]);
	}
	/* Chcek if previous DMA sending is completed - if no data will be enququed in RB */
	if (!isDmaSend) {
		uart_Send();
	}
	return HAL_OK;
}

/* Parsing raw frame from rcv_RB, returning struct of included data in the frame. */
usartMessage_t usart_dma_frameProcess(){
	uint8_t rawFrame[MAX_FRAME_SIZE];
	usartMessage_t msg;
	uint8_t i = 0;
	uint8_t isSof = 0;
	uint8_t val = 0;
	memset(&msg, 0, sizeof(msg));
	if (frameEndCnt > 0) {
			  // find SOF - start of the frame (ensure)
			  do {
				  RB_read(&ringBufRx, &val);
				  if (val == SOF_CHAR || isSof == 1) {
					  isSof = 1;
					  rawFrame[i] = val;
					  i++;
				  }
			  } while(val != EOF_CHAR && i < MAX_FRAME_SIZE);

			  // parse frame into data struct
			  frame_parser_analyze(rawFrame, i, &msg);

			  // parsing done.
			  __disable_irq();
			  frameEndCnt--;
			  __enable_irq();
		  }
	return msg;
}


HAL_StatusTypeDef uart_Send() {
	// DMA -> busy
	__disable_irq();
	isDmaSend = 1;
	__enable_irq();
	// ensure if there is sth in RB_tx
	if (ringBufTx.head != ringBufTx.tail) {
		uint16_t size = (ringBufTx.head > ringBufTx.tail) ? (ringBufTx.head - ringBufTx.tail): (RING_BUFFER_SIZE - ringBufTx.tail);
		static uint8_t tempBuf[RING_BUFFER_SIZE];
		for (uint16_t i = 0; i < size; i++) {
			RB_read(&ringBufTx, &tempBuf[i]);
		}
		HAL_UART_Transmit_DMA(huartHandler, tempBuf, size);
	}
	else {
		// no data in RB
		__disable_irq();
		isDmaSend = 0;
		__enable_irq();
		return HAL_ERROR;
	}
	return HAL_OK;
}

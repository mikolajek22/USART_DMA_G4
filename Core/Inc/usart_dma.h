/*
 * usart_dma.h
 *
 *  Created on: Feb 14, 2025
 *      Author: mikop
 */

#ifndef INC_USART_DMA_H_
#define INC_USART_DMA_H_
#include <frame_parser.h>
#include "stm32g4xx_hal.h"
#include "stdint.h"
#include "ring_buf.h"

//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);

HAL_StatusTypeDef usart_dma_SendFrame(uint8_t *pData, size_t size);
usartMessage_t usart_dma_frameProcess();
void usart_dma_init(UART_HandleTypeDef *huart);
#endif /* INC_USART_DMA_H_ */

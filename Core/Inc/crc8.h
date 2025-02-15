/*
 * crc8.h
 *
 *  Created on: Feb 13, 2025
 *      Author: mikop
 */

#ifndef INC_CRC8_H_
#define INC_CRC8_H_

#include <stdint.h>
#include <string.h>

uint8_t crc8ccitt(const void * data, size_t size);

#endif /* INC_CRC8_H_ */

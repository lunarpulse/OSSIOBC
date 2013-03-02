/*
 * flash.h
 *
 *  Created on: 2013. 1. 14.
 *      Author: OSSI
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "ossi_obc.h"

// This flash module use Segment A as main
// and Segment B for buffering
// So All result are stored to Segment A
// And read from Segment A

// DO NOT CHANGE THIS!!!
// Using Information Memory Section
// To store system status
// SEG A 128 Bytes
// SEG B 128 Bytes
// Use below address for boundary check

#define FLASH_SEG_A_START_ADDR	(0x1080)
#define FLASH_SEG_A_LAST_ADDR	(0x10FF)
#define FLASH_SEG_B_START_ADDR	(0x1000)
#define FLASH_SEG_B_LAST_ADDR	(0x107F)

#define FLASH_SEG_A_SIZE		(128)
#define FLASH_SEG_B_SIZE		(128)

#define FLASH_ACLK 				FSSEL_0                /* Flash clock select: 0 - ACLK */
#define FLASH_MCLK				FSSEL_1                /* Flash clock select: 1 - MCLK */
#define FLASH_SMCLK				FSSEL_2                /* Flash clock select: 2 - SMCLK */
#define FLASH_SMCLK2			FSSEL_3                /* Flash clock select: 3 - SMCLK */

uint8_t flash_writeBegin(uint8_t flashClockSelect, uint8_t flashClockDivider);
uint8_t flash_writeData(uint8_t address, uint8_t byteCount, uint8_t *data);
uint8_t flash_writeEnd(void);

uint8_t flash_readData(uint8_t address, uint8_t byteCount, uint8_t *data);

#endif /* FLASH_H_ */

/*
 * ossi_eeprom.h
 *
 *  Created on: Jan 20, 2013
 *      Author: donghee
 */

#ifndef OBC_EEPROM_H_
#define OBC_EEPROM_H_

#include "ossi_obc.h"

#define EEPROM_ADDR 0x50
#define FRAM_ADDR 0x51

uint8_t eeprom_byteWrite(uint16_t addr, uint8_t _data);
uint8_t eeprom_byteRead(uint16_t addr, uint8_t* _data);
uint8_t eeprom_pageWrite(uint16_t addr, uint8_t dataSize, const uint8_t* data);
uint8_t eeprom_pageRead(uint16_t addr, uint16_t read_size, uint8_t * data);
uint8_t eeprom_test(void);

void eeprom_setAddress(uint8_t i2cAddress);


#endif /* OBC_EEPROM_H_ */

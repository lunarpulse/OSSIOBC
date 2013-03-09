/*
 * ossiobc.h
 *
 *  Created on: 2013. 1. 6.
 *      Author: OSSI
 */

#ifndef OSSIOBC_H_
#define OSSIOBC_H_

#include "ossi_1611.h"
#include "clock.h"
#include "ossi_log.h"
#include "ossi_timer.h"
#include "ossi_data.h"
#include "i2c.h"
#include "adc12.h"
#include "printf.h"
#include "obc_rtc.h"


// PORT 1
#define EXT_WDT_PIN			(BIT0) // external wdt reset pin
#define LED_PIN				(BIT1) // status LED
#define UNUSED1_2			(BIT2) // unused
#define SC_PFO_IN_PIN		(BIT3) // LTC4425 supercap charger status
#define BAT_FAULT_IN_PIN	(BIT4) // LT3652 Bat charger status 1
#define BAT_CHRG_IN_PIN		(BIT5) // LT3652 Bat charger status 2
#define SC_ON_IN_PIN		(BIT6) // VBAT to Supercap charger powerpath status
#define BAT_ON_IN_PIN		(BIT7) // VBAT to VBUS powerpath status

// PORT 2
#define SOLAR_ON_IN_PIN		(BIT0) // VSOLAR to VBUS powerpath status
#define COMMS_FAULT_IN_PIN  (BIT1) // COMMS fault status HIGH: Normal / LOW: Fault
#define BEACON_FAULT_IN_PIN (BIT2) // Beacon fault status HIGH: Normal / LOW: Fault
#define LED_FAULT_IN_PIN 	(BIT3) // LED controller fault status HIGH: Normal / LOW: Fault
#define IO_OE_PIN 			(BIT4) // 74LVC8T245 Buffer input enable Active Low
#define ROSC_PIN 			(BIT5) // ROSC for DCO calibration
#define RTC_INT_PIN 		(BIT6) // DS3231 RTC interrupt pin
#define TEMP_ALERT_PIN		(BIT7) // TMP101 alert pin

// PORT 3
#define I2C_RST_PIN			(BIT0) // I2C Mux reset pin
#define I2C_SDA_PIN			(BIT1) // I2C SDA pin
#define OBC_I2C_OFF_PIN		(BIT2) // OBC I2C Power OFF pin
#define I2C_SCL_PIN			(BIT3) // I2C SCL pin
#define UNUSED3_4			(BIT4) // unused
#define UNUSED3_5			(BIT5) // unused
#define UART_TXD_PIN		(UART_TXD1_PIN) // UART TXD1
#define UART_RXD_PIN		(UART_RXD1_PIN) // UART RXD1

// PORT 4
#define VEXT_OFF_PIN		(BIT0) // VEXT Power in OFF
#define COMMS_OFF_PIN		(BIT1) // COMMS OFF pin to HIGH
#define BEACON_OFF_PIN		(BIT2) // BEACON OFF pin to HIGH
#define LED_OFF_PIN			(BIT3) // LED OFF pin to HIGH
#define SCOUT_EN_OUT_PIN	(BIT4) // VBAT to Supercap charger powerpath connect to HIGH
#define SCCHG_EN_OUT_PIN	(BIT5) // Supercap charger enable
#define ANT_DEPLOY2_PIN		(BIT6) // Antenna Deploy 1 to HIGH
#define ANT_DEPLOY1_PIN		(BIT7) // Antenna Deploy 2 to HIGH

// PORT 5
#define GYRO_CS_PIN			(BIT0) // Active Low
#define GYRO_SI_PIN			(BIT1) //
#define GYRO_SO_PIN			(BIT2) //
#define GYRO_SCLK_PIN		(BIT3) //
#define RS485_DE_PIN		(BIT4) // External Interface 485 HIGH to Tx
#define MUX_A0_PIN			(BIT5) // allow 10ms delay when switching (swith -> delay -> read)
#define MUX_A1_PIN			(BIT6) //
#define MUX_A2_PIN			(BIT7) //

// PORT 6
#define ADC0_PIN			(ADC12_PIN_6_0) // connected to analog Mux
#define ADC1_PIN			(ADC12_PIN_6_1) // connected to analog Mux
#define UNUSED6_2			(BIT2) // unused
#define UNUSED6_3			(BIT3) // unused
#define UNUSED6_4			(BIT4) // unused
#define UNUSED6_5			(BIT5) // unused
#define UNUSED6_6			(BIT6) // unused
#define UNUSED6_7			(BIT7) // unused


uint8_t obc_sendData(uint8_t slaveAddress, uint8_t dataAddr, uint8_t size, uint8_t* data);
uint8_t obc_sendCmd(uint8_t slaveAddress, uint8_t cmdAddr, uint8_t cmd);

#endif /* OSSIOBC_H_ */

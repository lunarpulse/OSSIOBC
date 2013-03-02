/*
 * dac12.h
 *
 *  Created on: 2013. 1. 17.
 *      Author: OSSI
 */

#ifndef DAC12_H_
#define DAC12_H_

#include "ossi_obc.h"

// ports setup is not needed for dac12
// amplifierSelect > 0 -> DAC port ON
#define DAC12_PIN_6_6		        (0x01)
#define DAC12_PIN_6_7              	(0x10)

// multiplierSelect
#define DAC12_VREFx1 				(DAC12IR)
#define DAC12_VREFx3 				(!(DAC12IR))

// amplifierSelect
#define DAC12_AMP_OFF_PINOUTHIGHZ 	(DAC12AMP_0)
#define DAC12_AMP_OFF_PINOUTLOW   	(DAC12AMP_1)
#define DAC12_AMP_LOWIN_LOWOUT    	(DAC12AMP_2)
#define DAC12_AMP_LOWIN_MEDOUT    	(DAC12AMP_3)
#define DAC12_AMP_LOWIN_HIGHOUT   	(DAC12AMP_4)
#define DAC12_AMP_MEDIN_MEDOUT    	(DAC12AMP_5)
#define DAC12_AMP_MEDIN_HIGHOUT   	(DAC12AMP_6)
#define DAC12_AMP_HIGHIN_HIGHOUT  	(DAC12AMP_7)

#define DAC12_TRIGGER_ENCBYPASS 	(DAC12LSEL_0)
#define DAC12_TRIGGER_ENC       	(DAC12LSEL_1)
#define DAC12_TRIGGER_TA        	(DAC12LSEL_2)
#define DAC12_TRIGGER_TB        	(DAC12LSEL_3)

// refVoltageSourceSelect
#define DAC12_VREF_INT  (DAC12SREF_0)
#define DAC12_VREF_EXT  (DAC12SREF_2)


void dac12_initDAC0(uint16_t multiplierSelect, uint8_t amplifierSelect);
void dac12_initDAC1(uint16_t multiplierSelect, uint8_t amplifierSelect);

// make sure you call adc12_setVolReference()
// to set dac12 reference
// and the refVoltageSourceSelect should be matched
// that of adc12
void dac12_setVolRefDAC0(uint16_t refVoltageSourceSelect);
void dac12_setVolRefDAC1(uint16_t refVoltageSourceSelect);

// Watchout the dacValue overflow!!!!!
void dac12_outputDAC0(uint16_t dacValue);
void dac12_outputDAC1(uint16_t dacValue);

// shut down dac12
void dac12_disableDAC0(void);
void dac12_disableDAC1(void);

#endif /* DAC12_H_ */

/*
 * dac12.c
 *
 *  Created on: 2013. 1. 17.
 *      Author: OSSI
 */

#include "dac12.h"

void dac12_initDAC0(uint16_t multiplierSelect, uint8_t amplifierSelect)
{
	// Clear DAC12ENC to set register
	DAC12_0CTL &= ~DAC12ENC;
	DAC12_0CTL = multiplierSelect + amplifierSelect + DAC12_TRIGGER_ENCBYPASS;
}

void dac12_initDAC1(uint16_t multiplierSelect, uint8_t amplifierSelect)
{
	// Clear DAC12ENC to set register
	DAC12_1CTL &= ~DAC12ENC;
	DAC12_1CTL = multiplierSelect + amplifierSelect + DAC12_TRIGGER_ENCBYPASS;
}

void dac12_setVolRefDAC0(uint16_t refVoltageSourceSelect)
{
	DAC12_0CTL &= ~DAC12ENC;
	DAC12_0CTL |= refVoltageSourceSelect;
}

void dac12_setVolRefDAC1(uint16_t refVoltageSourceSelect)
{
	DAC12_1CTL &= ~DAC12ENC;
	DAC12_1CTL |= refVoltageSourceSelect;
}

void dac12_outputDAC0(uint16_t dacValue)
{
//	DAC12_0CTL &= ~DAC12ENC;
	DAC12_0DAT = dacValue;
//	DAC12_0CTL &= ~DAC12ENC;
}

void dac12_outputDAC1(uint16_t dacValue)
{
//	DAC12_0CTL &= ~DAC12ENC;
	DAC12_1DAT = dacValue;
//	DAC12_0CTL &= ~DAC12ENC;
}

// if dac12_disableDAC0 is called,
// you need to dac12_init() again to start dac12
void dac12_disableDAC0(void)
{
	DAC12_0CTL &= ~DAC12ENC;
	DAC12_0CTL = 0x0000;

}

void dac12_disableDAC1(void)
{
	DAC12_1CTL &= ~DAC12ENC;
	DAC12_1CTL = 0x0000;
}

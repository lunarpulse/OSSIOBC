/*
 * adc12.c
 *
 *  Created on: 2013. 1. 16.
 *      Author: OSSI
 */

#include "adc12.h"

void adc12_portSetup(uint8_t ports)
{
	ASSERT(ports <= 0xFF);
	P6SEL |= ports;
}

void adc12_init(uint8_t clockSourceSelect, uint16_t clockSourceDivider, uint16_t clockCycleHoldCount)
{
	// Make sure ENC is cleared
	ADC12CTL0 &= ~ENC;
	// Turn off ADC and Clear interrupts and etc
	ADC12CTL0 &= ~(ADC12ON + ADC12OVIE + ADC12TOVIE + ENC + ADC12SC);

	// reset registers
	ADC12IFG &= 0x0000;
	ADC12IE &= 0x0000;
	// we only use ADC12MEM0
	ADC12MCTL0 &= 0x0000;

	// set clockSourceSelect / clockSourceDivider / ADC12_SAMPLEHOLDSOURCE_SC
	// and set SHP
	ADC12CTL1 = clockSourceSelect + clockSourceDivider + ADC12_SAMPLEHOLDSOURCE_SC + SHP;

	// we only use ADC12MEM0,
	// so we set hold count for lower conversion memory
	ADC12CTL0 = 0x0F00 & clockCycleHoldCount ;

}

void adc12_setVolReference(uint8_t refVoltageSourceSelect)
{
	// Clear ENC to set registers
	ADC12CTL0 &= ~ENC;

	if (refVoltageSourceSelect == ADC12_REF_VCC_VSS)
	{
		// default reference setting

		// reset internal reference setting
		ADC12CTL0 &= ~(REFON+REF2_5V);
		// reset reference voltage setting for ADC12MEM0
		ADC12MCTL0 &= 0x8F;
		ADC12MCTL0 |= ADC12_REF_VCC_VSS;
	}
	else if(refVoltageSourceSelect == ADC12_REF_VREF_VSS)
	{
		// internal reference setting

		// we only use 2.5V internal reference
		ADC12CTL0 |= (REFON+REF2_5V);
		// reset reference voltage setting for ADC12MEM0
		ADC12MCTL0 &= 0x8F;
		ADC12MCTL0 |= ADC12_REF_VREF_VSS;
		// make sure you wait for reference to stabilize about 17~20ms
		// TODO: change cycles based on system clock
		volatile uint8_t i;
		for(i = 0; i < 17; i++)
		{
			__delay_cycles(7372);
		}
	}
	else if(refVoltageSourceSelect == ADC12_REF_V_EREF_VSS)
	{
		// external reference setting

		// reset internal reference setting
		ADC12CTL0 &= ~(REFON+REF2_5V);
		// reset reference voltage setting for ADC12MEM0
		ADC12MCTL0 &= 0x8F;
		ADC12MCTL0 |= ADC12_REF_V_EREF_VSS;
	}

}

void adc12_offInternalVolReference(void)
{
	// Clear ENC to set registers
	ADC12CTL0 &= ~ENC;
	ADC12CTL0 &= ~(REFON+REF2_5V);
}

uint16_t adc12_readChannel(uint8_t channelSelect)
{
	volatile uint16_t adcValue = 0;

	// Clear ENC to set registers
	ADC12CTL0 &= ~ENC;

	// reset previous channel setting
	ADC12MCTL0 &= 0xF0;
	// set channel
	ADC12MCTL0 |= (channelSelect & 0x0F);
	ADC12CTL0 |= ADC12ON;
	// clear IFG before starting convertion
	ADC12IFG &= ~BIT0;
	ADC12CTL0 |= ENC;
	ADC12CTL0 |= ADC12SC;
	// wait conversion to be finished
	while ((ADC12IFG & BIT0)==0);
	// read value
	adcValue = ADC12MEM0;

	// wait adc12 not busy to turn off
	while(ADC12CTL1&ADC12BUSY);
	ADC12CTL0 &= ~ENC;
	ADC12IFG &= ~BIT0;

	return adcValue;
}

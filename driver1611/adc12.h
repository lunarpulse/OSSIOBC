/*
 * adc12.h
 *
 *  Created on: 2013. 1. 16.
 *      Author: OSSI
 */


#ifndef ADC12_H_
#define ADC12_H_

#include "ossi_1611.h"

#define ADC12_PIN_6_0		        (BIT0)
#define ADC12_PIN_6_1              	(BIT1)
#define ADC12_PIN_6_2              	(BIT2)
#define ADC12_PIN_6_3              	(BIT3)
#define ADC12_PIN_6_4              	(BIT4)
#define ADC12_PIN_6_5              	(BIT5)
#define ADC12_PIN_6_6              	(BIT6)
#define ADC12_PIN_6_7               (BIT7)

#define ADC12_INPUT_A0              (INCH_0)
#define ADC12_INPUT_A1              (INCH_1)
#define ADC12_INPUT_A2              (INCH_2)
#define ADC12_INPUT_A3              (INCH_3)
#define ADC12_INPUT_A4              (INCH_4)
#define ADC12_INPUT_A5              (INCH_5)
#define ADC12_INPUT_A6              (INCH_6)
#define ADC12_INPUT_A7              (INCH_7)
#define ADC12_INPUT_V_EREF_POS      (INCH_8) //TODO: what is this?
#define ADC12_INPUT_VREF_NEG        (INCH_9) //TODO: what is this?
#define ADC12_INPUT_TEMPSENSOR      (INCH_10)
#define ADC12_INPUT_VMID		  	(INCH_11)

#define ADC12_SAMPLEHOLDSOURCE_SC 	(SHS_0)
//#define ADC12_SAMPLEHOLDSOURCE_1  	(SHS_1)
//#define ADC12_SAMPLEHOLDSOURCE_2  	(SHS_2)
//#define ADC12_SAMPLEHOLDSOURCE_3  	(SHS_3)

#define ADC12_CLOCKSOURCE_ADC12OSC (ADC12SSEL_0)
#define ADC12_CLOCKSOURCE_ACLK     (ADC12SSEL_1)
#define ADC12_CLOCKSOURCE_MCLK     (ADC12SSEL_2)
#define ADC12_CLOCKSOURCE_SMCLK    (ADC12SSEL_3)

#define ADC12_CLOCKDIVIDER_1   (ADC12DIV_0)
#define ADC12_CLOCKDIVIDER_2   (ADC12DIV_1)
#define ADC12_CLOCKDIVIDER_3   (ADC12DIV_2)
#define ADC12_CLOCKDIVIDER_4   (ADC12DIV_3)
#define ADC12_CLOCKDIVIDER_5   (ADC12DIV_4)
#define ADC12_CLOCKDIVIDER_6   (ADC12DIV_5)
#define ADC12_CLOCKDIVIDER_7   (ADC12DIV_6)
#define ADC12_CLOCKDIVIDER_8   (ADC12DIV_7)

#define ADC12_CYCLEHOLD_4_CYCLES    (SHT0_0)
#define ADC12_CYCLEHOLD_8_CYCLES    (SHT0_1)
#define ADC12_CYCLEHOLD_16_CYCLES   (SHT0_2)
#define ADC12_CYCLEHOLD_32_CYCLES   (SHT0_3)
#define ADC12_CYCLEHOLD_64_CYCLES   (SHT0_4)
#define ADC12_CYCLEHOLD_96_CYCLES   (SHT0_5)
#define ADC12_CYCLEHOLD_128_CYCLES  (SHT0_6)
#define ADC12_CYCLEHOLD_192_CYCLES  (SHT0_7)
#define ADC12_CYCLEHOLD_256_CYCLES  (SHT0_8)
#define ADC12_CYCLEHOLD_384_CYCLES  (SHT0_9)
#define ADC12_CYCLEHOLD_512_CYCLES  (SHT0_10)
#define ADC12_CYCLEHOLD_768_CYCLES  (SHT0_11)
#define ADC12_CYCLEHOLD_1024_CYCLES (SHT0_12)

#define ADC12_REF_VCC_VSS			(SREF_0)
#define ADC12_REF_VREF_VSS			(SREF_1)
#define ADC12_REF_V_EREF_VSS		(SREF_2)

#define ADC12_SINGLECHANNEL          (CONSEQ_0)

void adc12_portSetup(uint8_t ports);

// call adc12_init() one time in the beginning
void adc12_init(uint8_t clockSourceSelect, uint16_t clockSourceDivider, uint16_t clockCycleHoldCount);

void adc12_setVolReference(uint8_t refVoltageSourceSelect);

// adc12_end() turns off internal reference
// as the reference is not automatically power down
// and turns off adc12 module to save power
void adc12_offInternalVolReference(void);

// returns the value
// single channel single conversion mode
uint16_t adc12_readChannel(uint8_t channelSelect);


#endif /* ADC12_H_ */

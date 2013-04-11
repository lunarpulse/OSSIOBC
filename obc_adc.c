#include "obc_adc.h"


void adg708_portInit()
{
  P5DIR |= MUX_A0_PIN + MUX_A1_PIN + MUX_A2_PIN;
}

void adg708_setChannel(uint8_t channel)
{
  uint8_t a0;
  uint8_t a1;
  uint8_t a2;

  a0 = (channel-1) & 1;
  a1 = ((channel-1)>>1) & 1;
  a2 = ((channel-1)>>2) & 1;

  P5OUT &= ~(MUX_A0_PIN + MUX_A1_PIN + MUX_A2_PIN);
//  P5OUT &= ~MUX_A0_PIN;
//  P5OUT &= ~MUX_A1_PIN;
//  P5OUT &= ~MUX_A2_PIN;

  // RC equation
  // t = -log((V-Vc)/V)R*C (Unit: R is kOhm, C is microfarads, t is millisecs) Up to 97% V
  // t = -log(0.03)*10*0.1
  // t = -(-3.50655)*1 = 3.5 ms;

  // TODO: Change delay cycles depends on SYSTEM CLOCK
  // 0.0035sec * clock = cycles;
  // 0.0035 * 8000000(8Mhz) = 28000
  //__delay_cycles(28000);
  delay_ms(20);

  if (a0 == 1)
	  P5OUT |= MUX_A0_PIN;
  else
	  P5OUT &= ~MUX_A0_PIN;

  if (a1 == 1)
	  P5OUT |= MUX_A1_PIN;
  else
	  P5OUT &= ~MUX_A1_PIN;

  if (a2 == 1)
	  P5OUT |= MUX_A2_PIN;
  else
	  P5OUT &= ~MUX_A2_PIN;

  //__delay_cycles(28000);
  delay_ms(20);
}

void adc_saveData1(viData_t* _viData, uint8_t muxCh, uint16_t value)
{
	uint8_t _value;

  _value = (value >> 4); // adc value is 12 bits data

	if (muxCh == MUX_VSUPERCAP) {
		_viData->u8.vsupercap = _value;
	}
	if (muxCh == MUX_VBAT) {
		_viData->u8.vbat = _value;
	}
	if (muxCh == MUX_VBUS) {
		_viData->u8.vbus = _value;
	}
	if (muxCh == MUX_VSOLAR) {
		_viData->u8.vsolar = _value;
	}
}

uint8_t obc_adcTest()
{
	volatile uint8_t muxCh;
	uint16_t value;
    viData_t v;

	adc12_setVolReference(ADC12_REF_VREF_VSS);
	adc12_offInternalVolReference();
	adc12_setVolReference(ADC12_REF_VCC_VSS);

	for (muxCh=MUX_VSUPERCAP; muxCh <= MUX_VSOLAR ; muxCh++) {
		adg708_portInit();
		adg708_setChannel(muxCh);
		value =  adc12_readChannel(1);
		adc_saveData1(&v, muxCh, value);
	}
	log_withNum(ADC_SERVICE, NOTICE_LEVEL, "adc v supercap1: ", v.u8.vsupercap);
	log_withNum(ADC_SERVICE, NOTICE_LEVEL, "adc vbat: ", v.u8.vbat);
	log_withNum(ADC_SERVICE, NOTICE_LEVEL, "adc vbus: ", v.u8.vbus);
	log_withNum(ADC_SERVICE, NOTICE_LEVEL, "adc vsolar: ", v.u8.vsolar);
	__delay_cycles(1000000);

	for (muxCh=MUX_SOLAR_CS4; muxCh <= MUX_BUS_CS ; muxCh++) {
		adg708_portInit();
		adg708_setChannel(muxCh);
		value =  adc12_readChannel(0);
	}
	__delay_cycles(1000000);
	return 1;
}

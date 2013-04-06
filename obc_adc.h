#ifndef OBC_ADC_H_
#define OBC_ADC_H_

#include "ossi_obc.h"

#define MUX_VSOLAR 4
#define MUX_VBUS 3
#define MUX_VBAT 2
#define MUX_VSUPERCAP 1

#define MUX_SOLAR_CS1 4
#define MUX_SOLAR_CS2 3
#define MUX_SOLAR_CS3 2
#define MUX_SOLAR_CS4 1
#define MUX_SOLAR_CS5 5
#define MUX_BUS_CS 6
#define MUX_SUPERCAP_TEMP 7  //connector changed (originally battery temp)
#define MUX_BAT_TEMP 8  // connector changed (originally supercap temp)

void adg708_setChannel(uint8_t channel);
uint8_t obc_adcTest(void);


#endif /* OBC_ADC_H_ */

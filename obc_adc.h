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
#define MUX_SUN_IN 6

uint8_t obc_adcTest(void);


#endif /* OBC_ADC_H_ */

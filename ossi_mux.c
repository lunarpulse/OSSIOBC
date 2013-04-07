/*
 * ossi_mux.c
 *
 *  Created on: 2013. 4. 7.
 *      Author: OSSI
 */

#include "ossi_mux.h"

void mux_reset(void)
{
	P3OUT &= ~I2C_RST_PIN;
	delay_ms(1);
	P3OUT |= I2C_RST_PIN;
	delay_ms(1);
}

// mux
uint8_t mux_setChannel(mux_chan_t channel)
{
     uint8_t chan[1];
     uint8_t result;

     chan[0] = 1 << channel;
     // set channel
     mux_reset();
     result = i2c_masterWrite(MUX_ADDRESS, 1, chan);
     return result;
}

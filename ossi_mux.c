/*
 * ossi_mux.c
 *
 *  Created on: 2013. 4. 7.
 *      Author: OSSI
 */

#include "ossi_mux.h"

// mux
uint8_t mux_setChannel(uint8_t address, mux_chan_t channel)
{
     uint8_t chan[1];
     uint8_t result;

     chan[0] = 1 << channel;
     // set channel
     result = i2c_masterWrite(address, 1, chan);
     return result;
}

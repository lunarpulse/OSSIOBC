/*
 * ossi_mux.h
 *
 *  Created on: 2013. 4. 7.
 *      Author: OSSI
 */

#ifndef OSSI_MUX_H_
#define OSSI_MUX_H_

#include "ossi_obc.h"

#define MUX_ADDRESS (0x70)


typedef enum {
	MUX_COMMS_CHANNEL = 0,
	MUX_BEACON_CHANNEL = 1,
	MUX_LED_CHANNEL = 2,
	MUX_PANELTEMP_CHANNEL = 3,
	MUX_OBC_INTERNAL_CHANNEL = 4
} mux_chan_t;

void mux_reset(void);
uint8_t mux_setChannel(mux_chan_t channel);

#endif /* OSSI_MUX_H_ */

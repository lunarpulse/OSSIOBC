#include "i2c.h"
#include "ossi_log.h"

#define MUX_ADDRESS (0x70)

typedef enum {
	MUX_COMMS_CHANNEL = 0,
	MUX_BEACON_CHANNEL = 1,
	MUX_LED_CHANNEL = 2,
	MUX_PANELTEMP_CHANNEL = 3
} mux_chan_t;

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


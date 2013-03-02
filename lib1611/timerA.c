#include "timerA.h"


volatile uint8_t i2cTimeOut;

// If variable timeout is 10, Timeout is 3.2768 sec.
// If ACLK = 32768Hz
// We want to One tick (ellapsed_time) is 0.03 sec, so CCR0 is 122.88 (CCR0 = 0.03 * 4096(TACLK))


void i2c_timerInit(uint16_t timerASourceSelect, uint8_t timerADividerSelect, uint8_t timerAMode, uint16_t timerAThreshold)
{
	// reset timer A
	TACTL = TACLR;

	CCR0 = timerAThreshold;
	// set divider first
	TACTL = timerADividerSelect;    // ID_3: Timer A input divider: 3 - /8
	TACTL |= timerASourceSelect;  // TASSEL_1: Timer A clock source select: 1 - ACLK
	TACTL |= timerAMode;   //  MC_1: Timer A mode control: 1 - Up to CCR0 // timer start counting from now
}


void i2c_timerTimeoutStart(void)
{
	// P6OUT ^= BIT3;
	// reset timeout variable
	i2cTimeOut = 0;

	// init timer A counter
	TAR = 0;

	// make sure clear timer A flag before enable interrupt
	CCTL0 &= ~CCIFG;
	CCTL0 = CCIE;
}


void i2c_timerTimeoutStop(void)
{
	CCTL0 &= ~CCIE;
	CCTL0 &= ~CCIFG;

	// Timer Stop
	TACTL &= ~MC_3;
	TACTL |= MC_0;

	TAR = 0;

	i2cTimeOut = 0;
//	 P6OUT ^= BIT1;
}


#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A(void)
{
	// disable timer interrupt
	CCTL0 &= ~CCIE;
	i2cTimeOut = 1;
	 P6OUT ^= BIT0;
	 log_saveWithNum(I2CIFG);
}

/*
 * timerB.c
 *
 *  Created on: 2013. 1. 19.
 *      Author: OSSI
 */

#include "ossi_timer.h"

static volatile uint32_t sysMsTick = 0;
static volatile uint32_t sysSecTick = 0;
static volatile uint16_t sysMsDelayTick = 0;
static volatile uint8_t sysMsDelayOn = 0;
static volatile uint16_t sysMsDelay = 0;
static volatile uint8_t sysSecWakeOn = 0;
static volatile uint16_t sysSecWakePeriod = 0;
static volatile uint16_t sysSecWakeTick = 0;

static uint16_t msUnit;


/* systimer uses 16 bit counter by default
 * LPM3 -> ACLK only
 * For double buffer we chose, CLLDx = 0 so  TBCL0 -> TBCCR0
 */
void systimer_init(uint16_t timerBSourceSelect, uint8_t timerBDividerSelect, uint8_t timerBMode, uint16_t timerBMsThreshold ,uint16_t timerBSecThreshold)
{
	// Clear the timer
	TBCTL = TBCLR;
	// set tick unit time
	TBCCR0 = timerBSecThreshold;
	TBCCR1 = msUnit = timerBMsThreshold;
	// set divider first
	TBCTL = timerBDividerSelect;
	// set timer source
	TBCTL |= timerBSourceSelect;
	// set timer mode
	TBCTL |= timerBMode;
}

void systimer_start(void)
{

	// init sysMsTick counter
	sysMsTick = 0;
	sysSecTick = 0;

	// reset timer B counter
	TBR = 0;

	TBCCTL0 &= ~CCIFG;
	TBCCTL0 |= CCIE;

	TBCCTL1 &= ~CCIFG;
	TBCCTL1 |= CCIE;

}

void systimer_stop(void)
{
	// disable interrupt to stop sysMsTick from increasing

	TBCCTL1 &= ~CCIE;
	TBCCTL1 &= ~CCIFG;

	TBCCTL0 &= ~CCIE;
	TBCCTL0 &= ~CCIFG;

	// Timer Stop
	TBCTL &= ~MC_3;
	TBCTL |= MC_0;

	// reset timer B counter
	TBR = 0;

	// init sysMsTick counter
	sysMsTick = 0;
	sysSecTick = 0;

}

uint32_t systimer_getMsTick(void)
{
	return sysMsTick;
}

uint32_t systimer_getSecTick(void)
{
	return sysSecTick;
}

void systimer_msDelay(uint16_t msDelay)
{
	sysMsDelay = msDelay;
	sysMsDelayTick =0;
	sysMsDelayOn = 1;
	while(sysMsDelayOn);
}

void systimer_setWakeUpPeriod(uint16_t sec)
{
	sysSecWakePeriod = sec;
}

void systimer_startWakeUpPeriod(void)
{
	sysSecWakeTick = 0;
	sysSecWakeOn = 1;
}

void systimer_stopWakeUpPeriod(void)
{
	sysSecWakeOn = 0;
	sysSecWakeTick = 0;
}

// Timer B0 interrupt service routine
#pragma vector=TIMERB0_VECTOR
__interrupt void systimer_sec (void)
{
	// TODO: overflow check needed?
	//P6OUT ^= BIT0;
	sysSecTick++;
	sysMsTick = sysSecTick*1000;
	TBCCR1 = msUnit;
	if (sysSecWakeOn)
	{
		sysSecWakeTick++;
		if (sysSecWakeTick >= sysSecWakePeriod)
		{
			sysSecWakeTick = 0;
			__bic_SR_register_on_exit(LPM3_bits);
		}
	}
}


// Timer B1 interrupt service routine
#pragma vector=TIMERB1_VECTOR
__interrupt void systimer_ms (void)
{
	switch(TBIV)
	{
	case TBIV_TBCCR1:
		//P6OUT ^= BIT1;
		sysMsTick++;
		TBCCR1 += msUnit;
		if(sysMsDelayOn)
		{
			sysMsDelayTick++;
			if (sysMsDelayTick >= sysMsDelay)
			{
				sysMsDelayOn = 0;
			}
		}
		break;
	default:
		break;
	}
}


/*
 * timerB.h
 *
 *  Created on: 2013. 1. 19.
 *      Author: OSSI
 */

#ifndef TIMERB_H_
#define TIMERB_H_

#include "ossi_obc.h"

#define TIMER_B_TBCLK 		TBSSEL_0
#define TIMER_B_ACLK 		TBSSEL_1
#define TIMER_B_SMCLK 		TBSSEL_2
#define TIMER_B_INCLK 		TBSSEL_3

#define TIMER_B_DIVIDED_BY_1	ID_0                   	     /* Timer B input divider: 0 - /1 */
#define TIMER_B_DIVIDED_BY_2	ID_1                         /* Timer B input divider: 1 - /2 */
#define TIMER_B_DIVIDED_BY_4	ID_2                         /* Timer B input divider: 2 - /4 */
#define TIMER_B_DIVIDED_BY_8	ID_3                         /* Timer B input divider: 3 - /8 */

#define TIMER_B_STOP			MC_0                         /* Timer B mode control: 0 - Stop */
#define TIMER_B_UP_MODE			MC_1                         /* Timer B mode control: 1 - Up to CCR0 */
#define TIMER_B_CONT_MODE		MC_2                         /* Timer B mode control: 2 - Continuous up */
#define TIMER_B_UPDOWN_MODE		MC_3                         /* Timer B mode control: 3 - Up/Down */

void systimer_init(uint16_t timerBSourceSelect, uint8_t timerBDividerSelect, uint8_t timerBMode, uint16_t timerBMsThreshold ,uint16_t timerBSecThreshold);
void systimer_start(void);
void systimer_stop(void);
uint32_t systimer_getMsTick(void);
uint32_t systimer_getSecTick(void);

void systimer_msDelay(uint16_t msDelay);
void systimer_setWakeUpPeriod(uint16_t sec);
void systimer_startWakeUpPeriod(void);
void systimer_stopWakeUpPeriod(void);


#endif /* TIMERB_H_ */

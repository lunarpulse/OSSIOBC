/*
 * clock.h
 *
 *  Created on: 2013. 1. 14.
 *      Author: OSSI
 */

/* Clock setting for OSSI MSP430F1611
 *
 * default: CLOCK_XT2_LFXT1
 * XT2 ON: 7.3728 MHz or 8 MHz
 * LFXT1 Low Power Mode: 32.768 kHz
 * MCLK = XT2CLK / SMCLK = XT2CLK / ACLK = LFXT1
 *
 * alternative 1: CLOCK_DCO_LFXT1
 * XT2 OFF: DCO set to ~ 1MHz
 * LFXT1 Low Power Mode: 32.768 kHz
 * MCLK = DCOCLK / SMCLK = DCOCLK / ACLK = LFXT1
 *
 *
 * alternative 2: CLOCK_DCO_DCO
 * DCO set to ~ 1MHz
 * MCLK = DCOCLK / SMCLK = DCOCLK / ACLK = DCOCLK / 30
 *
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include "msp430f1611.h"
#include "ossi_types.h"
#include "debug.h"

#define CLOCK_XT2_LFXT1			 	(0)
#define CLOCK_DCO_LFXT1			 	(1)
#define CLOCK_DCO_DCO			 	(2)

#define CLOCK_XT2_FREQ_8MHz			(0)
#define CLOCK_XT2_FREQ_7_3728MHz	(1)
#define CLOCK_DCO_FREQ_1MHz			(2)

//#define DELTA 900                          // target DCO = DELTA*(4096) = 3686400
//#define DELTA 70                           // target DCO = DELTA*(4096) = 286720
#define DCO_DELTA 					(245)        // target DCO = DELTA*(4096) = 1003275

#define MCLK_DIVIDED_BY_1 		DIVM_0 /* MCLK Divider 0: /1 */
#define MCLK_DIVIDED_BY_2		DIVM_1 /* MCLK Divider 1: /2 */
#define MCLK_DIVIDED_BY_4		DIVM_2 /* MCLK Divider 2: /4 */
#define	MCLK_DIVIDED_BY_8		DIVM_3 /* MCLK Divider 3: /8 */

#define SMCLK_DIVIDED_BY_1		DIVS_0 /* SMCLK Divider 0: /1 */
#define SMCLK_DIVIDED_BY_2		DIVS_1 /* SMCLK Divider 1: /2 */
#define SMCLK_DIVIDED_BY_4		DIVS_2 /* SMCLK Divider 2: /4 */
#define SMCLK_DIVIDED_BY_8		DIVS_3 /* SMCLK Divider 3: /8 */

#define ACLK_DIVIDED_BY_1		DIVA_0 /* ACLK Divider 0: /1 */
#define ACLK_DIVIDED_BY_2		DIVA_1 /* ACLK Divider 1: /2 */
#define ACLK_DIVIDED_BY_4		DIVA_2 /* ACLK Divider 2: /4 */
#define ACLK_DIVIDED_BY_8		DIVA_3 /* ACLK Divider 3: /8 */

uint8_t clock_getMode(void);
uint8_t clock_setup(void);
void clock_dividerSetup(uint8_t MCLKDividerSelect, uint8_t SMCLKDividerSelect, uint8_t ACLKDividerSelect);

#endif /* CLOCK_H_ */

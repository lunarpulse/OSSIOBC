/*
 * obc_interface.h
 *
 *  Created on: 2013. 3. 11.
 *      Author: OSSI
 */

#ifndef OBC_INTERFACE_H_
#define OBC_INTERFACE_H_

#include "ossi_obc.h"

void interface_txEnable(void);
void interface_txDisable(void);
void begin_report(void);
void end_report(void);
void interface_init(void);
void interface_check(void);

#endif /* OBC_INTERFACE_H_ */

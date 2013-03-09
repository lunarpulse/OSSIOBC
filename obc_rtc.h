/*
 * ossi_rtc.h
 *
 *  Created on: Jan 19, 2013
 *      Author: donghee
 */

#ifndef OBC_RTC_H_
#define OBC_RTC_H_

#include "ossi_obc.h"

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
} rtcTime_t;

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t date;
    union {
        uint8_t data;
        struct {
            unsigned a1m1 :1;
            unsigned a1m2 :1;
            unsigned a1m3 :1;
            unsigned a1m4 :1;
            unsigned padding :4;
        } b;
    } mask;
} rtcAlarm_t;

typedef union {
    uint8_t data;
    struct {
        unsigned a1ie :1;
        unsigned a2ie :1;
        unsigned intcn :1;
        unsigned rs1 :1;
        unsigned rs2 :1;
        unsigned conv :1;
        unsigned bbsqw :1;
        unsigned esoc :1;
    } b;
} controlReg_t;

#define RTC_ADDRESS (104)

#define ERROR 0
#define SUCCESS 1

uint8_t rtc_readTime(rtcTime_t* time);
uint8_t rtc_setTime(rtcTime_t* time);

uint8_t rtc_setAlarm1(rtcAlarm_t* alarm);
uint8_t rtc_readAlarm1(rtcAlarm_t* alarm);
uint8_t rtc_clearAlarm1();

uint8_t obc_rtcAlarm1Save(ledTimeData_t* ledTime);

uint8_t obc_rtcInit(void);

uint8_t obc_rtcTest(void);

#endif /* OBC_RTC_H_ */

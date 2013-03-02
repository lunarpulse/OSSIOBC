#ifndef LOG_H_
#define LOG_H_

#include "ossi_obc.h"
#include "ossi_rtc.h"
#include "uart.h"

/* #include "ossitypes.h" */
/* #include "aclkuart.h" */

typedef enum {
    LOG_OFF = 0,
    LOG_UART = 1
    //    LOG_MEMORY = 2,
} logType_t;

typedef enum {
    I2C_SERVICE = 0,
    ADC_SERVICE = 1,
    LOG_SERVICE = 2,
    RTC_SERVICE = 3
} system_t;

typedef enum {
	NONE_LEVEL = 0,
	NOTICE_LEVEL = 1,
	WARNING_LEVEL = 2,
	ERROR_LEVEL = 3
} logLevel_t;

typedef enum {
    LOG_NO_MSG = 0,
    LOG_I2C_START = 1,
    LOG_I2C_STOP = 2,
    LOG_ADC_START = 3,
    LOG_ADC_STOP = 4,
    LOG_BUF_OVER = 5,
} logMsg_t;

typedef struct {
  system_t system;
  logMsg_t type;
  char* message;
  logLevel_t level;
} log_message_t;

#define MAX_BUFFER_SIZE 254

void log_init(void);
// set logging level;
// NONE_LEVEL, NOTICE_LEVEL, WARNING_LEVEL, ERROR_LEVEL.
void log_setOutputLevel(system_t sys, logLevel_t level);

// turn on/ turn off log
void log_globalOn();
void log_globalOff();

// logging with buffer.
void log_save(system_t s , logMsg_t msgTag);
void log_saveWithNum(uint16_t n);
// print saved buffer.
void log_printAll();
void log_printAllNum();

// log with uart.
void log(system_t s, logLevel_t level, char* msg);
void log_withNum(system_t s, logLevel_t level, char* msg, uint16_t n);

// print rtc clock
//void log_time(system_t s, logLevel_t level, rtcTime_t* time);
#endif

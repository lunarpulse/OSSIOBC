#include "ossi_log.h"

static logType_t  t;
static logLevel_t outputLevel;
static logMsg_t logBuffer[MAX_BUFFER_SIZE] = {LOG_NO_MSG,};
static uint16_t numBuffer[MAX_BUFFER_SIZE] = {0,};

static char* logLevelTags[] = {
		"none", // none level
		"\033[0;32mnotice\033[0m",
		"\033[1;33mwarning\033[0m",
		"\033[1;31merror\033[0m"
};

static char* subSystemTags[] = {
		"I2C",
	    "ADC",
	    "LOG",
	    "RTC"
};

// log example

//Time, Subsystem, Level, Message
//00:00:01, RTC, notice, RTC Reading Start
//00:00:01, RTC, debug, Current Hour Expect 1 but, 3
//00:00:01, RTC, warning, RTC Warning
//00:00:05, RTC, error, RTC Reading Error

log_message_t logMessages[] =  {
  {I2C_SERVICE, LOG_I2C_START, "Init i2c module\0", NOTICE_LEVEL},
  {I2C_SERVICE, LOG_I2C_STOP, "End i2c module\0", NOTICE_LEVEL},
  {ADC_SERVICE, LOG_ADC_START, "Init adc module\0", NOTICE_LEVEL},
  {ADC_SERVICE, LOG_ADC_STOP, "End adc module\0", NOTICE_LEVEL},
  {LOG_SERVICE, LOG_BUF_OVER, "Buffer overflow\0", ERROR_LEVEL},
};

static uint16_t bufIndex;
static uint8_t bufIsOverflow;

static uint16_t numBufIndex;
static uint8_t numBufIsOverflow;

void log_globalOn()
{
	t = LOG_UART;
}

void log_globalOff()
{
	t = LOG_OFF;
}

void log_setOutputLevel(system_t sys, logLevel_t level)
{
	// TODO: Each system have their level.
	outputLevel = level;
}

static void logBufferReset()
{
  bufIndex = 0;
  bufIsOverflow = 0;
}

static void numBufferReset()
{
  numBufIndex = 0;
  numBufIsOverflow = 0;
}


static char* itoa(int32_t value, char* str, int radix) {
  static char dig[] =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz";
  int n = 0, neg = 0;
  unsigned int v;
  char* p, *q;
  char c;
  if (radix == 10 && value < 0) {
    value = -value;
    neg = 1;
  }
  v = value;
  do {
    str[n++] = dig[v%radix];
    v /= radix;
  } while (v);
  if (neg)
    str[n++] = '-';
  str[n] = '\0';
  for (p = str, q = p + (n-1); p < q; ++p, --q)
    c = *p, *p = *q, *q = c;
  return str;
}

static char* getSystemTag(system_t s)
{
  volatile uint16_t j = 0;

  return subSystemTags[s];
}

static char* getLevelTag(logLevel_t level)
{
	if (NONE_LEVEL <= level && level <= ERROR_LEVEL)
        return logLevelTags[level];
	return "\0";
}

static void puts_systemTag(system_t s)
{
  char* systemTag;
  systemTag = getSystemTag(s);
  uart_puts(systemTag);
  uart_puts(", ");
}

static void puts_levelTag(logLevel_t level)
{
  char* levelTag;
  levelTag = getLevelTag(level);
  uart_puts(levelTag);
  uart_puts(", ");
}

static void puts_paddingZeroLessThen10(uint8_t n)
{
	char buf[12];
	itoa(n, buf, 10);
	if (n < 10)
		uart_puts("0");
	uart_puts(buf);

}
static void puts_timeTag(uint32_t tick)
{
  uint8_t hour;
  uint8_t minute;
  uint8_t sec;

  hour = (tick / 3600) % 100;
  minute = (tick / 60) % 60;
  sec = tick % 60;

  puts_paddingZeroLessThen10(hour);
  uart_puts(":");

  puts_paddingZeroLessThen10(minute);
  uart_puts(":");

  puts_paddingZeroLessThen10(sec);
  uart_puts(", ");
}

static void _log(system_t s, logLevel_t level, char* msg)
{
  uint32_t tick;
  tick = systimer_getMsTick();
//  //tick = 3600; // 01:00:00
//  tick = 3599; // 00:59:59
//  tick = 7199; // 00:59:59
//  //tick = 0; // 00:00:00
//  tick = 4294967295; // MAX VALUE

  puts_timeTag(tick);
  puts_systemTag(s);
  puts_levelTag(level);

  uart_puts(msg);
  return;
}

void log_init()
{
  char* initLog = "Time, Subsystem, Level, Message\r\n";

  uart_start();

  logBufferReset();
  numBufferReset();

  log_globalOn();
  //log_setOutputLevel(LOG_SERVICE, WARNING_LEVEL);
  log_setOutputLevel(LOG_SERVICE, NOTICE_LEVEL);

  _log(LOG_SERVICE, NOTICE_LEVEL, initLog);
}


void log(system_t s, logLevel_t level, char* msg)
{

  if (outputLevel > level || t == LOG_OFF)
	 return;

  _log(s, level, msg);
  uart_puts("\n\r");    
  return;
}

void log_withNum(system_t s, logLevel_t level, char* msg, uint16_t n)
{
  char buf[12];

  if (outputLevel > level || t == LOG_OFF)
  	 return;

  _log(s,level,msg);

  uart_puts(" ");
  itoa(n, buf, 10);
  uart_puts(buf);
  uart_puts("\n\r");  
  return;
}

void log_save(system_t s , logMsg_t msgTag)
{
  if (t == LOG_OFF)
	  return;

  if (bufIndex >= MAX_BUFFER_SIZE) { // buffer is overflow ?
    bufIsOverflow = 1;
    return;
  }
  
  logBuffer[bufIndex] = msgTag;
  bufIndex++;
}

void log_saveWithNum(uint16_t n)
{
  if (t == LOG_OFF)
	  return;

  if (numBufIndex >= MAX_BUFFER_SIZE) { // buffer is overflow ?
    numBufIsOverflow = 1;
    return;
  }
    
  numBuffer[numBufIndex] = n;
  numBufIndex++;
}


static void logPrintFrom(logMsg_t msg)
{
  volatile uint16_t j;
  volatile uint8_t log_size = sizeof(logMessages)/sizeof(log_message_t);

  for (j=0 ; j < log_size; j++) {
    if (msg == logMessages[j].type) {
    	log(logMessages[j].system, logMessages[j].level, logMessages[j].message);
      return;
    }
  }
  log(LOG_SERVICE, WARNING_LEVEL, "cannot find log messages from table.");
  return;
}

void log_printAll()
{
  volatile uint16_t i;

  if (t == LOG_OFF)
	  return;

  for (i = 0 ; i < bufIndex; i++) {
    logPrintFrom(logBuffer[i]);
  }
  if (bufIsOverflow) {
    log(LOG_SERVICE, WARNING_LEVEL, "buffer overflow");
  }

  logBufferReset();
  return;
}


void log_printAllWithNum()
{
  volatile uint16_t i;
  char buf[6];

  if (t == LOG_OFF)
	  return;

  log(LOG_SERVICE, NOTICE_LEVEL, "num buffer steps");

  if (numBufIsOverflow)
      log(LOG_SERVICE, WARNING_LEVEL, "number buffer overflow");

//  uart_puts("LOG: ");
  for (i = 0 ; i < numBufIndex; i++) {
    uart_puts("->");
    itoa(numBuffer[i], buf, 10);
    uart_puts(buf);
  }
  uart_puts("\n\r");

  numBufferReset();
  return;
}

void log_time(system_t s, logLevel_t level, rtcTime_t* time)
{
  uint32_t tick;

  if (outputLevel > level || t == LOG_OFF)
	 return;

  tick = systimer_getMsTick();
  
  puts_timeTag(tick);
  puts_systemTag(s);
  puts_levelTag(level);

  uart_puts("20");
  puts_paddingZeroLessThen10(time->year);
  uart_puts("-");

  puts_paddingZeroLessThen10(time->month);
  uart_puts("-");

  puts_paddingZeroLessThen10(time->date);
  uart_puts(" ");

  puts_paddingZeroLessThen10(time->hour);
  uart_puts(":");

  puts_paddingZeroLessThen10(time->minute);
  uart_puts(":");

  puts_paddingZeroLessThen10(time->second);
  uart_puts("\n\r");
}

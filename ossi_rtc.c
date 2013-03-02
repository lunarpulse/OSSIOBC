#include "ossi_rtc.h"

/// RTC
uint8_t status_reg;

enum {
    SECOND = 0,
    MINUTE = 1,
    HOUR = 2,
    DAY = 3,
    DATE = 4,
    MONTH = 5,
    YEAR = 6,
    ALARM1_SECOND = 0x07,
    ALARM1_MINUTE = 0x08,
    ALARM1_HOUR = 0x09,
    ALARM1_DATE = 0x0a,
    CONTROL_REG = 0x0e,
    STATUS_REG = 0x0f
} rtc_reg_addr_t;

static uint8_t decToBcn(uint8_t dec)
{
    uint8_t bcn, ten_dec;
    ten_dec = (dec / 10) << 4;
    dec = dec % 10;
    bcn = ten_dec + dec;
    return bcn;
}

static uint8_t bcnToDec(uint8_t bcn)
{
    uint8_t ten_dec, dec;
    ten_dec = (bcn >> 4) * 10;
    dec = (bcn) & 0x0f;
    return ten_dec+dec;
}

static uint8_t rtc_getAlarmRegister(rtcAlarm_t * time, uint8_t addr)
{
    switch(addr)
        {
        case ALARM1_SECOND:
            return (time->mask.b.a1m1 << 7) | decToBcn(time->second);
        case ALARM1_MINUTE:
            return (time->mask.b.a1m2 << 7) | decToBcn(time->minute);
        case ALARM1_HOUR:
            return (time->mask.b.a1m3 << 7) | decToBcn(time->hour);
        case ALARM1_DATE:
            return (time->mask.b.a1m4 << 7) | decToBcn(time->date);
        }
}

static uint8_t rtc_getRegister(rtcTime_t * time, uint8_t nth)
{
    // TODO: check time bound (00~99, 1~12, 0~60)
    switch(nth)
        {
        case SECOND:
            return decToBcn(time->second);
        case MINUTE:
            return decToBcn(time->minute);
        case HOUR:
            return decToBcn(time->hour);
        case DAY:
            return decToBcn(time->day);
        case DATE:
            return decToBcn(time->date);
        case MONTH:
            return decToBcn(time->month);
        case YEAR:
            return decToBcn(time->year);
        }
}

static void rtc_update(rtcTime_t * time, uint8_t reg_addr, uint8_t reg)
{
	// bound check
    switch(reg_addr)
        {
        case SECOND:
        	time->second = bcnToDec(reg);
            break;
        case MINUTE:
        	time->minute = bcnToDec(reg);
            break;
        case HOUR:
        	time->hour = bcnToDec(reg);
            break;
        case DAY:
        	time->day = bcnToDec(reg);
            break;
        case DATE:
        	time->date = bcnToDec(reg);
            break;
        case MONTH:
        	time->month = bcnToDec(reg);
            break;
        case YEAR:
        	time->year = bcnToDec(reg);
            break;
        }
}

static void rtc_alarmUpdate(rtcAlarm_t * alarm, const uint8_t reg_addr, const uint8_t reg)
{
    if (reg_addr  == ALARM1_SECOND) {
        alarm->second = reg & 0x7f;
        alarm->mask.b.a1m1 = (reg & 0x80) >> 7;
    } else if (reg_addr  == ALARM1_MINUTE) {
        alarm->minute = reg & 0x7f;
        alarm->mask.b.a1m2 = (reg & 0x80) >> 7;
    } else if (reg_addr  == ALARM1_HOUR) {
        alarm->hour = reg & 0x7f;
        // TODO: Get 12/-24, -AM/PM
        alarm->mask.b.a1m3 = (reg & 0x80) >> 7;
    } else if (reg_addr  == ALARM1_DATE) {
        alarm->date = reg & 0x7f;
        // TODO: Get DY/-DT
        alarm->mask.b.a1m4 = (reg & 0x80) >> 7;
    }
}


static uint8_t rtc_readRegister(uint8_t reg_addr, uint8_t* reg)
{
	int result;
    uint8_t internalAddr[1];

    internalAddr[0] = reg_addr;
    result = i2c_masterWrite(RTC_ADDRESS, 1, internalAddr);
    if (result == ERROR){return result;}
    result = i2c_masterRead(RTC_ADDRESS, 1, reg);
    return result;
}

static uint8_t rtc_setRegister(uint8_t reg_addr, uint8_t reg)
{
	int result;
    uint8_t data[2];
    uint8_t verify_data[2];

    data[0] = reg_addr;
    data[1] = reg;
    result = i2c_masterWrite(RTC_ADDRESS, 2, data);
    if (result == ERROR){return result;}

    // verify writing data
    result = rtc_readRegister(reg_addr, verify_data);
    if (result == ERROR){return result;}

    // compare
    if ((data[0]==verify_data[0]) && (data[1] == verify_data[1]))
    	// TODO: Differenct kinds report. data is not same.
    	return SUCCESS;

    return ERROR;
}

uint8_t rtc_readTime(rtcTime_t * time)
{
	int result;
	uint8_t data[2] = {0,};
    volatile uint8_t reg_addr;

    // read rtc time register through i2c;
    for (reg_addr = SECOND; reg_addr <= YEAR; reg_addr++) {
        result = rtc_readRegister(reg_addr, data);
        if (result == ERROR) {return result;}
        rtc_update(time, reg_addr, data[0]);
    }
    return SUCCESS;
}

// Interrupt Clear!
uint8_t rtc_clearAlarm1()
{
    return rtc_setRegister(STATUS_REG, 0);
}

uint8_t rtc_setTime(rtcTime_t * time)
{
	// Only use 24 hours mode.
    uint8_t result;
    volatile uint8_t reg_addr = 0;

    // set time
    for (reg_addr = SECOND; reg_addr <= YEAR; reg_addr++) {
        result = rtc_setRegister(reg_addr, rtc_getRegister(time, reg_addr));
        if (result == ERROR){return result;}
    }
    return SUCCESS;
}

uint8_t rtc_setAlarm1(rtcAlarm_t * alarm)
{
	// Only use Date mode.
	int result;
    volatile uint8_t reg_addr;

    // clear alarm mask
    rtc_clearAlarm1();

    // set alarm1 time register.
    for (reg_addr = ALARM1_SECOND; reg_addr <= ALARM1_DATE; reg_addr++) {
        result = rtc_setRegister(reg_addr, rtc_getAlarmRegister(alarm, reg_addr));
        if (result == ERROR){return result;}
    }
    return SUCCESS;
}

uint8_t rtc_readAlarm1(rtcAlarm_t * alarm) {
    uint8_t data[1] = {0};
    volatile uint8_t reg_addr;
	int result;

    alarm->mask.data = 0;
    for (reg_addr = ALARM1_SECOND; reg_addr <= ALARM1_DATE; reg_addr++) {
        result = rtc_readRegister(reg_addr, data);
        if (result == ERROR){return result;}
        rtc_alarmUpdate(alarm, reg_addr, data[0]);
    }
    return SUCCESS;
}

uint8_t obc_rtcInit(void)
{
	// Interrupt pin setting.
	// RTC RST: P2.5
	P2IES |= BIT5; // high low edge detect.
	P2IE |= BIT5; // P2.5 interrupt enabled
	P2IFG &= ~BIT5; // P2.5 IFG cleared

	// RTC INT: P2.6
	P2IES |= BIT6; // high low edge detect.
	P2IE |= BIT6;
	P2IFG &= ~BIT6;

	// init alarm1.
    rtc_clearAlarm1();
}

uint8_t obc_rtcAlarm1Save(ledTimeData_t* ledTime)
{
    rtcAlarm_t alarm;
    uint8_t result;

    result = rtc_readAlarm1(&alarm);
	if (result == ERROR)
		return ERROR;

    ledTime->u8.date = alarm.date;
    ledTime->u8.hour = alarm.hour;
    ledTime->u8.min = alarm.minute;
}


uint8_t obc_rtcTest(void)
{
	rtcTime_t set_time;
    /* uint8_t sec_data[2];  uint8_t min_data[2]; */
	int result = SUCCESS;
	uint8_t reg;
    rtcAlarm_t alarm;

    //// TIME
    // making set_time data
    set_time.second = 0;
    set_time.minute = 49;
    set_time.hour = 22;
    set_time.day = 7;
    set_time.date = 19;
    set_time.month = 1;
    set_time.year = 13;

    // set time to ds3231 from set_time data
    //rtc_setTime(&set_time);
//	if (result == ERROR)
//		return ERROR;

    // read time from ds3231, and stroe set_time
	result = rtc_readTime(&set_time);
	if (result == ERROR)
		return ERROR;

	log_time(RTC_SERVICE, NOTICE_LEVEL, &set_time);

    //// ALARM1
	// making alarm data for setAlarm1
    alarm.second = 2;
    alarm.minute = 2;
    alarm.hour = 3;
    alarm.date = 1;

    // alarm mask.
    // a1m1 0 is mask. If alarm second match with timer's second. interrupt occur.
    alarm.mask.data = 0;
    alarm.mask.b.a1m1 = 0;
    alarm.mask.b.a1m2 = 1;
    alarm.mask.b.a1m3 = 1;
    alarm.mask.b.a1m4 = 1;

    // set alarm
    rtc_setAlarm1(&alarm);
	if (result == ERROR)
		return ERROR;

    // read alarm register
    rtc_readAlarm1(&alarm);
	if (result == ERROR)
		return ERROR;

    log_withNum(RTC_SERVICE, NOTICE_LEVEL, "rtc: status alarm1 sec: expect 2, but ", alarm.second);
    log_withNum(RTC_SERVICE, NOTICE_LEVEL, "rtc: status alarm1 min: expect 2, but ", alarm.minute);
    log_withNum(RTC_SERVICE, WARNING_LEVEL, "rtc: status alarm1 hour: expect 3, but ", alarm.hour);
    log_withNum(RTC_SERVICE, ERROR_LEVEL, "rtc: status alarm1 date: expect 1, but ", alarm.date);
    log_withNum(RTC_SERVICE, ERROR_LEVEL, "rtc: status alarm1 mask: expect 14, but ", alarm.mask.data);

    // set control register
    controlReg_t ctl_reg;
    ctl_reg.data = 0;
    ctl_reg.b.intcn = 1;
    ctl_reg.b.a1ie = 1;

    rtc_setRegister(CONTROL_REG, ctl_reg.data);
	if (result == ERROR)
		return ERROR;

    // read control register
    rtc_readRegister(CONTROL_REG, &reg);
	if (result == ERROR)
		return ERROR;

    log_withNum(RTC_SERVICE, NOTICE_LEVEL, "rtc: control reg: expect 5, but ", reg);

    // status reg.
    rtc_readRegister(STATUS_REG, &reg);
	if (result == ERROR)
		return ERROR;

    log_withNum(RTC_SERVICE, NOTICE_LEVEL, "rtc: status reg: expect 0, but ", reg);

	return SUCCESS;
}

// Port 1 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	P1OUT ^= BIT1; // P1.1 = toggle
	P2IFG &= ~BIT6; // P2.6 IFG cleared

	// TODO: check to i2c is busy!
    rtc_clearAlarm1();
    log_saveWithNum(0);
}

/*
 * obc_interface.c
 *
 *  Created on: 2013. 3. 11.
 *      Author: OSSI
 */


#include "obc_interface.h"

void interface_txEnable(void)
{
	IE2 &= ~URXIE1;
	P5OUT |= RS485_DE_PIN;
	delay_ms(10);
}

void interface_txDisable(void)
{
	delay_ms(10);
	P5OUT &= ~RS485_DE_PIN;
	IE2 |= URXIE1;
}

void begin_report(void)
{
	interface_txEnable();
	printf("\r\n");
	printf("******************************\r\n");
	printf("[OSSI-1 Satellite Report Begin]\r\n");
	printf("\r\n");
}

void end_report(void)
{
	printf("\r\n");
	printf("[OSSI-1 Satellite Report End]\r\n");
	printf("******************************\r\n");
	printf("\r\n");
	interface_txDisable();
}


void interface_init(void)
{
	// External Interface 485 TX DE
	P5OUT &= ~RS485_DE_PIN;
	P5DIR |= RS485_DE_PIN;
	uart1_init();
	uart1_start();
}


void interface_check(void)
{
	char readChar;
	if(uart1_available())
	{
		readChar = uart1_getc();

		if (readChar =='r')
		{
			uint8_t resetBootTime[2];
			resetBootTime[0] = 0;
			resetBootTime[1] = 0;
			flash_writeBegin(FLASH_SMCLK,2);
			flash_writeData(0,2,resetBootTime);
			flash_writeEnd();
			begin_report();
			printf("Boot Time Reset Completed\r\n");
			end_report();
		}

		if(readChar == ' ')
		{
			// printout all the status

			// get current boot time
			uint8_t bootTime[2];
			flash_readData(0,2,bootTime);
			uint16_t currentBootTime =  (bootTime[0] << 8) | bootTime[1];

			// test i2c communication with modules

			// get total current consumption

			// get battery voltage

			// adc chan 0
			volatile uint16_t adc_vsolar;
			volatile uint16_t adc_vbus;
			volatile uint16_t adc_vbat;
			volatile uint16_t adc_vsupercap;

			adg708_setChannel(MUX_VSOLAR);
			adc_vsolar = adc12_readChannel(1) * 0.8056 * 2;

			adg708_setChannel(MUX_VBUS);
			adc_vbus = adc12_readChannel(1) * 0.8056 * 2;

			adg708_setChannel(MUX_VBAT);
			adc_vbat = adc12_readChannel(1) * 0.8056 * 2;

			adg708_setChannel(MUX_VSUPERCAP);
			adc_vsupercap = adc12_readChannel(1) * 0.8056 * 2;

			// adc chan 1
			volatile uint16_t adc_solar_cs1;
			volatile uint16_t adc_solar_cs2;
			volatile uint16_t adc_solar_cs3;
			volatile uint16_t adc_solar_cs4;
			volatile uint16_t adc_solar_cs5;
			volatile uint16_t adc_bus_cs;
			volatile uint16_t adc_batTemp;
			volatile uint16_t adc_supercapTemp;

			// VOUT = (GAIN)(RSENSE)(ILOAD)
			// * 0.8056 / 2.5 = * 0.3222 for 0.05 RSENSE
			// * 0.8056 / 0.5 = * 1.611 for 0.01 (10mOhm) RSENSE
			adg708_setChannel(MUX_SOLAR_CS1);
			adc_solar_cs1 = adc12_readChannel(0) * 0.3222;

			adg708_setChannel(MUX_SOLAR_CS2);
			adc_solar_cs2 = adc12_readChannel(0) * 0.3222;

			adg708_setChannel(MUX_SOLAR_CS3);
			adc_solar_cs3 = adc12_readChannel(0) * 0.3222;

			adg708_setChannel(MUX_SOLAR_CS4);
			adc_solar_cs4 = adc12_readChannel(0) * 0.3222;

			adg708_setChannel(MUX_SOLAR_CS5);
			adc_solar_cs5 = adc12_readChannel(0) * 0.3222;

			adg708_setChannel(MUX_BUS_CS);
			adc_bus_cs = adc12_readChannel(0) * 1.611;

			// need to convert to celsius degree

			adg708_setChannel(MUX_BAT_TEMP);
			adc_batTemp = adc12_readChannel(0) * 0.8056;

			adg708_setChannel(MUX_SUPERCAP_TEMP);
			adc_supercapTemp = adc12_readChannel(0) * 0.8056;

			begin_report();
			printf("- Current Boot Time in Sec: %u sec\r\n",currentBootTime * 10);
			printf("- Solar Voltage: %u mV\r\n", adc_vsolar);
			printf("- Battery Voltage: %u mV\r\n", adc_vbat);
			printf("- Bus Voltage: %u mV\r\n", adc_vbus);
			printf("- Supercap Voltage: %u mV\r\n", adc_vsupercap);
			printf("- Solar Panel 1 (+Y) Current: %u mA\r\n",adc_solar_cs1);
			printf("- Solar Panel 2 (-Y) Current: %u mA\r\n",adc_solar_cs2);
			printf("- Solar Panel 3 (+Z) Current: %u mA\r\n",adc_solar_cs3);
			printf("- Solar Panel 4 (-Z) Current: %u mA\r\n",adc_solar_cs4);
			printf("- Solar Panel 5 (-X) Current: %u mA\r\n",adc_solar_cs5);
			printf("- Total Current Consumption: %u mA\r\n",adc_bus_cs);
			printf("- Battery Temp: %u mV ? Celsius Degree\r\n",adc_batTemp);
			printf("- Supercap Temp: %u mV ? Celsius Degree\r\n",adc_supercapTemp);
			end_report();

			obc_dataAcquire();
			obc_reportSendDataToBeacon();

		}

		if (readChar =='a')
		{
			// supercap powerpath connect
			P4OUT ^= SCOUT_EN_OUT_PIN;


		}
		if(readChar =='s')
		{
			// buffer enable
			P2OUT &= ~IO_OE_PIN;

			if(P1IN & SC_ON_IN_PIN)
			{
				P5OUT |= RS485_DE_PIN;
				__delay_cycles(500000);
				printf("SCPPOn\r\n");
				__delay_cycles(500000);
				P5OUT &= ~RS485_DE_PIN;
			}
			else
			{
				P5OUT |= RS485_DE_PIN;
				__delay_cycles(500000);
				printf("SCPPOff\r\n");
				__delay_cycles(500000);
				P5OUT &= ~RS485_DE_PIN;
			}
			P2OUT |= IO_OE_PIN;
		}



		if (readChar=='d')
		{

			// supercap charge enable
			P4OUT ^= SCCHG_EN_OUT_PIN;


		}

		if(readChar =='l')
		{
			P4OUT ^= LED_OFF_PIN;
			volatile uint16_t adc_vsupercap;
			adg708_setChannel(MUX_VSUPERCAP);
			adc_vsupercap = adc12_readChannel(1) * 0.8056 * 2;

			interface_txEnable();
			printf("Super Cap Voltage: %u mV\r\n", adc_vsupercap);
			interface_txDisable();
		}

		if(readChar =='q')
		{
//    			 buffer enable
			P2OUT &= ~IO_OE_PIN;

			if(P1IN & BAT_ON_IN_PIN)
			{
				P5OUT |= RS485_DE_PIN;
				__delay_cycles(500000);
				printf("Conduct\r\n");
				__delay_cycles(500000);
				P5OUT &= ~RS485_DE_PIN;
			}
			else
			{
				P5OUT |= RS485_DE_PIN;
				__delay_cycles(500000);
				printf("NotConduct\r\n");
				__delay_cycles(500000);
				P5OUT &= ~RS485_DE_PIN;
			}

			P2OUT |= IO_OE_PIN;
		}

		if(readChar == 'z')
		{
			P4OUT ^= VEXT_OFF_PIN;
		}

//		if (readChar == 'c')
//		{
//			P4OUT ^= ANT_DEPLOY2_PIN;
//		}
//
//		if (readChar == 'v')
//		{
//			P4OUT ^= ANT_DEPLOY1_PIN;
//		}

	}

}

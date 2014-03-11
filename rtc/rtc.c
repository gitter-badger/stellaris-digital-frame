/*
 * rtc.c
 *
 *  Created on: 24 ���� 2014
 *      Author: Itay Komemy
 */
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "../touch/xpt2046.h" // Just for SPI speed value
#include "rtc.h"

#define BCD_TO_INT(X) ((X) & 0x0F) + (10*((X) >> 4))
#define INT_TO_BCD(X) ((X) - ((X)/10)*10) | (((X) / 10) << 4)

void (*_ds3234_init)();
uint8_t (*_ds3234_transfer)(uint8_t);
void (*_ds3234_slave_select)();
void (*_ds3234_slave_unselect)();

void rtc_deselect()
{
	GPIOPinWrite(RTC_CS_PORT_BASE, RTC_CS_PIN, RTC_CS_PIN);
	SSIDisable(RTC_SPI_PORT_BASE);
	SSIConfigSetExpClk(RTC_SPI_PORT_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
			SSI_MODE_MASTER, TOUCH_SPI_MAX_SPEED, 8);
	/* Deassert the chip select */
	SSIEnable(RTC_SPI_PORT_BASE);
}

void rtc_select()
{
	SSIDisable(RTC_SPI_PORT_BASE);
	SSIConfigSetExpClk(RTC_SPI_PORT_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_1,
			SSI_MODE_MASTER, RTC_SPI_MAX_SPEED, 8);
	SSIEnable(RTC_SPI_PORT_BASE);
	GPIOPinWrite(RTC_CS_PORT_BASE, RTC_CS_PIN, 0);
//	SysCtlDelay(10);
}

//
// write/read data to rtc spi
//
uint8_t rtc_sendCommand(uint8_t cmd)
{
	unsigned long dataToRead;
	SSIDataPut(RTC_SPI_PORT_BASE, cmd);

	SSIDataGet(RTC_SPI_PORT_BASE, &dataToRead);

	return dataToRead;
}

void rtc_init()
{
	_ds3234_slave_select = rtc_select;
	_ds3234_slave_unselect = rtc_deselect;
	_ds3234_transfer = rtc_sendCommand;

	/* Enable the peripherals used */
	SysCtlPeripheralEnable(RTC_SPI_PERIPH);
	SysCtlPeripheralEnable(RTC_GPIO_PERIPH);
	SysCtlPeripheralEnable(RTC_CS_PERIPH);
	SysCtlDelay(10);

	/*
	 * Configure the appropriate pins to be SSI instead of GPIO. The CS
	 * signal is directly driven.
	 */
	GPIOPinConfigure(GPIO_PF2_SSI1CLK);
	GPIOPinConfigure(GPIO_PF4_SSI1RX);
	GPIOPinConfigure(GPIO_PF5_SSI1TX);
	GPIOPinTypeSSI(RTC_GPIO_PORT_BASE,
			RTC_MISO_PIN | RTC_MOSI_PIN | RTC_CLK_PIN);
	GPIOPinTypeGPIOOutput(RTC_CS_PORT_BASE, RTC_CS_PIN);

	GPIOPadConfigSet(RTC_GPIO_PORT_BASE,
			RTC_CS_PIN | RTC_MOSI_PIN | RTC_CLK_PIN, GPIO_STRENGTH_4MA,
			GPIO_PIN_TYPE_STD);
	GPIOPadConfigSet(RTC_GPIO_PORT_BASE, RTC_MISO_PIN, GPIO_STRENGTH_4MA,
			GPIO_PIN_TYPE_STD_WPU);

	/* Deassert the chip select */
	GPIOPinWrite(RTC_CS_PORT_BASE, RTC_CS_PIN, RTC_CS_PIN);

	/* Configure the SPI port */
	SSIConfigSetExpClk(RTC_SPI_PORT_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_1,
			SSI_MODE_MASTER, RTC_SPI_MAX_SPEED, 8);
	SSIEnable(RTC_SPI_PORT_BASE);
}

void ds3234_read_time(DS3234_TIME *time)
{
	_ds3234_slave_select();
	_ds3234_transfer(0x00); //Seconds register
	uint8_t data = _ds3234_transfer(0x00); //Dummy byte to receive seconds register
	time->seconds = BCD_TO_INT(data);
	data = _ds3234_transfer(0x00); //Dummy byte to burst receive minutes register
	time->minutes = BCD_TO_INT(data);
	data = _ds3234_transfer(0x00); //Dummy byte to burst reveive hours register
	if (data & (1 << 6))
	{
		//Register is in 12 hour mode
		//If bit 4 is set (10hr bit) increment hour by 10 (Captain Obvious!)
		time->hours = (data & (1 << 4)) ? (data & 0x0F) + 10 : (data & 0x0F);
		time->ampm_mask = 1;
		time->ampm_mask |= (data & (1 << 5) >> 4); //bit 5 (AM/PM) of date shifted to bit 1 of ampm_mask
	}
	else
	{
		//Register is in 24 hour mode
		//Bit 6 and 7 are guaranteed to be zero in this mode
		time->hours = BCD_TO_INT(data);
		time->ampm_mask = 0;
	}
	_ds3234_slave_unselect();
}

void ds3234_write_time(DS3234_TIME *time)
{
	_ds3234_slave_select();
	_ds3234_transfer(0x80); //Write seconds register address
	_ds3234_transfer(INT_TO_BCD(time->seconds)); //Write seconds register
	_ds3234_transfer(INT_TO_BCD(time->minutes)); //Burst-write minutes register
	if (time->ampm_mask)
	{
		//The clock is in 12-hour mode;
		_ds3234_transfer(
				INT_TO_BCD(time->hours) | (1 << 6)
						| ((time->ampm_mask & 2) << 5));
		//Burst-write hours register, bit 6 meaning we are in 12 hours mode and bit 5 AM/PM value
	}
	else
	{
		//The clock is in 24-hour mode
		_ds3234_transfer(INT_TO_BCD(time->hours)); //Burst-write hours register
	}
	_ds3234_slave_unselect();
}

void ds3234_read_date(DS3234_DATE *date)
{
	uint8_t data;
	_ds3234_slave_select();
	_ds3234_transfer(0x03); //Day register address
	data = _ds3234_transfer(0x00); //Read day register
	date->day_of_week = BCD_TO_INT(data);
	data = _ds3234_transfer(0x00); //Burst-read day register
	date->day_of_month = BCD_TO_INT(data); //Burst-read date register
	data = _ds3234_transfer(0x03); //Burst-read month and century register
	date->month = BCD_TO_INT(data & 0x7F); //Ommit the century bit
	date->control = 0;
	if (data & 0x80)
		date->control |= 1;
	data = _ds3234_transfer(0x00); //Burst-read year register
	date->year = BCD_TO_INT(data);
	_ds3234_slave_unselect();
}

void ds3234_write_date(DS3234_DATE *date)
{
	_ds3234_slave_select();
	_ds3234_transfer(0x83); //Write day register address
	_ds3234_transfer(INT_TO_BCD(date->day_of_week));
	_ds3234_transfer(INT_TO_BCD(date->day_of_month));
	_ds3234_transfer(INT_TO_BCD(date->month));
	_ds3234_transfer(INT_TO_BCD(date->year));
	_ds3234_slave_unselect();
}

int16_t ds3234_read_temp()
{
	int16_t temperature;
	_ds3234_slave_select();
	_ds3234_transfer(0x11); //Temperature register MSB
	uint8_t msb = _ds3234_transfer(0x00); //Read MSB
	temperature = ((msb & 128) << 15);
	temperature |= ((msb & 127) << 2);
	temperature |= (_ds3234_transfer(0x00) >> 6); //Burst-read LSB
	_ds3234_slave_unselect();
	return temperature;
}

void ds3234_trigger_conversion()
{
	uint8_t control;
	_ds3234_slave_select();
	_ds3234_transfer(0x0E); //Control register
	control = _ds3234_transfer(0x00); //Read control register
	_ds3234_slave_unselect();

	control |= (1 << DS3234_CONTROL_CONV);

	_ds3234_slave_select();
	_ds3234_transfer(0x8E); //Control register write
	_ds3234_transfer(control); //Write
	_ds3234_slave_unselect();
}

uint8_t ds3234_read_register(uint8_t address)
{
	_ds3234_slave_select();
	_ds3234_transfer(address);
	int8_t data = _ds3234_transfer(0x00); //Read the register
	_ds3234_slave_unselect();
	return data;
}

void ds3234_write_register(uint8_t address, uint8_t data)
{
	_ds3234_slave_select();
	_ds3234_transfer(address | 0x80); //First byte of write address is one
	_ds3234_transfer(data);
	_ds3234_slave_unselect();
}

void ds3234_read_alarm1(DS3234_DATE *date, DS3234_TIME *time,
		uint8_t *alarm_mask)
{
	uint8_t seconds, minutes, hour, day;
	_ds3234_slave_select();
	_ds3234_transfer(0x07); //Alarm 1 seconds register;
	seconds = _ds3234_transfer(0x00);
	minutes = _ds3234_transfer(0x00);
	hour = _ds3234_transfer(0x00);
	day = _ds3234_transfer(0x00); //Burst-read alarm registers
	_ds3234_slave_unselect();

	date->control = 1; //day_of_week and day_of_month can not be both valid.

	*alarm_mask = 0;
	if (seconds & 0x80)
		*alarm_mask |= 0x01;
	if (minutes & 0x80)
		*alarm_mask |= 0x02;
	if (hour & 0x80)
		*alarm_mask |= 0x04;
	if (day & 0x80)
		*alarm_mask |= 0x08;
	if (day & 0x40)
	{
		*alarm_mask |= 0x10;
		date->control |= 2; //day_of_week is valid.
		day &= 0x3F;
		date->day_of_week = BCD_TO_INT(day);
	}
	else
	{
		day &= 0x3F;
		date->day_of_month = BCD_TO_INT(day);
	} //Extract settings mask from registers

	seconds &= 0x7F;
	minutes &= 0x7F;
	hour &= 0x7F; //Remove settings mask from registers

	time->seconds = BCD_TO_INT(seconds);
	time->minutes = BCD_TO_INT(minutes);

	if (hour & (1 << 6))
	{
		//Register is in 12 hour mode
		//If bit 4 is set (10hr bit) increment hour by 10 (Captain Obvious!)
		time->hours = (hour & (1 << 4)) ? (hour & 0x0F) + 10 : (hour & 0x0F);
		time->ampm_mask = 1;
		time->ampm_mask |= (hour & (1 << 5) >> 4); //bit 5 (AM/PM) of date shifted to bit 1 of ampm_mask
	}
	else
	{
		//Register is in 24 hour mode
		//Bit 6 and 7 are guaranteed to be zero in this mode
		time->hours = BCD_TO_INT(hour);
		time->ampm_mask = 0;
	}

}

void ds3234_write_alarm1(DS3234_DATE *date, DS3234_TIME *time,
		uint8_t alarm_mask)
{
	uint8_t seconds, minutes, hour, day;

	seconds = INT_TO_BCD(time->seconds);
	minutes = INT_TO_BCD(time->minutes);
	if (time->ampm_mask)
	{
		//The clock is in 12-hour mode;
		hour = INT_TO_BCD(time->hours) | (1 << 6)
				| ((time->ampm_mask & 2) << 5);
		//Burst-write hours register, bit 6 meaning we are in 12 hours mode and bit 5 AM/PM value
	}
	else
	{
		//The clock is in 24-hour mode
		hour = INT_TO_BCD(time->hours);
	}

	if (date->control & 2)
	{
		//Day of week is valid
		day = INT_TO_BCD(date->day_of_week);
		day |= (1 << 6); //Set Day (of week) bit in the day register
	}
	else
	{
		//Day of month is valid
		day = INT_TO_BCD(date->day_of_month);
	} //Prepare values to the registers

	if (alarm_mask & 1)
		seconds |= (1 << 7);
	if (alarm_mask & 2)
		minutes |= (1 << 7);
	if (alarm_mask & 4)
		hour |= (1 << 7);
	if (alarm_mask & 8)
		day |= (1 << 7);
	if (alarm_mask & 0x10)
		day |= (1 << 6);
	//Superimpose alarm settings mask to the registers

	_ds3234_slave_select();
	_ds3234_transfer(0x87); //Alarm 1 seconds register write
	_ds3234_transfer(seconds);
	_ds3234_transfer(minutes);
	_ds3234_transfer(hour);
	_ds3234_transfer(day);
	_ds3234_slave_unselect(); //Burst-write alarm 1 registers
}

void ds3234_read_alarm2(DS3234_DATE *date, DS3234_TIME *time,
		uint8_t *alarm_mask)
{
	uint8_t minutes, hour, day;

	_ds3234_slave_select();
	_ds3234_transfer(0x0B);
	minutes = _ds3234_transfer(0x00);
	hour = _ds3234_transfer(0x00);
	day = _ds3234_transfer(0x00); //Burst-read alarm 2 registers
	_ds3234_slave_unselect();

	*alarm_mask = 0;
	date->control = 1; //day_of_month and day_of_week can't be both valid

	if (minutes & 0x80)
		*alarm_mask |= 0x01;
	if (hour & 0x80)
		*alarm_mask |= 0x02;
	if (day & 0x80)
		*alarm_mask |= 0x04;
	if (day & 0x40)
	{
		*alarm_mask |= 0x08;
		date->control |= 2; //day_of_week is valid.
		day &= 0x3F;
		date->day_of_week = BCD_TO_INT(day);
	}
	else
	{
		day &= 0x3F;
		date->day_of_month = BCD_TO_INT(day);
	} //Extract settings mask from registers

	minutes &= 0x7F;
	hour &= 0x7F; //Remove settings mask from registers

	time->minutes = BCD_TO_INT(minutes);

	if (hour & (1 << 6))
	{
		//Register is in 12 hour mode
		//If bit 4 is set (10hr bit) increment hour by 10 (Captain Obvious!)
		time->hours = (hour & (1 << 4)) ? (hour & 0x0F) + 10 : (hour & 0x0F);
		time->ampm_mask = 1;
		time->ampm_mask |= (hour & (1 << 5) >> 4); //bit 5 (AM/PM) of date shifted to bit 1 of ampm_mask
	}
	else
	{
		//Register is in 24 hour mode
		//Bit 6 and 7 are guaranteed to be zero in this mode
		time->hours = BCD_TO_INT(hour);
		time->ampm_mask = 0;
	}
}

void ds3234_write_alarm2(DS3234_DATE *date, DS3234_TIME *time,
		uint8_t alarm_mask)
{
	uint8_t minutes, hour, day;

	minutes = INT_TO_BCD(time->minutes);
	if (time->ampm_mask)
	{
		//The clock is in 12-hour mode;
		hour = INT_TO_BCD(time->hours) | (1 << 6)
				| ((time->ampm_mask & 2) << 5);
		//Burst-write hours register, bit 6 meaning we are in 12 hours mode and bit 5 AM/PM value
	}
	else
	{
		//The clock is in 24-hour mode
		hour = INT_TO_BCD(time->hours);
	}

	if (date->control & 2)
	{
		//Day of week is valid
		day = INT_TO_BCD(date->day_of_week);
	}
	else
	{
		//Day of month is valid
		day = INT_TO_BCD(date->day_of_month);
	} //Prepare values to the registers

	if (alarm_mask & 1)
		minutes |= (1 << 7);
	if (alarm_mask & 2)
		hour |= (1 << 7);
	if (alarm_mask & 4)
		day |= (1 << 7);
	if (alarm_mask & 8)
		day |= (1 << 6);
	//Superimpose alarm settings mask to the registers

	_ds3234_slave_select();
	_ds3234_transfer(0x8B); //Alarm 2 minutes register write
	_ds3234_transfer(minutes);
	_ds3234_transfer(hour);
	_ds3234_transfer(day);
	_ds3234_slave_unselect(); //Burst-write alarm 2 registers
}

void ds3234_read_RAM(uint8_t address, uint8_t length, uint8_t *data)
{
	_ds3234_slave_select();
	_ds3234_transfer(0x98); //SRAM Address register
	_ds3234_transfer(address); //SRAM address
	_ds3234_slave_unselect();

	_ds3234_slave_select();
	_ds3234_transfer(0x19); //SRAM Data register
	while (length--)
	{
		*data = _ds3234_transfer(0x00); //Burst transfer contents of the register.
		//During this transfer the Register address won't change, hovewer, SRAM adress in
		//SRAM address register will increment automatically, see DS3234's datasheet page 17.
		data++;
	}
	_ds3234_slave_unselect();
}

void ds3234_write_RAM(uint8_t address, uint8_t length, uint8_t *data)
{
	_ds3234_slave_select();
	_ds3234_transfer(0x98); //SRAM Address register
	_ds3234_transfer(address); //SRAM address
	_ds3234_slave_unselect();

	_ds3234_slave_select();
	_ds3234_transfer(0x99); //SRAM write data register
	while (length--)
	{
		_ds3234_transfer(*data); //Burst transfer contents of the register.
		//During this transfer the Register address won't change, hovewer, SRAM adress in
		//SRAM address register will increment automatically, see DS3234's datasheet page 17.
		data++;
	}
	_ds3234_slave_unselect();
}


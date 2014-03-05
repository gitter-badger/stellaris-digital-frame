/*
 * rtc.h
 *
 *  Created on: 24 αταψ 2014
 *      Author: Itay Komemy
 */

#ifndef RTC_H_
#define RTC_H_

#define RTC_SPI_PORT_BASE   	SSI1_BASE
#define RTC_SPI_PERIPH   		SYSCTL_PERIPH_SSI1

#define RTC_GPIO_PERIPH 		SYSCTL_PERIPH_GPIOF
#define RTC_GPIO_PORT_BASE		GPIO_PORTF_BASE

#define RTC_MISO_PIN			GPIO_PIN_4
#define RTC_MOSI_PIN			GPIO_PIN_5
#define RTC_CLK_PIN				GPIO_PIN_2

#define RTC_CS_PERIPH			SYSCTL_PERIPH_GPIOH
#define RTC_CS_PORT_BASE		GPIO_PORTH_BASE
#define RTC_CS_PIN				GPIO_PIN_1

void rtc_init();
//char rtc_setDatetime(DateTime_t datetime);
//char rtc_getDatetime(DateTime_t *datetime);

//Please consult DS3234's datasheet on meaning and proper usage of these constants
#define DS3234_ALARM1_ONCE_PER_SECOND 0x0F
#define DS3234_ALARM1_SECONDS_MATCH 0x0E
#define DS3234_ALARM1_MINUTES_SECONDS_MATCH 0x0C
#define DS3234_ALARM1_HOURS_MINUTES_SECONDS_MATCH 0x08
#define DS3234_ALARM1_DATE_HOURS_MINUTES_SECONDS_MATCH 0x00
#define DS3234_ALARM1_DAY_HOURS_MINUTES_SECONDS_MATCH 0x10

#define DS3234_ALARM2_ONCE_PER_MINUTE 0x07
#define DS3234_ALARM2_MINUTES_MATCH 0x06
#define DS3234_ALARM2_HOURS_MINUTES_MATCH 0x04
#define DS3234_ALARM2_DATE_HOUR_MINUTES_MATCH 0x00
#define DS3234_ALARM2_DAY_HOURS_MINUTES_MATCH 0x08

#define DS3234_CONTROL_EOSC 7
#define DS3234_CONTROL_BBSQW 6
#define DS3234_CONTROL_CONV 5
#define DS3234_CONTROL_RS2 4
#define DS3234_CONTROL_RS1 3
#define DS3234_CONTROL_INTCN 2
#define DS3234_CONTROL_A2IE 1
#define DS3234_CONTROL_A1IE 0

#define DS3234_CONSTAT_OSF 7
#define DS3234_CONSTAT_BB32KHZ 6
#define DS3234_CONSTAT_CRATE1 5
#define DS3234_CONSTAT_CRATE0 4
#define DS3234_CONSTAT_EN32KHZ 3
#define DS3234_CONSTAT_BSY 2
#define DS3234_CONSTAT_A2F 1
#define DS3234_CONSTAT_A1F 0

#define DS3234_REG_CONTROL 0x0E
#define DS3234_REG_CTRL_STATUS 0x0F
#define DS3234_REG_CRYSTAL_AGING 0x10
#define DS3234_REG_VBAT_TMP_DISABLE 0x13

/**
 * A structure holding time in a format with which the DS3234 is working.
 * ampm_mask holds information about whether the clock is in 12/24 mode (bit 0)
 * (1 if 12 mode, 0 if 24 mode)
 * and if in 12 mode bit 1 denotes AM/PM mode (0: AM, 1: PM)
 */
typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t ampm_mask;
} DS3234_TIME;

/**
 * A structure holding time in a format with which the DS3234 is working.
 * control holds a century bit (bit 0). If bit 1 is set only one of day_of_week
 * or day_of_month values are valid, and in that case bit 2 denotes which one
 * (0: day_of_week, 1: day_of_month)
 */
typedef struct {
	uint8_t day_of_week;
	uint8_t day_of_month;
	uint8_t month;
	uint8_t year;
	uint8_t control;
} DS3234_DATE;

/**
 * A function for reading time from DS3234's main time register.
 */
void ds3234_read_time(DS3234_TIME *time);

/**
 * A function for writing time to DS3234's main time register.
 * Please note that this function does not do any error checking, so
 * please make sure the values are valid and in range.
 */
void ds3234_write_time(DS3234_TIME *time);

/**
 * A function for reading date from DS3234's main date register.
 * In returned DS3234_DATE structure both day_of_week and day_of_month are valid.
 */
void ds3234_read_date(DS3234_DATE *date);

/**
 * A function for writing date to DS3234's main date register.
 * Please note that this function does not do any error checking. Both day_of_week
 * and day_of_month must be valid.
 */
void ds3234_write_date(DS3234_DATE *date);

/**
 * A function for reading temperature registers of DS3234.
 * See DS3234's datasheet page 16 for the format description.
 */
int16_t ds3234_read_temp();

/**
 * A function for triggering temperature conversion
 */
void ds3234_trigger_conversion();

/**
 * A function for reading a single register.
 */
uint8_t ds3234_read_register(uint8_t address);

/**
 * A function for writing a single register.
 * You may also use the read address, as this is converted to write address if necessary.
 */
void ds3234_write_register(uint8_t address, uint8_t data);

/**
 * A function for reading alarm 1 from DS3234.
 */
void ds3234_read_alarm1(DS3234_DATE *date, DS3234_TIME *time, uint8_t *alarm_mask);

/**
 * A function for writing Alarm 1 to DS3234.
 * Please note that this won't actualy enable the alarm, for that you need to modify
 * the control register. See DS3234's datasheet for more details.
 */
void ds3234_write_alarm1(DS3234_DATE *date, DS3234_TIME *time, uint8_t alarm_mask);

/**
 * A function for reading alarm 2 from DS3234.
 */
void ds3234_read_alarm2(DS3234_DATE *date, DS3234_TIME *time, uint8_t *alarm_mask);

/**
 * A function for writing Alarm 2 to DS3234.
 * Please note that this won't actualy enable the alarm, for that you need to modify
 * the control register. See DS3234's datasheet for more details.
 */
void ds3234_write_alarm2(DS3234_DATE *date, DS3234_TIME *time, uint8_t alarm_mask);

/**
 * A function for reading RAM of DS3234.
 * Make sure the *data is big enough for length of uint8_t's. If address + length > 256
 * the address will overflow and data from beginning will be read.
 */
void ds3234_read_RAM(uint8_t address, uint8_t length, uint8_t *data);

/**
 * A function for writing RAM of DS3234.
 * Make sure the *data is big enough for length of uint8_t's. If address + length > 256
 * the address will overflow and data will be written to the beginning of the RAM.
 */
void ds3234_write_RAM(uint8_t address, uint8_t length, uint8_t *data);

#endif /* RTC_H_ */

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

#endif /* RTC_H_ */

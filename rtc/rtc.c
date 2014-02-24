/*
 * rtc.c
 *
 *  Created on: 24 αταψ 2014
 *      Author: Itay Komemy
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "rtc.h"

void rtc_deselect()
{
	/* Deassert the chip select */
	GPIOPinWrite(RTC_CS_PORT_BASE, RTC_CS_PIN, RTC_CS_PIN);
}

void rtc_select()
{
	GPIOPinWrite(RTC_CS_PORT_BASE, RTC_CS_PIN, 0);
}

void rtc_init()
{
	/* Enable the peripherals used */
	SysCtlPeripheralEnable(RTC_SPI_PERIPH);
	SysCtlPeripheralEnable(RTC_GPIO_PERIPH);
	SysCtlPeripheralEnable(RTC_CS_PERIPH);


	/*
	 * Configure the appropriate pins to be SSI instead of GPIO. The CS
	 * signal is directly driven.
	 */
	GPIOPinTypeSSI(RTC_GPIO_PORT_BASE,
			RTC_MISO_PIN | RTC_MOSI_PIN | RTC_CLK_PIN);
	GPIOPinTypeGPIOOutput(RTC_CS_PORT_BASE, RTC_CS_PIN);

	rtc_deselect();

	GPIOPadConfigSet(RTC_GPIO_PORT_BASE, RTC_CS_PIN | RTC_MOSI_PIN | RTC_CLK_PIN,
			GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
	GPIOPadConfigSet(RTC_GPIO_PORT_BASE, RTC_MISO_PIN, GPIO_STRENGTH_4MA,
			GPIO_PIN_TYPE_STD_WPU);

	/* Configure the SPI port */
	SSIConfigSetExpClk(RTC_SPI_PORT_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
			SSI_MODE_MASTER, 200000, 8);
	SSIEnable(RTC_SPI_PORT_BASE);

}

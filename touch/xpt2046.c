/*
 * xpt2046.c
 *
 *  Created on: 21 αταψ 2014
 *      Author: Itay
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"

#include "xpt2046.h"

float mx = 0.1364;
float bx = -18.8864;
float my = 0.1810;
float by = -21.8100;

static long (*g_pfnTSHandler)(unsigned long ulMessage, long lX, long lY);

void xpt2046_select()
{
	GPIOPinWrite(TOUCH_SPI_PORT_BASE, TOUCH_CS_PIN, 0);
}

void xpt2046_deselect()
{
	GPIOPinWrite(TOUCH_SPI_PORT_BASE, TOUCH_CS_PIN, TOUCH_CS_PIN);
}

//write data to XPT2046 (same as ADS7843)
static unsigned char xpt2046_sendCommand(unsigned char cmd)
{
	unsigned long dataToRead;
	SSIDataPut(TOUCH_SPI_PORT_BASE, cmd);

	SSIDataGet(TOUCH_SPI_PORT_BASE, &dataToRead);

	if (dataToRead) printf("GOT DATA DIFF FROM ZERO: %d !!!",dataToRead);
	return dataToRead;
}

//(same as ADS7843) Return a byte of data
static unsigned char xpt2046_readByte()
{
	unsigned char result;
	result = xpt2046_sendCommand(0x0A);
	return result;
}

void xpt2046_init()
{
	g_pfnTSHandler = 0;

	/* Enable the peripherals used */
	SysCtlPeripheralEnable(TOUCH_GPIO_PERIPH);
	SysCtlPeripheralEnable(TOUCH_SPI_PERIPH);
	SysCtlPeripheralEnable(TOUCH_IRQ_PERIPH);
	SysCtlDelay(10);

	/* Set IRQ stuff */

	// turn weak pull-ups on
	GPIOPadConfigSet(TOUCH_IRQ_BASE, TOUCH_IRQ_PIN, GPIO_STRENGTH_2MA,
			GPIO_PIN_TYPE_STD_WPU);
	GPIOIntTypeSet(TOUCH_IRQ_BASE, TOUCH_IRQ_PIN, GPIO_FALLING_EDGE);
	IntEnable(TOUCH_IRQ_INT);
	GPIOPinIntEnable(TOUCH_IRQ_BASE, TOUCH_IRQ_PIN);

	//
	// Enable Interrupts
	//
	IntMasterEnable();

	GPIOPinTypeGPIOOutput(TOUCH_SPI_PORT_BASE, TOUCH_CS_PIN);
	xpt2046_deselect();

	/*
	 * Configure the appropriate pins to be SSI instead of GPIO. The CS
	 * signal is directly driven.
	 */


	GPIOPadConfigSet(TOUCH_GPIO_PORT_BASE, TOUCH_MISO_PIN, GPIO_STRENGTH_4MA,
			GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(TOUCH_GPIO_PORT_BASE,
			TOUCH_CS_PIN | TOUCH_MOSI_PIN | TOUCH_CLK_PIN, GPIO_STRENGTH_4MA,
			GPIO_PIN_TYPE_STD);
	GPIOPinTypeSSI(TOUCH_GPIO_PORT_BASE,
				TOUCH_MISO_PIN | TOUCH_MOSI_PIN | TOUCH_CLK_PIN);

	/* Configure the SPI port */
	SSIConfigSetExpClk(TOUCH_SPI_PORT_BASE, SysCtlClockGet(),
			SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 200000, 8);
	SSIEnable(TOUCH_SPI_PORT_BASE);

	xpt2046_select();
//	 Make a dummy read just so IRQ will be active
	xpt2046_sendCommand(0x90);
	unsigned char read;
	read = xpt2046_readByte();
	read = xpt2046_readByte();

}

void read_touch_ADC(unsigned short int* x, unsigned short int* y)
{
	unsigned long delay_1us;
	delay_1us = SysCtlClockGet() / 3000000;
	unsigned char data_x[] =
	{ 0xD0, 0x00, 0x00 };
	unsigned char data_y[] =
	{ 0x90, 0x00, 0x00 };
	// Send 'read x' command and read 2 bytes
	xpt2046_sendCommand(data_x[0]);
	SysCtlDelay(delay_1us * 2);
	data_x[1] = xpt2046_readByte();
	data_x[2] = xpt2046_readByte();
	SysCtlDelay(delay_1us * 2);
	xpt2046_sendCommand(data_y[0]);
	SysCtlDelay(delay_1us * 2);
	data_y[1] = xpt2046_readByte();
	data_y[2] = xpt2046_readByte();

	*x = data_x[1];
	*x = ((*x) << 8) + data_x[2];
	*x >>= 4;
	*y = data_y[1];
	*y = ((*y) << 8) + data_y[2];
	*y >>= 4;
	printf("X %d Y %d\n", *x, *y);
}

void convert_ADC_to_xy(unsigned short int adc_x, unsigned short int adc_y,
		unsigned short int* px_x, unsigned short int* px_y)
{

	*px_x = (mx * (float) adc_x + bx);
	*px_y = (my * (float) adc_y + by);
}

int xpt2046_getTouchPosition(unsigned short int* x, unsigned short int* y)
{
	unsigned short int adc_x, adc_y;
	read_touch_ADC(&adc_x, &adc_y);
	convert_ADC_to_xy(adc_x, adc_y, x, y);
	if (adc_x == 0 | adc_y == 0)
		return 0;
	else
		return 1;
}

void xpt2046_clearIRQ()
{
	GPIOPinIntClear(TOUCH_IRQ_BASE, TOUCH_IRQ_PIN);
}

void TouchScreenIntHandler()
{
	xpt2046_clearIRQ();

	//Do your thing
	unsigned short x = 0, y = 0;
	if (xpt2046_getTouchPosition(&x, &y) == 1)
	{
		if (g_pfnTSHandler)
		{
			//
			// Send the pen move message to the touch screen event
			// handler.
			//
			g_pfnTSHandler(WIDGET_MSG_PTR_DOWN, x, y);
			g_pfnTSHandler(WIDGET_MSG_PTR_UP, x, y);
		}
	}

}

void xpt2046_setTouchScreenCallback(
		long (*pfnCallback)(unsigned long ulMessage, long lX, long lY))
{
	g_pfnTSHandler = pfnCallback;
}

void xpt2046_enableTouchIRQ()
{
	GPIOPinIntEnable(TOUCH_IRQ_BASE, TOUCH_IRQ_PIN);
}

void xpt2046_disableTouchIRQ()
{
	GPIOPinIntDisable(TOUCH_IRQ_BASE, TOUCH_IRQ_PIN);
}

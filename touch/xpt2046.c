/*
 * xpt2046.c
 *
 *  Created on: 21 αταψ 2014
 *      Author: Itay
 */

#include <stdio.h>
#include "driverlib/pin_map.h"
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

static long (*g_pfnTSHandler)(unsigned long ulMessage, long lX, long lY);

double cal_x = (double)SCREEN_WIDTH / (double)TOUCH_X1;
double cal_y = (double)SCREEN_HIEGHT / (double)TOUCH_Y1;

unsigned char xpt2046_readIRQ()
{
	return GPIOPinRead(TOUCH_IRQ_BASE,TOUCH_IRQ_PIN);
}

void xpt2046_select()
{
	GPIOPinWrite(TOUCH_GPIO_PORT_BASE, TOUCH_CS_PIN, 0);
}

void xpt2046_deselect()
{
	GPIOPinWrite(TOUCH_GPIO_PORT_BASE, TOUCH_CS_PIN, TOUCH_CS_PIN);
}

//write data to XPT2046 (same as ADS7843)
static unsigned char xpt2046_sendCommand(unsigned char cmd)
{
	unsigned long dataToRead;
	SSIDataPut(TOUCH_SPI_PORT_BASE, cmd);

	SSIDataGet(TOUCH_SPI_PORT_BASE, &dataToRead);

	return dataToRead;
}

unsigned int _xpt2046_get_reading( unsigned char control )
{
	xpt2046_select();

	unsigned char rData[3] = { 0 , 0 , 0 };

	rData[0] = xpt2046_sendCommand(control);
	rData[1] = xpt2046_sendCommand(0);
	rData[2] = xpt2046_sendCommand(0);


	xpt2046_deselect();

	if ( ( control & 0x08 ) == 0 ) {
		return ( rData[1] << 5 ) | ( rData[2] >> 3 );
	}
	return ( rData[1] << 4 ) | ( rData[2] >> 4 );

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
	GPIOIntTypeSet(TOUCH_IRQ_BASE, TOUCH_IRQ_PIN, GPIO_LOW_LEVEL);
	IntEnable(TOUCH_IRQ_INT);

	/*
	 * Configure the appropriate pins to be SSI instead of GPIO. The CS
	 * signal is directly driven.
	 */
	GPIOPinConfigure(GPIO_PF2_SSI1CLK);
	GPIOPinConfigure(GPIO_PF4_SSI1RX);
	GPIOPinConfigure(GPIO_PF5_SSI1TX);
	GPIOPinTypeSSI(TOUCH_GPIO_PORT_BASE,
			TOUCH_MISO_PIN | TOUCH_MOSI_PIN | TOUCH_CLK_PIN);
	GPIOPinTypeGPIOOutput(TOUCH_GPIO_PORT_BASE, TOUCH_CS_PIN);
	GPIOPadConfigSet(TOUCH_GPIO_PORT_BASE, TOUCH_MISO_PIN, GPIO_STRENGTH_4MA,
			GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(TOUCH_GPIO_PORT_BASE,
			TOUCH_CS_PIN | TOUCH_MOSI_PIN | TOUCH_CLK_PIN, GPIO_STRENGTH_4MA,
			GPIO_PIN_TYPE_STD);

	// Deassert CS pin for touch
	xpt2046_deselect();
	SysCtlDelay(10);

	/* Configure the SPI port */
	SSIConfigSetExpClk(TOUCH_SPI_PORT_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
			SSI_MODE_MASTER, TOUCH_SPI_MAX_SPEED, 8);
	SSIEnable(TOUCH_SPI_PORT_BASE);

	// Make a dummy read just so IRQ will be active
	xpt2046_select();

	_xpt2046_get_reading(0x90);

	xpt2046_deselect();

	//
	// Enable Interrupts
	//
	IntMasterEnable();
}

void xpt2046GetCoordinates(unsigned int * pX, unsigned int * pY)
{
	int i;
	unsigned int allX[ 7 ] , allY[ 7 ];
	_xpt2046_get_reading( CHX );
	_xpt2046_get_reading( CHY );
	for ( i = 0 ; i < 7 ; i ++ ) {
		allX[ i ] = _xpt2046_get_reading( CHX );
		allY[ i ] = _xpt2046_get_reading( CHY );
	}

	int j;
	for ( i = 0 ; i < 4 ; i ++ ) {
		for ( j = i ; j < 7 ; j ++ ) {
			int temp = allX[ i ];
			if ( temp > allX[ j ] ) {
				allX[ i ] = allX[ j ];
				allX[ j ] = temp;
			}
			temp = allY[ i ];
			if ( temp > allY[ j ] ) {
				allY[ i ] = allY[ j ];
				allY[ j ] = temp;
			}
		}
	}
	_xpt2046_get_reading( CHX );

	*pX = allX[ 3 ];
	*pY = allY[ 3 ];
}

unsigned char xpt2046GetAverageCoordinates(unsigned int * pX , unsigned int * pY , int nSamples )
{
	int nRead = 0;
	unsigned int xAcc = 0 , yAcc = 0;
	unsigned int x , y;

	x = 0;
	y = 0;
	while ( nRead < nSamples ) {
		// data no longer available
		if (xpt2046_readIRQ())
			return 0;

		xpt2046GetCoordinates(&x , &y);

		xAcc += x;
		yAcc += y;
		nRead ++;


		// sprintf(buf, "Added: %u %u (%d)", x, y, nRead);
		// SendUARTStr(buf);

		// no more data available
		if (xpt2046_readIRQ()) break;
	}

	if (nRead >= 3)
	{
		*pX = xAcc / nRead;
		*pY = yAcc / nRead;

		return 1;
	}
	else
		// too few samples collected, ignore
		return 0;
}

void xpt2046_clearIRQ()
{
	GPIOPinIntClear(TOUCH_IRQ_BASE, TOUCH_IRQ_PIN);
}

void TouchScreenIntHandler()
{
	xpt2046_clearIRQ();
	xpt2046_disableTouchIRQ();
	//Do your thing
	unsigned int x = 0, y = 0;
	if (touch_GetCoordinates(&x, &y) == 1)
	{
		if (g_pfnTSHandler)
		{
			//
			// Send the pen move message to the touch screen event
			// handler.
			//
			g_pfnTSHandler(WIDGET_MSG_PTR_DOWN, x, y);
			g_pfnTSHandler(WIDGET_MSG_PTR_UP, x, y);

			printf("Touched x: %d, y: %d\n",x,y);
		}
	}
	printf("Interrupt called\n");

	// Block while pressed
	while(xpt2046_readIRQ() == 0);

	xpt2046_enableTouchIRQ();
	return;
}

unsigned char touch_GetCoordinates(unsigned int * pX, unsigned int * pY)
{
	// get the raw touch coordinates
	unsigned int tX = 0;
	unsigned int tY = 0;
	unsigned char isOK = xpt2046GetAverageCoordinates(&tX, &tY, NUM_OF_SAMPLES);

	// convert to LCD coordinates
	if (isOK)
	{
		*pX = (tX - TOUCH_X0) * cal_x;
		*pY = (tY - TOUCH_Y0) * cal_y;
	}

	return isOK;
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

/*
 * xpt2046.c
 *
 *  Created on: 21 αταψ 2014
 *      Author: Itay
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
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

void xpt2046_Init()
{
	g_pfnTSHandler = 0;

	/* Enable the peripherals used */
	SysCtlPeripheralEnable(TOUCH_SPI_PERIPH);
	SysCtlPeripheralEnable(TOUCH_IRQ_PERIPH);

	/*
	 * Configure the appropriate pins to be SSI instead of GPIO. The CS
	 * signal is directly driven.
	 */
	GPIOPinTypeSSI(TOUCH_SPI_PORT_BASE,
			TOUCH_MISO_PIN | TOUCH_MOSI_PIN | TOUCH_CLK_PIN);
	GPIOPinTypeGPIOOutput(TOUCH_SPI_PORT_BASE, TOUCH_CS_PIN);

	GPIOPadConfigSet(TOUCH_SPI_PORT_BASE, TOUCH_MISO_PIN, GPIO_STRENGTH_4MA,
			GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(TOUCH_SPI_PORT_BASE,
			TOUCH_CS_PIN | TOUCH_MOSI_PIN | TOUCH_CLK_PIN, GPIO_STRENGTH_4MA,
			GPIO_PIN_TYPE_STD);

	/* Deassert the chip select */
	GPIOPinWrite(TOUCH_SPI_PORT_BASE, TOUCH_CS_PIN, TOUCH_CS_PIN);

	/* Configure the SPI port */
	SSIConfigSetExpClk(TOUCH_SPI_PORT_BASE, SysCtlClockGet(),
			SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 400000, 8);
	SSIEnable(TOUCH_SPI_PORT_BASE);

	// Sets the chip to 12 bit sampling.
	unsigned char data[] =
	{ 0x80, 0x00, 0x00 };
	SSIDataPut(TOUCH_SPI_PORT_BASE, data[0]);
	SSIDataPut(TOUCH_SPI_PORT_BASE, data[1]);
	SSIDataPut(TOUCH_SPI_PORT_BASE, data[2]);

	/* Set IRQ stuff */
	GPIOPinTypeGPIOInput(TOUCH_IRQ_BASE, TOUCH_IRQ_PIN);
	GPIOIntTypeSet(TOUCH_IRQ_BASE, TOUCH_IRQ_PIN,
			GPIO_FALLING_EDGE | GPIO_DISCRETE_INT);
}

void read_touch_ADC(unsigned short int* x, unsigned short int* y)
{
	unsigned char data_x[] =
	{ 0xD0, 0x00, 0x00 };
	unsigned char data_y[] =
	{ 0x90, 0x00, 0x00 };
	// Send 'read x' command and read 2 bytes
	SSIDataPut(TOUCH_SPI_PORT_BASE, data_x[0]);
	SSIDataGet(TOUCH_SPI_PORT_BASE, (unsigned long*) (&data_x[1]));
	SSIDataGet(TOUCH_SPI_PORT_BASE, (unsigned long*) (&data_x[2]));

	SSIDataPut(TOUCH_SPI_PORT_BASE, data_y[0]);
	SSIDataGet(TOUCH_SPI_PORT_BASE, (unsigned long*) (&data_y[1]));
	SSIDataGet(TOUCH_SPI_PORT_BASE, (unsigned long*) (&data_y[2]));
	//  printf("x %x %x y %x %x\n",data_x[1],data_x[2],data_y[1],data_y[2]);
	*x = data_x[1];
	*x = ((*x) << 8) + data_x[2];
	*x >>= 4;
	*y = data_y[1];
	*y = ((*y) << 8) + data_y[2];
	*y >>= 4;
	// printf("X %i Y %i\n",*x ,*y);
}

void convert_ADC_to_xy(unsigned short int adc_x, unsigned short int adc_y,
		unsigned short int* px_x, unsigned short int* px_y)
{

	*px_x = (mx * (float) adc_x + bx);
	*px_y = (my * (float) adc_y + by);
}

int xpt2046_GetTouchPosition(unsigned short int* x, unsigned short int* y)
{
	unsigned short int adc_x, adc_y;
	read_touch_ADC(&adc_x, &adc_y);
	convert_ADC_to_xy(adc_x, adc_y, x, y);
	if (adc_x == 0 | adc_y == 0)
		return 0;
	else
		return 1;
}

void xpt2046_ClearIRQ()
{
	GPIOPinIntClear(TOUCH_IRQ_BASE, TOUCH_IRQ_PIN);
}

void TouchScreenIntHandler()
{
	xpt2046_DisableTouchIRQ();
	xpt2046_ClearIRQ();

	//Do your thing
	unsigned short x = 0, y = 0;
	if (xpt2046_GetTouchPosition(&x, &y) == 1)
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
	xpt2046_EnableTouchIRQ();
}

void xpt2046_SetTouchScreenCallback(
		long (*pfnCallback)(unsigned long ulMessage, long lX, long lY))
{
//
// Save the pointer to the callback function.
//
	g_pfnTSHandler = pfnCallback;
}

void xpt2046_EnableTouchIRQ()
{
	GPIOPinIntEnable(TOUCH_IRQ_BASE, TOUCH_IRQ_PIN);
}

void xpt2046_DisableTouchIRQ()
{
	GPIOPinIntDisable(TOUCH_IRQ_BASE, TOUCH_IRQ_PIN);
}

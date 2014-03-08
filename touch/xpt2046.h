/*
 * xpt2046.h
 *
 *  Created on: 21 αταψ 2014
 *      Author: Itay
 */

#ifndef XPT2046_H_
#define XPT2046_H_

#define TOUCH_SPI_PORT_BASE     SSI1_BASE
#define TOUCH_SPI_PERIPH   		SYSCTL_PERIPH_SSI1

#define TOUCH_GPIO_PERIPH		SYSCTL_PERIPH_GPIOF
#define TOUCH_GPIO_PORT_BASE 	GPIO_PORTF_BASE

#define TOUCH_MISO_PIN			GPIO_PIN_4
#define TOUCH_MOSI_PIN			GPIO_PIN_5
#define TOUCH_CLK_PIN			GPIO_PIN_2
#define TOUCH_CS_PIN			GPIO_PIN_3

#define TOUCH_IRQ_PERIPH		SYSCTL_PERIPH_GPIOG
#define TOUCH_IRQ_INT			INT_GPIOG
#define TOUCH_IRQ_BASE			GPIO_PORTG_BASE
#define TOUCH_IRQ_PIN			GPIO_PIN_0

// Maximum and minimum readings
#define TOUCH_X0 350
#define TOUCH_Y0 350
#define TOUCH_X1 3700
#define TOUCH_Y1 3700

// Configure screen size
#define SCREEN_WIDTH 320
#define SCREEN_HIEGHT 240

// Number of samples
#define NUM_OF_SAMPLES 10

/* AD channel selection command and register */
#define	CHX 	0x90 	/* channel Y+ selection command */
#define	CHY 	0xD0	/* channel X+ selection command*/


void xpt2046_init();
void xpt2046_enableTouchIRQ();
void xpt2046_disableTouchIRQ();
extern void TouchScreenIntHandler();
void xpt2046_setTouchScreenCallback(
long (*pfnCallback)(unsigned long ulMessage, long lX, long lY));
unsigned char touch_GetCoordinates(unsigned int* x, unsigned int* y);
#endif /* XPT2046_H_ */

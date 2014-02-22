/*
 * xpt2046.h
 *
 *  Created on: 21 αταψ 2014
 *      Author: Itay
 */

#ifndef XPT2046_H_
#define XPT2046_H_

#define TOUCH_SPI_PERIPH		SYSCTL_PERIPH_GPIOF
#define TOUCH_SPI_PORT_BASE 	GPIO_PORTF_BASE

#define TOUCH_MISO_PIN			GPIO_PIN_4
#define TOUCH_MOSI_PIN			GPIO_PIN_5
#define TOUCH_CLK_PIN			GPIO_PIN_2
#define TOUCH_CS_PIN			GPIO_PIN_3

#define TOUCH_IRQ_PERIPH		SYSCTL_PERIPH_GPIOG
#define TOUCH_IRQ_BASE			GPIO_PORTG_BASE
#define TOUCH_IRQ_PIN			GPIO_PIN_0

void xpt2046_Init();
void xpt2046_EnableTouchIRQ();
void xpt2046_DisableTouchIRQ();
int xpt2046_GetTouchPosition(unsigned short int* x, unsigned short int* y);
void TouchScreenIntHandler();
void xpt2046_SetTouchScreenCallback(
		long (*pfnCallback)(unsigned long ulMessage, long lX, long lY));

#endif /* XPT2046_H_ */

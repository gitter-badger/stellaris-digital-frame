/*
 * grlibDriver.h
 *
 *	Glue functions and structures for Texas Instruments Graphics Library
 *
 *  Created on: 17 αταψ 2014
 *      Author: Itay Komemy
 */

#ifndef GRLIBDRIVER_H_
#define GRLIBDRIVER_H_

#include "ssd1289.h"
#include "grlib/grlib.h"

unsigned long ColorTranslate (void *pvDisplayData, unsigned long ulValue);
void Flush (void *pvDisplayData);
void LineDrawH (void *pvDisplayData, long lX1, long lX2, long lY, unsigned long ulValue);
void LineDrawV (void *pvDisplayData, long lX, long lY1, long lY2, unsigned long ulValue);
void PixelDraw (void *pvDisplayData, long lX, long lY, unsigned long ulValue);
void PixelDrawMultiple (void *pvDisplayData, long lX, long lY, long lX0, long lCount, long lBPP,
		const unsigned char *pucData, const unsigned char *pucPalette);
void RectFill (void *pvDisplayData, const tRectangle *pRect, unsigned long ulValue);

extern const tDisplay DisplayStructure;


#endif /* GRLIBDRIVER_H_ */

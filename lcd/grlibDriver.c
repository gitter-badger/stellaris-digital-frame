/*
 * grlibDriver.c
 *
 *  Created on: 17 αταψ 2014
 *      Author: Itay Komemy
 */
#include "grlibDriver.h"


inline unsigned long ColorTranslate(void *pvDisplayData, unsigned long ulValue)
{
	//
	// Translate from a 24-bit RGB color to a 5-6-5 RGB color.
	//
	unsigned char  r = ulValue >> 16;
	unsigned char  g = ulValue >> 8;
	unsigned char b = ulValue;
	return ssd1289_rgb(r,g,b);
}

void Flush(void *pvDisplayData)
{
	//Do nothing
}

void LineDrawH(void *pvDisplayData, long lX1, long lX2, long lY,
		unsigned long ulValue)
{
	ssd1289_hline(ulValue, lY, lX1, lX2 - lX1);
}

void LineDrawV(void *pvDisplayData, long lX, long lY1, long lY2,
		unsigned long ulValue)
{
	ssd1289_vline(ulValue, lY1, lX, lY2 - lY1);
}
void PixelDraw(void *pvDisplayData, long lX, long lY, unsigned long ulValue)
{
	ssd1289_pixel(ulValue, lY, lX);
}

void PixelDrawMultiple(void *pvDisplayData, long lX, long lY, long lX0,
		long lCount, long lBPP, const unsigned char *pucData,
		const unsigned char *pucPalette)
{

}
void RectFill(void *pvDisplayData, const tRectangle *pRect,
		unsigned long ulValue)
{
	unsigned int x1, x2, y1, y2, trueColor;
	x1 = pRect->sXMin;
	x2 = pRect->sXMax;
	y1 = pRect->sYMin;
	y2 = pRect->sYMax;
	trueColor = ColorTranslate(0,ulValue);

	for (; x1 <=x2; x1++){
		ssd1289_vline(trueColor,y1,x1,y2-y1);
	}
}

const tDisplay DisplayStructure = {
    sizeof(tDisplay),
    0,
#if defined(PORTRAIT_FLIP) || defined(PORTRAIT)
    240, //240
    320, //320
#else
    320, //320
    240, //240 lado corto
#endif
    PixelDraw,
    PixelDrawMultiple,
    LineDrawH,
    LineDrawV,
    RectFill,
    ColorTranslate,
    Flush
};



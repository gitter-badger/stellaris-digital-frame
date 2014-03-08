/*
 * grlibDriver.c
 *
 *  Created on: 17 αταψ 2014
 *      Author: Itay Komemy
 */
#include "grlibDriver.h"

// Translates 24bit RGB to RGB565
unsigned long ColorTranslate(void *pvDisplayData, unsigned long ulValue)
{
	//
	// Translate from a 24-bit RGB color to a 5-6-5 RGB color.
	unsigned char r = ulValue >> 16;
	unsigned char g = ulValue >> 8;
	unsigned char b = ulValue;
	color_t color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
	return color;
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
	unsigned long ulByte;

	//
	// Determine how to interpret the pixel data based on the number of bits
	// per pixel.
	//
	switch (lBPP)
	{
	//
	// The pixel data is in 1 bit per pixel format.
	//
	case 1:
	{
		//
		// Loop while there are more pixels to draw.
		//
		while (lCount)
		{
			//
			// Get the next byte of image data.
			//
			ulByte = *pucData++;

			//
			// Loop through the pixels in this byte of image data.
			//
			for (; (lX0 < 8) && lCount; lX0++, lCount--)
			{
				//
				// Draw this pixel in the appropriate color.
				//
				ssd1289_pixel(
						ColorTranslate(0,(((((unsigned long *) pucPalette)[(ulByte
								>> (7 - lX0)) & 1])))), lY, lX++);
				//WriteData(((unsigned long *)pucPalette)[(ulByte >>(7 - lX0)) & 1]);
			}

			//
			// Start at the beginning of the next byte of image data.
			//
			lX0 = 0;
		}

		//
		// The image data has been drawn.
		//
		break;
	}

		//
		// The pixel data is in 4 bit per pixel format.
		//
	case 4:
	{
		//
		// Loop while there are more pixels to draw.  "Duff's device" is
		// used to jump into the middle of the loop if the first nibble of
		// the pixel data should not be used.  Duff's device makes use of
		// the fact that a case statement is legal anywhere within a
		// sub-block of a switch statement.  See
		// http://en.wikipedia.org/wiki/Duff's_device for detailed
		// information about Duff's device.
		//
		switch (lX0 & 1)
		{
		case 0:
			while (lCount)
			{
				//
				// Get the upper nibble of the next byte of pixel data
				// and extract the corresponding entry from the
				// palette.
				//
				ulByte = (*pucData >> 4) * 3;
				ulByte =
						(*(unsigned long *) (pucPalette + ulByte) & 0x00ffffff);

				//
				// Translate this palette entry and write it to the
				// screen.
				//
				ssd1289_pixel((color_t) ColorTranslate(0, ulByte), lY, lX++);

				//
				// Decrement the count of pixels to draw.
				//
				lCount--;

				//
				// See if there is another pixel to draw.
				//
				if (lCount)
				{
					case 1:
					//
					// Get the lower nibble of the next byte of pixel
					// data and extract the corresponding entry from
					// the palette.
					//
					ulByte = (*pucData++ & 15) * 3;
					ulByte = (*(unsigned long *) (pucPalette + ulByte)
							& 0x00ffffff);

					//
					// Translate this palette entry and write it to the
					// screen.
					//
					ssd1289_pixel((color_t) ColorTranslate(0, ulByte), lY,
							lX++);

					//
					// Decrement the count of pixels to draw.
					//
					lCount--;
				}
			}
		}

		//
		// The image data has been drawn.
		//
		break;
	}

		//
		// The pixel data is in 8 bit per pixel format.
		//
	case 8:
	{
		//
		// Loop while there are more pixels to draw.
		//
		while (lCount--)
		{
			//
			// Get the next byte of pixel data and extract the
			// corresponding entry from the palette.
			//
			ulByte = *pucData++ * 3;
			ulByte = *(unsigned long *) (pucPalette + ulByte) & 0x00ffffff;

			//
			// Translate this palette entry and write it to the screen.
			//
			ssd1289_pixel((color_t) ColorTranslate(0, ulByte), lY, lX++);
		}

		//
		// The image data has been drawn.
		//
		break;
	}

		//
		// We are being passed data in the display's native format.  Merely
		// write it directly to the display.  This is a special case which is
		// not used by the graphics library but which is helpful to
		// applications which may want to handle, for example, JPEG images.
		//
	case 16:
	{
		unsigned short usByte;

		//
		// Loop while there are more pixels to draw.
		//
		while (lCount--)
		{
			//
			// Get the next byte of pixel data and extract the
			// corresponding entry from the palette.
			//
			usByte = *((unsigned short *) pucData);
			pucData += 2;

			//
			// Translate this palette entry and write it to the screen.
			//
			ssd1289_pixel((color_t) ColorTranslate(0, ulByte), lY, lX++);
		}
	}
	}
}
void RectFill(void *pvDisplayData, const tRectangle *pRect,
		unsigned long ulValue)
{
	unsigned int x1, x2, y1, y2, trueColor;
	x1 = pRect->sXMin;
	x2 = pRect->sXMax;
	y1 = pRect->sYMin;
	y2 = pRect->sYMax;

	for (; x1 <= x2; x1++)
	{
		ssd1289_vline(ulValue, y1, x1, y2 - y1);
	}
}

const tDisplay DisplayStructure =
{ sizeof(tDisplay), 0,
#if defined(PORTRAIT_FLIP) || defined(PORTRAIT)
		240, //240
		320,//320
#else
		320, //320
		240, //240 lado corto
#endif
		PixelDraw, PixelDrawMultiple, LineDrawH, LineDrawV, RectFill,
		ColorTranslate, Flush };


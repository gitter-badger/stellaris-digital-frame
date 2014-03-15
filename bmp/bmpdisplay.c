//*****************************************************************************
//
// bmpdisplay.c - Functions to display Windows bitmaps on the DK LCD.
//
// Copyright (c) 2010 Texas Instruments Incorporated.  All rights reserved.
//
//*****************************************************************************
#include "inc/hw_types.h"
#include "grlib/grlib.h"
#include "bmpdisplay.h"
#include "bmpfile.h"

//*****************************************************************************
//
// Assumptions and Limitations:
//
// 1. This implementation allows only a single bitmap to be opened at any one
// time (since the instance data is held in a global structure rather than
// being dynamically allocated).  If you have a memory manager, it would be
// very easy to rework to remove this restriction.
//
// 2. At present we only support 8-bpp (256 color) bitmaps. Support for other
// flavors of the format could easily be added.
//
// 3. Bitmaps larger than the screen (320x240) are not currently supported.
//
//*****************************************************************************

//*****************************************************************************
//
// We support 8bpp images with 256 colors or less.
//
//*****************************************************************************
#define MAX_COLORS     256

//*****************************************************************************
//
// We support images up to 320 pixels wide.
//
//*****************************************************************************
#define MAX_LINE_WIDTH 320

//*****************************************************************************
//
// Macro used to pack the elements of a structure.
//
//*****************************************************************************
#ifndef PACKED
#if defined(ccs) ||             \
    defined(codered) ||         \
    defined(gcc) ||             \
    defined(rvmdk) ||           \
    defined(__ARMCC_VERSION) || \
    defined(sourcerygxx)
#define PACKED __attribute__ ((packed))
#elif defined(ewarm)
#define PACKED
#else
#error Unrecognized COMPILER!
#endif
#endif

//*****************************************************************************
//
// Structures related to Windows bitmaps
//
//*****************************************************************************
#ifdef ewarm
#pragma pack(1)
#endif

typedef struct
{
    unsigned short usType;
    unsigned long  ulSize;
    unsigned short usReserved1;
    unsigned short usReserved2;
    unsigned long  ulOffBits;
}
PACKED tBitmapFileHeader;

typedef struct
{
    unsigned long ulSize;
    long lWidth;
    long lHeight;
    unsigned short usPlanes;
    unsigned short usBitCount;
    unsigned long  ulCompression;
    unsigned long  ulSizeImage;
    long lXPelsPerMeter;
    long lYPelsPerMeter;
    unsigned long ulClrUsed;
    unsigned long ulClrImportant;
}
PACKED tBitmapInfoHeader;

#ifdef ewarm
#pragma pack()
#endif

//*****************************************************************************
//
// Bitmap instance data structure.
//
//*****************************************************************************
struct tBitmapInst
{
    tBitmapInfoHeader sHeader;
    tBitmapHandle     hHandle;
    unsigned short   *pusPalette;
    unsigned long     ulOffBits;
};

static tBitmapInst g_sBitmapInst;

//*****************************************************************************
//
// A buffer we use to hold the color lookup table for the image.
//
//*****************************************************************************
unsigned short g_usPalette[MAX_COLORS];

//*****************************************************************************
//
// A buffer we use to hold a line of converted pixels prior to writing them
// onto the display.
//
//*****************************************************************************
unsigned short g_usPixelBuffer[MAX_LINE_WIDTH];

//*****************************************************************************
//
// A buffer we use to hold a line of raw pixel data read from the bitmap.
//
//*****************************************************************************
unsigned char g_ucBmpLineBuffer[MAX_LINE_WIDTH];

//*****************************************************************************
//
// Public functions.
//
//*****************************************************************************
tBitmapInst *
BitmapOpen(char *pucFilename)
{
    tBitmapFileHeader sHeader;
    int iRead;

    //
    // Weed out obviously bad parameters.
    //
    if(!pucFilename)
    {
        return((tBitmapInst *)0);
    }

    //
    // Make sure the instance structure isn't currently in use.
    //
    if(g_sBitmapInst.hHandle)
    {
        return((tBitmapInst *)0);
    }

    //
    // Open the file supplied.
    //
    g_sBitmapInst.hHandle = BitmapFileOpen(pucFilename);

    //
    // Was the file opened successfully?
    //
    if(!g_sBitmapInst.hHandle)
    {
        return((tBitmapInst *)0);
    }

    //
    // Read the bitmap file header.
    //
    iRead = BitmapFileRead(g_sBitmapInst.hHandle, (unsigned char *)&sHeader,
                           sizeof(tBitmapFileHeader));
    if(iRead != sizeof(tBitmapFileHeader))
    {
        BitmapFileClose(g_sBitmapInst.hHandle);
        g_sBitmapInst.hHandle = (tBitmapHandle)0;
        return((tBitmapInst *)0);
    }

    //
    // Sanity check the header.  If this is a bitmap, the first 2 bytes must
    // be "BM".
    //
    if(sHeader.usType != 0x4D42)
    {
        //
        // The file is obviously not a bitmap.
        //
        BitmapFileClose(g_sBitmapInst.hHandle);
        g_sBitmapInst.hHandle = (tBitmapHandle)0;
        return((tBitmapInst *)0);
    }

    //
    // Remember the file offset to the start of the pixel data.
    //
    g_sBitmapInst.ulOffBits = sHeader.ulOffBits;

    //
    // Read the bitmap info header.
    //
    iRead = BitmapFileRead(g_sBitmapInst.hHandle, (unsigned char *)&g_sBitmapInst.sHeader,
                           sizeof(tBitmapInfoHeader));
    if(iRead != sizeof(tBitmapInfoHeader))
    {
        BitmapFileClose(g_sBitmapInst.hHandle);
        g_sBitmapInst.hHandle = (tBitmapHandle)0;
        return((tBitmapInst *)0);
    }

    //
    // We only currently support single plane, 8bpp bitmaps up to 320 pixels
    // wide so check for this and reject any other flavor.
    //
    if((g_sBitmapInst.sHeader.usBitCount != 8) ||
       (g_sBitmapInst.sHeader.usPlanes != 1) ||
       (g_sBitmapInst.sHeader.lWidth > MAX_LINE_WIDTH))
    {
        BitmapFileClose(g_sBitmapInst.hHandle);
        g_sBitmapInst.hHandle = (tBitmapHandle)0;
        return((tBitmapInst *)0);
    }

    //
    // If we get here, everything was well.
    //
    return(&g_sBitmapInst);
}

void
BitmapClose(tBitmapInst *pInst)
{
    //
    // If the pointer seems valid and the instance data contains a non-NULL
    // handle, go ahead and close the file.
    //
    if(pInst && pInst->hHandle)
    {
        BitmapFileClose(pInst->hHandle);
        pInst->hHandle = (tBitmapHandle)0;
        pInst->pusPalette = (unsigned short *)0;
    }
}

unsigned long
BitmapHeightGet(tBitmapInst *pInst)
{
    //
    // If the instance pointer seems valid and it contains a non-NULL bitmap
    // file handle, return the height.
    //
    if(pInst && pInst->hHandle)
    {
        //
        // All is well - return the height.
        //
        return((unsigned long)pInst->sHeader.lHeight);
    }
    else
    {
        //
        // The instance pointer is invalid or it doesn't contain a valid file
        // handle.
        //
        return(0);
    }
}

unsigned long
BitmapWidthGet(tBitmapInst *pInst)
{
    //
    // If the instance pointer seems valid and it contains a non-NULL bitmap
    // file handle, return the width.
    //
    if(pInst && pInst->hHandle)
    {
        //
        // All is well - return the width.
        //
        return((unsigned long)pInst->sHeader.lWidth);
    }
    else
    {
        //
        // The instance pointer is invalid or it doesn't contain a valid file
        // handle.
        //
        return(0);
    }
}

tBoolean
BitmapDraw(tBitmapInst *pInst, tContext *pContext, short sX, short sY)
{
    unsigned long ulLoop, ulNumColors, ulColor, ulLineOffset;
    unsigned long ulHeight, ulWidth, ulBytesPerLine;
    int iRead;

    //
    // Were we passed valid parameters?
    //
    if(!pInst || !pInst->hHandle || !pContext)
    {
        //
        // No - return an error.
        //
        return(false);
    }

    //
    // Does the position we've been given fall on the screen?
    //
    if((sY >= pContext->pDisplay->usHeight) ||
       (sX >= pContext->pDisplay->usWidth) ||
       (sY < 0) || (sX < 0))
    {
        //
        // No - it's completely off the screen so return an error.
        //
        return(false);
    }

    //
    // It looks as if we will be rendering something onto the screen so check
    // to see if we need to build a color lookup table for this image.
    //
    if(!g_sBitmapInst.pusPalette)
    {
        //
        // We need to build the color lookup table. How big will the table
        // be?
        //
        if(g_sBitmapInst.sHeader.ulClrUsed)
        {
            ulNumColors = g_sBitmapInst.sHeader.ulClrUsed;
        }
        else
        {
            ulNumColors = 256;
        }

        //
        // Loop through the entries in the bitmap's palette.
        //
        for(ulLoop = 0; ulLoop < ulNumColors; ulLoop++)
        {
            //
            // Read the next palette entry.
            //
            iRead = BitmapFileRead(g_sBitmapInst.hHandle,
                                   (unsigned char *)&ulColor, 4);
            if(iRead != 4)
            {
                //
                // Bitmap ended prematurely so it's invalid.
                //
                return(false);
            }
            else
            {
//            	unsigned long red = ulColor & 0x00FF0000;
//            	unsigned long green = ulColor & 0x0000FF00;
//            	unsigned long blue = ulColor & 0x000000FF;
//            	unsigned long fixedColor = (red << 16) | (green) | (blue >> 16);
            	g_usPalette[ulLoop] =
                    (unsigned short)pContext->pDisplay->pfnColorTranslate(
                                        pContext->pDisplay->pvDisplayData,
                                        ulColor);
            }
        }

        //
        // Hook the global color table to the instance data.
        //
        g_sBitmapInst.pusPalette = g_usPalette;
    }

    //
    // Calculate which line we start drawing on.  We clip on the bottom edge.
    //
    ulHeight = g_sBitmapInst.sHeader.lHeight;
    ulLineOffset = 0;
    if(((unsigned long)sY + ulHeight) >= pContext->pDisplay->usHeight)
    {
        //
        // Clip the height.
        //
        ulHeight = (unsigned long)pContext->pDisplay->usHeight -
                   (unsigned long)sY;

        //
        // Set the line offset so that we skip the clipped lines in the file.
        //
        ulLineOffset = g_sBitmapInst.sHeader.lHeight - ulHeight;
    }

    //
    // Determine the width of each line we will be drawing.  We clip on the
    // right edge.
    //
    ulWidth = g_sBitmapInst.sHeader.lWidth;
    if(((unsigned long)sX + ulWidth) >= pContext->pDisplay->usWidth)
    {
        ulWidth = (unsigned long)pContext->pDisplay->usWidth -
                  (unsigned long)sX;
    }

    //
    // Lines of data in a Windows bitmap are always multiples of 4 bytes so
    // determine the data length for each line.
    //
    ulBytesPerLine = (g_sBitmapInst.sHeader.lWidth + 3) & 0xFFFFFFFC;

    //
    // Now draw the image a line at a time. We start from the bottom since
    // Windows bitmaps are stored upside down. This is weird but it's at least
    // been more or less consistently weird for 25 years (except for those
    // weird formats in DirectDraw where the height is reported as negative and
    // the data appears the right way up. We won't go there for now though).
    //

    //
    // Set the initial file offset such that we will read the first line we
    // want.
    //
    BitmapFileOffsetSet(g_sBitmapInst.hHandle, (g_sBitmapInst.ulOffBits +
                        (ulBytesPerLine * ulLineOffset)));

    //
    // We're all set to go.  We read a line of pixel data at a time, convert
    // this into 16-bit pixels suitable for the display driver and then
    // draw them onto the display.
    //
    while(ulHeight)
    {
        //
        // Get a line of pixels from the file
        //
        iRead = BitmapFileRead(g_sBitmapInst.hHandle, g_ucBmpLineBuffer,
                               (int)ulBytesPerLine);
        if(iRead != (int)ulBytesPerLine)
        {
            //
            // The file ended prematurely! Return an error.
            //
            return(false);
        }

        //
        // Convert as many of these pixels as we need using the palette lookup
        // table.
        //
        for(ulLoop = 0; ulLoop < ulWidth; ulLoop++)
        {
            g_usPixelBuffer[ulLoop] = g_usPalette[g_ucBmpLineBuffer[ulLoop]];
        }

        //
        // Now draw the line of converted pixels onto the display.
        //
        int x_index = 0;
        for (x_index = 0; x_index < ulWidth; x_index++)
        {
        	pContext->pDisplay->pfnPixelDraw((void*)pContext->pDisplay,x_index,ulHeight,g_usPixelBuffer[x_index]);
        }

        //
        // Move to the next (or previous depending upon our point-of-view) line.
        //
        ulHeight--;
    }

    //
    // If we get here, all is well.
    //
    return(true);
}


//*****************************************************************************
//
// bmpdisplay.h - Public header file for the Windows bitmap display module.
//
// Copyright (c) 2010 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __BMPDISPLAY_H__
#define __BMPDISPLAY_H__

typedef struct tBitmapInst tBitmapInst;

//*****************************************************************************
//
// Public function prototypes.
//
//*****************************************************************************
extern tBitmapInst *BitmapOpen(char *pucFilename);
extern void BitmapClose(tBitmapInst *pInst);
extern unsigned long BitmapHeightGet(tBitmapInst *pInst);
extern unsigned long BitmapWidthGet(tBitmapInst *pInst);
extern tBoolean BitmapDraw(tBitmapInst *pInst, tContext *pContext,
                           short sX, short sY);

#endif // __BMPDISPLAY_H__

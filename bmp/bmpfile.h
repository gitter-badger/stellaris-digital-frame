//*****************************************************************************
//
// bmpfile.h - Public header file for the Windows bitmap file reader module.
//
// Copyright (c) 2010 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#ifndef __BMPFILE_H__
#define __BMPFILE_H__

typedef struct tBitmapFile *tBitmapHandle;

//*****************************************************************************
//
// Public function prototypes.
//
//*****************************************************************************
extern tBitmapHandle BitmapFileOpen(char *pucFilename);
extern void BitmapFileClose(tBitmapHandle hHandle);
extern int BitmapFileRead(tBitmapHandle hHandle, unsigned char *pucData,
                          int iSize);
extern tBoolean BitmapFileOffsetSet(tBitmapHandle hHandle,
                                    unsigned long ulOffset);
extern unsigned long BitmapFileSizeGet(tBitmapHandle hHandle);

#endif // __BMPFILE_H__

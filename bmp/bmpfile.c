//*****************************************************************************
//
// bmpfile.h - Public header file for the Windows bitmap file reader module.
//
// Copyright (c) 2010 Texas Instruments Incorporated.  All rights reserved.
// TI Information - Selective Disclosure
//
//*****************************************************************************
#include "inc/hw_types.h"
#include "../third_party/fatfs/src/diskio.h"
#include "../third_party/fatfs/src/ff.h"
#include "bmpfile.h"

//*****************************************************************************
//
// This file provides an abstraction layer for whichever underlying method is
// used to store the bitmap files used by the bmpdisplay API. This particular
// version sits above the FAT file system and reads files from it but the
// file could easily be rewritten to read information from the EEPROM or from
// a file system image linked into the main flash image itself.
//
// Note that this implementation allows only a single bitmap to be opened at
// any one time (since the instance data is held in a global structure rather
// than being dynamically allocated).  If you have a memory manager, it would
// be very easy to rework to remove this restriction.
//
//*****************************************************************************

//*****************************************************************************
//
// File instance data structure.
//
//*****************************************************************************
typedef struct tBitmapFile
{
    tBoolean bInUse;
    FIL sFile;
}
tBitmapFile;

//*****************************************************************************
//
// The following are data structures used by FatFs.
//
//*****************************************************************************
static FATFS g_sFatFs;
static tBitmapFile g_sFileInst;
static tBoolean g_bInitialized;

//*****************************************************************************
//
// Public functions.
//
//*****************************************************************************

//*****************************************************************************
//
// Open the given bitmap file and return a handle to it.
//
//*****************************************************************************
tBitmapHandle
BitmapFileOpen(char *pucFilename)
{
    FRESULT eRetcode;

    //
    // Mount the file system, using logical disk 0 if we have not done so
    // already.
    //
    if(!g_bInitialized)
    {
        eRetcode = f_mount(&g_sFatFs,"0:",1);
        if(eRetcode != FR_OK)
        {
            return((tBitmapHandle)0);
        }
    }

    //
    // Make sure the global file object is not already in use.
    //
    if(g_sFileInst.bInUse)
    {
        return((tBitmapHandle)0);
    }

    //
    // Open the file for reading.
    //
    eRetcode = f_open(&g_sFileInst.sFile, pucFilename, FA_READ);

    if(eRetcode == FR_OK)
    {
        g_sFileInst.bInUse = true;
        return((tBitmapHandle)&g_sFileInst);
    }
    else
    {
        return((tBitmapHandle)0);
    }
}

//*****************************************************************************
//
// Close the bitmap file identified by the handle passed.
//
//*****************************************************************************
void
BitmapFileClose(tBitmapHandle hHandle)
{
    //
    // If the handle is valid and the file is already open, close it.
    //
    if(g_sFileInst.bInUse && (hHandle == (tBitmapHandle)&g_sFileInst))
    {
        f_close(&g_sFileInst.sFile);
        g_sFileInst.bInUse = false;
    }
}

//*****************************************************************************
//
// Read a block of data from the given bitmap file into a supplied buffer and
// return the number of bytes read.
//
//*****************************************************************************
int
BitmapFileRead(tBitmapHandle hHandle, unsigned char *pucData, int iSize)
{
    FRESULT eRetcode;
    unsigned int usBytesRead;

    //
    // If the handle is valid and the file is already open read from it.
    //
    if(g_sFileInst.bInUse && (hHandle == (tBitmapHandle)&g_sFileInst))
    {
        //
        // Read the amount of data requested and return the number of bytes
        // read or -1 to indicate an error.
        //
        eRetcode = f_read(&g_sFileInst.sFile, pucData, iSize, &usBytesRead);
        if((eRetcode == FR_OK) && (usBytesRead != 0))
        {
            return((int)usBytesRead);
        }
    }

    //
    // If we get here, there was an error so return -1 to indicate this.
    //
    return(-1);
}

//*****************************************************************************
//
// Move the file pointer (dictating the position that the next data will be
// read from) for the given bitmap file.
//
//*****************************************************************************
tBoolean
BitmapFileOffsetSet(tBitmapHandle hHandle, unsigned long ulOffset)
{
    FRESULT eRetcode;

    //
    // If the handle is valid and the file is already open set the file pointer.
    //
    if(g_sFileInst.bInUse && (hHandle == (tBitmapHandle)&g_sFileInst))
    {
        eRetcode = f_lseek(&g_sFileInst.sFile, ulOffset);
        if(eRetcode == FR_OK)
        {
            return(true);
        }
    }

    //
    // If we get here, some error occurred so let the caller know.
    //
    return(false);
}


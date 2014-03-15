/*
 * slideshowApp.c
 *
 *  Created on: 9 במרס 2014
 *      Author: Itay
 */
#include <string.h>
#include <stdio.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "../lcd/grlibDriver.h"
#include "../bmp/bmpfile.h"
#include "../bmp/bmpdisplay.h"
#include "../third_party/fatfs/src/ff.h"
#include "../touch/xpt2046.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/pushbutton.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "../clock/clock.h"
#include "mainMenuApp.h"
#include "slideshowApp.h"

// Forward deceleration
void onPictureClick();

RectangularButton(exitBtn, WIDGET_ROOT, 0, 0, &DisplayStructure, 0, 0, 319, 239,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, onPictureClick);

FATFS sildeShowFatfs;
tContext Context;
tBoolean ready = false;
DIR slideShowRootDir;
FILINFO currentFileInfo;
tBitmapInst* pBtmpInst;

//tBitmapInst* pBtmpInst = BitmapOpen("0:4.bmp");
//BitmapDraw(pBtmpInst, &Context, 0, 0);
//BitmapClose(pBtmpInst);

extern tCanvasWidget GeneralErrorMsg;

void startTimer(int interval)
{
	if (interval < 3)
	{
		interval = 3;
	}
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	SysCtlDelay(10);
	IntMasterEnable();
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() * interval); // Every "interval" seconds
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER0_BASE, TIMER_A);
}

// Returns 0 if no viewable bmp files are found, 1 if only one, 2 if more than one
unsigned char areThereViewableBmpFiles()
{
	FILINFO finfo;
	unsigned char retVal = 0;
	char fullPath[16];
	do
	{
		f_readdir(&slideShowRootDir, &finfo);
		sprintf(fullPath, "0:%s", finfo.fname);
		pBtmpInst = BitmapOpen(fullPath);
		if (pBtmpInst != 0)
		{
			BitmapClose(pBtmpInst);
			retVal++;
			if (retVal == 2)
				return retVal;
		}
	} while (strcmp(finfo.fname, "\0") != 0);

	return retVal;
}

void findNextFile()
{
	FILINFO fileInfo;
	int completedRound = 0;
	while (true)
	{
		// get the first file
		f_readdir(&slideShowRootDir, &fileInfo);
		// if we are looking for a ghost or checked all the files already, return
		if (completedRound || strcmp(currentFileInfo.fname, "\0") == 0)
		{
			currentFileInfo = fileInfo;
			return;
		}
		// if we find what we are looking for
		if (strcmp(fileInfo.fname, currentFileInfo.fname) == 0)
		{
			// get the next file
			f_readdir(&slideShowRootDir, &currentFileInfo);
			// if it was the last file get the first file
			if (strcmp(currentFileInfo.fname, "\0") == 0)
			{
				f_closedir(&slideShowRootDir);
				f_opendir(&slideShowRootDir, "0:");
				f_readdir(&slideShowRootDir, &currentFileInfo);
			}
			return;
		}
		// Completed a round with no find
		if (strcmp(fileInfo.fname, "\0") == 0)
		{
			completedRound = 1;
			f_closedir(&slideShowRootDir);
			f_opendir(&slideShowRootDir, "0:");
		}
	}
}

void findNextViewable()
{
	char fullPath[16];
	f_opendir(&slideShowRootDir, "0:");
	while (true)
	{
		findNextFile();
		sprintf(fullPath, "0:%s", currentFileInfo.fname);
		pBtmpInst = BitmapOpen(fullPath);
		if (pBtmpInst != 0)
		{
			break;
		}
	}
	f_closedir(&slideShowRootDir);
}

void DrawNextPicture()
{
	findNextViewable();
	BitmapDraw(pBtmpInst, &Context, 0, 0);
	BitmapClose(pBtmpInst);
}

void DrawNextPictureHandler()
{
	TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	DrawNextPicture();
	updateClock();
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

// interval between pictures (in seconds >= 3)
void startSlideShowApp(int interval)
{
	showClock(WIDGET_ROOT);
	GrContextInit(&Context, &DisplayStructure);
	WidgetAdd(WIDGET_ROOT, (tWidget*) &exitBtn);

	FRESULT fresult = f_mount(&sildeShowFatfs, "0:", 1);
	if (fresult != FR_OK)
	{
		WidgetAdd((tWidget*) &exitBtn, (tWidget*) &GeneralErrorMsg);
		CanvasTextSet(&GeneralErrorMsg, "No filesystem found!");
		WidgetPaint(WIDGET_ROOT);
		return;
	}

	f_opendir(&slideShowRootDir, "0:");

	unsigned char numOfViewables = areThereViewableBmpFiles();
	if (numOfViewables == 0) // No viewable photos
	{
		WidgetAdd((tWidget*) &exitBtn, (tWidget*) &GeneralErrorMsg);
		CanvasTextSet(&GeneralErrorMsg, "There are no pictures on SD!");
	}
	else if (numOfViewables == 1) // Just one viewable photo
	{
		findNextViewable();
		BitmapDraw(pBtmpInst, &Context, 0, 0);
		BitmapClose(pBtmpInst);
	}
	else // More than one viewable photos. Start slideshow timer
	{
		DrawNextPicture();
		startTimer(interval);
	}
	WidgetPaint(WIDGET_ROOT);
	xpt2046_enableTouchIRQ();
}

void exitSlideshowApp()
{
	xpt2046_disableTouchIRQ();
	hideClock();
	TimerDisable(TIMER0_BASE, TIMER_A);
	TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	WidgetRemove((tWidget*) &GeneralErrorMsg);
	WidgetRemove((tWidget*) &exitBtn);
	BitmapClose(pBtmpInst);
}

void onPictureClick()
{
	exitSlideshowApp();
	startMainMenuApplication();
}

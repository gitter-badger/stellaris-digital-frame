/*
 * selectPic.c
 *
 *  Created on: 10 במרס 2014
 *      Author: Itay
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lcd/grlibDriver.h"
#include "../third_party/fatfs/src/ff.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/listbox.h"
#include "grlib/canvas.h"
#include "grlib/pushbutton.h"
#include "../clock/clock.h"
#include "mainMenuApp.h"
#include "selectPicApp.h"

FATFS fatfs;
DIR selectPicRootDir;
FILINFO selectedFileInfo;
int currentPage = 0;
int numOfFiles = 0;
int maxPageNumber = 0;
void onGoBackClick();
void onChange(tWidget* pWidget, short index);
void onUpClick();
void onDownClick();

char * strings[FILE_LIST_LENGTH];

Canvas(selectPicBackgroundCanvas, 0, 0, 0, &DisplayStructure, 0, 0, 319, 239,
		CANVAS_STYLE_FILL, ClrBlack, ClrWhite, ClrBlack, 0, 0, 0, 0);

ListBox(fileListBox, &selectPicBackgroundCanvas, 0,0, &DisplayStructure,0, CLOCK_HEIGHT,
		180, 200-CLOCK_HEIGHT, LISTBOX_STYLE_OUTLINE, ClrBlack, ClrWhite, ClrWhite, ClrBlack,
		ClrWhite, g_pFontCm16, strings, FILE_LIST_LENGTH, 0, onChange);

CircularButton(upBtn, &selectPicBackgroundCanvas, 0, 0, &DisplayStructure,
		180 - BTN_RADIUS, CLOCK_HEIGHT+BTN_RADIUS, BTN_RADIUS,
		PB_STYLE_TEXT |PB_STYLE_OUTLINE | PB_STYLE_FILL, ClrBlue, ClrRed,
		ClrWhite, ClrYellow, g_pFontCm12, "UP", 0, 0, 0, 0, onUpClick);

CircularButton(downBtn, &selectPicBackgroundCanvas, 0, 0, &DisplayStructure,
		180-BTN_RADIUS, 200-BTN_RADIUS, BTN_RADIUS,
		PB_STYLE_TEXT |PB_STYLE_OUTLINE | PB_STYLE_FILL, ClrBlue, ClrRed,
		ClrWhite, ClrYellow, g_pFontCm12, "DN", 0, 0, 0, 0, onDownClick);

RectangularButton(backToMenuBtn, &selectPicBackgroundCanvas, 0, 0,
		&DisplayStructure, 5, 200, 80, 40,
		PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_OUTLINE, ClrRed, ClrRed,
		ClrWhite, ClrBlack, g_pFontCm16b, "Go Back", 0, 0, 0, 0, onGoBackClick);

extern tCanvasWidget GeneralErrorMsg;

void skipPages(int pages)
{
	FILINFO fileInfo;
	if (pages > 0)
	{
		int i = 0;
		for (; i < FILE_LIST_LENGTH * pages; i++)
		{
			f_readdir(&selectPicRootDir, &fileInfo);
			if (strcmp(fileInfo.fname, "\0") == 0)
			{
				return;
			}
		}
	}
}

// populates the list with the page number
void populateList(int pageNum)
{
	FILINFO fileInfo;
	int index = 0;
	f_opendir(&selectPicRootDir, "0:");
	ListBoxClear(&fileListBox);
	skipPages(pageNum);
	while (index < FILE_LIST_LENGTH)
	{
		f_readdir(&selectPicRootDir, &fileInfo);
		if (strcmp(fileInfo.fname, "\0") == 0)
			break;
		strcpy(strings[index], fileInfo.fname);
		ListBoxTextAdd(&fileListBox, strings[index]);
		index++;
	}
	f_closedir(&selectPicRootDir);
}

unsigned int countFiles()
{
	FILINFO fileInfo;
	int retVal = -1;
	f_opendir(&selectPicRootDir, "0:");
	do
	{
		f_readdir(&selectPicRootDir, &fileInfo);
		retVal++;
	} while (strcmp(fileInfo.fname, "\0") != 0);
	f_closedir(&selectPicRootDir);
	return retVal;
}

void startSelectPicApp()
{
	int i;
	size_t allocSize = sizeof(char) * 15;
	for (i = 0; i < FILE_LIST_LENGTH; i++)
	{
		strings[i] = (char *) malloc(allocSize);
	}
	showClock(WIDGET_ROOT);
	WidgetAdd(WIDGET_ROOT, (tWidget*) &selectPicBackgroundCanvas);
	WidgetAdd((tWidget*) &selectPicBackgroundCanvas, (tWidget*) &backToMenuBtn);
	WidgetAdd((tWidget*) &fileListBox, (tWidget*) &upBtn);
	WidgetAdd((tWidget*) &fileListBox, (tWidget*) &downBtn);
	FRESULT result = f_mount(&fatfs, "0:", 1);

	if (result != FR_OK)
	{
		WidgetAdd((tWidget*) &selectPicBackgroundCanvas,
				(tWidget*) &GeneralErrorMsg);
		CanvasTextSet(&GeneralErrorMsg, "No filesystem found!");
		WidgetPaint(WIDGET_ROOT);
		return;
	}

	populateList(currentPage);
	numOfFiles =  countFiles();
	maxPageNumber = numOfFiles / FILE_LIST_LENGTH;
	WidgetAdd((tWidget*) &selectPicBackgroundCanvas, (tWidget*) &fileListBox);
	WidgetPaint(WIDGET_ROOT);
}

void exitSelectPicApp()
{
	hideClock();
	int i = 0;
	for (; i < FILE_LIST_LENGTH; i++)
		free((void*)strings[i]);
	WidgetRemove((tWidget*) &GeneralErrorMsg);
	WidgetRemove((tWidget*) &selectPicBackgroundCanvas);
	WidgetRemove((tWidget*) &backToMenuBtn);
}

void onGoBackClick()
{
	exitSelectPicApp();
	startMainMenuApplication();
}
void onChange(tWidget* pWidget, short index)
{

}

void onUpClick()
{
	if (currentPage == 0)
		return;
	currentPage--;
	populateList(currentPage);
	WidgetPaint((tWidget*)&fileListBox);
}

void onDownClick()
{
	if (currentPage == maxPageNumber)
		return;
	currentPage++;
	populateList(currentPage);
	WidgetPaint((tWidget*)&fileListBox);
}

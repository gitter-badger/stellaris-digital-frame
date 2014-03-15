/*
 * selectPic.c
 *
 *  Created on: 10 במרס 2014
 *      Author: Itay
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "inc/hw_types.h"
#include "../lcd/grlibDriver.h"
#include "../third_party/fatfs/src/ff.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/listbox.h"
#include "grlib/canvas.h"
#include "grlib/pushbutton.h"
#include "../bmp/bmpfile.h"
#include "../bmp/bmpdisplay.h"
#include "../clock/clock.h"
#include "mainMenuApp.h"
#include "viewPicApp.h"
#include "selectPicApp.h"

FATFS fatfs;
DIR selectPicRootDir;
FILINFO selectedFileInfo;
int currentPage = 0;
int numOfFiles = 0;
int maxPageNumber = 0;
void onGoBackClick();
void onViewClick();
void onListBoxSelectionChange(tWidget* pWidget, short index);
void onUpClick();
void onDownClick();
void refreshPageCounter();
void onClickDoNothing(tWidget* pWidget);

char * strings[FILE_LIST_LENGTH];
char selectedFilename[15];
char pageCounterString[10];

Canvas(selectPicBackgroundCanvas, 0, 0, 0, &DisplayStructure, 0, 0, 320, 240,
		CANVAS_STYLE_FILL, ClrBlack, ClrWhite, ClrBlack, 0, 0, 0, 0);

ListBox(fileListBox, &selectPicBackgroundCanvas, 0, 0, &DisplayStructure, 0,
		CLOCK_HEIGHT, 180, 200-CLOCK_HEIGHT, LISTBOX_STYLE_OUTLINE, ClrBlack,
		ClrWhite, ClrWhite, ClrBlack, ClrWhite, g_pFontCm16, strings,
		FILE_LIST_LENGTH, 0, onListBoxSelectionChange);

CircularButton(upBtn, &fileListBox, 0, 0, &DisplayStructure, 180 - BTN_RADIUS,
		CLOCK_HEIGHT+BTN_RADIUS, BTN_RADIUS,
		PB_STYLE_TEXT |PB_STYLE_OUTLINE | PB_STYLE_FILL, ClrBlue, ClrRed,
		ClrWhite, ClrYellow, g_pFontCm12, "UP", 0, 0, 0, 0, onUpClick);

CircularButton(downBtn, &fileListBox, 0, 0, &DisplayStructure, 180-BTN_RADIUS,
		200-BTN_RADIUS, BTN_RADIUS,
		PB_STYLE_TEXT |PB_STYLE_OUTLINE | PB_STYLE_FILL, ClrBlue, ClrRed,
		ClrWhite, ClrYellow, g_pFontCm12, "DN", 0, 0, 0, 0, onDownClick);

RectangularButton(backToMenuBtn, &selectPicBackgroundCanvas, 0, 0,
		&DisplayStructure, 5, 200, 80, 40,
		PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_OUTLINE, ClrRed, ClrRed,
		ClrWhite, ClrBlack, g_pFontCm18b, "Go Back", 0, 0, 0, 0, onGoBackClick);

RectangularButton(viewPicBtn, &selectPicBackgroundCanvas, 0, 0,
		&DisplayStructure, 100, 200, 80, 40,
		PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_OUTLINE, ClrBlack, ClrBlack,
		ClrWhite, ClrWhiteSmoke, g_pFontCm18b, "View", 0, 0, 0, 0, onClickDoNothing);

Canvas(pageCounter, &fileListBox, 0, 0, &DisplayStructure,
		180-PAGE_COUNTER_WIDTH-1, (200+CLOCK_HEIGHT)/2 - PAGE_COUNTER_HEIGHT /2,
		PAGE_COUNTER_WIDTH, PAGE_COUNTER_HEIGHT,
		CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_HCENTER | CANVAS_STYLE_TEXT_VCENTER,
		ClrBlack, 0, ClrWhite, g_pFontCm12, "", 0, 0);

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
	WidgetAdd((tWidget*) &selectPicBackgroundCanvas, (tWidget*) &viewPicBtn);
	WidgetAdd((tWidget*) &fileListBox, (tWidget*) &upBtn);
	WidgetAdd((tWidget*) &fileListBox, (tWidget*) &downBtn);
	WidgetAdd((tWidget*) &fileListBox, (tWidget*) &pageCounter);
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
	numOfFiles = countFiles();
	maxPageNumber = numOfFiles / FILE_LIST_LENGTH;
	refreshPageCounter();
	WidgetAdd((tWidget*) &selectPicBackgroundCanvas, (tWidget*) &fileListBox);
	WidgetPaint(WIDGET_ROOT);
}

void exitSelectPicApp()
{
	hideClock();
	int i = 0;
	for (; i < FILE_LIST_LENGTH; i++)
		free((void*) strings[i]);
	WidgetRemove((tWidget*) &GeneralErrorMsg);
	WidgetRemove((tWidget*) &selectPicBackgroundCanvas);
	WidgetRemove((tWidget*)&fileListBox);
	WidgetRemove((tWidget*) &backToMenuBtn);
	WidgetRemove((tWidget*) &viewPicBtn);
}

void onGoBackClick()
{
	exitSelectPicApp();
	startMainMenuApplication();
}

void onListBoxSelectionChange(tWidget* pWidget, short index)
{
	sprintf(selectedFilename,"0:%s",strings[index]);
	tBitmapInst* pBtmp = BitmapOpen(selectedFilename);
	if (pBtmp != 0)
	{
		BitmapClose(pBtmp);
		PushButtonFillColorSet(&viewPicBtn,ClrBlue);
		PushButtonFillColorPressedSet(&viewPicBtn,ClrBlue);
		PushButtonCallbackSet(&viewPicBtn,onViewClick);
	} else {
		PushButtonFillColorSet(&viewPicBtn,ClrBlack);
		PushButtonFillColorPressedSet(&viewPicBtn,ClrBlack);
		PushButtonCallbackSet(&viewPicBtn,onClickDoNothing);
	}
	WidgetPaint((tWidget*)&viewPicBtn);
}

void refreshPageCounter()
{
	sprintf(pageCounterString, "%d/%d", currentPage + 1, maxPageNumber + 1);
	CanvasTextSet(&pageCounter, pageCounterString);
}

void onUpClick()
{
	if (currentPage == 0)
		return;
	currentPage--;
	populateList(currentPage);
	refreshPageCounter();
	WidgetPaint((tWidget*)&fileListBox);
}

void onDownClick()
{
	if (currentPage == maxPageNumber)
		return;
	currentPage++;
	populateList(currentPage);
	refreshPageCounter();
	WidgetPaint((tWidget*)&fileListBox);
}

void onViewClick()
{
	exitSelectPicApp();
	startViewPicApp(selectedFilename);
}

void onClickDoNothing(tWidget* pWidget)
{
	return;
}

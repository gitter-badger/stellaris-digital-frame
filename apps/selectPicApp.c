/*
 * selectPic.c
 *
 *  Created on: 10 ���� 2014
 *      Author: Itay
 */
#include <string.h>
#include <stdio.h>

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

void onGoBackClick();
void onChange(tWidget* pWidget, short index);

Canvas(selectPicBackgroundCanvas, 0, 0, 0, &DisplayStructure, 0, 0, 319, 239,
		CANVAS_STYLE_FILL, ClrBlack, ClrWhite, ClrBlack, 0, 0, 0, 0);

ListBox(fileListBox, &selectPicBackgroundCanvas, 0, 0, &DisplayStructure, 0, 0,
		300, 200, LISTBOX_STYLE_OUTLINE, ClrBlack, ClrWhite, ClrWhite, ClrBlack,
		ClrWhite, g_pFontCm16, 0, 10, 0, onChange);

RectangularButton(backToMenuBtn, &selectPicBackgroundCanvas, 0, 0,
		&DisplayStructure, 5, 195, 80, 40,
		PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_OUTLINE, ClrRed, ClrRed,
		ClrWhite, ClrBlack, g_pFontCm16, "Go Back", 0, 0, 0, 0, onGoBackClick);

extern tCanvasWidget GeneralErrorMsg;

char * strings[10];
FATFS fatfs;

void startSelectPicApp()
{
	WidgetAdd(WIDGET_ROOT, (tWidget*) &selectPicBackgroundCanvas);
	WidgetAdd((tWidget*) &selectPicBackgroundCanvas, (tWidget*) &backToMenuBtn);
	FRESULT result = f_mount(&fatfs, "0:", 1);
	if (result != FR_OK)
	{
		WidgetAdd((tWidget*) &selectPicBackgroundCanvas,
				(tWidget*) &GeneralErrorMsg);
		CanvasTextSet(&GeneralErrorMsg,"No filesystem found!");
	}
	else
	{
		WidgetAdd((tWidget*) &selectPicBackgroundCanvas,
				(tWidget*) &fileListBox);
	}
	DIR root;
	FILINFO fileInfo;
	f_opendir(&root, "0:");
	do
	{
		f_readdir(&root, &fileInfo);
		printf("%s\n", fileInfo.fname);
	} while (strcmp(fileInfo.fname,"\0") != 0);
	f_closedir(&root);
	WidgetPaint(WIDGET_ROOT);
}

void exitSelectPicApp()
{
	WidgetRemove((tWidget*) & selectPicBackgroundCanvas);
}

void onGoBackClick()
{
	exitSelectPicApp();
	startMainMenuApplication();
}
void onChange(tWidget* pWidget, short index) {

}

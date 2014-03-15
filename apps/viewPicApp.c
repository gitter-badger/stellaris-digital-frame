/*
 * viewPicApp.c
 *
 *  Created on: 12 במרס 2014
 *      Author: Itay Komemy
 */


#include <string.h>
#include <stdio.h>

#include "inc/hw_types.h"
#include "../lcd/grlibDriver.h"
#include "../touch/xpt2046.h"
#include "../bmp/bmpfile.h"
#include "../bmp/bmpdisplay.h"
#include "../third_party/fatfs/src/ff.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/pushbutton.h"
#include "../clock/clock.h"
#include "selectPicApp.h"
#include "viewPicApp.h"


tContext viewPicContext;

void onExitBtnClick();

RectangularButton(exitViewPicAppBtn, WIDGET_ROOT, 0, 0, &DisplayStructure, 0, 0, 320, 240,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, onExitBtnClick);


void startViewPicApp(char* pfname)
{

	showClock(WIDGET_ROOT);
	WidgetAdd(WIDGET_ROOT,(tWidget*)&exitViewPicAppBtn);
	WidgetPaint(WIDGET_ROOT);
	GrContextInit(&viewPicContext,&DisplayStructure);
	tBitmapInst* pBtmap = BitmapOpen(pfname);
	BitmapDraw(pBtmap,&viewPicContext,0,0);
	BitmapClose(pBtmap);
	xpt2046_enableTouchIRQ();
}

void exitViewPicApp()
{
	xpt2046_disableTouchIRQ();
	hideClock();
	WidgetRemove((tWidget*)&exitViewPicAppBtn);
}

void onExitBtnClick() {
	exitViewPicApp();
	startSelectPicApp();
}

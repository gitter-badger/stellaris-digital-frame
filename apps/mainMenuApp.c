/*
 * mainMenuApp.c
 *
 *  Created on: 9 במרס 2014
 *      Author: Itay
 */

#include "../lcd/grlibDriver.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/pushbutton.h"
#include "../clock/clock.h"
#include "setTimeApp.h"
#include "slideshowApp.h"
#include "selectPicApp.h"
#include "mainMenuApp.h"
#include "../resources/images.h"

// Forward declerations
void onSetTimeBtnClk();
void onStartSlideshowBtnClk();
void onSelectPicApp();
void exitMainMenuApp();

void onPaintBackgroundPaint(tWidget* pWidget, tContext* ctx)
{
	// Put the application name in the middle of the banner.
	GrContextFontSet(ctx, g_pFontCm30b);
	GrContextForegroundSet(ctx, ClrRed);
	GrStringDrawCentered(ctx, APPLICATION_NAME, -1,
			GrContextDpyWidthGet(ctx) / 2, 40, 0);
	// Put the application name in the middle of the banner.
	GrContextFontSet(ctx, g_pFontCm14);
	GrContextForegroundSet(ctx, ClrAquamarine);
	GrStringDrawCentered(ctx, FOOTER, -1, GrContextDpyWidthGet(ctx) / 2,
			GrContextDpyHeightGet(ctx)-14, 0);
	GrImageDraw(ctx, g_pucLogo, 5, GrContextDpyHeightGet(ctx) - 70);
}

Canvas(mainMenuBackgroundCanvas, WIDGET_ROOT, 0, 0, &DisplayStructure, 0, 0,
		320, 240, CANVAS_STYLE_FILL | CANVAS_STYLE_APP_DRAWN, ClrBlack,
		ClrWhite, ClrBlack, 0, 0, 0, onPaintBackgroundPaint);

RectangularButton(startSlideshowBtn, &mainMenuBackgroundCanvas, 0, 0,
		&DisplayStructure, FIRST_BTN_X_COR, FIRST_BTN_Y_COR, MENU_BTN_WIDTH,
		MENU_BTN_HIEGHT, PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_OUTLINE,
		ClrRed, ClrRed, ClrWhite, ClrWhite, g_pFontCm26, "Start Slideshow", 0,
		0, 0, 0, onStartSlideshowBtnClk);

RectangularButton(viewSinglePicBtn, &mainMenuBackgroundCanvas, &startSlideshowBtn, 0,
		&DisplayStructure, FIRST_BTN_X_COR,
		FIRST_BTN_Y_COR + MENU_BTN_HIEGHT + MENU_BTN_PADDING_TOP,
		MENU_BTN_WIDTH, MENU_BTN_HIEGHT,
		PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_OUTLINE, ClrRed, ClrRed,
		ClrWhite, ClrWhite, g_pFontCm26, "View a Picture", 0, 0, 0, 0,
		onSelectPicApp);

RectangularButton(setTimeBtn, &mainMenuBackgroundCanvas, &viewSinglePicBtn, 0,
		&DisplayStructure, FIRST_BTN_X_COR,
		FIRST_BTN_Y_COR + MENU_BTN_HIEGHT*2 + MENU_BTN_PADDING_TOP*2,
		MENU_BTN_WIDTH, MENU_BTN_HIEGHT,
		PB_STYLE_TEXT | PB_STYLE_FILL | PB_STYLE_OUTLINE, ClrRed, ClrRed,
		ClrWhite, ClrWhite, g_pFontCm26, "Set Time", 0, 0, 0, 0,
		onSetTimeBtnClk);

void onSetTimeBtnClk()
{
	exitMainMenuApp();
	startSetTimeApp();
}

void onStartSlideshowBtnClk()
{
	exitMainMenuApp();
	startSlideShowApp(5);
}

void onSelectPicApp()
{
	exitMainMenuApp();
	startSelectPicApp();
}

// ctx is an initialized context
void startMainMenuApplication()
{
	showClock((tWidget*) &mainMenuBackgroundCanvas);
	WidgetAdd(WIDGET_ROOT, (tWidget*) &mainMenuBackgroundCanvas);
	WidgetAdd((tWidget*) &mainMenuBackgroundCanvas, (tWidget*) &setTimeBtn);
	WidgetAdd((tWidget*) &mainMenuBackgroundCanvas,
			(tWidget*) &viewSinglePicBtn);
	WidgetAdd((tWidget*) &mainMenuBackgroundCanvas,
				(tWidget*) &startSlideshowBtn);

	WidgetPaint(WIDGET_ROOT);
}

void exitMainMenuApp()
{
	hideClock();
	WidgetRemove((tWidget*) &mainMenuBackgroundCanvas);
	WidgetRemove((tWidget*) &setTimeBtn);
	WidgetRemove((tWidget*) &viewSinglePicBtn);
	WidgetRemove((tWidget*) &startSlideshowBtn);
}

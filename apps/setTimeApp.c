/*
 * setTimeApp.c
 *
 *  Created on: 8 במרס 2014
 *      Author: Itay
 */

#include <stdio.h>
#include "../rtc/rtc.h"
#include "setTimeApp.h"
#include "grlib/container.h"
#include "grlib/pushbutton.h"
#include "../lcd/grlibDriver.h"

// Positioning and sizing constants
#define CONTAINER_HIEGHT 90
#define CONTAINER_WIDTH 150

#define FIRST_CONTAINER_X 5
#define FIRST_CONTAINER_Y 50

#define SECOND_CONTAINER_X (CONTAINER_WIDTH + FIRST_CONTAINER_X + 5)
#define SECOND_CONTAINER_Y FIRST_CONTAINER_Y

#define TIME_UP_BTN_Y 10
#define BTN_HEIGHT 20
#define BTN_WIDTH 40

#define TEXTBOX_HEIGHT 30
#define TEXTBOX_WIDTH 40

#define COMMIT_BTN_WIDTH 80
#define COMMIT_BTN_HEIGHT 40

#define CANCEL_BTN_WIDTH 80
#define CANCEL_BTN_HEIGHT 40

DS3234_DATE date;
DS3234_TIME time;

char days[3] = "00";
char months[3] = "00";
char years[5] = "2000";
char hours[3] = "00";
char minutes[3] = "00";
char seconds[3] = "00";

// Forward declarations of OnClick handlers
void hoursUpBtnClk();
void hoursDownBtnClk();
void minutesUpBtnClk();
void minutesDownBtnClk();
void secondsUpBtnClk();
void secondsDownBtnClk();
void daysUpBtnClk();
void daysDownBtnClk();
void monthsUpBtnClk();
void monthsDownBtnClk();
void yearsUpBtnClk();
void yearsDownBtnClk();

void exitBackToMenu();

///
/// Containers for controls
///
Container(timeSetBox, 0, 0, 0, &DisplayStructure, FIRST_CONTAINER_X,
		FIRST_CONTAINER_Y, CONTAINER_WIDTH, CONTAINER_HIEGHT,
		CTR_STYLE_FILL | CTR_STYLE_OUTLINE | CTR_STYLE_TEXT, ClrChocolate,
		ClrWhite, ClrWhite, g_pFontCm14, "Set Time:");
Container(dateSetBox, 0, 0, 0, &DisplayStructure, SECOND_CONTAINER_X,
		FIRST_CONTAINER_Y, CONTAINER_WIDTH, CONTAINER_HIEGHT,
		CTR_STYLE_FILL | CTR_STYLE_OUTLINE | CTR_STYLE_TEXT, ClrChocolate,
		ClrWhite, ClrWhite, g_pFontCm14, "Set Date:");

#define UPSYMBOL "UP"
#define DOWNSYMBOL "DOWN"

///
/// Hours control group
///
RectangularButton(hoursUpBtn, 0, 0, 0, &DisplayStructure, FIRST_CONTAINER_X + 5,
		FIRST_CONTAINER_Y + 15, BTN_WIDTH, BTN_HEIGHT,
		PB_STYLE_OUTLINE | PB_STYLE_FILL | PB_STYLE_TEXT, ClrBlue, ClrRed,
		ClrWhite, ClrWhite, g_pFontCm12, UPSYMBOL, 0, 0, 0, 0, hoursUpBtnClk);

Canvas(hoursTextBox, 0, 0, 0, &DisplayStructure, FIRST_CONTAINER_X + 5,
		FIRST_CONTAINER_Y + 15 + BTN_HEIGHT, TEXTBOX_WIDTH, TEXTBOX_HEIGHT,
		CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT, ClrWhite, 0, ClrBlack,
		g_pFontCm14, hours, 0, 0);

RectangularButton(hoursDownBtn, 0, 0, 0, &DisplayStructure,
		FIRST_CONTAINER_X + 5,
		FIRST_CONTAINER_Y + 15 + BTN_HEIGHT + TEXTBOX_HEIGHT, BTN_WIDTH,
		BTN_HEIGHT, PB_STYLE_OUTLINE | PB_STYLE_FILL | PB_STYLE_TEXT, ClrBlue,
		ClrRed, ClrWhite, ClrWhite, g_pFontCm12, DOWNSYMBOL, 0, 0, 0, 0,
		hoursDownBtnClk);

///
/// Minutes control group
///
RectangularButton(minutesUpBtn, (tWidget*) &timeSetBox, 0, 0, &DisplayStructure,
		FIRST_CONTAINER_X + 5 + TEXTBOX_WIDTH + 5, FIRST_CONTAINER_Y + 15,
		BTN_WIDTH, BTN_HEIGHT, PB_STYLE_OUTLINE | PB_STYLE_FILL | PB_STYLE_TEXT,
		ClrBlue, ClrRed, ClrWhite, ClrWhite, g_pFontCm12, UPSYMBOL, 0, 0, 0, 0,
		minutesUpBtnClk);

Canvas(minutesTextBox, (tWidget*) &timeSetBox, 0, 0, &DisplayStructure,
		FIRST_CONTAINER_X + 5 + TEXTBOX_WIDTH + 5,
		FIRST_CONTAINER_Y + 15 + BTN_HEIGHT, TEXTBOX_WIDTH, TEXTBOX_HEIGHT,
		CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT, ClrWhite, 0, ClrBlack,
		g_pFontCm14, minutes, 0, 0);

RectangularButton(minutesDownBtn, 0, 0, 0, &DisplayStructure,
		FIRST_CONTAINER_X + 5 + TEXTBOX_WIDTH + 5,
		FIRST_CONTAINER_Y + 15 + BTN_HEIGHT + TEXTBOX_HEIGHT, BTN_WIDTH,
		BTN_HEIGHT, PB_STYLE_OUTLINE | PB_STYLE_FILL | PB_STYLE_TEXT, ClrBlue,
		ClrRed, ClrWhite, ClrWhite, g_pFontCm12, DOWNSYMBOL, 0, 0, 0, 0,
		minutesDownBtnClk);

///
/// Seconds control group
///
RectangularButton(secondsUpBtn, 0, 0, 0, &DisplayStructure,
		FIRST_CONTAINER_X + 5 + (TEXTBOX_WIDTH + 5) * 2, FIRST_CONTAINER_Y + 15,
		BTN_WIDTH, BTN_HEIGHT, PB_STYLE_OUTLINE | PB_STYLE_FILL | PB_STYLE_TEXT,
		ClrBlue, ClrRed, ClrWhite, ClrWhite, g_pFontCm12, UPSYMBOL, 0, 0, 0, 0,
		secondsUpBtnClk);

Canvas(secondsTextBox, 0, 0, 0, &DisplayStructure,
		FIRST_CONTAINER_X + 5 + (TEXTBOX_WIDTH + 5) * 2,
		FIRST_CONTAINER_Y + 15 + BTN_HEIGHT, TEXTBOX_WIDTH, TEXTBOX_HEIGHT,
		CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT, ClrWhite, 0, ClrBlack,
		g_pFontCm14, seconds, 0, 0);

RectangularButton(secondsDownBtn, 0, 0, 0, &DisplayStructure,
		FIRST_CONTAINER_X + 5 + (TEXTBOX_WIDTH + 5) * 2,
		FIRST_CONTAINER_Y + 15 + BTN_HEIGHT + TEXTBOX_HEIGHT, BTN_WIDTH,
		BTN_HEIGHT, PB_STYLE_OUTLINE | PB_STYLE_FILL | PB_STYLE_TEXT, ClrBlue,
		ClrRed, ClrWhite, ClrWhite, g_pFontCm12, DOWNSYMBOL, 0, 0, 0, 0,
		secondsDownBtnClk);

///
/// Days control group
///
RectangularButton(daysUpBtn, 0, 0, 0, &DisplayStructure, SECOND_CONTAINER_X + 5,
		SECOND_CONTAINER_Y + 15, BTN_WIDTH, BTN_HEIGHT,
		PB_STYLE_OUTLINE | PB_STYLE_FILL | PB_STYLE_TEXT, ClrBlue, ClrRed,
		ClrWhite, ClrWhite, g_pFontCm12, UPSYMBOL, 0, 0, 0, 0, daysUpBtnClk);

Canvas(daysTextBox, 0, 0, 0, &DisplayStructure, SECOND_CONTAINER_X + 5,
		SECOND_CONTAINER_Y + 15 + BTN_HEIGHT, TEXTBOX_WIDTH, TEXTBOX_HEIGHT,
		CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT, ClrWhite, 0, ClrBlack,
		g_pFontCm14, days, 0, 0);

RectangularButton(daysDownBtn, 0, 0, 0, &DisplayStructure,
		SECOND_CONTAINER_X + 5,
		SECOND_CONTAINER_Y + 15 + BTN_HEIGHT + TEXTBOX_HEIGHT, BTN_WIDTH,
		BTN_HEIGHT, PB_STYLE_OUTLINE | PB_STYLE_FILL | PB_STYLE_TEXT, ClrBlue,
		ClrRed, ClrWhite, ClrWhite, g_pFontCm12, DOWNSYMBOL, 0, 0, 0, 0,
		daysDownBtnClk);

///
/// Months control group
///
RectangularButton(monthsUpBtn, 0, 0, 0, &DisplayStructure,
		SECOND_CONTAINER_X + 5 + TEXTBOX_WIDTH + 5, SECOND_CONTAINER_Y + 15,
		BTN_WIDTH, BTN_HEIGHT, PB_STYLE_OUTLINE | PB_STYLE_FILL | PB_STYLE_TEXT,
		ClrBlue, ClrRed, ClrWhite, ClrWhite, g_pFontCm12, UPSYMBOL, 0, 0, 0, 0,
		monthsUpBtnClk);

Canvas(monthsTextBox, 0, 0, 0, &DisplayStructure,
		SECOND_CONTAINER_X + 5 + TEXTBOX_WIDTH + 5,
		SECOND_CONTAINER_Y + 15 + BTN_HEIGHT, TEXTBOX_WIDTH, TEXTBOX_HEIGHT,
		CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT, ClrWhite, 0, ClrBlack,
		g_pFontCm14, months, 0, 0);

RectangularButton(monthsDownBtn, 0, 0, 0, &DisplayStructure,
		SECOND_CONTAINER_X + 5 + TEXTBOX_WIDTH + 5,
		SECOND_CONTAINER_Y + 15 + BTN_HEIGHT + TEXTBOX_HEIGHT, BTN_WIDTH,
		BTN_HEIGHT, PB_STYLE_OUTLINE | PB_STYLE_FILL | PB_STYLE_TEXT, ClrBlue,
		ClrRed, ClrWhite, ClrWhite, g_pFontCm12, DOWNSYMBOL, 0, 0, 0, 0,
		monthsDownBtnClk);

///
/// Years control group
///
RectangularButton(yearsUpBtn, 0, 0, 0, &DisplayStructure,
		SECOND_CONTAINER_X + 5 + (TEXTBOX_WIDTH + 5) * 2,
		SECOND_CONTAINER_Y + 15, BTN_WIDTH, BTN_HEIGHT,
		PB_STYLE_OUTLINE | PB_STYLE_FILL | PB_STYLE_TEXT, ClrBlue, ClrRed,
		ClrWhite, ClrWhite, g_pFontCm12, UPSYMBOL, 0, 0, 0, 0, yearsUpBtnClk);

Canvas(yearsTextBox, 0, 0, 0, &DisplayStructure,
		SECOND_CONTAINER_X + 5 + (TEXTBOX_WIDTH + 5) * 2,
		SECOND_CONTAINER_Y + 15 + BTN_HEIGHT, TEXTBOX_WIDTH, TEXTBOX_HEIGHT,
		CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT, ClrWhite, 0, ClrBlack,
		g_pFontCm14, years, 0, 0);

RectangularButton(yearsDownBtn, 0, 0, 0, &DisplayStructure,
		SECOND_CONTAINER_X + 5 + (TEXTBOX_WIDTH + 5) * 2,
		SECOND_CONTAINER_Y + 15 + BTN_HEIGHT + TEXTBOX_HEIGHT, BTN_WIDTH,
		BTN_HEIGHT, PB_STYLE_OUTLINE | PB_STYLE_FILL | PB_STYLE_TEXT, ClrBlue,
		ClrRed, ClrWhite, ClrWhite, g_pFontCm12, DOWNSYMBOL, 0, 0, 0, 0,
		yearsDownBtnClk);

void hoursUpBtnClk()
{
	if (time.hours == 23)
		time.hours = 0;
	else
		time.hours++;
	sprintf(hours, "%02d", time.hours);
	WidgetPaint((tWidget*)&hoursTextBox);
}

void hoursDownBtnClk()
{
	if (time.hours == 0)
		time.hours = 23;
	else
		time.hours--;
	sprintf(hours, "%02d", time.hours);
	WidgetPaint((tWidget*)&hoursTextBox);
}

void minutesUpBtnClk()
{
	if (time.minutes == 59)
		time.minutes = 0;
	else
		time.minutes++;
	sprintf(minutes, "%02d", time.minutes);
	WidgetPaint((tWidget*)&minutesTextBox);
}
void minutesDownBtnClk()
{
	if (time.minutes == 0)
		time.minutes = 59;
	else
		time.minutes--;
	sprintf(minutes, "%02d", time.minutes);
	WidgetPaint((tWidget*)&minutesTextBox);
}

void secondsUpBtnClk()
{
	if (time.seconds == 59)
		time.seconds = 0;
	else
		time.seconds++;
	sprintf(seconds, "%02d", time.seconds);
	WidgetPaint((tWidget*)&secondsTextBox);
}
void secondsDownBtnClk()
{
	if (time.seconds == 0)
		time.seconds = 59;
	else
		time.seconds--;
	sprintf(seconds, "%02d", time.seconds);
	WidgetPaint((tWidget*)&secondsTextBox);
}

void daysUpBtnClk()
{
	if (date.day_of_month == 31)
		date.day_of_month = 0;
	else
		date.day_of_month++;
	sprintf(days, "%02d", date.day_of_month);
	WidgetPaint((tWidget*)&daysTextBox);
}
void daysDownBtnClk()
{
	if (date.day_of_month == 0)
		date.day_of_month = 31;
	else
		date.day_of_month--;
	sprintf(days, "%02d", date.day_of_month);
	WidgetPaint((tWidget*)&daysTextBox);
}
void monthsUpBtnClk()
{
	if (date.month == 12)
		date.month = 0;
	else
		date.month++;
	sprintf(months, "%02d", date.month);
	WidgetPaint((tWidget*)&monthsTextBox);
}
void monthsDownBtnClk()
{
	if (date.month == 0)
		date.month = 31;
	else
		date.month--;
	sprintf(months, "%02d", date.month);
	WidgetPaint((tWidget*)&monthsTextBox);
}
void yearsUpBtnClk()
{
	if (date.year == 99)
		date.year = 0;
	else
		date.year++;
	sprintf(years, "20%02d", date.year);
	WidgetPaint((tWidget*)&yearsTextBox);
}
void yearsDownBtnClk()
{
	if (date.year == 0)
		date.year = 99;
	else
		date.year--;
	sprintf(years, "20%02d", date.year);
	WidgetPaint((tWidget*)&yearsTextBox);
}

void onCommitClick()
{
	ds3234_write_date(&date);
	ds3234_write_time(&time);
	exitBackToMenu();
}

void onCancelClick()
{
	exitBackToMenu();
}

RectangularButton(CommitBtn, 0, 0, 0, &DisplayStructure,
		320/2 - COMMIT_BTN_WIDTH - 10,
		FIRST_CONTAINER_Y + CONTAINER_HIEGHT + 20, COMMIT_BTN_WIDTH,
		COMMIT_BTN_HEIGHT, PB_STYLE_FILL | PB_STYLE_TEXT | PB_STYLE_OUTLINE,
		ClrGreen, ClrGreen, ClrWhite, ClrBlack, g_pFontCm18, "Commit", 0, 0, 0,
		0, onCommitClick);

RectangularButton(CancelBtn, 0, 0, 0, &DisplayStructure, 320/2 + 10,
		FIRST_CONTAINER_Y + CONTAINER_HIEGHT + 20, CANCEL_BTN_WIDTH,
		CANCEL_BTN_HEIGHT, PB_STYLE_FILL | PB_STYLE_TEXT | PB_STYLE_OUTLINE,
		ClrRed, ClrRed, ClrWhite, ClrBlack, g_pFontCm18, "Cancel", 0, 0, 0, 0,
		onCancelClick);

// Forward declaration
void updateControls();

void StartSetTimeApp(tCanvasWidget* pCanvas)
{
	ds3234_read_date(&date);
	ds3234_read_time(&time);

	updateControls();

	WidgetAdd((tWidget*) pCanvas, (tWidget*) &timeSetBox);
	WidgetAdd((tWidget*) pCanvas, (tWidget*) &dateSetBox);

	WidgetAdd((tWidget*) pCanvas, (tWidget*) &CommitBtn);
	WidgetAdd((tWidget*) pCanvas, (tWidget*) &CancelBtn);

	WidgetAdd((tWidget*) &timeSetBox, (tWidget*) &hoursUpBtn);
	WidgetAdd((tWidget*) &timeSetBox, (tWidget*) &hoursDownBtn);
	WidgetAdd((tWidget*) &timeSetBox, (tWidget*) &hoursTextBox);

	WidgetAdd((tWidget*) &timeSetBox, (tWidget*) &minutesUpBtn);
	WidgetAdd((tWidget*) &timeSetBox, (tWidget*) &minutesDownBtn);
	WidgetAdd((tWidget*) &timeSetBox, (tWidget*) &minutesTextBox);

	WidgetAdd((tWidget*) &timeSetBox, (tWidget*) &secondsUpBtn);
	WidgetAdd((tWidget*) &timeSetBox, (tWidget*) &secondsDownBtn);
	WidgetAdd((tWidget*) &timeSetBox, (tWidget*) &secondsTextBox);

	WidgetAdd((tWidget*) &dateSetBox, (tWidget*) &daysUpBtn);
	WidgetAdd((tWidget*) &dateSetBox, (tWidget*) &daysDownBtn);
	WidgetAdd((tWidget*) &dateSetBox, (tWidget*) &daysTextBox);

	WidgetAdd((tWidget*) &dateSetBox, (tWidget*) &monthsUpBtn);
	WidgetAdd((tWidget*) &dateSetBox, (tWidget*) &monthsDownBtn);
	WidgetAdd((tWidget*) &dateSetBox, (tWidget*) &monthsTextBox);

	WidgetAdd((tWidget*) &dateSetBox, (tWidget*) &yearsUpBtn);
	WidgetAdd((tWidget*) &dateSetBox, (tWidget*) &yearsDownBtn);
	WidgetAdd((tWidget*) &dateSetBox, (tWidget*) &yearsTextBox);

	WidgetPaint(WIDGET_ROOT);
}

void updateControls()
{
	sprintf(hours, "%02d", time.hours);
	CanvasTextSet(&hoursTextBox, hours);

	sprintf(minutes, "%02d", time.minutes);
	CanvasTextSet(&minutesTextBox, minutes);

	sprintf(seconds, "%02d", time.seconds);
	CanvasTextSet(&secondsTextBox, seconds);

	sprintf(days, "%02d", date.day_of_month);
	CanvasTextSet(&daysTextBox, days);

	sprintf(months, "%02d", date.month);
	CanvasTextSet(&monthsTextBox, months);

	sprintf(years, "20%02d", date.year);
	CanvasTextSet(&yearsTextBox, years);
}

void exitBackToMenu()
{
	WidgetRemove((tWidget*) &timeSetBox);
	WidgetRemove((tWidget*) &dateSetBox);

	WidgetRemove((tWidget*) &CommitBtn);
	WidgetRemove((tWidget*) &CancelBtn);

	WidgetRemove((tWidget*) &hoursUpBtn);
	WidgetRemove((tWidget*) &hoursDownBtn);
	WidgetRemove((tWidget*) &hoursTextBox);

	WidgetRemove((tWidget*) &minutesUpBtn);
	WidgetRemove((tWidget*) &minutesDownBtn);
	WidgetRemove((tWidget*) &minutesTextBox);

	WidgetRemove((tWidget*) &secondsUpBtn);
	WidgetRemove((tWidget*) &secondsDownBtn);
	WidgetRemove((tWidget*) &secondsTextBox);

	WidgetRemove((tWidget*) &daysUpBtn);
	WidgetRemove((tWidget*) &daysDownBtn);
	WidgetRemove((tWidget*) &daysTextBox);

	WidgetRemove((tWidget*) &monthsUpBtn);
	WidgetRemove((tWidget*) &monthsDownBtn);
	WidgetRemove((tWidget*) &monthsTextBox);

	WidgetRemove((tWidget*) &yearsUpBtn);
	WidgetRemove((tWidget*) &yearsDownBtn);
	WidgetRemove((tWidget*) &yearsTextBox);

	startMainMenuApplication();
}

/*
 * clock.c
 *
 *  Created on: 5 במרס 2014
 *      Author: Itay Komemy
 */

#include <stdio.h>
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "../rtc/rtc.h"

#include "clock.h"

static volatile int initiallized = 0;
static volatile int show = 0;
static char* timeDisplayFormat = "%02d:%02d:%02d %02d/%02d/%02d";
static tCanvasWidget clockWidget;

static const tDisplay* p_clockDisplay;

DS3234_DATE today;
DS3234_TIME now;

char datetime[30];

void initClock(const tDisplay* pDisplay)
{
	if (initiallized)
		return;

	ds3234_read_time(&now);
	ds3234_read_date(&today);

	p_clockDisplay = pDisplay;

	CanvasInit(&clockWidget, p_clockDisplay, 1, 1, 120, 20);
	CanvasFillOn(&clockWidget);
	CanvasFillColorSet(&clockWidget, ClrBlack);
	CanvasTextOn(&clockWidget);
	CanvasTextAlignment(&clockWidget,
			CANVAS_STYLE_TEXT_HCENTER | CANVAS_STYLE_TEXT_VCENTER);
	CanvasTextColorSet(&clockWidget, ClrAqua);

	sprintf(datetime, timeDisplayFormat, now.hours, now.minutes, now.seconds,
			today.day_of_month, today.month, today.year);

	CanvasTextSet(&clockWidget, datetime);
	CanvasFontSet(&clockWidget, g_pFontCm14);

	initiallized = 1;
}

///
/// Should be called every minute
///
void updateClock()
{
	if (!initiallized)
		return;

	if (!show)
		return;

	ds3234_read_time(&now);
	ds3234_read_date(&today);

	sprintf(datetime, timeDisplayFormat, now.hours, now.minutes, now.seconds,
			today.day_of_month, today.month, today.year);

	CanvasTextSet(&clockWidget, datetime);

	WidgetPaint((tWidget*)&clockWidget);
}

void hideClock()
{
	show = 0;
	WidgetRemove((tWidget*) &clockWidget);
}

void showClock(tWidget* parent)
{
	show = 1;
	WidgetAdd(parent, (tWidget*) &clockWidget);
}

void setClock(DS3234_DATE date, DS3234_TIME time)
{
	now = time;
	today = date;
}

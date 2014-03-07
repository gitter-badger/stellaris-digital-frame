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
static char* timeDisplayFormat = "%d:%d:%d, %d/%d/%d";
static tCanvasWidget clockWidget;

static tContext* p_clockContext;
static const tDisplay* p_clockDisplay;

DS3234_DATE today;
DS3234_TIME now;

char datetime[25];

void initClock(tContext* pContext, const tDisplay* pDisplay)
{
	if (initiallized)
		return;

	ds3234_read_time(&now);
	ds3234_read_date(&today);

	p_clockContext = pContext;
	p_clockDisplay = pDisplay;

	CanvasInit(&clockWidget, p_clockDisplay, 1, 1, 80, 10);
	CanvasFillOn(&clockWidget);
	CanvasFillColorSet(&clockWidget,ClrBlack);
	CanvasTextOn(&clockWidget);
	CanvasTextAlignment(&clockWidget,
			CANVAS_STYLE_TEXT_HCENTER | CANVAS_STYLE_TEXT_VCENTER);
	CanvasTextColorSet(&clockWidget, ClrAqua);

	sprintf(datetime, timeDisplayFormat, now.hours, now.minutes, now.seconds,
			today.day_of_month, today.month, today.year);

	CanvasTextSet(&clockWidget, datetime);
	CanvasFontSet(&clockWidget, g_pFontCm12);
	WidgetAdd(WIDGET_ROOT, (tWidget*) &clockWidget);

	initiallized = 1;
}

///
/// Should be called every minute
///
void updateClock()
{
	if (!initiallized)
		return;

	ds3234_read_time(&now);
	ds3234_read_date(&today);

	sprintf(datetime, timeDisplayFormat, now.hours, now.minutes, now.seconds,
			today.day_of_month, today.month, today.year);

	CanvasTextSet(&clockWidget, datetime);

	WidgetPaint((tWidget*)&clockWidget);
}

void setClock(DS3234_DATE date, DS3234_TIME time)
{
	now = time;
	today = date;
}

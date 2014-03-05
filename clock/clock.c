/*
 * clock.c
 *
 *  Created on: 5 במרס 2014
 *      Author: Itay Komemy
 */

#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "../rtc/rtc.h"

#include "clock.h"

DS3234_DATE today;
DS3234_TIME now;

tWidget clockWidget =

void updateClock(tContext* pContext, tDisplay* pDisplay)
{
	ds3234_read_date(&today);
	ds3234_read_time(&now);

}

void initClock(tContext* pContext, tDisplay* pDisplay)
{

}


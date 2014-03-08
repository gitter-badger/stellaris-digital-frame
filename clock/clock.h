/*
 * clock.h
 *
 *  Created on: 5 במרס 2014
 *      Author: Itay Komemy
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"


extern tCanvasWidget clockWidget;

void initClock(tContext*,const tDisplay*);
void updateClock();
void removeClock();
void showClock();
void hideClock();

#endif /* CLOCK_H_ */

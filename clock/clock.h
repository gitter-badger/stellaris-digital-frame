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

#define CLOCK_WIDTH 	120
#define CLOCK_HEIGHT	20

extern tCanvasWidget clockWidget;

void initClock(const tDisplay*);
void updateClock();
void removeClock();
void showClock(tWidget* parent);
void hideClock();

#endif /* CLOCK_H_ */

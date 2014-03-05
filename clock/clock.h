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


extern tWidget clockWidget;

void initClock(tContext*, tDisplay*);
void updateClock(tContext*, tDisplay*);
void removeClock(tContext*, tDisplay*);


#endif /* CLOCK_H_ */

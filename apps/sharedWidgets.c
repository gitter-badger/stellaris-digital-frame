/*
 * sharedWidgets.h
 *
 *  Created on: 11 במרס 2014
 *      Author: Itay
 */

#include "../lcd/grlibDriver.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"

Canvas(GeneralErrorMsg, 0, 0, 0,
		&DisplayStructure, 0, 80, 320, 60 ,
		CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT_HCENTER | CANVAS_STYLE_TEXT_VCENTER,
		ClrBlack, ClrWhite, ClrRed, g_pFontCm20b, "", 0, 0);


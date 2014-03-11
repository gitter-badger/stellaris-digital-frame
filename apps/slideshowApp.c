/*
 * slideshowApp.c
 *
 *  Created on: 9 במרס 2014
 *      Author: Itay
 */

#include "inc/hw_types.h"

#include "grlib/grlib.h"
#include "../lcd/grlibDriver.h"
#include "../bmp/bmpfile.h"
#include "../bmp/bmpdisplay.h"

tContext pCtx;

void startSlideShowApp()
{
	GrContextInit(&pCtx,&DisplayStructure);

	tBitmapInst* pBtmpInst = BitmapOpen("0:4.bmp");
	BitmapDraw(pBtmpInst,&pCtx,0,0);
	BitmapClose(pBtmpInst);
}

#include <string.h>
#include "io.h"
#include "inc/lm3s9b92.h"
#include "driverlib/rom.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/ustdlib.h"
#include "third_party/fatfs/src/diskio.h"
#include "third_party/fatfs/src/ff.h"
#include "third_party/fatfs/port/ssi_hw.h"
#include "grlib/grlib.h"
#include "lcd/grlibDriver.h"
#include "touch/xpt2046.h"
#include "rtc/rtc.h"

volatile unsigned int status = 0, standby_active = 0;
volatile long on_time = 0;
volatile unsigned long sec_time = 0;

//*****************************************************************************
//
// The following are data structures used by FatFs.
//
//*****************************************************************************
static FATFS fs;
//*****************************************************************************
//
// The number of SysTick ticks per second.
//
//*****************************************************************************
#define TICKS_PER_SECOND 100

//*****************************************************************************
//
// This is the handler for this SysTick interrupt.  FatFs requires a
// timer tick every 10 ms for internal timing purposes.
//
//*****************************************************************************
void SysTickHandler(void)
{
	//
	// Call the FatFs tick timer.
	//
	disk_timerproc();
}

long touchScreenCallback(unsigned long message, long x,long y){

	printf("Touched x: %d, y: %d",x,y);
}


int main(void)
{

	FRESULT fresult;

	//
	// Set the system clock to run at 50MHz from the PLL.
	//
	SysCtlClockSet(
			SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_16MHZ);
	//
	// Configure SysTick for a 100Hz interrupt.
	//
	SysTickPeriodSet(SysCtlClockGet() / TICKS_PER_SECOND);
	SysTickEnable();
	SysTickIntEnable();
	long systickPeriod = SysTickPeriodGet();
	long sysCtlClk = SysCtlClockGet();

	rtc_init();


	// Mount fs
	fresult = f_mount(&fs, "0:", 0);

	// Init touch
	xpt2046_init();
	xpt2046_setTouchScreenCallback(touchScreenCallback);


	// Init LCD
	ssd1289_init();

	//Draw something
	tContext sContext;
	const tDisplay* pDisplay = &DisplayStructure;
	GrContextInit(&sContext, pDisplay);
	tRectangle sRect;
	//
	// Fill the top 24 rows of the screen with blue to create the banner.
	//
	sRect.sXMin = 0;
	sRect.sYMin = 0;
	sRect.sXMax = GrContextDpyWidthGet(&sContext) - 1;
	sRect.sYMax = 23;
	GrContextForegroundSet(&sContext, ClrDarkBlue);
	GrRectFill(&sContext, &sRect);

	//
	// Put a white box around the banner.
	//
	GrContextForegroundSet(&sContext, ClrWhite);
	GrRectDraw(&sContext, &sRect);

	//
	// Put the application name in the middle of the banner.
	//
	GrContextFontSet(&sContext, g_pFontCm20);
	GrStringDrawCentered(&sContext, "17:54 17/2/2014", -1,
			GrContextDpyWidthGet(&sContext) / 2, 8, 0);

	while (true);
}

#include <string.h>

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
#include "grlib/widget.h"
#include "grlib/pushbutton.h"
#include "lcd/grlibDriver.h"
#include "touch/xpt2046.h"
#include "rtc/rtc.h"
#include "bmp/bmpdisplay.h"
#include "bmp/bmpfile.h"
#include "clock/clock.h"
#include "apps/mainMenuApp.h"
#include "main.h"

//*****************************************************************************
//
// GrLib related structures.
//
//*****************************************************************************

const tDisplay* pDisplay = &DisplayStructure;
tContext sContext;

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
volatile int ticks = 0;
void SysTickHandler(void)
{
	//
	// Call the FatFs tick timer.
	//
	disk_timerproc();
	ticks++;
	if (ticks == 100)
	{
		ticks = 0;
		updateClock();
	}
}

int main(void)
{
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

	/// Init RTC
	rtc_init();

	// Init LCD
	ssd1289_init();

	//
	// Initialize the display context
	//
	GrContextInit(&sContext, pDisplay);
	initClock(pDisplay);

	// Init touch
	xpt2046_init();
	xpt2046_setTouchScreenCallback(WidgetPointerMessage);

	xpt2046_enableTouchIRQ();
	SysTickIntEnable();

	startMainMenuApplication();

		while (true)
		{
			//
			// Process any messages in the widget message queue.
			//
			WidgetMessageQueueProcess();
		}
	}

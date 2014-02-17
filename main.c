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
#include "lcd/LCDBP320x240x16_SSD1289.h"

volatile unsigned int status=0, standby_active=0;
volatile long on_time=0;
volatile unsigned long sec_time=0;


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
void
SysTickHandler(void)
{
    //
    // Call the FatFs tick timer.
    //
    disk_timerproc();
}

int main(void)
{
    int nStatus;
    FRESULT fresult;
    DRESULT res;
    tContext sContext;
    tRectangle sRect;

    //
    // Set the system clock to run at 50MHz from the PLL.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
    //
    // Configure SysTick for a 100Hz interrupt.
    //
    SysTickPeriodSet(SysCtlClockGet() / TICKS_PER_SECOND);
    SysTickEnable();
    SysTickIntEnable();
    long systickPeriod = SysTickPeriodGet();
    long sysCtlClk = SysCtlClockGet();

    //
    // Enable Interrupts
    //
    IntMasterEnable();

    // Mount fs

    fresult = f_mount(&fs,"0:",0);
    FIL file1;
    fresult = f_open(&file1,"0:/test.txt",FA_CREATE_ALWAYS);
    printf("Created file with %d",fresult);
    fresult = f_close(&file1);
    fresult = f_open(&file1,"0:/test.txt",FA_OPEN_EXISTING);
    printf("Opened file with %d",fresult);
    fresult = f_open(&file1,"0:/test1.txt",FA_OPEN_EXISTING);

    // Init LCD
    LCDBP320x240x16_SSD1289Init();

    //Draw something
    int i = 0;
    for(i=0; i<200; i++) {
    	LCDBPV2_LineDrawV(i,0,200,ClrCoral);
    }


}

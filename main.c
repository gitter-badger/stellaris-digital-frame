#define TARGET_IS_TEMPEST_RB1
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
static FATFS g_sFatFs;
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


    // Init LCD
    LCDBP320x240x16_SSD1289Init();

    //Draw something

    uINT_16 x0 = 10,x1 = 10,y0 = 10, y1 = 50;
    for (x0 = 10; x0 < 50; x0++)
    {
    	LCDBPV2_LineDraw(x0,y0,x0,y1,ClrGreen);
    }




    return 0;


    //
    // Mount the file system, using logical disk 0.
    //
    res = disk_initialize(0);
	fresult = f_mount(&g_sFatFs,"0:/",1);

    //fresult = f_mkfs("0:",0,0);
    if(fresult != FR_OK)
    {
        return(1);
    }


    FIL file;
    fresult = f_open(&file,"0:/itay.txt", FA_WRITE | FA_CREATE_NEW);
    UINT btw = 1;
    UINT bw = 0;
    BYTE b = 0xAA;

    fresult = f_write(&file,&b,btw,&bw);
    fresult = f_close(&file);
    fresult = f_open(&file,"0:/itay.txt",FA_READ);

    UINT br;
    b=0x00;
    fresult = f_read(&file,&b,1,&br);

    fresult = f_open(&file,"0:/itay1.txt",FA_OPEN_EXISTING);

}

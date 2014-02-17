/////////CONFIG///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//
//
//
//**Define modo de comunicacion: 	SERIAL (#define SERIAL) o PARALELO (defecto)
//#define SERIAL

/////////////////////////////////////////////////////

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "grlib/grlib.h"
#include "driverlib/i2c.h"
#include "LCD_Backlight.h"
#include "LCDBP320x240x16_SSD1289.h"

//*****************************************************************************
//
// This driver operates in four different screen orientations.  They are:
//
// * Portrait - The screen is taller than it is wide, and the flex connector is
//              on the left of the display.  This is selected by defining
//              PORTRAIT.
//
// * Landscape - The screen is wider than it is tall, and the flex connector is
//               on the bottom of the display.  This is selected by defining
//               LANDSCAPE.
//
// * Portrait flip - The screen is taller than it is wide, and the flex
//                   connector is on the right of the display.  This is
//                   selected by defining PORTRAIT_FLIP.
//
// * Landscape flip - The screen is wider than it is tall, and the flex
//                    connector is on the top of the display.  This is
//                    selected by defining LANDSCAPE_FLIP.
//
// These can also be imagined in terms of screen rotation; if portrait mode is
// 0 degrees of screen rotation, landscape is 90 degrees of counter-clockwise
// rotation, portrait flip is 180 degrees of rotation, and landscape flip is
// 270 degress of counter-clockwise rotation.
//
// If no screen orientation is selected, landscape mode will be used.
//
//*****************************************************************************
#if ! defined(PORTRAIT) && ! defined(PORTRAIT_FLIP) && \
    ! defined(LANDSCAPE) && ! defined(LANDSCAPE_FLIP)
#define LANDSCAPE
#endif

//*****************************************************************************
//
// Various definitions controlling coordinate space mapping and drawing
// direction in the four supported orientations.
//
//*****************************************************************************
#ifdef PORTRAIT
#define HORIZ_DIRECTION 0x28        //28  //18  		38
#define VERT_DIRECTION 0x20         //20	10			30
#define MAPPED_X(x, y) (239-y)  //(319 - (y))		y		y
#define MAPPED_Y(x, y) (x)          //(x)	239-x		x
#endif
#ifdef LANDSCAPE
#define HORIZ_DIRECTION 0x28 //00           //30	38
#define VERT_DIRECTION  0x20 //08			//38	30
#define MAPPED_X(x, y) (239-y) //319 - (x)		//x		y
#define MAPPED_Y(x, y) (x) //239 - (y)		//y		x
#endif
#ifdef PORTRAIT_FLIP
#define HORIZ_DIRECTION 0x18        //
#define VERT_DIRECTION 0x10         //
#define MAPPED_X(x, y) (y)          //
#define MAPPED_Y(x, y) (239 - (x)) //
#endif
#ifdef LANDSCAPE_FLIP
#define HORIZ_DIRECTION 0x30 //
#define VERT_DIRECTION  0x38 //
#define MAPPED_X(x, y) (x)   //
#define MAPPED_Y(x, y) (y)   //
#endif

//*****************************************************************************
//
// Various internal SD2119 registers name labels
//
//*****************************************************************************
#define SSD1289_DEVICE_CODE_READ_REG  0x00
#define SSD1289_OSC_START_REG         0x00
#define SSD1289_OUTPUT_CTRL_REG       0x01
#define SSD1289_LCD_DRIVE_AC_CTRL_REG 0x02
#define SSD1289_PWR_CTRL_1_REG        0x03
#define SSD1289_COMPARE_1_REG		  0x05
#define SSD1289_COMPARE_2_REG		  0x06
#define SSD1289_DISPLAY_CTRL_REG      0x07
#define SSD1289_FRAME_CYCLE_CTRL_REG  0x0B
#define SSD1289_PWR_CTRL_2_REG        0x0C
#define SSD1289_PWR_CTRL_3_REG        0x0D
#define SSD1289_PWR_CTRL_4_REG        0x0E
#define SSD1289_GATE_SCAN_START_REG   0x0F
#define SSD1289_SLEEP_MODE_1_REG      0x10
#define SSD1289_ENTRY_MODE_REG        0x11
#define SSD1289_SLEEP_MODE_2_REG      0x12
#define SSD1289_GEN_IF_CTRL_REG       0x15
#define SSD1289_H_PORCH_REG			  0x16
#define SSD1289_V_PORCH_REG			  0x17
#define SSD1289_PWR_CTRL_5_REG        0x1E
#define SSD1289_RAM_DATA_REG          0x22
#define SSD1289_RAM_WRITE_DATA_MASK_1_REG 0x23
#define SSD1289_RAM_WRITE_DATA_MASK_2_REG 0x24
#define SSD1289_FRAME_FREQ_REG        0x25
#define SSD1289_ANALOG_SET_REG        0x26
#define SSD1289_VCOM_OTP_1_REG        0x28
#define SSD1289_OPT_ACCESS_SPD_1_REG  0x28
#define SSD1289_VCOM_OTP_2_REG        0x29
#define SSD1289_OPT_ACCESS_SPD_2_REG  0x2F
#define SSD1289_GAMMA_CTRL_1_REG      0x30
#define SSD1289_GAMMA_CTRL_2_REG      0x31
#define SSD1289_GAMMA_CTRL_3_REG      0x32
#define SSD1289_GAMMA_CTRL_4_REG      0x33
#define SSD1289_GAMMA_CTRL_5_REG      0x34
#define SSD1289_GAMMA_CTRL_6_REG      0x35
#define SSD1289_GAMMA_CTRL_7_REG      0x36
#define SSD1289_GAMMA_CTRL_8_REG      0x37
#define SSD1289_GAMMA_CTRL_9_REG      0x3A
#define SSD1289_GAMMA_CTRL_10_REG     0x3B
#define SSD1289_V_SCROLL_CTRL_1_REG   0x41
#define SSD1289_V_SCROLL_CTRL_2_REG   0x42
#define SSD1289_H_RAM_POS_REG         0x44
#define SSD1289_V_RAM_START_REG       0x45
#define SSD1289_V_RAM_END_REG         0x46
#define SSD1289_FIRST_WINDOW_START    0x48
#define SSD1289_FIRST_WINDOW_END      0x49
#define SSD1289_SECOND_WINDOW_START   0x4A
#define SSD1289_SECOND_WINDOW_END     0x4B
#define SSD1289_X_RAM_ADDR_REG        0x4E
#define SSD1289_Y_RAM_ADDR_REG        0x4F

#define ENTRY_MODE_DEFAULT 0x6030 //0x6830 //6820:10 6800:00
#define MAKE_ENTRY_MODE(x) ((ENTRY_MODE_DEFAULT & 0x7F00) | (x))

//*****************************************************************************
//
// Read Access Timing
// ------------------
//
// Direction OOOIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIOOOOOOOOOOOOO
//
// ~RD        -----                    --------------------------
//                 \                  /                          |
//                  ------------------
//                 <       Trdl       ><        Trdh             >
//                 <                  Tcycle                     >
//                 < Tacc >
//                         /------------------|
// DATA       -------------                    ------------------
//                         \------------------/
//                                     < Tdh  >
//
// Delays          <   Trad  >< Tdhd ><    Trhd   ><  Trcd      >
//
// This design keeps CS tied low so pulse width constraints relating to CS
// have been transfered to ~RD here.
//
// Tcycle Read Cycle Time  1000nS
// Tacc   Data Access Time  100nS
// Trdl   Read Data Low     500nS
// Trdh   Read Data High    500nS
// Tdh    Data Hold Time    100nS
//
// Trad (READ_DATA_ACCESS_DELAY) controls the delay between asserting ~RD and
//       reading the data from the bus.
// Tdhd (READ_DATA_HOLD_DELAY) controls the delay after reading the data and
//       before deasserting ~RD.
// Trhd (READ_HOLD_DELAY) controls the delay between deasserting ~RD and
//       switching the data bus direction back to output.
// Trcd (READ_DATA_CYCLE_DELAY) controls the delay after switching the
//       direction of the data bus and before returning from the ReadData()
//       function.
//
//*****************************************************************************

//*****************************************************************************
//
// The delay to impose after setting the state of the read/write line and
// before reading the data bus.  This is expressed in terms of cycles of a tight
// loop whose body performs a single GPIO register access and needs to comply
// with the 500nS read cycle pulse width constraint.
//
//*****************************************************************************
#define READ_DATA_ACCESS_DELAY  5

//*****************************************************************************
//
// The delay to impose after reading the data and before resetting the state of
// the read/write line during a read operation.  This is expressed in terms of
// cycles of a tight loop whose body performs a single GPIO register access and
// needs to comply with the 500nS read cycle pulse width constraint.
//
//*****************************************************************************
#define READ_DATA_HOLD_DELAY 5

//*****************************************************************************
//
// The delay to impose after deasserting ~RD and before setting the bus back to
// an output.  This is expressed in terms of cycles of a tight loop whose body
// performs a single GPIO register access.
//
//*****************************************************************************
#define READ_HOLD_DELAY 5

//*****************************************************************************
//
// The delay to impose after completing a read cycle and before returning to
// the caller.  This is expressed in terms of cycles of a tight loop whose body
// performs a single GPIO register access and needs to comply with the 1000nS
// read cycle pulse width constraint.
//
//*****************************************************************************
#define READ_DATA_CYCLE_DELAY 5

//*****************************************************************************
//
// The dimensions of the LCD panel.
//
//*****************************************************************************
//#define LCD_VERTICAL_MAX 320   //320
//#define LCD_HORIZONTAL_MAX 240 //240

//*****************************************************************************
//
// Translates a 24-bit RGB color to a display driver-specific color.
//
// \param c is the 24-bit RGB color.  The least-significant byte is the blue
// channel, the next byte is the green channel, and the third byte is the red
// channel.
//
// This macro translates a 24-bit RGB color into a value that can be written
// into the display's frame buffer in order to reproduce that color, or the
// closest possible approximation of that color.
//
// \return Returns the display-driver specific color.
//
//*****************************************************************************
#define DPYCOLORTRANSLATE(c)    ((((c) & 0x00f80000) >> 8) |               \
                                 (((c) & 0x0000fc00) >> 5) |               \
                                 (((c) & 0x000000f8) >> 3))
unsigned long ulClockMS, ulClockMS3;
double midelay, midelayns, midelaynsr;

//void WriteDataSec(unsigned short int usData, unsigned int num);
//void WriteData(unsigned short int usData);
//void WriteCommand(unsigned char usData);
void init0();
void initpower();
void writeLow(uINT_8);
void writeHigh(uINT_8);

//void setxy(unsigned short int x,unsigned short  int y );
void Config();

void LCD_init_periph(void)
{
	//init pwm for backlight
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
	SysCtlPeripheralReset(SYSCTL_PERIPH_PWM);
//	PWMGenConfigure(PWM1_BASE, PWM_GEN_2,
//			PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

	return;
}

void LCD_init_pins(void)
{

	//GPIO D: LCD DATA 0-7
	SysCtlPeripheralEnable(LCD_DATAL_PERIPH);
	GPIOPinTypeGPIOOutput(LCD_DATAL_BASE, 0xFF);
	GPIOPinWrite(LCD_DATAL_BASE, 0xFF, 0x0); // All low.

	//GPIO E: LCD DATA 8-15
	SysCtlPeripheralEnable(LCD_DATAH_PERIPH);
	GPIOPinTypeGPIOOutput(LCD_DATAH_BASE, 0xFF);
	GPIOPinWrite(LCD_DATAH_BASE, 0xFF, 0x0);

	//GPIO F: LCD_RST, LCD_BL

	//SysCtlPeripheralEnable(LCD_BL_PERIPH);
	//GPIOPinTypeGPIOOutput(LCD_BL_BASE, LCD_BL_PIN); //TS_DI = input

	SysCtlPeripheralEnable(LCD_RST_PERIPH);
	GPIOPinTypeGPIOOutput(LCD_RST_BASE, LCD_RST_PIN);
	GPIOPinWrite(LCD_RST_BASE, LCD_RST_PIN, LCD_RST_PIN); // RST = 1

	//Commands
	SysCtlPeripheralEnable(LCD_COMMAND_PERIPH);
	GPIOPinTypeGPIOOutput(LCD_COMMAND_BASE,
			LCD_RD_PIN | LCD_WR_PIN | LCD_CS_PIN | LCD_DC_PIN);
	GPIOPadConfigSet(LCD_COMMAND_BASE, LCD_RD_PIN, GPIO_STRENGTH_4MA,
			GPIO_PIN_TYPE_STD_WPU);
	GPIOPinWrite(LCD_COMMAND_BASE,
			LCD_RD_PIN | LCD_WR_PIN | LCD_CS_PIN | LCD_DC_PIN,
			LCD_CS_PIN | LCD_DC_PIN);

	return;
}

void WriteDataSec(uINT_16 usData, uINT_16 num)
{
	//Select
	GPIOPinWrite(LCD_CS_BASE, LCD_CS_PIN, 0);
	//HWREG(LCD_CS_BASE + GPIO_O_DATA + (LCD_CS_PIN << 2)) = 0;

	writeHigh(usData >> 8);

	SysCtlDelay(1);
	//
	// Assert the write enable signal. This needs to be at least 50nS long so
	// we stick in a dummy write to pad it a bit.
	//
	GPIOPinWrite(LCD_WR_BASE, LCD_WR_PIN, LCD_WR_PIN);
	SysCtlDelay(1);
	// Deassert the write enable signal
	GPIOPinWrite(LCD_WR_BASE, LCD_WR_PIN, LCD_WR_PIN);
	SysCtlDelay(1);
	// Deassert the chip select
	GPIOPinWrite(LCD_CS_BASE, LCD_CS_PIN, LCD_CS_PIN);

}

//*****************************************************************************
//
//! Turns on the backlight.
//!
//! \param ucBrightness is the brightness of the backlight with 0xFF
//! representing "on at full brightness" and 0x00 representing "off".
//!
//! This function sets the brightness of the display backlight.
//!
//! \return None.
//
//*****************************************************************************

/*void LCDBP320x240x16_SSD1289BacklightOn(unsigned char ucBrightness)
 {
 unsigned long ulPeriod;

 //
 // If the backlight is to be set for full brightness, just turn on the
 // control GPIO.
 //
 if(ucBrightness == 255)
 {
 //
 // Assert the signal that turns on the backlight.
 //
 GPIOPinTypeGPIOOutput(LCD_BL_BASE, LCD_BL_PIN);
 GPIOPinWrite(LCD_BL_BASE, LCD_BL_PIN, LCD_BL_PIN);
 }
 else
 {
 //
 // A brightness level of 0 turns off the backlight completely.
 //
 if(ucBrightness == 0)
 {
 GPIOPinTypeGPIOOutput(LCD_BL_BASE, LCD_BL_PIN);
 GPIOPinWrite(LCD_BL_BASE, LCD_BL_PIN, 0);
 }
 else
 {
 //
 // We are being asked for some partial brightness. Modify the
 // duty cycle of the PWM output to effect the desired brightness
 // level.
 //
 ulPeriod = TimerLoadGet(LCD_BL_TIMER_BASE, LCD_BL_TIMER);
 TimerMatchSet(LCD_BL_TIMER_BASE, LCD_BL_TIMER,
 (ulPeriod * (unsigned long)ucBrightness) / 256);

 //
 // Switch the output pin from GPIO to timer.
 //
 GPIOPinTypeTimer(LCD_BL_BASE, LCD_BL_PIN);
 }
 }
 }

 //*****************************************************************************
 //
 //! Turns off the backlight.
 //!
 //! This function turns off the backlight on the display.
 //!
 //! \return None.
 //
 //*****************************************************************************
 void
 LCDBP320x240x16_SSD1289BacklightOff(void)
 {
 //
 // Deassert the signal that turns on the backlight.
 //
 GPIOPinTypeGPIOOutput(LCD_BL_BASE, LCD_BL_PIN);
 GPIOPinWrite(LCD_BL_BASE, LCD_BL_PIN, 0);
 }*/

//*****************************************************************************
//
// Writes a data word to the SSD1289.
//
//*****************************************************************************
void inline writeHigh(uINT_8 wh)
{
	GPIOPinWrite(LCD_DATAH_BASE, 0xFF, wh);
}

void inline writeLow(uINT_8 wl)
{
	GPIOPinWrite(LCD_DATAL_BASE, 0xFF, wl);
}

void WriteData(unsigned short usData)
{
	GPIOPinWrite(LCD_CS_BASE, LCD_CS_PIN, 0);
	//HWREG(LCD_CS_BASE + GPIO_O_DATA + (LCD_CS_PIN << 2)) = 0;

	writeHigh(usData >> 8);

	//SysCtlDelay(1);
	//
	// Assert the write enable signal. This needs to be at least 50nS long so
	// we stick in a dummy write to pad it a bit.
	//
	HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;
	HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;

	//
	// Deassert the write enable signal.
	//
	HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;
	HWREG(LCD_CS_BASE + GPIO_O_DATA + (LCD_CS_PIN << 2)) = LCD_CS_PIN;
	//GPIOPinWrite(LCD_CS_BASE, LCD_CS_PIN, LCD_CS_PIN);
}

//*****************************************************************************
//
// Writes a command to the SSD1289.
//
//*****************************************************************************
void WriteCommand(unsigned char ucData)
{
	GPIOPinWrite(LCD_CS_BASE, LCD_CS_PIN, 0);
	//
	// Write the command to the data bus.
	//
	writeHigh(0);
	writeLow(ucData);
	//SysCtlDelay(1);
	//
	// Set the DC and WR signals low, indicating a command.  This should be
	// OK according to the SSD1289 datasheet. Do this twice to ensure that the
	//
	GPIOPinWrite(LCD_COMMAND_BASE, LCD_DC_PIN | LCD_WR_PIN, 0x0);
	SysCtlDelay(1);
	//
	// Assert the write enable signal.  We need to do this 2 times to ensure
	// that we don't violate the timing requirements for the display.
	//

	GPIOPinWrite(LCD_COMMAND_BASE, LCD_WR_PIN, LCD_WR_PIN);
	SysCtlDelay(1);
	//
	// Deassert the write enable signal.
	//

	GPIOPinWrite(LCD_COMMAND_BASE, LCD_WR_PIN, 0x0);

	//
	// Set the DC signal high, indicating that following writes are data.
	//
	GPIOPinWrite(LCD_COMMAND_BASE, LCD_DC_PIN, LCD_DC_PIN);

	// Deassert chip select
	GPIOPinWrite(LCD_CS_BASE, LCD_CS_PIN, LCD_CS_PIN);
}
void init1()
{
	unsigned long ulClockMS;
	ulClockMS = SysCtlClockGet() / (3 * 1000);

	//
	// Set the LCD control pins to their default values.  This also asserts the
	// LCD reset signal.
	//
	writeHigh(0);
	GPIOPinWrite(LCD_DATAL_BASE, 0xff, 0x00);
	SysCtlDelay(100);

	GPIOPinWrite(LCD_COMMAND_BASE, (LCD_DC_PIN | LCD_WR_PIN), (LCD_WR_PIN)); //GPIOPinWrite(LCD_DC_BASE, (LCD_DC_PIN | LCD_RD_PIN | LCD_WR_PIN),(LCD_RD_PIN | LCD_WR_PIN));
	GPIOPinWrite(LCD_RST_BASE, LCD_RST_PIN, 0x00);

	//
	// Delay for 1ms.
	//
	SysCtlDelay(ulClockMS);

	//
	// Deassert the LCD reset signal.
	//
	GPIOPinWrite(LCD_RST_BASE, LCD_RST_PIN, LCD_RST_PIN);

	//
	// Delay for 1ms while the LCD comes out of reset.
	//
	SysCtlDelay(ulClockMS);
}

//*****************************************************************************
//
//! Initializes the display driver.
//!
//! This function initializes the SSD1289 display controller on the panel,
//! preparing it to display data.
//!
//! \return None.
//
//*****************************************************************************
void LCDBP320x240x16_SSD1289Init(void)
{
	unsigned long ulCount;

	//
	// Get the current processor clock frequency.
	//
	ulClockMS = SysCtlClockGet() / (3 * 1000);
	ulClockMS3 = 1;    //SysCtlClockGet() / (3 * 1000000);

	midelay = SysCtlClockGet() / (3 * 10000000);
	//midelayns=100*ulClockMS;
	midelayns = SysCtlClockGet() / (3 * 10000000); //100nS
	midelaynsr = SysCtlClockGet() / (3 * 100000); //1uS

	LCD_init_periph();

	LCD_init_pins();

	initLCDBacklight();

	init1();

	//
	// Start the oscillator.
	//
	WriteCommand(SSD1289_OSC_START_REG);
	WriteData(0x0001);
	//
	// Set pixel format and basic display orientation (scanning direction).
	//
	WriteCommand(SSD1289_OUTPUT_CTRL_REG);
	WriteData(0x3B3F); //no es valor default en el datasheet //REVISAR //0x30EF 0x313F  0x2B3F

	//SysCtlDelay(1000000);

	WriteCommand(SSD1289_LCD_DRIVE_AC_CTRL_REG);
	WriteData(0x600); //0x0600 no es valor default en el datasheet 400 o 600

	//SysCtlDelay(1000000);

	//
	// Exit sleep mode.
	//
	WriteCommand(SSD1289_SLEEP_MODE_1_REG);
	WriteData(0x0000);

	// SysCtlDelay(1000000);

	//
	// Delay 30mS
	//
	SysCtlDelay(30 * ulClockMS);

	//
	// Configure pixel color format and MCU interface parameters.
	//
	WriteCommand(SSD1289_ENTRY_MODE_REG);
	WriteData(ENTRY_MODE_DEFAULT); //0x6830 0x6070 ENTRY_MODE_DEFAULT **************************

	//
	// Enable the display.
	//
	WriteCommand(SSD1289_DISPLAY_CTRL_REG);
	WriteData(0x233); //no es valor default en el datasheet 0x0033 233 233

	initpower();

	//
	// Configure gamma correction.
	//
	WriteCommand(SSD1289_GAMMA_CTRL_1_REG);
	WriteData(0x0707); //0x0000
	WriteCommand(SSD1289_GAMMA_CTRL_2_REG);
	WriteData(0x0204); //0x0303
	WriteCommand(SSD1289_GAMMA_CTRL_3_REG);
	WriteData(0x0204); //0x0407
	WriteCommand(SSD1289_GAMMA_CTRL_4_REG);
	WriteData(0x0502); // 0x0301
	WriteCommand(SSD1289_GAMMA_CTRL_5_REG);
	WriteData(0x0507); //0x0301
	WriteCommand(SSD1289_GAMMA_CTRL_6_REG);
	WriteData(0x0204); //0x0403
	WriteCommand(SSD1289_GAMMA_CTRL_7_REG);
	WriteData(0x0204); //0x0707
	WriteCommand(SSD1289_GAMMA_CTRL_8_REG);
	WriteData(0x0502); //0x0400
	WriteCommand(SSD1289_GAMMA_CTRL_9_REG);
	WriteData(0x0302); //0x0a00
	WriteCommand(SSD1289_GAMMA_CTRL_10_REG);
	WriteData(0x00302); //0x1000

	init0();
	initpower();

	WriteCommand(SSD1289_V_RAM_START_REG);
	WriteData(0x0000);

	WriteCommand(SSD1289_X_RAM_ADDR_REG);
	WriteData(0x00);

	WriteCommand(SSD1289_Y_RAM_ADDR_REG);
	WriteData(0x00);

	WriteCommand(SSD1289_H_RAM_POS_REG);
	WriteData(0xEF00); //((LCD_VERTICAL_MAX-1) << 8); 0xEF00

	WriteCommand(SSD1289_V_RAM_END_REG);
	WriteData(0x13F); //(LCD_HORIZONTAL_MAX-1); 0x013F 0x23F

	WriteCommand(SSD1289_GATE_SCAN_START_REG);
	WriteData(0x0);

	WriteCommand(SSD1289_RAM_DATA_REG);
	WriteDataSec(0x00, 76800);

    for(ulCount = 0; ulCount < (76800); ulCount++) //320*240=76800
    {
        WriteData(ulCount);

    }
  SysCtlDelay(1000000);

	WriteCommand(SSD1289_RAM_DATA_REG);
	WriteData(0x0);
	WriteDataSec(0x777, 36800);

	SysCtlDelay(10000000);
	 WriteCommand(SSD1289_RAM_DATA_REG);
	 WriteDataSec(0x700, 76800);
	 SysCtlDelay(10000000);
	 WriteCommand(SSD1289_RAM_DATA_REG);
	 WriteDataSec(0x500, 76800);
	 SysCtlDelay(10000000);

}

void init0()
{
	WriteCommand(SSD1289_COMPARE_1_REG);
	WriteData(0x00);
	WriteCommand(SSD1289_COMPARE_2_REG);
	WriteData(0x00);
	WriteCommand(SSD1289_FRAME_CYCLE_CTRL_REG);
	WriteData(0x00);
	WriteCommand(SSD1289_H_PORCH_REG);
	WriteData(0xEF1C);
	WriteCommand(SSD1289_V_PORCH_REG);
	WriteData(0x03);
	WriteCommand(SSD1289_RAM_WRITE_DATA_MASK_1_REG);
	WriteData(0x00);
	WriteCommand(SSD1289_RAM_WRITE_DATA_MASK_2_REG);
	WriteData(0x00);
	WriteCommand(SSD1289_FRAME_FREQ_REG);
	WriteData(0x8000);
	WriteCommand(SSD1289_V_SCROLL_CTRL_1_REG);
	WriteData(0x00);
	WriteCommand(SSD1289_V_SCROLL_CTRL_2_REG);
	WriteData(0x00);
	WriteCommand(SSD1289_FIRST_WINDOW_START);
	WriteData(0x00);
	WriteCommand(SSD1289_FIRST_WINDOW_END);
	WriteData(0x013F);
	WriteCommand(SSD1289_SECOND_WINDOW_START);
	WriteData(0x00);
	WriteCommand(SSD1289_SECOND_WINDOW_END);
	WriteData(0x00);
	//
	// Set analog parameters.
	//
	WriteCommand(SSD1289_SLEEP_MODE_2_REG);
	WriteData(0x0999);
	WriteCommand(SSD1289_ANALOG_SET_REG);
	WriteData(0x3800);

	WriteCommand(SSD1289_PWR_CTRL_1_REG);
	WriteData(0xA8A4);
	WriteCommand(SSD1289_COMPARE_1_REG);
	WriteData(0x00);
	WriteCommand(SSD1289_COMPARE_2_REG);
	WriteData(0x00);
	WriteCommand(SSD1289_H_PORCH_REG);
	WriteData(0xEF1C);
	WriteCommand(SSD1289_V_PORCH_REG);
	WriteData(0x003);
	WriteCommand(SSD1289_FRAME_CYCLE_CTRL_REG);
	WriteData(0x00);
	WriteCommand(SSD1289_V_SCROLL_CTRL_1_REG);
	WriteData(0x000);
	WriteCommand(SSD1289_V_SCROLL_CTRL_2_REG);
	WriteData(0x0);
	WriteCommand(SSD1289_FIRST_WINDOW_START);
	WriteData(0x00);
	WriteCommand(SSD1289_FIRST_WINDOW_END);
	WriteData(0x013F);
	WriteCommand(SSD1289_SECOND_WINDOW_START);
	WriteData(0x00);
	WriteCommand(SSD1289_SECOND_WINDOW_END);
	WriteData(0x00);
	WriteCommand(SSD1289_RAM_WRITE_DATA_MASK_1_REG);
	WriteData(0x00);
	WriteCommand(SSD1289_RAM_WRITE_DATA_MASK_2_REG);
	WriteData(0x00);
	WriteCommand(SSD1289_FRAME_FREQ_REG);
	WriteData(0x8000);
	//
	// set pixel per line
	WriteCommand(SSD1289_H_PORCH_REG);
	WriteData(0xEF1C);
	WriteCommand(SSD1289_V_PORCH_REG);
	WriteData(0x103);
	//
	// Enter sleep mode (if we are not already there).
	//
	//WriteCommand(SSD1289_SLEEP_MODE_1_REG);
	//WriteData(0x0001);

	//
	// Set initial power parameters.
	//
	WriteCommand(SSD1289_VCOM_OTP_1_REG);
	WriteData(0x000A); //no es valor default en el datasheet 0x0006

}
void initpower()
{
	int mode = 4;
	switch (mode)
	{
	case 1: //kentec ssd2119
		//WriteCommand(SSD1289_PWR_CTRL_1_REG);
		//WriteData(0xA8A4);
		WriteCommand(SSD1289_PWR_CTRL_2_REG);
		WriteData(0x0005);
		WriteCommand(SSD1289_PWR_CTRL_3_REG);
		WriteData(0x0007); // 0x000A 80c
		WriteCommand(SSD1289_PWR_CTRL_4_REG);
		WriteData(0x3100); //0x2E00 2B00
		WriteCommand(SSD1289_PWR_CTRL_5_REG);
		WriteData(0x00BA);
		break;
	case 2: //datasheet
			//WriteCommand(SSD1289_PWR_CTRL_1_REG);
			//WriteData(0xA8A4);
		WriteCommand(SSD1289_PWR_CTRL_2_REG);
		WriteData(0x0004);
		WriteCommand(SSD1289_PWR_CTRL_3_REG);
		WriteData(0x0009);
		WriteCommand(SSD1289_PWR_CTRL_4_REG);
		WriteData(0x3200);
		WriteCommand(SSD1289_PWR_CTRL_5_REG);
		WriteData(0x29); //0x00B2
		break;
	case 3: //utft
			//WriteCommand(SSD1289_PWR_CTRL_1_REG);
			//WriteData(0xA8A4);
		WriteCommand(SSD1289_PWR_CTRL_2_REG);
		WriteData(0x0000);
		WriteCommand(SSD1289_PWR_CTRL_3_REG);
		WriteData(0x080C);
		WriteCommand(SSD1289_PWR_CTRL_4_REG);
		WriteData(0x2b00);
		WriteCommand(SSD1289_PWR_CTRL_5_REG);
		WriteData(0x00B7); // 0x00B2
		break;
	case 4: //lcdbpv2
		WriteCommand(SSD1289_PWR_CTRL_1_REG);
		WriteData(0xA8A4); //0x6664
		WriteCommand(SSD1289_PWR_CTRL_2_REG);
		WriteData(0x0007);
		WriteCommand(SSD1289_PWR_CTRL_3_REG);
		WriteData(0x080C);
		WriteCommand(SSD1289_PWR_CTRL_4_REG);
		WriteData(0x2b00);
		WriteCommand(SSD1289_PWR_CTRL_5_REG);
		WriteData(0x00B7); // 0x00B2
		break;
	}

}

//*****************************************************************************
//
//! Draws a pixel on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//! \param ulValue is the color of the pixel.
//!
//! This function sets the given pixel to a particular color.  The coordinates
//! of the pixel are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void LCDBP320x240x16_SSD1289PixelDraw(void *pvDisplayData, long lX,
		long lY, unsigned long ulValue)
{
	//
	// Set the X address of the display cursor.
	//
	WriteCommand(SSD1289_X_RAM_ADDR_REG);
	WriteData(MAPPED_X(lX, lY));

	//
	// Set the Y address of the display cursor.
	//
	WriteCommand(SSD1289_Y_RAM_ADDR_REG);
	WriteData(MAPPED_Y(lX, lY));

	//
	// Write the pixel value.
	//
	WriteCommand(SSD1289_RAM_DATA_REG);
	WriteData(ulValue);
}

//*****************************************************************************
//
//! Draws a horizontal sequence of pixels on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the first pixel.
//! \param lY is the Y coordinate of the first pixel.
//! \param lX0 is sub-pixel offset within the pixel data, which is valid for 1
//! or 4 bit per pixel formats.
//! \param lCount is the number of pixels to draw.
//! \param lBPP is the number of bits per pixel; must be 1, 4, or 8.
//! \param pucData is a pointer to the pixel data.  For 1 and 4 bit per pixel
//! formats, the most significant bit(s) represent the left-most pixel.
//! \param pucPalette is a pointer to the palette used to draw the pixels.
//!
//! This function draws a horizontal sequence of pixels on the screen, using
//! the supplied palette.  For 1 bit per pixel format, the palette contains
//! pre-translated colors; for 4 and 8 bit per pixel formats, the palette
//! contains 24-bit RGB values that must be translated before being written to
//! the display.
//!
//! \return None.
//
//*****************************************************************************
static void LCDBP320x240x16_SSD1289PixelDrawMultiple(void *pvDisplayData,
		long lX, long lY, long lX0, long lCount, long lBPP,
		const unsigned char *pucData, const unsigned char *pucPalette)
{
	unsigned long ulByte;

	//
	// Set the cursor increment to left to right, followed by top to bottom.
	//
	WriteCommand(SSD1289_ENTRY_MODE_REG);
	WriteData(MAKE_ENTRY_MODE(HORIZ_DIRECTION));

	//
	// Set the starting X address of the display cursor.
	//
	WriteCommand(SSD1289_X_RAM_ADDR_REG);
	WriteData(MAPPED_X(lX, lY));

	//
	// Set the Y address of the display cursor.
	//
	WriteCommand(SSD1289_Y_RAM_ADDR_REG);
	WriteData(MAPPED_Y(lX, lY));

	//
	// Write the data RAM write command.
	//
	WriteCommand(SSD1289_RAM_DATA_REG);

	//
	// Determine how to interpret the pixel data based on the number of bits
	// per pixel.
	//
	switch (lBPP)
	{
	//
	// The pixel data is in 1 bit per pixel format.
	//
	case 1:
	{
		//
		// Loop while there are more pixels to draw.
		//
		while (lCount)
		{
			//
			// Get the next byte of image data.
			//
			ulByte = *pucData++;

			//
			// Loop through the pixels in this byte of image data.
			//
			for (; (lX0 < 8) && lCount; lX0++, lCount--)
			{
				//
				// Draw this pixel in the appropriate color.
				//
				WriteData(
						((unsigned long *) pucPalette)[(ulByte >> (7 - lX0)) & 1]);
			}

			//
			// Start at the beginning of the next byte of image data.
			//
			lX0 = 0;
		}

		//
		// The image data has been drawn.
		//
		break;
	}

		//
		// The pixel data is in 4 bit per pixel format.
		//
	case 4:
	{
		//
		// Loop while there are more pixels to draw.  "Duff's device" is
		// used to jump into the middle of the loop if the first nibble of
		// the pixel data should not be used.  Duff's device makes use of
		// the fact that a case statement is legal anywhere within a
		// sub-block of a switch statement.  See
		// http://en.wikipedia.org/wiki/Duff's_device for detailed
		// information about Duff's device.
		//
		switch (lX0 & 1)
		{
		case 0:
			while (lCount)
			{
				//
				// Get the upper nibble of the next byte of pixel data
				// and extract the corresponding entry from the
				// palette.
				//
				ulByte = (*pucData >> 4) * 3;
				ulByte =
						(*(unsigned long *) (pucPalette + ulByte) & 0x00ffffff);

				//
				// Translate this palette entry and write it to the
				// screen.
				//
				WriteData(DPYCOLORTRANSLATE(ulByte));

				//
				// Decrement the count of pixels to draw.
				//
				lCount--;

				//
				// See if there is another pixel to draw.
				//
				if (lCount)
				{
					case 1:
					//
					// Get the lower nibble of the next byte of pixel
					// data and extract the corresponding entry from
					// the palette.
					//
					ulByte = (*pucData++ & 15) * 3;
					ulByte = (*(unsigned long *) (pucPalette + ulByte)
							& 0x00ffffff);

					//
					// Translate this palette entry and write it to the
					// screen.
					//
					WriteData(DPYCOLORTRANSLATE(ulByte));

					//
					// Decrement the count of pixels to draw.
					//
					lCount--;
				}
			}
		}

		//
		// The image data has been drawn.
		//
		break;
	}

		//
		// The pixel data is in 8 bit per pixel format.
		//
	case 8:
	{
		//
		// Loop while there are more pixels to draw.
		//
		while (lCount--)
		{
			//
			// Get the next byte of pixel data and extract the
			// corresponding entry from the palette.
			//
			ulByte = *pucData++ * 3;
			ulByte = *(unsigned long *) (pucPalette + ulByte) & 0x00ffffff;

			//
			// Translate this palette entry and write it to the screen.
			//
			WriteData(DPYCOLORTRANSLATE(ulByte));
		}

		//
		// The image data has been drawn.
		//
		break;
	}
	}
}

//*****************************************************************************
//
//! Draws a horizontal line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX1 is the X coordinate of the start of the line.
//! \param lX2 is the X coordinate of the end of the line.
//! \param lY is the Y coordinate of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a horizontal line on the display.  The coordinates of
//! the line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void LCDBP320x240x16_SSD1289LineDrawH(void *pvDisplayData, long lX1,
		long lX2, long lY, unsigned long ulValue)
{
	//
	// Set the cursor increment to left to right, followed by top to bottom.
	//
	WriteCommand(SSD1289_ENTRY_MODE_REG);
	WriteData(MAKE_ENTRY_MODE(HORIZ_DIRECTION));

	//
	// Set the starting X address of the display cursor.
	//
	WriteCommand(SSD1289_X_RAM_ADDR_REG);
	WriteData(MAPPED_X(lX1, lY));

	//
	// Set the Y address of the display cursor.
	//
	WriteCommand(SSD1289_Y_RAM_ADDR_REG);
	WriteData(MAPPED_Y(lX1, lY));

	//
	// Write the data RAM write command.
	//
	WriteCommand(SSD1289_RAM_DATA_REG);

	//
	// Loop through the pixels of this horizontal line.
	//
	long lX0;
	//while(lX1++ <= lX2)

	WriteDataSec(ulValue, 1 + lX2 - lX1);

	/*
	 for(lX0=lX1; lX0<=lX2;lX0++)
	 {
	 //
	 // Write the pixel value.
	 //
	 WriteData(ulValue);
	 //SysCtlDelay(20000);
	 }
	 */

	/*
	 unsigned long ulCount;
	 WriteCommand(SSD1289_RAM_DATA_REG);
	 for(ulCount = 0; ulCount < (76800); ulCount++) //320*240=76800
	 {
	 WriteData(0x555);
	 SysCtlDelay(100);
	 }
	 */
}

//*****************************************************************************
//
//! Draws a vertical line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the line.
//! \param lY1 is the Y coordinate of the start of the line.
//! \param lY2 is the Y coordinate of the end of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a vertical line on the display.  The coordinates of the
//! line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void LCDBP320x240x16_SSD1289LineDrawV(void *pvDisplayData, long lX,
		long lY1, long lY2, unsigned long ulValue)
{

	//lX=100;
	//lY1=1;
	//lY2=lY2-1;//319;

	//
	// Set the cursor increment to top to bottom, followed by left to right.
	//

	WriteCommand(SSD1289_ENTRY_MODE_REG);
	WriteData(MAKE_ENTRY_MODE(VERT_DIRECTION));

	//
	// Set the X address of the display cursor.
	//
	WriteCommand(SSD1289_X_RAM_ADDR_REG);
	WriteData(MAPPED_X(lX, lY1));

	//
	// Set the starting Y address of the display cursor.
	//
	WriteCommand(SSD1289_Y_RAM_ADDR_REG);
	WriteData(MAPPED_Y(lX, lY1));

	//
	// Write the data RAM write command.
	//
	WriteCommand(SSD1289_RAM_DATA_REG);

	//
	// Loop through the pixels of this vertical line.
	//
	long lY0;

	WriteDataSec(ulValue, 1 + lY2 - lY1);
	/*    for(lY0=lY1; lY0<=lY2;lY0++)//while(lY1++ <= lY2)
	 {
	 //
	 // Write the pixel value.
	 //
	 WriteData(ulValue);
	 //SysCtlDelay(20000);
	 }*/

	//SysCtlDelay(200000);
	/*
	 unsigned long ulCount;
	 WriteCommand(SSD1289_RAM_DATA_REG);
	 for(ulCount = 0; ulCount < (76800); ulCount++) //320*240=76800
	 {
	 WriteData(0x250);
	 SysCtlDelay(2000);
	 }
	 */
}

void LCDBP320x240x16_SSD1289LineDrawVM(long lX, long lY1, long lY2,
		unsigned long ulValue)
{

	//lX=100;
	//lY1=1;
	//lY2=lY2-1;//319;

	//
	// Set the cursor increment to top to bottom, followed by left to right.
	//

	WriteCommand(SSD1289_ENTRY_MODE_REG);
	WriteData(0x6830); //MAKE_ENTRY_MODE(VERT_DIRECTION)

	//
	// Set the X address of the display cursor.
	//
	WriteCommand(SSD1289_X_RAM_ADDR_REG);
	WriteData(MAPPED_X(lX, lY1));

	//
	// Set the starting Y address of the display cursor.
	//
	WriteCommand(SSD1289_Y_RAM_ADDR_REG);
	WriteData(MAPPED_Y(lX, lY1));

	//
	// Write the data RAM write command.
	//
	WriteCommand(SSD1289_RAM_DATA_REG);

	//
	// Loop through the pixels of this vertical line.
	//
	long lY0;

	for (lY0 = lY1; lY0 <= lY2; lY0++)    //while(lY1++ <= lY2)
	{
		//
		// Write the pixel value.
		//
		WriteData(ulValue);

	}

	//SysCtlDelay(200000);

	/*
	 unsigned long ulCount;
	 WriteCommand(SSD1289_RAM_DATA_REG);
	 for(ulCount = 0; ulCount < (76800); ulCount++) //320*240=76800
	 {
	 WriteData(0x250);
	 SysCtlDelay(2000);
	 }
	 */
}
//*****************************************************************************
//
//! Fills a rectangle.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param pRect is a pointer to the structure describing the rectangle.
//! \param ulValue is the color of the rectangle.
//!
//! This function fills a rectangle on the display.  The coordinates of the
//! rectangle are assumed to be within the extents of the display, and the
//! rectangle specification is fully inclusive (in other words, both sXMin and
//! sXMax are drawn, along with sYMin and sYMax).
//!
//! \return None.
//
//*****************************************************************************

static void LCDBP320x240x16_SSD1289RectFill(void *pvDisplayData,
		const tRectangle *pRect, unsigned long ulValue)
{
	long lCount;

	//
	// Write the Y extents of the rectangle.
	//
	WriteCommand(SSD1289_ENTRY_MODE_REG);
	WriteData(MAKE_ENTRY_MODE(VERT_DIRECTION));

	//
	// Write the X extents of the rectangle.
	//
	WriteCommand(SSD1289_V_RAM_START_REG);
#if (defined PORTRAIT) || (defined LANDSCAPE)
	WriteData(MAPPED_Y(pRect->sXMin, pRect->sYMin));
#else
	WriteData(MAPPED_Y(pRect->sXMax, pRect->sYMax));
#endif

	WriteCommand(SSD1289_V_RAM_END_REG);
#if (defined PORTRAIT) || (defined LANDSCAPE)
	WriteData(MAPPED_Y(pRect->sXMax, pRect->sYMax));
#else
	WriteData(MAPPED_Y(pRect->sXMin, pRect->sYMin));
#endif

	//
	// Write the Y extents of the rectangle
	//
	WriteCommand(SSD1289_H_RAM_POS_REG);
#if (defined LANDSCAPE_FLIP) || (defined PORTRAIT)
	WriteData(MAPPED_X(pRect->sXMin, pRect->sYMin) |(MAPPED_X(pRect->sXMax, pRect->sYMax) << 8));
#else
	WriteData(
			MAPPED_X(pRect->sXMax, pRect->sYMax)
					| (MAPPED_X(pRect->sXMin, pRect->sYMin) << 8));
#endif

	//
	// Set the display cursor to the upper left of the rectangle (in application
	// coordinate space).
	//
	WriteCommand(SSD1289_X_RAM_ADDR_REG);
	WriteData(MAPPED_X(pRect->sXMin, pRect->sYMin));

	WriteCommand(SSD1289_Y_RAM_ADDR_REG);
	WriteData(MAPPED_Y(pRect->sXMin, pRect->sYMin));

	//
	// Tell the controller we are about to write data into its RAM.
	//
	WriteCommand(SSD1289_RAM_DATA_REG);

	//
	// Loop through the pixels of this filled rectangle.
	//
	WriteDataSec(ulValue,
			1
					+ ((pRect->sXMax - pRect->sXMin + 1)
							* (pRect->sYMax - pRect->sYMin + 1)));

	/*    for(lCount = ((pRect->sXMax - pRect->sXMin + 1) * (pRect->sYMax - pRect->sYMin + 1)); lCount >= 0; lCount--)
	 {
	 //
	 // Write the pixel value.
	 //
	 WriteData(ulValue);
	 }*/

	//
	// Reset the X extents to the entire screen.
	//
	WriteCommand(SSD1289_V_RAM_START_REG);
	WriteData(0x0000);
	WriteCommand(SSD1289_V_RAM_END_REG);
	WriteData(0x013F);

	//
	// Reset the Y extent to the full screen
	//
	WriteCommand(SSD1289_H_RAM_POS_REG);
	WriteData(0xEF00);

}

//*****************************************************************************
//
//! Translates a 24-bit RGB color to a display driver-specific color.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param ulValue is the 24-bit RGB color.  The least-significant byte is the
//! blue channel, the next byte is the green channel, and the third byte is the
//! red channel.
//!
//! This function translates a 24-bit RGB color into a value that can be
//! written into the display's frame buffer in order to reproduce that color,
//! or the closest possible approximation of that color.
//!
//! \return Returns the display-driver specific color.
//
//*****************************************************************************
uINT_16 LCDBPV2_ColorTranslate(uINT_16 ulValue)
{
	//
	// Translate from a 24-bit RGB color to a 5-6-5 RGB color.
	//
	return (DPYCOLORTRANSLATE(ulValue));
}
LCDBP320x240x16_SSD1289ColorTranslate(void *pvDisplayData,
		unsigned long ulValue)
{
	return (DPYCOLORTRANSLATE(ulValue));
}

//*****************************************************************************
//
//! Flushes any cached drawing operations.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//!
//! This functions flushes any cached drawing operations to the display.  This
//! is useful when a local frame buffer is used for drawing operations, and the
//! flush would copy the local frame buffer to the display.  For the SSD1289
//! driver, the flush is a no operation.
//!
//! \return None.
//
//*****************************************************************************
static void LCDBP320x240x16_SSD1289Flush(void *pvDisplayData)
{
	//
	// There is nothing to be done.
	//
}

void setxy(uINT_16 x, uINT_16 y)
{
	WriteCommand(SSD1289_X_RAM_ADDR_REG);
	WriteData(x);

	WriteCommand(SSD1289_Y_RAM_ADDR_REG);
	WriteData(y);
}
//*****************************************************************************
//
//! The display structure that describes the driver for the LCDBP
//! K350QVG-V1-F TFT panel with an SSD1289 controller.
//
//*****************************************************************************
const tDisplay g_sLCDBP320x240x16_SSD1289 =
{
		sizeof(tDisplay),
		0,

#if defined(PORTRAIT_FLIP) || defined(PORTRAIT)
		240, //240
		320,//320
#else
		320, //320
		240, //240 lado corto
#endif
		LCDBP320x240x16_SSD1289PixelDraw,
		LCDBP320x240x16_SSD1289PixelDrawMultiple,
		LCDBP320x240x16_SSD1289LineDrawH,
		LCDBP320x240x16_SSD1289LineDrawV,
		LCDBP320x240x16_SSD1289RectFill,
		LCDBP320x240x16_SSD1289ColorTranslate,
		LCDBP320x240x16_SSD1289Flush };

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//stop

//LCDBPV2library
////////////////
void LCDBPV2_LineDrawV(uINT_16 lX, uINT_16 lY1, uINT_16 lY2, uINT_16 ulValue)
{

	WriteCommand(SSD1289_ENTRY_MODE_REG);
	WriteData(MAKE_ENTRY_MODE(VERT_DIRECTION));

	//
	// Set the X address of the display cursor.
	//
	WriteCommand(SSD1289_X_RAM_ADDR_REG);
	WriteData(MAPPED_X(lX, lY1));

	//
	// Set the starting Y address of the display cursor.
	//
	WriteCommand(SSD1289_Y_RAM_ADDR_REG);
	WriteData(MAPPED_Y(lX, lY1));

	//
	// Write the data RAM write command.
	//
	WriteCommand(SSD1289_RAM_DATA_REG);

	//
	// Loop through the pixels of this vertical line.
	//
	long lY0;

	WriteDataSec(ulValue, 1 + lY2 - lY1);

}

void LCDBPV2_LineDrawH(uINT_16 lX1, uINT_16 lX2, uINT_16 lY, uINT_16 ulValue)
{
	//
	// Set the cursor increment to left to right, followed by top to bottom.
	//
	WriteCommand(SSD1289_ENTRY_MODE_REG);
	WriteData(MAKE_ENTRY_MODE(HORIZ_DIRECTION));

	//
	// Set the starting X address of the display cursor.
	//
	WriteCommand(SSD1289_X_RAM_ADDR_REG);
	WriteData(MAPPED_X(lX1, lY));

	//
	// Set the Y address of the display cursor.
	//
	WriteCommand(SSD1289_Y_RAM_ADDR_REG);
	WriteData(MAPPED_Y(lX1, lY));

	//
	// Write the data RAM write command.
	//
	WriteCommand(SSD1289_RAM_DATA_REG);

	//
	// Loop through the pixels of this horizontal line.
	//
	long lX0;

	WriteDataSec(ulValue, 1 + lX2 - lX1);

}

void LCDBPV2_PixelDraw(uINT_16 lX, uINT_16 lY, uINT_16 ulValue)
{
	//
	// Set the X address of the display cursor.
	//
	WriteCommand(SSD1289_X_RAM_ADDR_REG);
	WriteData(MAPPED_X(lX, lY));

	//
	// Set the Y address of the display cursor.
	//
	WriteCommand(SSD1289_Y_RAM_ADDR_REG);
	WriteData(MAPPED_Y(lX, lY));

	//
	// Write the pixel value.
	//
	WriteCommand(SSD1289_RAM_DATA_REG);
	WriteData(ulValue);
}
void LCDBPV2_LineDraw(uINT_16 lX1, uINT_16 lY1, uINT_16 lX2, uINT_16 lY2,
		uINT_16 ulValue)
{
	long lError, lDeltaX, lDeltaY, lYStep, bSteep;

	//
	// See if this is a vertical line.
	//
	if (lX1 == lX2)
	{
		//
		// It is more efficient to avoid Bresenham's algorithm when drawing a
		// vertical line, so use the vertical line routine to draw this line.
		//
		LCDBPV2_LineDrawV(lX1, lY1, lY2, ulValue);

		//
		// The line has ben drawn, so return.
		//
		return;
	}

	//
	// See if this is a horizontal line.
	//
	if (lY1 == lY2)
	{
		//
		// It is more efficient to avoid Bresenham's algorithm when drawing a
		// horizontal line, so use the horizontal line routien to draw this
		// line.
		//
		LCDBPV2_LineDrawH(lX1, lX2, lY1, ulValue);

		//
		// The line has ben drawn, so return.
		//
		return;
	}

	//
	// Clip this line if necessary, and return without drawing anything if the
	// line does not cross the clipping region.
	//
	/*	    if(GrLineClip(pContext, &lX1, &lY1, &lX2, &lY2) == 0)
	 {
	 return;
	 }*/

	//
	// Determine if the line is steep.  A steep line has more motion in the Y
	// direction than the X direction.
	//
	if (((lY2 > lY1) ? (lY2 - lY1) : (lY1 - lY2))
			> ((lX2 > lX1) ? (lX2 - lX1) : (lX1 - lX2)))
	{
		bSteep = 1;
	}
	else
	{
		bSteep = 0;
	}

	//
	// If the line is steep, then swap the X and Y coordinates.
	//
	if (bSteep)
	{
		lError = lX1;
		lX1 = lY1;
		lY1 = lError;
		lError = lX2;
		lX2 = lY2;
		lY2 = lError;
	}

	//
	// If the starting X coordinate is larger than the ending X coordinate,
	// then swap the start and end coordinates.
	//
	if (lX1 > lX2)
	{
		lError = lX1;
		lX1 = lX2;
		lX2 = lError;
		lError = lY1;
		lY1 = lY2;
		lY2 = lError;
	}

	//
	// Compute the difference between the start and end coordinates in each
	// axis.
	//
	lDeltaX = lX2 - lX1;
	lDeltaY = (lY2 > lY1) ? (lY2 - lY1) : (lY1 - lY2);

	//
	// Initialize the error term to negative half the X delta.
	//
	lError = -lDeltaX / 2;

	//
	// Determine the direction to step in the Y axis when required.
	//
	if (lY1 < lY2)
	{
		lYStep = 1;
	}
	else
	{
		lYStep = -1;
	}

	//
	// Loop through all the points along the X axis of the line.
	//
	for (; lX1 <= lX2; lX1++)
	{
		//
		// See if this is a steep line.
		//
		if (bSteep)
		{
			//
			// Plot this point of the line, swapping the X and Y coordinates.
			//
			LCDBPV2_PixelDraw(lY1, lX1, ulValue);
		}
		else
		{
			//
			// Plot this point of the line, using the coordinates as is.
			//
			LCDBPV2_PixelDraw(lX1, lY1, ulValue);
		}

		//
		// Increment the error term by the Y delta.
		//
		lError += lDeltaY;

		//
		// See if the error term is now greater than zero.
		//
		if (lError > 0)
		{
			//
			// Take a step in the Y axis.
			//
			lY1 += lYStep;

			//
			// Decrement the error term by the X delta.
			//
			lError -= lDeltaX;
		}
	}
}

void LCDBPV2_RectFill(const gRectangle *pRect, uINT_16 ulValue)
{
	long lCount;

	//
	// Write the Y extents of the rectangle.
	//
	WriteCommand(SSD1289_ENTRY_MODE_REG);
	WriteData(MAKE_ENTRY_MODE(VERT_DIRECTION));

	//
	// Write the X extents of the rectangle.
	//
	WriteCommand(SSD1289_V_RAM_START_REG);
#if (defined PORTRAIT) || (defined LANDSCAPE)
	WriteData(MAPPED_Y(pRect->sXMin, pRect->sYMin));
#else
	WriteData(MAPPED_Y(pRect->sXMax, pRect->sYMax));
#endif

	WriteCommand(SSD1289_V_RAM_END_REG);
#if (defined PORTRAIT) || (defined LANDSCAPE)
	WriteData(MAPPED_Y(pRect->sXMax, pRect->sYMax));
#else
	WriteData(MAPPED_Y(pRect->sXMin, pRect->sYMin));
#endif

	//
	// Write the Y extents of the rectangle
	//
	WriteCommand(SSD1289_H_RAM_POS_REG);
#if (defined LANDSCAPE_FLIP) || (defined PORTRAIT)
	WriteData(MAPPED_X(pRect->sXMin, pRect->sYMin) |(MAPPED_X(pRect->sXMax, pRect->sYMax) << 8));
#else
	WriteData(
			MAPPED_X(pRect->sXMax, pRect->sYMax)
					| (MAPPED_X(pRect->sXMin, pRect->sYMin) << 8));
#endif

	//
	// Set the display cursor to the upper left of the rectangle (in application
	// coordinate space).
	//
	WriteCommand(SSD1289_X_RAM_ADDR_REG);
	WriteData(MAPPED_X(pRect->sXMin, pRect->sYMin));

	WriteCommand(SSD1289_Y_RAM_ADDR_REG);
	WriteData(MAPPED_Y(pRect->sXMin, pRect->sYMin));

	//
	// Tell the controller we are about to write data into its RAM.
	//
	WriteCommand(SSD1289_RAM_DATA_REG);

	//
	// Loop through the pixels of this filled rectangle.
	//
	WriteDataSec(ulValue,
			1
					+ ((pRect->sXMax - pRect->sXMin + 1)
							* (pRect->sYMax - pRect->sYMin + 1)));

	/*    for(lCount = ((pRect->sXMax - pRect->sXMin + 1) * (pRect->sYMax - pRect->sYMin + 1)); lCount >= 0; lCount--)
	 {
	 //
	 // Write the pixel value.
	 //
	 WriteData(ulValue);
	 }*/

	//
	// Reset the X extents to the entire screen.
	//
	WriteCommand(SSD1289_V_RAM_START_REG);
	WriteData(0x0000);
	WriteCommand(SSD1289_V_RAM_END_REG);
	WriteData(0x013F);

	//
	// Reset the Y extent to the full screen
	//
	WriteCommand(SSD1289_H_RAM_POS_REG);
	WriteData(0xEF00);

}

void LCDBPV2_RectDraw(const gRectangle *pRect, uINT_16 ulValue)
{
	//
	// Check the arguments.
	//
	//ASSERT(pContext);
	// ASSERT(pRect);

	//
	// Draw a line across the top of the rectangle.
	//
	LCDBPV2_LineDrawH(pRect->sXMin, pRect->sXMax, pRect->sYMin, ulValue);

	//
	// Return if the rectangle is one pixel tall.
	//
	if (pRect->sYMin == pRect->sYMax)
	{
		return;
	}

	//
	// Draw a line down the right side of the rectangle.
	//
	LCDBPV2_LineDrawV(pRect->sXMax, pRect->sYMin, pRect->sYMax, ulValue);

	//
	// Return if the rectangle is one pixel wide.
	//
	if (pRect->sXMin == pRect->sXMax)
	{
		return;
	}

	//
	// Draw a line across the bottom of the rectangle.
	//
	LCDBPV2_LineDrawH(pRect->sXMin, pRect->sXMax + 1, pRect->sYMax, ulValue);

	//
	// Return if the rectangle is two pixels tall.
	//
	if ((pRect->sYMin + 1) == pRect->sYMax)
	{
		return;
	}

	//
	// Draw a line up the left side of the rectangle.
	//
	LCDBPV2_LineDrawV(pRect->sXMin, pRect->sYMin, pRect->sYMax, ulValue);
}

void LCDBPV2_CircleDraw(uINT_16 lX, uINT_16 lY, uINT_16 lRadius,
		uINT_16 ulValue)
{
	INT_16 lA, lB, lD, lX1, lY1;

	//
	// Check the arguments.
	//
	//ASSERT(pContext);

	//
	// Initialize the variables that control the Bresenham circle drawing
	// algorithm.
	//
	lA = 0;
	lB = lRadius;
	lD = 3 - (2 * lRadius);

	//
	// Loop until the A delta is greater than the B delta, meaning that the
	// entire circle has been drawn.
	//
	while (lA <= lB)
	{
		//
		// Determine the row when subtracting the A delta.
		//
		lY1 = lY - lA;

		//
		// See if this row is within the clipping region.
		//
		//if((lY1 >= pContext->sClipRegion.sYMin) && (lY1 <= pContext->sClipRegion.sYMax))
		//{
		//
		// Determine the column when subtracting the B delta.
		//
		lX1 = lX - lB;

		//
		// If this column is within the clipping region, then draw a pixel
		// at that position.
		//
		//if((lX1 >= pContext->sClipRegion.sXMin) && (lX1 <= pContext->sClipRegion.sXMax))
		//{
		LCDBPV2_PixelDraw(lX1, lY1, ulValue);
		//}

		//
		// Determine the column when adding the B delta.
		//
		lX1 = lX + lB;

		//
		// If this column is within the clipping region, then draw a pixel
		// at that position.
		//
		//if((lX1 >= pContext->sClipRegion.sXMin) && (lX1 <= pContext->sClipRegion.sXMax))
		//{
		LCDBPV2_PixelDraw(lX1, lY1, ulValue);
		//}
		//}

		//
		// Determine the row when adding the A delta.
		//
		lY1 = lY + lA;

		//
		// See if this row is within the clipping region, and the A delta is
		// not zero (otherwise, it will be the same row as when the A delta was
		// subtracted).
		//
		//if((lY1 >= pContext->sClipRegion.sYMin) &&  (lY1 <= pContext->sClipRegion.sYMax) &&       (lA != 0))
		//{
		//
		// Determine the column when subtracting the B delta.
		//
		lX1 = lX - lB;

		//
		// If this column is within the clipping region, then draw a pixel
		// at that position.
		//
		//if((lX1 >= pContext->sClipRegion.sXMin) &&  (lX1 <= pContext->sClipRegion.sXMax))
		//{
		LCDBPV2_PixelDraw(lX1, lY1, ulValue);
		//}

		//
		// Determine the column when adding the B delta.
		//
		lX1 = lX + lB;

		//
		// If this column is within the clipping region, then draw a pixel
		// at that position.
		//
		//if((lX1 >= pContext->sClipRegion.sXMin) &&   (lX1 <= pContext->sClipRegion.sXMax))
		//{
		LCDBPV2_PixelDraw(lX1, lY1, ulValue);
		//}
		//}

		//
		// Only draw the complementary pixels if the A and B deltas are
		// different (otherwise, they describe the same set of pixels).
		//
		if (lA != lB)
		{
			//
			// Determine the row when subtracting the B delta.
			//
			lY1 = lY - lB;

			//
			// See if this row is within the clipping region.
			//
			// if((lY1 >= pContext->sClipRegion.sYMin) && (lY1 <= pContext->sClipRegion.sYMax))
			//{
			//
			// Determine the column when subtracting the a delta.
			//
			lX1 = lX - lA;

			//
			// If this column is within the clipping region, then draw a
			// pixel at that position.
			//
			//if((lX1 >= pContext->sClipRegion.sXMin) && (lX1 <= pContext->sClipRegion.sXMax))
			//{
			LCDBPV2_PixelDraw(lX1, lY1, ulValue);
			//}

			//
			// Only draw the mirrored pixel if the A delta is non-zero
			// (otherwise, it will be the same pixel).
			//
			if (lA != 0)
			{
				//
				// Determine the column when adding the A delta.
				//
				lX1 = lX + lA;

				//
				// If this column is within the clipping region, then draw
				// a pixel at that position.
				//
				//if((lX1 >= pContext->sClipRegion.sXMin) &&(lX1 <= pContext->sClipRegion.sXMax))
				//{
				LCDBPV2_PixelDraw(lX1, lY1, ulValue);
				//}
			}
			//}

			//
			// Determine the row when adding the B delta.
			//
			lY1 = lY + lB;

			//
			// See if this row is within the clipping region.
			//
			//if((lY1 >= pContext->sClipRegion.sYMin) &&(lY1 <= pContext->sClipRegion.sYMax))
			//{
			//
			// Determine the column when subtracting the A delta.
			//
			lX1 = lX - lA;

			//
			// If this column is within the clipping region, then draw a
			// pixel at that position.
			//
			//if((lX1 >= pContext->sClipRegion.sXMin) && (lX1 <= pContext->sClipRegion.sXMax))
			//{
			LCDBPV2_PixelDraw(lX1, lY1, ulValue);
			//}

			//
			// Only draw the mirrored pixel if the A delta is non-zero
			// (otherwise, it will be the same pixel).
			//
			if (lA != 0)
			{
				//
				// Determine the column when adding the A delta.
				//
				lX1 = lX + lA;

				//
				// If this column is within the clipping region, then draw
				// a pixel at that position.
				//
				//if((lX1 >= pContext->sClipRegion.sXMin) &&(lX1 <= pContext->sClipRegion.sXMax))
				//{
				LCDBPV2_PixelDraw(lX1, lY1, ulValue);
				//}
			}
			//}
		}

		//
		// See if the error term is negative.
		//
		if (lD < 0)
		{
			//
			// Since the error term is negative, adjust it based on a move in
			// only the A delta.
			//
			lD += (4 * lA) + 6;
		}
		else
		{
			//
			// Since the error term is non-negative, adjust it based on a move
			// in both the A and B deltas.
			//
			lD += (4 * (lA - lB)) + 10;

			//
			// Decrement the B delta.
			//
			lB -= 1;
		}

		//
		// Increment the A delta.
		//
		lA++;
	}
}

//*****************************************************************************
//
//! Draws a filled circle.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param lX is the X coordinate of the center of the circle.
//! \param lY is the Y coordinate of the center of the circle.
//! \param lRadius is the radius of the circle.
//!
//! This function draws a filled circle, utilizing the Bresenham circle drawing
//! algorithm.  The extent of the circle is from \e lX - \e lRadius to \e lX +
//! \e lRadius and \e lY - \e lRadius to \e lY + \e lRadius, inclusive.
//!
//! \return None.
//
//*****************************************************************************
void LCDBPV2_CircleFill(uINT_16 lX, uINT_16 lY, uINT_16 lRadius,
		uINT_16 ulValue)
{
	INT_16 lA, lB, lD, lX1, lX2, lY1;

	//
	// Check the arguments.
	//
	// ASSERT(pContext);

	//
	// Initialize the variables that control the Bresenham circle drawing
	// algorithm.
	//
	lA = 0;
	lB = lRadius;
	lD = 3 - (2 * lRadius);

	//
	// Loop until the A delta is greater than the B delta, meaning that the
	// entire circle has been filled.
	//
	while (lA <= lB)
	{
		//
		// Determine the row when subtracting the A delta.
		//
		lY1 = lY - lA;

		//
		// See if this row is within the clipping region.
		//
		//if((lY1 >= pContext->sClipRegion.sYMin) && (lY1 <= pContext->sClipRegion.sYMax))
		//{
		//
		// Determine the column when subtracting the B delta, and move it
		// to the left edge of the clipping region if it is to the left of
		// the clipping region.
		//
		lX1 = lX - lB;
		/*if(lX1 < pContext->sClipRegion.sXMin)
		 {
		 lX1 = pContext->sClipRegion.sXMin;
		 }*/

		//
		// Determine the column when adding the B delta, and move it to the
		// right edge of the clipping region if it is to the right of the
		// clipping region.
		//
		lX2 = lX + lB;
		/*if(lX2 > pContext->sClipRegion.sXMax)
		 {
		 lX2 = pContext->sClipRegion.sXMax;
		 }*/

		//
		// Draw a horizontal line if this portion of the circle is within
		// the clipping region.
		//
		if (lX1 <= lX2)
		{
			LCDBPV2_LineDrawH(lX1, lX2 + 1, lY1, ulValue);
		}
		//}

		//
		// Determine the row when adding the A delta.
		//
		lY1 = lY + lA;

		//
		// See if this row is within the clipping region, and the A delta is
		// not zero (otherwise, this describes the same row of the circle).
		//
		//if((lY1 >= pContext->sClipRegion.sYMin) && (lY1 <= pContext->sClipRegion.sYMax) && (lA != 0))
		//{
		//
		// Determine the column when subtracting the B delta, and move it
		// to the left edge of the clipping region if it is to the left of
		// the clipping region.
		//
		lX1 = lX - lB;
		/*if(lX1 < pContext->sClipRegion.sXMin)
		 {
		 lX1 = pContext->sClipRegion.sXMin;
		 }*/

		//
		// Determine the column when adding the B delta, and move it to the
		// right edge of the clipping region if it is to the right of the
		// clipping region.
		//
		lX2 = lX + lB;
		/*if(lX2 > pContext->sClipRegion.sXMax)
		 {
		 lX2 = pContext->sClipRegion.sXMax;
		 }*/

		//
		// Draw a horizontal line if this portion of the circle is within
		// the clipping region.
		//
		if (lX1 <= lX2)
		{
			LCDBPV2_LineDrawH(lX1, lX2 + 1, lY1, ulValue);
		}
		//}

		//
		// Only draw the complementary lines if the B delta is about to change
		// and the A and B delta are different (otherwise, they describe the
		// same set of pixels).
		//
		if ((lD >= 0) && (lA != lB))
		{
			//
			// Determine the row when subtracting the B delta.
			//
			lY1 = lY - lB;

			//
			// See if this row is within the clipping region.
			//
			// if((lY1 >= pContext->sClipRegion.sYMin) && (lY1 <= pContext->sClipRegion.sYMax))
			//{
			//
			// Determine the column when subtracting the A delta, and move
			// it to the left edge of the clipping regino if it is to the
			// left of the clipping region.
			//
			lX1 = lX - lA;
			/*if(lX1 < pContext->sClipRegion.sXMin)
			 {
			 lX1 = pContext->sClipRegion.sXMin;
			 }*/

			//
			// Determine the column when adding the A delta, and move it to
			// the right edge of the clipping region if it is to the right
			// of the clipping region.
			//
			lX2 = lX + lA;
			/*if(lX2 > pContext->sClipRegion.sXMax)
			 {
			 lX2 = pContext->sClipRegion.sXMax;
			 }*/

			//
			// Draw a horizontal line if this portion of the circle is
			// within the clipping region.
			//
			if (lX1 <= lX2)
			{
				LCDBPV2_LineDrawH(lX1, lX2 + 1, lY1, ulValue);
			}
			//}

			//
			// Determine the row when adding the B delta.
			//
			lY1 = lY + lB;

			//
			// See if this row is within the clipping region.
			//
			//if((lY1 >= pContext->sClipRegion.sYMin) && (lY1 <= pContext->sClipRegion.sYMax))
			//{
			//
			// Determine the column when subtracting the A delta, and move
			// it to the left edge of the clipping region if it is to the
			// left of the clipping region.
			//
			lX1 = lX - lA;
			/*if(lX1 < pContext->sClipRegion.sXMin)
			 {
			 lX1 = pContext->sClipRegion.sXMin;
			 }*/

			//
			// Determine the column when adding the A delta, and move it to
			// the right edge of the clipping region if it is to the right
			// of the clipping region.
			//
			lX2 = lX + lA;
			/*if(lX2 > pContext->sClipRegion.sXMax)
			 {
			 lX2 = pContext->sClipRegion.sXMax;
			 }*/

			//
			// Draw a horizontal line if this portion of the circle is
			// within the clipping region.
			//
			if (lX1 <= lX2)
			{
				LCDBPV2_LineDrawH(lX1, lX2 + 1, lY1, ulValue);
			}
			//}
		}

		//
		// See if the error term is negative.
		//
		if (lD < 0)
		{
			//
			// Since the error term is negative, adjust it based on a move in
			// only the A delta.
			//
			lD += (4 * lA) + 6;
		}
		else
		{
			//
			// Since the error term is non-negative, adjust it based on a move
			// in both the A and B deltas.
			//
			lD += (4 * (lA - lB)) + 10;

			//
			// Decrement the B delta.
			//
			lB -= 1;
		}

		//
		// Increment the A delta.
		//
		lA++;
	}
}
void LCDBPV2_StringDraw(const tFont *pFont, const uINT_8 *pcString, uINT_16 lX,
		uINT_16 lY, uINT_16 bOpaque, uINT_16 ulValue)
{
	long lIdx, lX0, lY0, lCount, lOff, lOn, lBit;
	const unsigned char *pucData;
	const unsigned char *pucGlyphs;
	const unsigned short *pusOffset;
	unsigned char ucFirst, ucLast, ucAbsent;

	pucGlyphs = pFont->pucData;
	pusOffset = pFont->pusOffset;
	ucFirst = 32;
	ucLast = 126;

	//
	// Loop through the characters in the string.
	//
	while (*pcString)
	{

		//
		// Get a pointer to the font data for the next character from the
		// string.  If there is not a glyph for the next character, replace it
		// with the "absent" character (usually '.').
		//
		if ((*pcString >= ucFirst) && (*pcString <= ucLast))
		{
			pucData = (pucGlyphs + pusOffset[*pcString - ucFirst]);
		}
		else
		{
			pucData = (pucGlyphs + pusOffset[ucAbsent - ucFirst]);
		}
		pcString++;

		//
		// Loop through the bytes in the encoded data for this glyph.
		//
		for (lIdx = 2, lX0 = 0, lBit = 0, lY0 = 0; lIdx < pucData[0];)
		{
			// the font is compressed with a pixel RLE scheme.
			// See if this is a byte that encodes some on and off pixels.
			if (pucData[lIdx])
			{
				// Extract the number of off pixels.
				lOff = (pucData[lIdx] >> 4) & 15;
				// Extract the number of on pixels.
				lOn = pucData[lIdx] & 15;
				// Skip past this encoded byte.
				lIdx++;
			}
			// Otherwise, see if this is a repeated on pixel byte.
			else if (pucData[lIdx + 1] & 0x80)
			{
				// There are no off pixels in this encoding.
				lOff = 0;
				// Extract the number of on pixels.
				lOn = (pucData[lIdx + 1] & 0x7f) * 8;
				// Skip past these two encoded bytes.
				lIdx += 2;
			}
			// Otherwise, this is a repeated off pixel byte.
			else
			{
				// Extract the number of off pixels.
				lOff = pucData[lIdx + 1] * 8;
				// There are no on pixels in this encoding.
				lOn = 0;
				// Skip past these two encoded bytes.
				lIdx += 2;
			}
			// Loop while there are any off pixels.
			while (lOff)
			{
				// See if the bottom of the clipping region has been exceeded.

				// See if there is more than one on pixel that will fit onto
				// the current row.
				if ((lOff > 1) && ((lX0 + 1) < pucData[1]))
				{
					// Determine the number of on pixels that will fit on this row.
					lCount = (
							((lX0 + lOff) > pucData[1]) ?
									pucData[1] - lX0 : lOff);
					// If this row is within the clipping region, draw a horizontal line that corresponds to the sequence of on pixels.
					//LCDBPV2_LineDrawH(lX + lX0, lX + lX0 + lCount - 1,lY + lY0, bOpaque); //foreground

					// Decrement the count of on pixels by the number on this row.
					lOff -= lCount;
					// Increment the X offset by the number of on pixels.
					lX0 += lCount;
				}
				// Otherwise, there is only a single on pixel that can be drawn.
				else
				{
					// If this pixel is within the clipping region, then draw it.

					// Decrement the count of on pixels.
					lOff--;
					// Increment the X offset.
					lX0++;
				}

				// See if the X offset has reached the right side of the character glyph.
				if (lX0 == pucData[1])
				{
					// Increment the Y offset.
					lY0++;

					// Reset the X offset to the left side of the character glyph.
					lX0 = 0;
				}
			}

			// Loop while there are any on pixels.
			while (lOn)
			{
				// See if the bottom of the clipping region has been exceeded.

				// See if there is more than one on pixel that will fit onto the current row.
				if ((lOn > 1) && ((lX0 + 1) < pucData[1]))
				{
					// Determine the number of on pixels that will fit on this row.
					lCount =
							(((lX0 + lOn) > pucData[1]) ? pucData[1] - lX0 : lOn);

					// If this row is within the clipping region, draw a horizontal line that corresponds to the sequence of on pixels.
					LCDBPV2_LineDrawH(lX + lX0, lX + lX0 + lCount - 1, lY + lY0,
							ulValue); //rojo

					// Decrement the count of on pixels by the number on this row.
					lOn -= lCount;

					// Increment the X offset by the number of on pixels.
					lX0 += lCount;
				}

				// Otherwise, there is only a single on pixel that can be drawn.
				else
				{
					// If this pixel is within the clipping region, then draw it.
					LCDBPV2_PixelDraw(lX + lX0, lY + lY0, ulValue); //amarillo

					// Decrement the count of on pixels.
					lOn--;

					// Increment the X offset.
					lX0++;
				}

				// See if the X offset has reached the right side of the character glyph.
				if (lX0 == pucData[1])
				{
					// Increment the Y offset.
					lY0++;

					// Reset the X offset to the left side of the character glyph.
					lX0 = 0;
				}
			}
		}

		// Increment the X coordinate by the width of the character.
		lX += pucData[1];
	}
}
void LCDBPV2_StringDraw_(const tContext *pContext, const char *pcString,
		long lLength, long lX, long lY, unsigned long bOpaque)
{
	long lIdx, lX0, lY0, lCount, lOff, lOn, lBit;
	const unsigned char *pucData;
	const unsigned char *pucGlyphs;
	const unsigned short *pusOffset;
	unsigned char ucFirst, ucLast, ucAbsent;
	tContext sCon;

	//
	// Check the arguments.
	//
	//ASSERT(pContext);
	//ASSERT(pcString);

	//
	// This function doesn't support wide character fonts or wrapped fonts.
	//
	//ASSERT(!(pContext->pFont->ucFormat && (FONT_FMT_WRAPPED | FONT_WIDE_MARKER)));

	//
	// Copy the drawing context into a local structure that can be modified.
	//
	sCon = *pContext;

	//
	// Extract various parameters from the font depending upon whether it's
	// in the tFont or tFontEx format.
	//
	if (pContext->pFont->ucFormat & FONT_EX_MARKER)
	{
		tFontEx *pFont;

		pFont = (tFontEx *) (pContext->pFont);

		pucGlyphs = pFont->pucData;
		pusOffset = pFont->pusOffset;
		ucFirst = pFont->ucFirst;
		ucLast = pFont->ucLast;

		//
		// Does the default absent character replacement exist in the font?
		//
		/*	        if((ABSENT_CHAR_REPLACEMENT >= ucFirst) && (ABSENT_CHAR_REPLACEMENT <= ucLast))
		 {
		 //
		 // Yes - use the standard character when an absent character is
		 // found.
		 //
		 ucAbsent = ABSENT_CHAR_REPLACEMENT;
		 }
		 else
		 {*/
		//
		// The default absent character is not present in the font so use
		// the first character instead.
		//
		ucAbsent = pFont->ucFirst;
		//}
	}
	else
	{
		pucGlyphs = pContext->pFont->pucData;
		pusOffset = pContext->pFont->pusOffset;
		ucFirst = 32;
		ucLast = 126;
		//ucAbsent = ABSENT_CHAR_REPLACEMENT;
		// ucAbsent = pContext->pFont->ucFirst;
	}

	//
	// Loop through the characters in the string.
	//
	while (*pcString && lLength--)
	{
		//
		// Stop drawing the string if the right edge of the clipping region has
		// been exceeded.
		//
		if (lX > sCon.sClipRegion.sXMax)
		{
			break;
		}

		//
		// Get a pointer to the font data for the next character from the
		// string.  If there is not a glyph for the next character, replace it
		// with the "absent" character (usually '.').
		//
		if ((*pcString >= ucFirst) && (*pcString <= ucLast))
		{
			pucData = (pucGlyphs + pusOffset[*pcString - ucFirst]);
		}
		else
		{
			pucData = (pucGlyphs + pusOffset[ucAbsent - ucFirst]);
		}
		pcString++;

		//
		// See if the entire character is to the left of the clipping region.
		//
		if ((lX + pucData[1]) < sCon.sClipRegion.sXMin)
		{
			//
			// Increment the X coordinate by the width of the character.
			//
			lX += pucData[1];

			//
			// Go to the next character in the string.
			//
			continue;
		}

		//
		// Loop through the bytes in the encoded data for this glyph.
		//
		for (lIdx = 2, lX0 = 0, lBit = 0, lY0 = 0; lIdx < pucData[0];)
		{
			//
			// See if the bottom of the clipping region has been exceeded.
			//
			if ((lY + lY0) > sCon.sClipRegion.sYMax)
			{
				//
				// Stop drawing this character.
				//
				break;
			}

			//
			// See if the font is uncompressed.
			//
			if ((sCon.pFont->ucFormat & ~FONT_EX_MARKER)
					== FONT_FMT_UNCOMPRESSED)
			{
				//
				// Count the number of off pixels from this position in the
				// glyph image.
				//
				for (lOff = 0; lIdx < pucData[0];)
				{
					//
					// Get the number of zero pixels at this position.
					//
					lCount = _norm(pucData[lIdx] << (24 + lBit));

					//
					// If there were more than 8, then it is a "false" result
					// since it counted beyond the end of the current byte.
					// Therefore, simply limit it to the number of pixels
					// remaining in this byte.
					//
					if (lCount > 8)
					{
						lCount = 8 - lBit;
					}

					//
					// Increment the number of off pixels.
					//
					lOff += lCount;

					//
					// Increment the bit position within the byte.
					//
					lBit += lCount;

					//
					// See if the end of the byte has been reached.
					//
					if (lBit == 8)
					{
						//
						// Advance to the next byte and continue counting off
						// pixels.
						//
						lBit = 0;
						lIdx++;
					}
					else
					{
						//
						// Since the end of the byte was not reached, there
						// must be an on pixel.  Therefore, stop counting off
						// pixels.
						//
						break;
					}
				}

				//
				// Count the number of on pixels from this position in the
				// glyph image.
				//
				for (lOn = 0; lIdx < pucData[0];)
				{
					//
					// Get the number of one pixels at this location (by
					// inverting the data and counting the number of zeros).
					//
					lCount = _norm(~(pucData[lIdx] << (24 + lBit)));

					//
					// If there were more than 8, then it is a "false" result
					// since it counted beyond the end of the current byte.
					// Therefore, simply limit it to the number of pixels
					// remaining in this byte.
					//
					if (lCount > 8)
					{
						lCount = 8 - lBit;
					}

					//
					// Increment the number of on pixels.
					//
					lOn += lCount;

					//
					// Increment the bit position within the byte.
					//
					lBit += lCount;

					//
					// See if the end of the byte has been reached.
					//
					if (lBit == 8)
					{
						//
						// Advance to the next byte and continue counting on
						// pixels.
						//
						lBit = 0;
						lIdx++;
					}
					else
					{
						//
						// Since the end of the byte was not reached, there
						// must be an off pixel.  Therefore, stop counting on
						// pixels.
						//
						break;
					}
				}
			}

			//
			// Otherwise, the font is compressed with a pixel RLE scheme.
			//
			else
			{
				//
				// See if this is a byte that encodes some on and off pixels.
				//
				if (pucData[lIdx])
				{
					//
					// Extract the number of off pixels.
					//
					lOff = (pucData[lIdx] >> 4) & 15;

					//
					// Extract the number of on pixels.
					//
					lOn = pucData[lIdx] & 15;

					//
					// Skip past this encoded byte.
					//
					lIdx++;
				}

				//
				// Otherwise, see if this is a repeated on pixel byte.
				//
				else if (pucData[lIdx + 1] & 0x80)
				{
					//
					// There are no off pixels in this encoding.
					//
					lOff = 0;

					//
					// Extract the number of on pixels.
					//
					lOn = (pucData[lIdx + 1] & 0x7f) * 8;

					//
					// Skip past these two encoded bytes.
					//
					lIdx += 2;
				}

				//
				// Otherwise, this is a repeated off pixel byte.
				//
				else
				{
					//
					// Extract the number of off pixels.
					//
					lOff = pucData[lIdx + 1] * 8;

					//
					// There are no on pixels in this encoding.
					//
					lOn = 0;

					//
					// Skip past these two encoded bytes.
					//
					lIdx += 2;
				}
			}

			//
			// Loop while there are any off pixels.
			//
			while (lOff)
			{
				//
				// See if the bottom of the clipping region has been exceeded.
				//
				if ((lY + lY0) > sCon.sClipRegion.sYMax)
				{
					//
					// Ignore the remainder of the on pixels.
					//
					break;
				}

				//
				// See if there is more than one on pixel that will fit onto
				// the current row.
				//
				if ((lOff > 1) && ((lX0 + 1) < pucData[1]))
				{
					//
					// Determine the number of on pixels that will fit on this
					// row.
					//
					lCount = (
							((lX0 + lOff) > pucData[1]) ?
									pucData[1] - lX0 : lOff);

					//
					// If this row is within the clipping region, draw a
					// horizontal line that corresponds to the sequence of on
					// pixels.
					//
					if (((lY + lY0) >= sCon.sClipRegion.sYMin) && bOpaque)
					{
						sCon.ulForeground = pContext->ulBackground;
						GrLineDrawH(&sCon, lX + lX0, lX + lX0 + lCount - 1,
								lY + lY0);
					}

					//
					// Decrement the count of on pixels by the number on this
					// row.
					//
					lOff -= lCount;

					//
					// Increment the X offset by the number of on pixels.
					//
					lX0 += lCount;
				}

				//
				// Otherwise, there is only a single on pixel that can be
				// drawn.
				//
				else
				{
					//
					// If this pixel is within the clipping region, then draw
					// it.
					//
					if (((lX + lX0) >= sCon.sClipRegion.sXMin)
							&& ((lX + lX0) <= sCon.sClipRegion.sXMax)
							&& ((lY + lY0) >= sCon.sClipRegion.sYMin)
							&& bOpaque)
					{
						DpyPixelDraw(pContext->pDisplay, lX + lX0, lY + lY0,
								pContext->ulBackground);
					}

					//
					// Decrement the count of on pixels.
					//
					lOff--;

					//
					// Increment the X offset.
					//
					lX0++;
				}

				//
				// See if the X offset has reached the right side of the
				// character glyph.
				//
				if (lX0 == pucData[1])
				{
					//
					// Increment the Y offset.
					//
					lY0++;

					//
					// Reset the X offset to the left side of the character
					// glyph.
					//
					lX0 = 0;
				}
			}

			//
			// Loop while there are any on pixels.
			//
			while (lOn)
			{
				//
				// See if the bottom of the clipping region has been exceeded.
				//
				if ((lY + lY0) > sCon.sClipRegion.sYMax)
				{
					//
					// Ignore the remainder of the on pixels.
					//
					break;
				}

				//
				// See if there is more than one on pixel that will fit onto
				// the current row.
				//
				if ((lOn > 1) && ((lX0 + 1) < pucData[1]))
				{
					//
					// Determine the number of on pixels that will fit on this
					// row.
					//
					lCount =
							(((lX0 + lOn) > pucData[1]) ? pucData[1] - lX0 : lOn);

					//
					// If this row is within the clipping region, draw a
					// horizontal line that corresponds to the sequence of on
					// pixels.
					//
					if ((lY + lY0) >= sCon.sClipRegion.sYMin)
					{
						sCon.ulForeground = pContext->ulForeground;
						GrLineDrawH(&sCon, lX + lX0, lX + lX0 + lCount - 1,
								lY + lY0);
					}

					//
					// Decrement the count of on pixels by the number on this
					// row.
					//
					lOn -= lCount;

					//
					// Increment the X offset by the number of on pixels.
					//
					lX0 += lCount;
				}

				//
				// Otherwise, there is only a single on pixel that can be
				// drawn.
				//
				else
				{
					//
					// If this pixel is within the clipping region, then draw
					// it.
					//
					if (((lX + lX0) >= sCon.sClipRegion.sXMin)
							&& ((lX + lX0) <= sCon.sClipRegion.sXMax)
							&& ((lY + lY0) >= sCon.sClipRegion.sYMin))
					{
						DpyPixelDraw(pContext->pDisplay, lX + lX0, lY + lY0,
								pContext->ulForeground);
					}

					//
					// Decrement the count of on pixels.
					//
					lOn--;

					//
					// Increment the X offset.
					//
					lX0++;
				}

				//
				// See if the X offset has reached the right side of the
				// character glyph.
				//
				if (lX0 == pucData[1])
				{
					//
					// Increment the Y offset.
					//
					lY0++;

					//
					// Reset the X offset to the left side of the character
					// glyph.
					//
					lX0 = 0;
				}
			}
		}

		//
		// Increment the X coordinate by the width of the character.
		//
		lX += pucData[1];
	}
}

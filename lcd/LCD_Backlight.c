/*
 * LCD_Backlight.c
 *
 *  Created on: 27 αιπε 2014
 *      Author: Itay Komemy
 */



#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "grlib/grlib.h"
#include "driverlib/i2c.h"
#include "LCD_Backlight.h"


//Inits and sets to 50%
void initLCDBacklight()
{
    SysCtlPWMClockSet(SYSCTL_PWMDIV_4);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
    SysCtlPeripheralEnable(LCD_BL_PERIPH);

    GPIOPinTypePWM(LCD_BL_BASE, GPIO_PIN_1);

}

//returns int from 0 to 100
int getBacklightValue();

//Value from 0 to 100
void setBacklight(int value);

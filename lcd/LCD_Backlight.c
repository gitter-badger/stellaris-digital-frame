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

static int BL_Period;
static int BL_Duty;

//Inits and sets to 50%
void initLCDBacklight()
{
    SysCtlPWMClockSet(SYSCTL_PWMDIV_4);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
    SysCtlPeripheralEnable(LCD_BL_PERIPH);

    GPIOPinTypePWM(LCD_BL_BASE, GPIO_PIN_1);
    // PWMGenConfigure(LCD_BL_BASE, PWM_GEN_1,PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    BL_Period = 400;
    PWMGenPeriodSet(LCD_BL_BASE, PWM_GEN_1, BL_Period);
    PWMPulseWidthSet(LCD_BL_BASE,PWM_OUT_1,100);
    PWMOutputState(LCD_BL_BASE,PWM_OUT_1_BIT,true);
    setBacklight(50);

}

//returns int from 0 to 100
int getBacklightValue() {

}

//Value from 0 to 100
void setBacklight(int value) {
	if (value > 100) {
		value = 100;
	}
	if (value < 1) {
		value = 1;
	}

	BL_Duty = value;
	int dutyPeriod = BL_Period * value / 100;
	PWMPulseWidthSet(LCD_BL_BASE,PWM_OUT_1,dutyPeriod);


}

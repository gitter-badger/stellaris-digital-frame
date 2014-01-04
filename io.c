#include <stdint.h>
#include <stdlib.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/watchdog.h"
#include "driverlib/pwm.h"
#include "driverlib/ssi.h"
#include "io.h"

volatile int sw_pressed = 0;

void ssi_wait(void)
{
	unsigned long r;

	while (HWREG(SSI0_BASE + SSI_O_SR) & SSI_SR_BSY)
		;  //busy?
	//while(!(HWREG(SSI0_BASE + SSI_O_SR) & SSI_SR_TFE));  //transmit fifo empty?

	while (SSIDataGetNonBlocking(SSI0_BASE, &r))
		; //clear receive fifo

	return;
}

void ssi_write(unsigned char c)
{
	SSIDataPut(SSI0_BASE, c);

	return;
}

unsigned char ssi_read(void)
{
	unsigned long r;

	SSIDataGet(SSI0_BASE, &r);

	return (unsigned char) r;
}

unsigned char ssi_readwrite(unsigned char c)
{
	unsigned long r;

	SSIDataPut(SSI0_BASE, c);
	SSIDataGet(SSI0_BASE, &r);

	return (unsigned char) r;
}

void ssi_speed(unsigned long speed)
{
	unsigned long clk;

	clk = SysCtlClockGet();

	if ((speed == 0) || (speed > (clk / 2)))
	{
		speed = clk / 2;
	}

	if (speed > SSI_SPEED)
	{
		speed = SSI_SPEED;
	}

	SSIDisable(SSI0_BASE);
	SSIConfigSetExpClk(SSI0_BASE, clk, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER,
			speed, 8);
	SSIEnable(SSI0_BASE);

	return;
}

void ssi_off(void)
{
	ssi_wait();

	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_5); //SCK, SI = gpio
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_5, 0); //SCK, SI = low

	return;
}

void ssi_on(void)
{
	GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_5); //SCK, SI = ssi

	return;
}

void pwm_led(unsigned int power)
{
	unsigned long period, duty;

	if (power) //switch on
	{
		if (power < LCD_PWMMIN)
		{
			power = LCD_PWMMIN;
		}

		period = SysCtlClockGet() / LCD_PWMFREQ;
		duty = (power * (period / 2)) / 100;

		PWMGenPeriodSet(PWM_BASE, PWM_GEN_2, period); //freq Hz
		PWMPulseWidthSet(PWM_BASE, PWM_OUT_5, duty); //duty %
		PWMGenEnable(PWM_BASE, PWM_GEN_2); //GEN2 on
		PWMOutputState(PWM_BASE, PWM_OUT_5_BIT, 1); //PWM5 on
		GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_7); //PWM5 assign to PWM
	}
	else //switch off
	{
		PWMOutputState(PWM_BASE, PWM_OUT_5_BIT, 0); //PWM5 off
		PWMGenDisable(PWM_BASE, PWM_GEN_2); //GEN2 off
		GPIOPinTypePWM(GPIO_PORTE_BASE, 0); //PWM5 release from PWM
		GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_7, 0); //PWM5 low
	}

	return;
}

void cpu_speed(unsigned int low_speed)
{
	delay_ms(10);

	IntMasterDisable();
	pwm_led(0);

	if (low_speed)
	{
//for rev A1 & A2 set LDO=2.75V for correct PLL function -> reset to 2.50V
#if defined LM3S_REV_A1 || defined LM3S_REV_A2
		SysCtlLDOSet(SYSCTL_LDO_2_50V);
#endif
		SysCtlClockSet(
				SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN
						| SYSCTL_XTAL_8MHZ); //8 MHz
		SysTickDisable();
		SysTickPeriodSet(SysCtlClockGet() / 100); //100 Hz
		SysTickEnable();
		pwm_led(LCD_PWMSTANDBY);
		ssi_speed(0);

	}
	else
	{
//for rev A1 & A2 set LDO=2.75V for correct PLL function
#if defined LM3S_REV_A1 || defined LM3S_REV_A2
		SysCtlLDOSet(SYSCTL_LDO_2_75V);
#endif
		SysCtlClockSet(
				LM3S_SYSDIV | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
						| SYSCTL_XTAL_8MHZ); //speed up
		SysTickDisable();
		SysTickPeriodSet(SysCtlClockGet() / 100); //100 Hz
		SysTickEnable();
		pwm_led(100);
		ssi_speed(0);
	}

	IntMasterEnable();

	delay_ms(10);

#ifdef DEBUG
	UARTDisable(DEBUGUART);
	UARTConfigSetExpClk(DEBUGUART, SysCtlClockGet(), DEBUGBAUD, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	UARTEnable(DEBUGUART);
#endif

	return;
}

/*void cpu_reset(void)
 {
 //disable interrupts
 cpu_speed(1);
 SysTickDisable();
 IntMasterDisable();
 delay_ms(1);

 //jump to 0x0000
 __asm("cpsid   i\n"               //disable interrupts

 "ldr     r0, =%0\n"         //load app start address

 "ldr     r1, =0xe000ed08\n" //set vector table addr to the beginning of the app
 "str     r0, [r1]\n"

 "ldr     r1, [r0]\n"        //load stack ptr from the app's vector table
 "mov     sp, r1\n"

 "ldr     r0, [r0, #4]\n"    //load the initial PC from the app's vector table and
 "bx      r0\n"              //branch to the app's entry point
 :
 : "i" (0x0000)); //0x0000 0x5000

 return;
 }*/

void init_bor(unsigned int on)
{
	unsigned long reset;

	if (on)
	{
		reset = SYSCTL_BOR_RESET;
	}
	else
	{
		reset = 0;
	}

	SysCtlBrownOutConfigSet(reset | SYSCTL_BOR_RESAMPLE, 4000); //delay 4000 (<8192)

	return;
}

void init_periph(void)
{
	//init pwm for backlight
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
//	SysCtlPeripheralReset(SYSCTL_PERIPH_PWM);
//	PWMGenConfigure(PWM1_BASE, PWM_GEN_2,
//			PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

	//init ssi0: SD
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralReset(SYSCTL_PERIPH_SSI0);
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
			SSI_MODE_MASTER, SysCtlClockGet() / 4, 8);
	SSIEnable(SSI0_BASE);
	SSIDataPutNonBlocking(SSI0_BASE, 0xff);
	SSIDataPutNonBlocking(SSI0_BASE, 0xff);
	SSIDataPutNonBlocking(SSI0_BASE, 0xff); //dummy write to set ssi fifo bits

	//init ssi1: Touch
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
	SysCtlPeripheralReset(SYSCTL_PERIPH_SSI1);
	SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
			SSI_MODE_MASTER, SysCtlClockGet() / 4, 8);
	SSIEnable(SSI1_BASE);
	SSIDataPut(SSI1_BASE, 0xff);
	SSIDataPutNonBlocking(SSI1_BASE, 0xff);
	SSIDataPutNonBlocking(SSI1_BASE, 0xff); //dummy write to set ssi fifo bits

	return;
}

void init_pins(void)
{
	//GPIO A: SD
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOSetInput(GPIO_PORTA_BASE, GPIO_PIN_4); //SD_SI = input
	GPIOSetOutput(GPIO_PORTA_BASE,
			GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6); //SD_SCK, SD_SI, SD_CS ,SD_PWR = output
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3 | GPIO_PIN_6, GPIO_PIN_3); //SD_PWR = low / SD_CS = high
	GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5); //SD_SCK, SD_SO, SD_SI = ssi

	//GPIO C: JTAG
/*
#if defined LM3S_REV_A1 || defined LM3S_REV_A2 //for rev A1 & A2 set JTAG pullups on
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPadConfigSet(GPIO_PORTC_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
			GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); //pullup
#endif
*/

	//GPIO D: LCD DATA 0-7
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOSetOutput(GPIO_PORTD_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
					| GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	GPIOPinWrite(GPIO_PORTD_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
					| GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, 0x0); // All low.

	//GPIO E: LCD DATA 8-15
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOSetOutput(GPIO_PORTE_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
					| GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	GPIOPinWrite(GPIO_PORTE_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
					| GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, 0x0); // All low.

	//GPIO F: LCD_RST, LCD_BL, Touch_SSI
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOSetInput(GPIO_PORTF_BASE, GPIO_PIN_4); //TS_DI = input
	GPIOSetOutput(GPIO_PORTF_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5); //LCD_RST, LCD_PWM, TS_CLK, TS_CS, TS_DO = output
	GPIOPinWrite(GPIO_PORTF_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3,
			GPIO_PIN_3); //LCD_RST,LCD_PWM = low / TS_CS = high
	GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5); //SCK, SO, SI = ssi

	//GPIO G: Touch_IRQ
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	GPIOSetInput(GPIO_PORTG_BASE, GPIO_PIN_0); //TS_IRQ = input
	//TODO: make it interrupt sensitive


	//GPIO J: LCD
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
	GPIOSetOutput(GPIO_PORTJ_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // LCD_CS, LCD_RS, LCD_RD, LCD_WR -> ouput
	GPIOPinWrite(GPIO_PORTJ_BASE,
			GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
			GPIO_PIN_0 | GPIO_PIN_1); //LCD_RD, LCD_WR = low / LCD_RS, LCD_CS = high

	return;
}

void delay_ms(unsigned long ms)
{
	unsigned long t;

	t = ms * (SysCtlClockGet() / 3000);
	SysCtlDelay(t); //3 cycles

	return;
}

void GPIOSetOutputOD(unsigned long port, unsigned char pins)
{
	GPIOPinTypeGPIOOutputOD(port, pins); //open-drain output
	GPIOPadConfigSet(port, pins, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_OD); //open-drain

	return;
}

void GPIOSetOutput(unsigned long port, unsigned char pins)
{
	GPIOPinTypeGPIOOutput(port, pins); //output
	GPIOPadConfigSet(port, pins, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD); //push-pull

	return;
}

void GPIOSetInput(unsigned long port, unsigned char pins)
{
	GPIOPinTypeGPIOInput(port, pins); //input
	GPIOPadConfigSet(port, pins, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); //pullup

	return;
}

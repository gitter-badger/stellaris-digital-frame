#include "includes.h"


int main(void)
{
	DeviceSetup();

	unsigned long ulPeriod;
	unsigned long ulDelay;

	ulPeriod = SysCtlClockGet() / 10;
	ulDelay = ((ulPeriod / 2) / 3) - 4;

	ulPeriod = (SysCtlClockGet() / 10) / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod - 1);

	TimerEnable(TIMER0_BASE, TIMER_A);

	while (1)
	{
//		// Turn on the LED
//		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x01);
//		// Delay for a bit
//		SysCtlDelay(ulDelay);
//		// Turn off the LED
//		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x00);
//		// Delay for a bit
//		SysCtlDelay(ulDelay);
	}

}

void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	// Read the current state of the GPIO pin and
	// write back the opposite state
	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 1);
	}
}

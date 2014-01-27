
#ifndef LCD_Backlight_H
#define LCD_Backlight_H



#define LCD_BL_PERIPH           SYSCTL_PERIPH_GPIOF		//d6
#define LCD_BL_BASE             GPIO_PORTF_BASE
#define LCD_BL_PIN              GPIO_PIN_1

#define LCD_BL_TIMER_PERIPH     SYSCTL_PERIPH_TIMER2
#define LCD_BL_TIMER            TIMER_B
#define LCD_BL_TIMER_BASE       TIMER2_BASE

//Inits and sets to 50%
void initLCDBacklight();

//returns int from 0 to 100
int getBacklightValue();

//Value from 0 to 100
void setBacklight(int value);

#endif

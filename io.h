#ifndef _IO_H_
#define _IO_H_


//----- DEFINES -----
//CPU Freq.
// 50.0 MHz - SYSCTL_SYSDIV_4
// 40.0 MHz - SYSCTL_SYSDIV_5
// 33.3 MHz - SYSCTL_SYSDIV_6
// 28.6 MHz - SYSCTL_SYSDIV_7
// 25.0 MHz - SYSCTL_SYSDIV_8
// 22.2 MHz - SYSCTL_SYSDIV_9
// 20.0 MHz - SYSCTL_SYSDIV_10
#define LM3S_SYSDIV                    SYSCTL_SYSDIV_6

//LM3S6950 Revision: LM3S_REV_A1, LM3S_REV_A2 or LM3S_REV_B0
#define LM3S_REV_A2

//Display: LCD_L2F50, LCD_LPH88, LCD_LS020, LCD_MIO283QT1, LCD_MIO283QT2
#define LCD_LPH88
//#define LCD_MIRROR                     //mirror display
//#define LCD_ROTATE                     //rotate display (90 degree)
#define LCD_PWMFREQ                    (60000) //60000 Hz LED PWM Freq
#define LCD_PWMMIN                     (5)     // 5 % (1...100%)
#define LCD_PWMSTANDBY                 (15)    //15 % (1...100%)

//Standby
#define STANDBY_TIME                   (3*60)  //standby after x seconds

//SSI Speed: LCD, SD, F-RAM, VS
#define SSI_SPEED                      (8000000UL) //8 MHz (max ssi speed)

//Check hardware config
#if defined LM3S_REV_A1                //LM3S Rev
# define LM3S_NAME "LM3S-A1"
#elif defined LM3S_REV_A2
# define LM3S_NAME "LM3S-A2"
#elif defined LM3S_REV_B0
# define LM3S_NAME "LM3S-B0"
#else
# warning "LM3S Rev not defined"
#endif
#if defined LCD_L2F50                  //LCD
# define LCD_NAME "S65-L2F50"
#elif defined LCD_LPH88
# define LCD_NAME "S65-LPH88"
#elif defined LCD_LS020
# define LCD_NAME "S65-LS020"
#elif defined LCD_MIO283QT1
# define LCD_NAME "MIO283QT1"
#elif defined LCD_MIO283QT2
# define LCD_NAME "MIO283QT2"
#else
# warning "LCD not defined"
#endif

//--- Pins ---

//LCD
#define LCD_RST_DISABLE()              GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0)
#define LCD_RST_ENABLE()               GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0)
#define LCD_CS_DISABLE()               GPIOPinWrite(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_PIN_0)
#define LCD_CS_ENABLE()                GPIOPinWrite(GPIO_PORTJ_BASE, GPIO_PIN_0, 0)
#define LCD_RS_DISABLE()               GPIOPinWrite(GPIO_PORTJ_BASE, GPIO_PIN_1, GPIO_PIN_1)
#define LCD_RS_ENABLE()                GPIOPinWrite(GPIO_PORTJ_BASE, GPIO_PIN_1, 0)
//SD
#define MMC_POWERON()                  GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_PIN_6); \
                                       GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD)
#define MMC_POWEROFF()                 GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, 0); \
                                       GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU)
#define MMC_SELECT()                   GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0)
#define MMC_DESELECT()                 GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3)
#define MMC_SCK_LOW()                  GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0)
#define MMC_SCK_HIGH()                 GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2)
#define MMC_SO_LOW()                   GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0)
#define MMC_SO_HIGH()                  GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_PIN_5)

//----- PROTOTYPES -----
void                                   ssi_wait(void);
void                                   ssi_write(unsigned char c);
unsigned char                          ssi_readwrite(unsigned char c);
void                                   ssi_speed(unsigned long speed);
void                                   ssi_off(void);
void                                   ssi_on(void);

void                                   pwm_led(unsigned int power);

void                                   cpu_speed(unsigned int low_speed);
void                                   cpu_reset(void);
void                                   init_bor(unsigned int on);
void                                   init_periph(void);
void                                   init_pins(void);

void                                   delay_ms(unsigned long ms);

void                                   GPIOSetOutputOD(unsigned long port, unsigned char pins);
void                                   GPIOSetOutput(unsigned long port, unsigned char pins);
void                                   GPIOSetInput(unsigned long port, unsigned char pins);


#endif //_IO_H_

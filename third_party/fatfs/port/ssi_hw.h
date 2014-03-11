//*****************************************************************************
//
// ssi_hw.h - Board connection information for the SDCard reader.
//
// Copyright (c) 2008-2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 10636 of the DK-LM3S9D96 Firmware Package.
//
//*****************************************************************************

#ifndef __SSI_HW_H__
#define __SSI_HW_H__

//*****************************************************************************
//
// Peripheral definitions for the dk-lm3s9b95 board
//
//*****************************************************************************

//*****************************************************************************
//
// SDCard SSI port
//
//*****************************************************************************
#define SDC_SSI_BASE            SSI0_BASE
#define SDC_SSI_SYSCTL_PERIPH   SYSCTL_PERIPH_SSI0

//*****************************************************************************
//
// GPIO for SDCard SSI pins
//
//*****************************************************************************
#define SDC_GPIO_PORT_BASE      GPIO_PORTA_BASE
#define SDC_GPIO_SYSCTL_PERIPH  SYSCTL_PERIPH_GPIOA
#define SDC_SSI_CLK             GPIO_PIN_2
#define SDC_SSI_TX              GPIO_PIN_5
#define SDC_SSI_RX              GPIO_PIN_4

#define SDC_SSI_PINS            (SDC_SSI_TX | SDC_SSI_RX | SDC_SSI_CLK)

//*****************************************************************************
//
// GPIO for the SDCard chip select
//
//*****************************************************************************
#define SDCARD_CS_PERIPH   SYSCTL_PERIPH_GPIOA
#define SDCARD_CS_BASE     GPIO_PORTA_BASE
#define SDCARD_CS_PIN      GPIO_PIN_3
//*****************************************************************************
//
// GPIO for the SDCard power
//
//*****************************************************************************
#define SDCARD_VCC_PERIPH   SYSCTL_PERIPH_GPIOA
#define SDCARD_VCC_BASE     GPIO_PORTA_BASE
#define SDCARD_VCC_PIN      GPIO_PIN_6

#define SDC_SSI_MAX_SPEED 	12500000

#endif // __SSI_HW_H__

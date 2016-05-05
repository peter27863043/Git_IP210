/******************************************************************************
*
*   Name:           gpio.h
*
*   Description:    
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#include <module.h>
#ifndef GPIO_H
#define GPIO_H

#include "type_def.h"
#include "ip210reg.h"
#include <stdio.h>

sfr  P4    = 0xE8;

sbit G0  = P3^2;
sbit G1  = P3^3;
sbit G2  = P3^4;
sbit G3  = P3^5;
sbit G4  = P1^2;

sbit P0_0  = P0^0;
sbit P0_1  = P0^1;
sbit P0_2  = P0^2;
sbit P0_3  = P0^3;
sbit P0_4  = P0^4;
sbit P0_5  = P0^5;
sbit P0_6  = P0^6;
sbit P0_7  = P0^7;

sbit P1_0  = P1^0;
sbit P1_1  = P1^1;
sbit P1_2  = P1^2;
sbit P1_3  = P1^3;
sbit P1_4  = P1^4;
sbit P1_5  = P1^5;
sbit P1_6  = P1^6;
sbit P1_7  = P1^7;

sbit P2_0  = P2^0;
sbit P2_1  = P2^1;
sbit P2_2  = P2^2;
sbit P2_3  = P2^3;
sbit P2_4  = P2^4;
sbit P2_5  = P2^5;
sbit P2_6  = P2^6;
sbit P2_7  = P2^7;

sbit P3_0  = P3^0;
sbit P3_1  = P3^1;
sbit P3_4  = P3^4;
sbit P3_5  = P3^5;
sbit P3_6  = P3^6;
sbit P3_7  = P3^7;
#ifdef MODULE_FLASH_485
sbit P3_2  = P3^2;
sbit P3_3  = P3^3;
#endif
sbit P4_1  = P4^1;
sbit P4_2  = P4^2;
sbit P4_3  = P4^3;
sbit P4_4  = P4^4;
sbit P4_6  = P4^6;
sbit P4_7  = P4^7;
#ifdef MODULE_FLASH_485
sbit P4_0  = P4^0;
sbit P4_5  = P4^5;
#else
sbit P4_0  = P3^2;
sbit P4_5  = P3^3;
#endif

#ifdef IC_PACKAGE_128_PIN
#define LOAD_DEFAULT_PIN P4_5	//press = low
#else
#define LOAD_DEFAULT_PIN P3_0
#endif

#ifdef MODULE_GPIO
void ReadGPIO(u8_t PortMask,u8_t BitMask,u8_t* ReadDate);
void WriteGPIO(u8_t PortMask,u8_t BitMask,u8_t WriteOP,u8_t* WriteDate);
u16_t ReadAllGPIO();
#endif
#endif //MODULE_GPIO

/******************************************************************************
*
*   Name:           gpio.c
*
*   Description:    GPIO module
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#include <module.h>
#ifdef MODULE_GPIO
#define __OTP64

#include "gpio.h"
#include "type_def.h"
#include "ip210reg.h"
#include <stdio.h>

#define TCP_CMD_MASK_P1 1
#define TCP_CMD_MASK_P3 3
#define BitMask0 0x01
#define BitMask1 0x02
#define BitMask2 0x04
#define BitMask3 0x08
#define BitMask4 0x10
#define BitMask5 0x20
#define BitMask6 0x40
#define BitMask7 0x80
u8_t bdata tmp;
sbit tmp_0=tmp^0;
sbit tmp_1=tmp^1;
sbit tmp_2=tmp^2;
sbit tmp_3=tmp^3;
sbit tmp_4=tmp^4;
sbit tmp_5=tmp^5;
sbit tmp_6=tmp^6;
sbit tmp_7=tmp^7;
u8_t bdata tmp2;
sbit tmp2_0=tmp2^0;
sbit tmp2_1=tmp2^1;
sbit tmp2_2=tmp2^2;
sbit tmp2_3=tmp2^3;
sbit tmp2_4=tmp2^4;
sbit tmp2_5=tmp2^5;
sbit tmp2_6=tmp2^6;
sbit tmp2_7=tmp2^7;

/******************************************************************************
*
*  Function:    ReadGPIO
*
*  Description: Read GPIO data
*               
*  Parameters:  PortMask: mask of port number =0~7
*               BitMask:  mask of bit, ex: the value 0x0f will read bit[0~3] back
*               ReadData: buffer for return data
*  Returns:     
*               
*******************************************************************************/
#ifdef __OTP64
void ReadGPIO(u8_t PortMask,u8_t BitMask,u8_t* ReadData)
{	
	tmp=BitMask;
	tmp2=*ReadData;
	switch(PortMask)
	{
		case 1:
			tmp&=0x04;
			if(tmp_2){if(tmp2_2)P1_2=1;tmp_2=P1_2;}
			break;
		case 3:
			tmp&=0x3C;
			if(tmp_2){if(tmp2_2)P3_2=1;tmp_2=P3_2;}
			if(tmp_3){if(tmp2_3)P3_3=1;tmp_3=P3_3;}
			if(tmp_4){if(tmp2_4)P3_4=1;tmp_4=P3_4;}
			if(tmp_5){if(tmp2_5)P3_5=1;tmp_5=P3_5;}
			break;
	}
	*ReadData=tmp;
}
#else
void ReadGPIO(u8_t PortMask,u8_t BitMask,u8_t* ReadData)
{	
	tmp=BitMask;

	switch(PortMask)
	{
		case 0:
			if(tmp_0){if(tmp2_0)P0_0=1;tmp_0=P0_0;}
			if(tmp_1){if(tmp2_1)P0_1=1;tmp_1=P0_1;}
			if(tmp_2){if(tmp2_2)P0_2=1;tmp_2=P0_2;}
			if(tmp_3){if(tmp2_3)P0_3=1;tmp_3=P0_3;}
			if(tmp_4){if(tmp2_4)P0_4=1;tmp_4=P0_4;}
			if(tmp_5){if(tmp2_5)P0_5=1;tmp_5=P0_5;}
			if(tmp_6){if(tmp2_6)P0_6=1;tmp_6=P0_6;}
			if(tmp_7){if(tmp2_7)P0_7=1;tmp_7=P0_7;}
			break;
		case 1:
			if(tmp_0){if(tmp2_0)P1_0=1;tmp_0=P1_0;}
			if(tmp_1){if(tmp2_1)P1_1=1;tmp_1=P1_1;}
			if(tmp_2){if(tmp2_2)P1_2=1;tmp_2=P1_2;}
			if(tmp_3){if(tmp2_3)P1_3=1;tmp_3=P1_3;}
			if(tmp_4){if(tmp2_4)P1_4=1;tmp_4=P1_4;}
			if(tmp_5){if(tmp2_5)P1_5=1;tmp_5=P1_5;}
			if(tmp_6){if(tmp2_6)P1_6=1;tmp_6=P1_6;}
			if(tmp_7){if(tmp2_7)P1_7=1;tmp_7=P1_7;}
			break;
		case 2:
			if(tmp_0){if(tmp2_0)P2_0=1;tmp_0=P2_0;}
			if(tmp_1){if(tmp2_1)P2_1=1;tmp_1=P2_1;}
			if(tmp_2){if(tmp2_2)P2_2=1;tmp_2=P2_2;}
			if(tmp_3){if(tmp2_3)P2_3=1;tmp_3=P2_3;}
			if(tmp_4){if(tmp2_4)P2_4=1;tmp_4=P2_4;}
			if(tmp_5){if(tmp2_5)P2_5=1;tmp_5=P2_5;}
			if(tmp_6){if(tmp2_6)P2_6=1;tmp_6=P2_6;}
			if(tmp_7){if(tmp2_7)P2_7=1;tmp_7=P2_7;}
			break;
		case 3:
			if(tmp_0){if(tmp2_0)P3_0=1;tmp_0=P3_0;}
			if(tmp_1){if(tmp2_1)P3_1=1;tmp_1=P3_1;}
			if(tmp_2){if(tmp2_2)P3_2=1;tmp_2=P3_2;}
			if(tmp_3){if(tmp2_3)P3_3=1;tmp_3=P3_3;}
			if(tmp_4){if(tmp2_4)P3_4=1;tmp_4=P3_4;}
			if(tmp_5){if(tmp2_5)P3_5=1;tmp_5=P3_5;}
			if(tmp_6){if(tmp2_6)P3_6=1;tmp_6=P3_6;}
			if(tmp_7){if(tmp2_7)P3_7=1;tmp_7=P3_7;}
			break;
		case 4:
			if(tmp_0){if(tmp2_0)P4_0=1;tmp_0=P4_0;}
			if(tmp_1){if(tmp2_1)P4_1=1;tmp_1=P4_1;}
			if(tmp_2){if(tmp2_2)P4_2=1;tmp_2=P4_2;}
			if(tmp_3){if(tmp2_3)P4_3=1;tmp_3=P4_3;}
			if(tmp_4){if(tmp2_4)P4_4=1;tmp_4=P4_4;}
			if(tmp_5){if(tmp2_5)P4_5=1;tmp_5=P4_5;}
			if(tmp_6){if(tmp2_6)P4_6=1;tmp_6=P4_6;}
			if(tmp_7){if(tmp2_7)P4_7=1;tmp_7=P4_7;}
			break;
	}
	*ReadData=tmp;
}
#endif


/******************************************************************************
*
*  Function:    WriteGPIO
*
*  Description: 
*               
*  Parameters:  PortMask: mask of port number =0~7
*               BitMask:  mask of bit, ex: the value 0x0f will write bit[0~3]
*               WriteData: data for write to GPIO
*  Returns:     
*               
*******************************************************************************/
#ifdef __OTP64
void WriteGPIO(u8_t PortMask,u8_t BitMask,u8_t WriteOP,u8_t* WriteData)
{
#define WOP_ASSIGN 0
#define WOP_AND    1
#define WOP_OR     2
#define WOP_XOR    3
#define WOP_NOT    4
#define WOP_NAND   5

	ReadGPIO(PortMask,BitMask,&tmp);
	switch(WriteOP)
	{
		case WOP_ASSIGN:
			tmp=*WriteData;
			break;
		case WOP_AND:
			tmp=tmp&*WriteData;
			break;
		case WOP_OR:
			tmp=tmp|*WriteData;
			break;
		case WOP_XOR:
			tmp=tmp^*WriteData;
			break;
		case WOP_NOT:
			tmp=~tmp;
			break;
		case WOP_NAND:
			tmp=(~tmp)&*WriteData;
			break;
	}

	switch(PortMask)
	{
		case 1:
			if(BitMask&BitMask2)P1_2=tmp_2;
			break;
		case 3:
			if(BitMask&BitMask2)P3_2=tmp_2;
			if(BitMask&BitMask3)P3_3=tmp_3;
			if(BitMask&BitMask4)P3_4=tmp_4;
			if(BitMask&BitMask5)P3_5=tmp_5;
			break;
	}
}
#else
void WriteGPIO(u8_t PortMask,u8_t BitMask,u8_t WriteOP,u8_t* WriteData)
{
#define WOP_ASSIGN 0
#define WOP_AND    1
#define WOP_OR     2
#define WOP_XOR    3
#define WOP_NOT    4
#define WOP_NAND   5

	ReadGPIO(PortMask,BitMask,&tmp);
	switch(WriteOP)
	{
		case WOP_ASSIGN:
			tmp=*WriteData;
			break;
		case WOP_AND:
			tmp=tmp&*WriteData;
			break;
		case WOP_OR:
			tmp=tmp|*WriteData;
			break;
		case WOP_XOR:
			tmp=tmp^*WriteData;
			break;
		case WOP_NOT:
			tmp=~tmp;
			break;
		case WOP_NAND:
			tmp=(~tmp)&*WriteData;
			break;
	}

	switch(PortMask)
	{
		case 0:
			if(BitMask&BitMask0)P0_0=tmp_0;
			if(BitMask&BitMask1)P0_1=tmp_1;
			if(BitMask&BitMask2)P0_2=tmp_2;
			if(BitMask&BitMask3)P0_3=tmp_3;
			if(BitMask&BitMask4)P0_4=tmp_4;
			if(BitMask&BitMask5)P0_5=tmp_5;
			if(BitMask&BitMask6)P0_6=tmp_6;
			if(BitMask&BitMask7)P0_7=tmp_7;
			break;
		case 1:
			if(BitMask&BitMask0)P1_0=tmp_0;
			if(BitMask&BitMask1)P1_1=tmp_1;
			if(BitMask&BitMask2)P1_2=tmp_2;
			if(BitMask&BitMask3)P1_3=tmp_3;
			if(BitMask&BitMask4)P1_4=tmp_4;
			if(BitMask&BitMask5)P1_5=tmp_5;
			if(BitMask&BitMask6)P1_6=tmp_6;
			if(BitMask&BitMask7)P1_7=tmp_7;
			break;
		case 2:
			if(BitMask&BitMask0)P2_0=tmp_0;
			if(BitMask&BitMask1)P2_1=tmp_1;
			if(BitMask&BitMask2)P2_2=tmp_2;
			if(BitMask&BitMask3)P2_3=tmp_3;
			if(BitMask&BitMask4)P2_4=tmp_4;
			if(BitMask&BitMask5)P2_5=tmp_5;
			if(BitMask&BitMask6)P2_6=tmp_6;
			if(BitMask&BitMask7)P2_7=tmp_7;
			break;
		case 3:
			if(BitMask&BitMask0)P3_0=tmp_0;
			if(BitMask&BitMask1)P3_1=tmp_1;
			if(BitMask&BitMask2)P3_2=tmp_2;
			if(BitMask&BitMask3)P3_3=tmp_3;
			if(BitMask&BitMask4)P3_4=tmp_4;
			if(BitMask&BitMask5)P3_5=tmp_5;
			if(BitMask&BitMask6)P3_6=tmp_6;
			if(BitMask&BitMask7)P3_7=tmp_7;
			break;
		case 4:
			if(BitMask&BitMask0)P4_0=tmp_0;
			if(BitMask&BitMask1)P4_1=tmp_1;
			if(BitMask&BitMask2)P4_2=tmp_2;
			if(BitMask&BitMask3)P4_3=tmp_3;
			if(BitMask&BitMask4)P4_4=tmp_4;
			if(BitMask&BitMask5)P4_5=tmp_5;
			if(BitMask&BitMask6)P4_6=tmp_6;
			if(BitMask&BitMask7)P4_7=tmp_7;
			break;
	}
}
#endif


/******************************************************************************
*
*  Function:    ReadAllGPIO
*
*  Description: Read G0~G4 out
*               
*  Parameters:  
*               
*  Returns:     value of G[0:4]
*               
*******************************************************************************/
u16_t ReadAllGPIO()
{
	return (u8_t)G4<<4|(u8_t)G3<<3|(u8_t)G2<<2|(u8_t)G1<<1|(u8_t)G0;
}

#endif //MODULE_GPIO

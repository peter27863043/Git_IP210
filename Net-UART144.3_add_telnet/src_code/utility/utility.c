/****************************************************************************
*
*	Name:			utility.C
*
*	Description:	utility
*
*	Copyright:		(c) 2005-2050    IC Plus Crop.
*	                All rights reserved.	by Chance
*
****************************************************************************/
//*******************Include files area**************************
#include "type_def.h"
#include "public.h"
#include "utility.h"
#include "eeprom.h"
#include <string.h>
#include <intrins.h>
#include "wdg.h"
// 02/10/2006 Grace
#define  OTPCODE_ON		1		//Turn off define if SWITCH code
//-------------------------------------------------------------
/******************************************************************************
*
*  Function:    IP210_SWReset
*
*  Description: software reset
*
*  Parameters:  None
*
*  Returns:     None
*
*******************************************************************************/
void IP210_SWReset(void)
{
  	u8_t i;

    IP210RegWrite(REG_SW_RESET, 0x1);
    for(i=0;i<10;i++)
        _nop_();
}
/******************************************************************************
*
*  Function:    CPUInit
*
*  Description: Disable Watch dog timer
*
*  Parameters:  None
*
*  Returns:     None
*
*******************************************************************************/
void CPUInit(void)
{
    WDTEN=0;
	WatchDogTimerInit();
	WatchDogTimerEnable();
}

/******************************************************************************
*
*  Function:    Delay
*
*  Description: Delay
*
*  Parameters:  delay : delay times
*
*  Returns:     None
*
*******************************************************************************/
void Delay(u16_t data delay)
{
    while(delay-->0)
        _nop_();
}

// 02/10/2006 Grace
#ifndef OTPCODE_ON
/******************************************************************************
*
*  Function:    Write_EEP
*
*  Description: Write eeprom data
*
*  Parameters:  phyad : PHY register address, reg1: address offset, Regdata: data
*
*  Returns:     None
*
*******************************************************************************/
void Write_EEP(u8_t phyad,u8_t reg1,u8_t Regdata)
{
  if( (phyad==30)||(phyad==31) )
   {
	EEPROM_Write_Byte( 64*(phyad-30)+2*reg1  , (u8_t)(Regdata & 0x00ff) );
	EEPROM_Write_Byte( 64*(phyad-30)+2*reg1+1, (u8_t)(Regdata >>8)    );
   }
}
#endif
//--------------------------------------------------------
/******************************************************************************
*
*  Function:    asc_to_hex
*
*  Description: ASCII to HEX
*
*  Parameters:  asc: pointer of character
*
*  Returns:
*				=0     : string value=0 or error
*				others : string value
*
*******************************************************************************/
u16_t asc_to_hex(char asc[])
{
	u16_t	 value=0x0;
	u8_t	 i, len;

	len = strlen(asc);
	if (len > 4) return 0;
	for(i=0;i < len;i++)
		{
			value <<= 4;
			if(  (asc[i]>='0')&&(asc[i]<='9')  )
			  {value|=(asc[i]-'0');}
			 else if( (asc[i]>='a')&&(asc[i]<='f'))
			  {value|=(asc[i]-'a'+10);}
   			 else if( (asc[i]>='A')&&(asc[i]<='F'))
			  {value|=(asc[i]-'A'+10);}
			 else // ERROR HEX Digit
			  {return 0;}
		}
	return value;
}
/******************************************************************************
*
*  Function:    hex_to_asc
*
*  Description: Hex to ASSCII
*
*  Parameters:  hex: input data byte
*				buf: 2 byte buffer
*
*  Returns:     None
*
*******************************************************************************/
void hex_to_asc(u8_t hex, s8_t * buf)
{
	u8_t i;

	i = hex >> 4; // get high nible
	*buf = (i < 10) ? (0x30 + i) : (0x40 + i - 9);
	i = hex & 0xF; // get low nible
	*(buf+1) = (i < 10) ? (0x30 + i) : (0x40 + i - 9);
    /*-----------------------------------------------------------
	for(i=0;i < len; i++)
	{
		if( ( (hex>>((len-1-i)*4)  )&0x0f ) <0xa  )
		  {value[i]=0x30+((hex>>((len-1-i)*4))&0x0f );  //TXD=1;printf("D");
		  }
		 else
		  {value[i]=0x61+((hex>>((len-1-i)*4))&0x0f )-0xa;// TXD=1;printf("H");
		  }
	}
	value[len]=0x0;
	return value;
	----------------------------------------------------------------*/
}


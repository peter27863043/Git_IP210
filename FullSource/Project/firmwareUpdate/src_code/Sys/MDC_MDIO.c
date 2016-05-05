/****************************************************************************
*
*	Name:			IP210 MDC_MDIO.C
*
*	Description:	MAC Driver
*
*	Copyright:		(c) 2005-2050    IC Plus Crop.  
*	                All rights reserved.	by DeNa
*
*	Version:		v1.11
****************************************************************************/

#include "type_def.h"
#include "ip210reg.h"


u8_t	MDC_SPEED	=	MDC_SPEED_640;//normal setting
//u8_t	MDC_SPEED	=	MDC_SPEED_320;//1826 setting, for faster speed
u8_t	MDC_PREM_OFF=	0x0;

/******************************************************************************
*
*  Function:    Read_MDC
*
*  Description: MDC/MDIO Read function,IP175 , IP178 MODE (5+5 bits)
*               
*  Parameters:  phyad: PHY address
*				regad: Reg address
*               
*  Returns:     16bits Register Value
*               
*******************************************************************************/
u16_t Read_MDC(u8_t phyad,u8_t regad)
{	u8_t	reg;
	u16_t	ret_reg,i=0;
	IP210RegWrite(REG_MDC_PHYADDR, phyad );
	IP210RegWrite(REG_MDC_REGADDR, regad );
//	IP210RegWrite(REG_MDC_CONTROL, MDC_SPEED_640|MDC_READ|MDC_START);
	IP210RegWrite(REG_MDC_CONTROL, MDC_SPEED | MDC_READ |MDC_PREM_OFF| MDC_START);
	do{
	  reg=IP210RegRead(REG_MDC_CONTROL);
	  i++;
	}while( (reg& MDC_START) &&(i<30000) );
	ret_reg=IP210RegRead(REG_MDC_DATA_HIGH);
	ret_reg<<=8;
	ret_reg|=IP210RegRead(REG_MDC_DATA_LOW);
	return ret_reg;
}


/******************************************************************************
*
*  Function:    Write_MDC
*
*  Description: MDC/MDIO Write function,   IP175/178 MODE (5+5 bits)
*               
*  Parameters:  phyad	: PHY address
*				regad	: Reg address
*				Regdata	: data to write
*               
*  Returns:     No return value
*               
*******************************************************************************/
void Write_MDC(u8_t phyad,u8_t regad,u16_t Regdata)
{
	u8_t	reg;
	u16_t	i=0;
	IP210RegWrite(REG_MDC_DATA_HIGH	, (u8_t)(Regdata>>8));
	IP210RegWrite(REG_MDC_DATA_LOW	, (u8_t)(Regdata&0xff));
	IP210RegWrite(REG_MDC_PHYADDR		, phyad );
	IP210RegWrite(REG_MDC_REGADDR		, regad );
//	IP210RegWrite(REG_MDC_CONTROL, MDC_SPEED_640 | MDC_WRITE | MDC_START );
	IP210RegWrite(REG_MDC_CONTROL, MDC_SPEED | MDC_WRITE |MDC_PREM_OFF| MDC_START );
	do{
	  reg=IP210RegRead(REG_MDC_CONTROL);
	  i++;
	}while( (reg & MDC_START) &&(i<30000) );
}





/******************************************************************************
*
*  Function:    Read_MDC13
*
*  Description: MDC/MDIO Read function,IP113s/1826 MODE (9+1 bits)
*               
*  Parameters:  
*    regad: Reg address
*               
*  Returns:     16bits Register Value
*               
*******************************************************************************/
u16_t Read_MDC13(u16_t regad)
{ regad&=0x03ff;
 return Read_MDC(   ( regad>>5 ) ,(regad & 0x1f) );
}
 
 
 
/******************************************************************************
*
*  Function:    Write_MDC13
*
*  Description: MDC/MDIO Write function,   IP113s/1826 MODE (9+2 bits)
*               
*  Parameters:  
*    regad : Reg address
*    Regdata : data to write
*               
*  Returns:     No return value
*               
*******************************************************************************/
void Write_MDC13(u16_t regad,u16_t Regdata)
{ regad&=0x03ff;
 Write_MDC(  ( regad>>5 ) , (regad & 0x1f),Regdata );
}

 
/******************************************************************************
*
*  Function:    Read_MDC26
*
*  Description: MDC/MDIO Read function,IP1726/1826 MODE (8+2 bits)
*               
*  Parameters:  phyad: PHY address
*				regad: Reg address
*               
*  Returns:     16bits Register Value
*               
*******************************************************************************/
//u16_t Read_MDC26(u8_t phyad,u8_t regad)
//{	
//	return Read_MDC(   ( ((phyad&0x3)<<3)|(regad>>5) )	,(regad & 0x1f) );
//}



/******************************************************************************
*
*  Function:    Write_MDC26
*
*  Description: MDC/MDIO Write function,   IP1726/1726 MODE (8+2 bits)
*               
*  Parameters:  phyad	: PHY address
*				regad	: Reg address
*				Regdata	: data to write
*               
*  Returns:     No return value
*               
*******************************************************************************/
//void Write_MDC26(u8_t phyad,u8_t regad,u16_t Regdata)
//{
//	Write_MDC(  ( ((phyad&0x3)<<3)|(regad>>5) ) , (regad & 0x1f),Regdata );
//}
void Write_MDC26(u16_t regad,u16_t Regdata)
{
	Write_MDC13(regad,Regdata);
}




////add for IP1826 Phy accessing function
u16_t Read_Phy(u8_t phy_addr, u8_t reg_addr)
{	u8_t i;
//	(u16_t)phy_addr | (((u16_t)reg_addr)<<5)
	Write_MDC26( 0xda ,( (u16_t)phy_addr | (((u16_t)reg_addr)<<5) |0x8000 ) );
	for(i=0;i<250;i++)
	  { if(Read_MDC13(0xda)&0x2000){break;} }
	return Read_MDC13(0xdb);
}

void Write_Phy(u8_t phy_addr, u8_t reg_addr, u16_t reg_data)
{	u8_t i;
//	(u16_t)phy_addr | (((u16_t)reg_addr)<<5)
	Write_MDC26(0xdb,reg_data);
	Write_MDC26( 0xda ,( (u16_t)phy_addr | (((u16_t)reg_addr)<<5) |0xc000 ) );
	for(i=0;i<250;i++)
	  { if(Read_MDC13(0xda)&0x2000){break;} }
}


/****************************************************************************
*
*	Name:			IP210 MDC_MDIO.h
*
*	Description:	MAC Driver
*
*	Copyright:		(c) 2005-2050    IC Plus Crop.  
*	                All rights reserved.	by DeNa
*
*	Version:		v1.10
****************************************************************************/

//IP1726 or IP1826 MODE (8 bits)
u16_t Read_MDC26(u8_t phyad,u8_t regad);
void  Write_MDC26(u8_t phyad,u8_t regad,u16_t Regdata);

//IP175 , IP178 MODE (5+5 bits)
u16_t Read_MDC(u8_t phyad,u8_t regad);
void  Write_MDC(u8_t phyad,u8_t regad,u16_t Regdata);

#define Read_Reg(phyad,regad)	Read_MDC(phyad,regad)
#define Write_Reg(phyad,regad,Regdata) Write_MDC(phyad,regad,Regdata)
#define Read_Reg26(phyad,regad)	Read_MDC26(phyad,regad)
#define Write_Reg26(phyad,regad,Regdata) Write_MDC26(phyad,regad,Regdata)



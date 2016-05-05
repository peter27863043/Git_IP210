/****************************************************************************
*
*	Name:			IP210 MDC_MDIO.h
*
*	Description:	MAC Driver
*
*	Copyright:		(c) 2005-2050    IC Plus Crop.  
*	                All rights reserved.	by DeNa
*
*	Version:		v1.11
****************************************************************************/

//IP175 , IP178 MODE (5+5 bits)
u16_t Read_MDC(u8_t phyad,u8_t regad);
void  Write_MDC(u8_t phyad,u8_t regad,u16_t Regdata);

//IP113s or IP1826 or IP1726
u16_t Read_MDC13(u16_t regad);
void Write_MDC13(u16_t regad,u16_t Regdata);


//#define Read_Reg(regad)				Read_MDC13(regad)
//#define Write_Reg(regad,Regdata) 	Write_MDC13(regad,Regdata)

//IP1726 or IP1826 MODE (8 bits)
void Write_MDC26(u16_t regad,u16_t Regdata);

//#define Read_Reg26(regad)	Read_MDC13(regad)
//#define Write_Reg26(regad,Regdata) Write_MDC26(regad,Regdata)

u16_t Read_Phy(u8_t phy_addr, u8_t reg_addr);
void Write_Phy(u8_t phy_addr, u8_t reg_addr, u16_t reg_data);


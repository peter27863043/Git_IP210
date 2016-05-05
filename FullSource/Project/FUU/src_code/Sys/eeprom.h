/******************************************************************************
*
*   Name:           eeprom.h
*
*   Description:    EEPROM Header
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/

#ifndef _EEPROM_H
#define _EEPROM_H
#include "eepdef.h"
//**************Function Prototype********************
unsigned char EEPROM_Read_Byte(unsigned short addr, unsigned char *read_data);
unsigned char EEPROM_Write_Byte(unsigned short addr, unsigned char write_data);
unsigned char EEPROM_Read_Multi(unsigned short addr, unsigned char *read_data, unsigned char length);
unsigned char EEPROM_Write_Multi(unsigned short addr, unsigned char *write_data, unsigned char length);
void EEPROM_Enable(void);
void EEPROM_Disable(void);



#endif
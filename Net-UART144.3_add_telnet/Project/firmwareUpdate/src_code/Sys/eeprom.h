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

//**************Function Prototype********************
u8_t EEPROM_Read_Byte(u16_t addr, u8_t *read_data);
u8_t EEPROM_Write_Byte(u16_t addr, u8_t write_data);
//u8_t EEPROM_Read_Multi(u16_t addr, u8_t *read_data, u8_t length);
//u8_t EEPROM_Write_Multi(u16_t addr, u8_t *write_data, u8_t length);
void EEPROM_Enable(void);
void EEPROM_Disable(void);

#define eep_read(addr,read_data)  EEPROM_Read_Byte(addr,read_data);
#define eep_write(addr,write_data) EEPROM_Write_Byte(addr,write_data);


#endif
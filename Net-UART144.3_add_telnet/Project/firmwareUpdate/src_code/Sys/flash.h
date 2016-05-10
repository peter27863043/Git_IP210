/******************************************************************************
*
*   Name:           flash.h
*
*   Description:    Flash access function Header
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/

#ifndef _FLASH_H
#define _FLASH_H

//Prototype fucntions
void Start_Mirror_Mode(u16_t mirror_addr);
void Stop_Mirror_Mode(void);
void Flash_Access_Enable(void);
void Flash_Access_Disable(void);
u8_t Flash_Erase_Bank(u8_t index);extern u8_t Flash_Read_Byte(u16_t flash_addr);
u8_t Flash_Read_Byte(u16_t flash_addr);
u8_t Flash_Write_Byte(u16_t flash_addr, u8_t byte_data);
void Flash_Switch_Bank(u8_t index);
void FirmwareUpdatMirrorMode(u16_t src_addr, u16_t des_addr, u16_t length);
void FirmwareUpdateEraseFlash(void);
u8_t FirmwareUpdateWriteData(u8_t *ptr_data, u16_t length);
u8_t FirmwareUpdateFlashCheck(void);
void FlashIdentification(void);
#endif
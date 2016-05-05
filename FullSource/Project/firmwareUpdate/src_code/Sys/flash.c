/******************************************************************************
*
*   Name:           flash.c
*
*   Description:    Driver functions for flash access operations
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/
#include "public.h"
#include "flash.h"
#include "dma.h"
#include "timer.h"
#include "ip210reg.h"

#define SST_SECTOR_SIZE                             0x1000
#define SYNCMOS_SECTOR_SIZE                         0x0400
#define SECTOR_SIZE                                 SYNCMOS_SECTOR_SIZE
#define SECTOR_NUMBER_OF_4K_SIZE                    0x1000/SECTOR_SIZE
#define MS_DELAY_COUNT 								4000
//variables
u32_t Global_flash_count=0;
u16_t Global_flash_addr=0;
u8_t Global_cal_chechsum=0, Global_bank_index=0;
u8_t FlashBankSum(u16_t sddr_limit);
u8_t FlashBlankCheck(u16_t sddr_limit);
u16_t Global_flash_ID;
u16_t Phase_Flash_Erase=0;
u32_t last_timercounter=0;
u8_t Global_erase_flash_wait_time=5;
u16_t  next_erase_bank=0;//0-31
u16_t  next_erase_sector=0;//0-15
u16_t Flash_Bankno;
u16_t Flash_Erase_Bankno;//chance20100423 for erase bank number

void NOP(void){}

void Delay_10ms(unsigned short count)
{
  	unsigned short i, j;

	for(j=0;j<count;j++)
	{
		for(i=0;i<MS_DELAY_COUNT;i++)
	    	NOP();
	}
}
/******************************************************************************
*
*  Function:    Start_Mirror_Mode
*
*  Description: switch CPU into Mirror mode(codes are fetched from Xdata)
*               
*  Parameters: 
*    u16_t mirror_addr: the address to be mirrorred to 0x0000 of xdata
*               
*  Returns: none
*               
*******************************************************************************/
void Start_Mirror_Mode(u16_t mirror_addr)
{
  	u8_t reg_byte;

    TRACE_FLASH(("\n-->Start_Mirror_Mode~~"));  

    IP210RegWrite(REG_MIRROR_ADDRESS, mirror_addr>>8);
    reg_byte = IP210RegRead(REG_MIRROR_ADDRESS);
    TRACE_FLASH(("\nREG_MIRROR_ADDRESS = %x", (u16_t)reg_byte));
            
    reg_byte = IP210RegRead(REG_CHIP_CONFIGURE_0);  
    reg_byte |= BIT_MIRROR_EN;
    IP210RegWrite(REG_CHIP_CONFIGURE_0, reg_byte);    
}
/******************************************************************************
*
*  Function:    Stop_Mirror_Mode
*
*  Description: switch CPU bakc from Mirror mode(codes are as normal fetched from Xdata)
*               
*  Parameters:  none
*               
*  Returns: none
*               
*******************************************************************************/
void Stop_Mirror_Mode(void)
{
  	u8_t reg_byte;	

    TRACE_FLASH(("\n-->Stop_Mirror_Mode~~"));
	Flash_Switch_Bank(0);
	reg_byte = IP210RegRead(REG_CHIP_CONFIGURE_0);  
    reg_byte &= ~BIT_MIRROR_EN;
    IP210RegWrite(REG_CHIP_CONFIGURE_0, reg_byte);
}
/******************************************************************************
*
*  Function:    Flash_Access_Enable
*
*  Description: allow Flash to be accessed (xdata bus is redirected to flash bus)
*               
*  Parameters:  none
*               
*  Returns: none
*               
*******************************************************************************/
void Flash_Access_Enable(void)
{
    CKCON |= 0x04;
    FLASH_ACCESS_EN = 1;
}

/******************************************************************************
*
*  Function:    Flash_Access_Disable
*
*  Description: flash is not longer accessible(xdata bus returns normal)
*               
*  Parameters:  none
*               
*  Returns: none
*               
*******************************************************************************/
void Flash_Access_Disable(void)
{
    FLASH_ACCESS_EN = 0;
    CKCON &= ~0x04;    
}


/******************************************************************************
*
*  Function:    Flash_Read_Byte
*
*  Description: to read a byte of data from flash
*               
*  Parameters:
*    u16_t flash_addr: the address of data in Flash
*               
*  Returns: data 
*  
*******************************************************************************/
u8_t Flash_Read_Byte(u16_t flash_addr)
{
  	u8_t byte_data;
  	u16_t data flash_read_addr;

    flash_read_addr = flash_addr;
    Flash_Access_Enable(); 
    byte_data = XBYTE[flash_read_addr];
    Flash_Access_Disable();
    return byte_data;
}
/******************************************************************************
*
*  Function:    FirmwareUpdatMirrorMode
*
*  Description: to copy & run FirmwareUpdate program under mirror mode
*               
*  Parameters:
*    u16_t src_addr: the source address of codes to be mapped
*    u16_t des_addr: the destination address of codes to be mapped
*    u16_t length: the length of data to be mapped
*               
*  Returns: none
*  
*******************************************************************************/
void FirmwareUpdatMirrorMode(u16_t src_addr, u16_t des_addr, u16_t length)
{
	IP210_DMA(src_addr, des_addr, length, BIT_CMD_MODE_F2M);
	Start_Mirror_Mode(src_addr);
}
/******************************************************************************
*
*  Function:    FirmwareUpdateFlashBankCheck
*
*  Description: to calculate the checksum of each bank of Flash data
*               
*  Parameters:
*    u8_t bank_index: current index number of flash bank
*            
*  Returns: none
*  
*******************************************************************************/
void FirmwareUpdateFlashBankCheck(u8_t bank_index)
{
  	u32_t ii, addr_limit=0xFFFF;
  	u16_t byte_data;

    if(bank_index==0) addr_limit=0x5FFF;
    if(bank_index==Flash_Bankno-1) addr_limit=0xFFFA;
    
    Flash_Switch_Bank(bank_index);
    byte_data = FlashBankSum(addr_limit);
    printf("\nFlashBank-%x(%x)", (u16_t)bank_index, (u16_t) byte_data);
    Global_cal_chechsum += byte_data;
    Flash_Switch_Bank(0);
}
/******************************************************************************
*
*  Function:    FirmwareUpdateFlashCheck
*
*  Description: to examine the validity of Flash
*               
*  Parameters:  none
*            
*  Returns: 
*  1: succeeded
*  0: failed
*  
*******************************************************************************/
u8_t FirmwareUpdateFlashCheck(void)
{
	u16_t i=0;
  	u8_t byte_data, flash_checksum;
  	u8_t magic[4]={0x13, 0xf0, 0xf0, 0x13};

  	//printf("\nChecking MagicCode, bank=[%d]", (u16_t)Flash_Bankno);      
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
    for(i=0;i<4;i++)
	{
    	Flash_Switch_Bank(Flash_Bankno-1);
        byte_data = Flash_Read_Byte(0xfffc+i);
		WatchDogTimerReset();
		//printf("\n magic[%d]=[%x]", (u16_t)i, (u16_t)byte_data);
		if(byte_data!=magic[i])
			break;
        Flash_Switch_Bank(0);
	}
	if(i!=4)
    {
        Stop_Mirror_Mode();
	    return 0;
    }
	//calculate ap checksum
    printf("\nChecking Flash...");
	Global_cal_chechsum=0;
    for(i=0;i<Flash_Bankno;i++)
	{
    	FirmwareUpdateFlashBankCheck(i);
		WatchDogTimerReset();
	}
    //compare checksum  
    Flash_Switch_Bank(Flash_Bankno-1);
    flash_checksum = Flash_Read_Byte(0xfffb);    
    Flash_Switch_Bank(0);
    Stop_Mirror_Mode();
    printf("\nFlash Checksum[%02x], Cal Checksum[%02x]", (u16_t)flash_checksum, (u16_t)Global_cal_chechsum);
    if(flash_checksum!=Global_cal_chechsum){
		return 0;
	}
    return 1;
}
#include "flash_all.c"



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
//include data
#include "type_def.h"
#include "public.h"
#include "utility.h"
#include "flash.h"
#include "dma.h"
#include "timer.h"

#define SST_SECTOR_SIZE                               0x1000
#define SYNCMOS_SECTOR_SIZE                           0x0400

#define SECTOR_SIZE                                   SYNCMOS_SECTOR_SIZE
#define SECTOR_NUMBER_OF_4K_SIZE                      0x1000/SECTOR_SIZE

//variables
u32_t Global_flash_count=0;
u16_t Global_flash_addr=0;
u8_t Global_cal_chechsum=0, Global_bank_index=0;
u8_t FlashBankSum(u16_t sddr_limit);
u8_t FlashBlankCheck(u16_t sddr_limit);

u16_t Global_flash_ID;
//sorbica071018, 
//0:first time into while(), flash_erase will start and flag will become 1.
//1-99, percentage
//100:flash erase completed.
//if program changes this flag to 0, flash will be erased again.
extern u16_t Phase_Flash_Erase;
u32_t last_timercounter=0;
u8_t Global_erase_flash_wait_time=10;
u8_t  next_erase_bank=0;//0-7
u8_t  next_erase_sector=0;//0-15

#if 0//for just test
extern u8_t Global_flag_restart;
#endif
//sorbica071018
//functions
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
	Flash_Switch_Bank(4);
	reg_byte = IP210RegRead(REG_CHIP_CONFIGURE_0);  
	//printf("\n\r Read reg_byte=%x",(u16_t)reg_byte);
    reg_byte &= ~BIT_MIRROR_EN;
	//printf("\n\r Write reg_byte=%x",(u16_t)reg_byte);
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
*  Function:    Flash_Erase_Sector
*
*  Description: to erase 4k flash data sector where index refers to in current bank of flash
*               
*  Parameters:  u8_t index(index 0~15 sector)
*               
*  Returns: 
*  1: succeeded           
*  0: failed
*******************************************************************************/
#ifdef  FLASHERASE_IN_WHILE//sorbica071101
u8_t Flash_Erase_Sector_AT49BV040B_while(u8_t index)
{
  u8_t i;
//  u8_t data byte_data1, byte_data2;

    Flash_Access_Enable();	
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      XBYTE[0x555] = 0x80;
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
	  
      XBYTE[0x1234] = 0x30;  
    Flash_Access_Disable();
    
    //delay 2s
	for(i=0;i<4;i++)
	{
        delay_ms(500);    
//        Flash_Access_Enable();	
//        byte_data1 = XBYTE[0x555];   
//        byte_data2 = XBYTE[0x555];   
//        Flash_Access_Disable();
//		printf("\n FESW[%02x][%02x] ", (u16_t)byte_data1, (u16_t)byte_data1);
	}
    
return 1;
}
u8_t Flash_Erase_Sector_AT49BV040B_BottomSector_while(u8_t index)
{
  u8_t i;
//  u8_t data byte_data1, byte_data2;

   Flash_Access_Enable();	
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      XBYTE[0x555] = 0x80;
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      switch(index)	  
      {
	      case 0x0:
                   XBYTE[0x0000] = 0x30;  
	               break;
	      case 0x1:
                   XBYTE[0x4000] = 0x30;  
	               break;
	      case 0x2:
                   XBYTE[0x6000] = 0x30;  
	               break;
	      case 0x3:
                   XBYTE[0x8000] = 0x30;  
	               break;
      }

    Flash_Access_Disable();
    
    //delay 2s
	for(i=0;i<4;i++)
	{
        delay_ms(500);    
//        Flash_Access_Enable();	
//        byte_data1 = XBYTE[0x555];   
//        byte_data2 = XBYTE[0x555];   
//        Flash_Access_Disable();
//		printf("\n FESW[%02x][%02x] ", (u16_t)byte_data1, (u16_t)byte_data1);
	}
return 1;

}
u8_t Flash_Erase_Sector_MX29LV004CB_while(u8_t index)
{
  u8_t i;
//  u8_t data byte_data1, byte_data2;

    Flash_Access_Enable();	
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      XBYTE[0x555] = 0x80;
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
	  
      XBYTE[0x1234] = 0x30;  
    Flash_Access_Disable();
    //delay 2s
	for(i=0;i<4;i++)
	{
        delay_ms(500);    
//        Flash_Access_Enable();	
//        byte_data1 = XBYTE[0x555];   
//        byte_data2 = XBYTE[0x555];   
//        Flash_Access_Disable();
//		printf("\n FESW[%02x][%02x] ", (u16_t)byte_data1, (u16_t)byte_data1);
	}
return 1;
}
u8_t Flash_Erase_Sector_MX29LV004CB_BottomSector_while(u8_t index)
{
  u8_t i;
  u8_t data byte_data1, byte_data2;

   Flash_Access_Enable();	
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      XBYTE[0x555] = 0x80;
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      switch(index)	  
      {
	      case 0x0:
                   XBYTE[0x0000] = 0x30;  
	               break;
	      case 0x1:
                   XBYTE[0x4000] = 0x30;  
	               break;
	      case 0x2:
                   XBYTE[0x6000] = 0x30;  
	               break;
	      case 0x3:
                   XBYTE[0x8000] = 0x30;  
	               break;
      }

    Flash_Access_Disable();
    //delay 2s
	for(i=0;i<4;i++)
	{
        delay_ms(500);    
//        Flash_Access_Enable();	
//        byte_data1 = XBYTE[0x555];   
//        byte_data2 = XBYTE[0x555];   
//        Flash_Access_Disable();
//		printf("\n FESW[%02x][%02x] ", (u16_t)byte_data1, (u16_t)byte_data1);
	}
   
return 1;

}
#else//#ifdef  FLASHERASE_IN_WHILE//sorbica071101
u8_t Flash_Erase_Sector_AT49BV040B_BottomSector(u8_t index)
{
  u8_t i;

   Flash_Access_Enable();	
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      XBYTE[0x555] = 0x80;
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      switch(index)	  
      {
	      case 0x0:
                   XBYTE[0x0000] = 0x30;  
	               break;
	      case 0x1:
                   XBYTE[0x4000] = 0x30;  
	               break;
	      case 0x2:
                   XBYTE[0x6000] = 0x30;  
	               break;
	      case 0x3:
                   XBYTE[0x8000] = 0x30;  
	               break;
      }

    Flash_Access_Disable();
    
    //delay 2s
	for(i=0;i<3;i++)
        delay_ms(1000);
    
return 1;

}
u8_t Flash_Erase_Sector_AT49BV040B(u8_t index)
{
  u8_t i;

    Flash_Access_Enable();	
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      XBYTE[0x555] = 0x80;
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
	  
      XBYTE[0x1234] = 0x30;  
    Flash_Access_Disable();
    
    //delay 2s
	for(i=0;i<3;i++)
        delay_ms(2000);
    
return 1;
}

u8_t Flash_Erase_Sector_MX29LV004CB_BottomSector(u8_t index)
{
  u8_t i;

   Flash_Access_Enable();	
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      XBYTE[0x555] = 0x80;
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      switch(index)	  
      {
	      case 0x0:
                   XBYTE[0x0000] = 0x30;  
	               break;
	      case 0x1:
                   XBYTE[0x4000] = 0x30;  
	               break;
	      case 0x2:
                   XBYTE[0x6000] = 0x30;  
	               break;
	      case 0x3:
                   XBYTE[0x8000] = 0x30;  
	               break;
      }

    Flash_Access_Disable();
    
    //delay 2s
	for(i=0;i<3;i++)
        delay_ms(2000);
    
return 1;

}
u8_t Flash_Erase_Sector_MX29LV004CB(u8_t index)
{
  u8_t i;

    Flash_Access_Enable();	
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
      XBYTE[0x555] = 0x80;
      XBYTE[0x555] = 0xAA;
      XBYTE[0x2AA] = 0x55;
	  
      XBYTE[0x1234] = 0x30;  
    Flash_Access_Disable();
    
    //delay 2s
	for(i=0;i<3;i++)
        delay_ms(2000);
    
return 1;
}
#endif//#ifdef  FLASHERASE_IN_WHILE//sorbica071101

u8_t Flash_Erase_Sector_SST39VLF040(u8_t index)
{
  u16_t data flash_addr;
  u16_t ii;
  u8_t  times;

    times=SECTOR_NUMBER_OF_4K_SIZE;
do{
    flash_addr = 0x1000*index+SECTOR_SIZE*(4-times);   
//    DBYTE[TEMP_0] = (u8_t)(flash_addr&0xff);
//    DBYTE[TEMP_1] = (u8_t)((flash_addr>>8)&0xff);  

    Flash_Access_Enable();	
      XBYTE[0x5555] = 0xAA;
      XBYTE[0x2AAA] = 0x55;
      XBYTE[0x5555] = 0x80;
      XBYTE[0x5555] = 0xAA;
      XBYTE[0x2AAA] = 0x55;
	  
      XBYTE[flash_addr] = 0x30;  
    Flash_Access_Disable();
    
    //delay 20ms
    delay_ms(60);
    
    times--;
}while(times>0);
/*
    //blank check
    for(ii=0;ii<0x1000;ii++)
    {
        Flash_Access_Enable(); 
        DBYTE[TEMP_0] = XBYTE[flash_addr+ii];
        Flash_Access_Disable();    
        
        if(DBYTE[TEMP_0] != 0xff)
		    {
            break;
		    }
    }
       
    return (ii!=0x1000)?0:1;
*/
return 1;
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
*  Function:    Flash_Write_Byte
*
*  Description: to write a byte of data to flash
*               
*  Parameters:
*    u16_t flash_addr: the address of data in Flash
*    u8_t byte_data: the data to be written
*           
*  Returns: 
*  1: succeeded
*  0: failed 
*  
*******************************************************************************/
u8_t Flash_Write_Byte(u16_t flash_addr, u8_t byte_data)
{
  u8_t read_data;
  u16_t ii;
  u16_t data flash_write_addr;

    flash_write_addr = flash_addr;

    DBYTE[TEMP_0] = (u8_t)(flash_addr&0xff);
    DBYTE[TEMP_1] = (u8_t)((flash_addr>>8)&0xff);    
    DBYTE[TEMP_2] = byte_data;    

    Flash_Access_Enable();	
      XBYTE[0x5555] = 0xAA;
      XBYTE[0x2AAA] = 0x55;
      XBYTE[0x5555] = 0xA0;
      XBYTE[flash_write_addr] = DBYTE[TEMP_2];
//      XBYTE[(DBYTE[TEMP_1]<<8)+DBYTE[TEMP_0]] = DBYTE[TEMP_2];
    Flash_Access_Disable();
    
    for(ii=0;ii<60000;ii++)
    {
        read_data = Flash_Read_Byte(flash_addr);
        if(read_data == byte_data)
             break;
    }
    
    return (ii==60000)?0:1;
	
}


/******************************************************************************
*
*  Function:    Flash_Switch_Bank
*
*  Description: to switch the bank number of flash that CPU fetches
*               
*  Parameters:  u8_t index(bank number 0-7)
*               
*  Returns: none
*  
*******************************************************************************/
void Flash_Switch_Bank(u8_t index)
{
FLASH_ADDR_16 = index & 0x01;
FLASH_ADDR_17 = (index>>1) &0x01;
FLASH_ADDR_18 = (index>>2) &0x01;
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
	//DMA
	IP210_DMA(src_addr, des_addr, length, BIT_CMD_MODE_F2M);
	//printf("\n\r DMA OK!");
	//Mirror
	Start_Mirror_Mode(src_addr);
}


/******************************************************************************
*
*  Function:    FirmwareUpdateEraseFlash
*
*  Description: to erase all flash except for firmware update program sectors
*               
*  Parameters: none
*            
*  Returns: none
*  
*******************************************************************************/
#ifdef  FLASHERASE_IN_WHILE//sorbica071101
void FirmwareUpdateEraseFlash_AT49BV040B_while()
{
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
#endif//sorbica071008
       Flash_Switch_Bank(next_erase_bank);   	
	   if(next_erase_bank==0)
	   {
           if(next_erase_sector==0)
		   {
               Flash_Erase_Sector_AT49BV040B_BottomSector_while(0);//(0)16k, (1)8k, (2)8k, (3)32k
               next_erase_bank=0;
               next_erase_sector=4;
		   }
           else
	       if(next_erase_sector==4)
		   {
               Flash_Erase_Sector_AT49BV040B_BottomSector_while(1);//(0)16k, (1)8k, (2)8k, (3)32k
               next_erase_bank=0;
               next_erase_sector=6;
           }
           else
	       if(next_erase_sector==6)
		   {
               Flash_Erase_Sector_AT49BV040B_BottomSector_while(2);//(0)16k, (1)8k, (2)8k, (3)32k
               next_erase_bank=1;
               next_erase_sector=0;
           }
           Phase_Flash_Erase=next_erase_bank*16+next_erase_sector;
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
    Stop_Mirror_Mode();
#endif//sorbica071008
		   return;
	   }
	   Flash_Erase_Sector_AT49BV040B_while(next_erase_bank);
       Flash_Switch_Bank(4); 
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
    Stop_Mirror_Mode();
#endif//sorbica071008
       next_erase_bank+=1;
       Phase_Flash_Erase=next_erase_bank*16;
}

void FirmwareUpdateEraseFlash_MX29LV004CB_while()
{
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
#endif//sorbica071008
       Flash_Switch_Bank(next_erase_bank);   	
	   if(next_erase_bank==0)
	   {
           if(next_erase_sector==6)
		   {
               Flash_Erase_Sector_MX29LV004CB_BottomSector_while(2);//(0)16k, (1)8k, (2)8k, (3)32k
               next_erase_bank=0;
               next_erase_sector=8;
		   }
           else
	       if(next_erase_sector==8)
		   {
               Flash_Erase_Sector_MX29LV004CB_BottomSector_while(3);//(0)16k, (1)8k, (2)8k, (3)32k
               next_erase_bank=1;
               next_erase_sector=0;
           }
           else
				next_erase_sector++;
           Phase_Flash_Erase=next_erase_bank*16+next_erase_sector;
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
    Stop_Mirror_Mode();
#endif//sorbica071008
		   return;
	   }
	   //Flash_Erase_Sector_MX29LV004CB_while(next_erase_bank);
       Flash_Switch_Bank(4); 
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
    Stop_Mirror_Mode();
#endif//sorbica071008
       next_erase_bank+=1;
       Phase_Flash_Erase=next_erase_bank*16;

}

void FirmwareUpdateEraseFlash_SST39VLF040_while()
{
  u8_t i,j;


	   printf("\nFirmwareUpdateEraseFlash_SST39VLF040(%x)(%x)", (u16_t)next_erase_bank, (u16_t)next_erase_sector);  
       i=(next_erase_sector+1)/16;
       j=(next_erase_sector+1)%16;  

	   if( next_erase_bank==0 && next_erase_sector>7)//don't erase Sector0 Page 8-15
       {
               next_erase_bank+=i;
               next_erase_sector=j;
		       return;
       }

#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
#endif//sorbica071008

       Flash_Switch_Bank(next_erase_bank);   	
	   Flash_Erase_Sector_SST39VLF040(next_erase_sector);
       Flash_Switch_Bank(4); 

#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
    Stop_Mirror_Mode();
#endif//sorbica071008

       next_erase_bank+=i;
       next_erase_sector=j;
       Phase_Flash_Erase=next_erase_bank*16+next_erase_sector;

}
#else//ifdef  FLASHERASE_IN_WHILE//sorbica071101
void FirmwareUpdateEraseFlash_AT49BV040B()
{
  u8_t i, j, k;

   Flash_Switch_Bank(4);   	
   Flash_Erase_Sector_AT49BV040B_BottomSector(2);//(0)16k, (1)8k, (2)8k, (3)32k
   Flash_Erase_Sector_AT49BV040B_BottomSector(3);//(0)16k, (1)8k, (2)8k, (3)32k
   printf("\nFirmwareUpdateEraseFlash_AT49BV040B(%x)", (u16_t)i);

   for(i=1;i<8;i++)
   {
       Flash_Switch_Bank(i);   	
       Flash_Erase_Sector_AT49BV040B(i);

	   printf("\nFirmwareUpdateEraseFlash_AT49BV040B(%x)", (u16_t)i);
   }
}
void FirmwareUpdateEraseFlash_MX29LV004CB()
{
  u8_t i, j, k;

   Flash_Switch_Bank(4);   	
   Flash_Erase_Sector_MX29LV004CB_BottomSector(2);//(0)16k, (1)8k, (2)8k, (3)32k
   Flash_Erase_Sector_MX29LV004CB_BottomSector(3);//(0)16k, (1)8k, (2)8k, (3)32k
   printf("\nFirmwareUpdateEraseFlash_MX29LV004CB(%x)", (u16_t)i);

   for(i=1;i<8;i++)
   {
       Flash_Switch_Bank(i);   	
       Flash_Erase_Sector_MX29LV004CB(i);

	   printf("\nFirmwareUpdateEraseFlash_MX29LV004CB(%x)", (u16_t)i);
   }
}

void FirmwareUpdateEraseFlash_SST39VLF040()
{
  u8_t i, j, k;

#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
#endif//sorbica071008
   for(i=0;i<8;i++)
   {
       Flash_Switch_Bank(i);   	
       for(j=0;j<16;j++)
	   {
	       if( i==0 && j>7)//don't erase Sector0 Page 8-15
		       continue;
	           Flash_Erase_Sector_SST39VLF040(j);
	   }
	   printf("\nFirmwareUpdateEraseFlash_SST39VLF040(%x)", (u16_t)i);
   }
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
    Stop_Mirror_Mode();
#endif//sorbica071008
}
#endif//#ifdef  FLASHERASE_IN_WHILE//sorbica071101



/******************************************************************************
*
*  Function:    FirmwareUpdateWriteData
*
*  Description: to write data into Flash when doing Firmware update
*               
*  Parameters:
*    u8_t *ptr_data: the begining pointer of datas to be written into Flash
*    u16_t length: the length of data to be written
*            
*  Returns: none
*  
*******************************************************************************/
u8_t FirmwareUpdateWriteData(u8_t *ptr_data, u16_t length)
{
  u16_t ii;

#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
#endif//sorbica071008

    for(ii=0;ii<length;ii++)
    {
        if( (Global_bank_index==0)&&(Global_flash_addr>=0x8000) )
        {}
        else
        {
            Flash_Switch_Bank(Global_bank_index);
	        if(Flash_Write_Byte(Global_flash_addr, ptr_data[ii])!=1)
	        break;
	    }
        Global_flash_count++;Global_flash_addr++;
        if(Global_flash_addr==0) Global_bank_index++;
    }
    //printf("\n\r Global_flash_addr=%ul",(u32_t)Global_flash_addr);
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
    Stop_Mirror_Mode();
#endif//sorbica071008
    return (ii==length)?1:0;

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
  u8_t byte_data;

    printf("\nFlashBankCheck(%x)", (u16_t) bank_index);
    if(bank_index==0) addr_limit=0x7FFF;
    if(bank_index==7) addr_limit=0xFFFA;
    Flash_Switch_Bank(bank_index);
	WatchDogTimerReset();
    byte_data = FlashBankSum(addr_limit);
    printf("\nFlashBankSumk%x(%x)", (u16_t)bank_index, (u16_t) byte_data);

    Global_cal_chechsum += byte_data;
    Flash_Switch_Bank(4);
}

void NOP(void)
{
}
#define MS_DELAY_COUNT 4000
void Delay_10ms(unsigned short count)
{
  unsigned short i, j;

      for(j=0;j<count;j++)
      {
          for(i=0;i<MS_DELAY_COUNT;i++)
              NOP();
      }
}
void FlashIdentification(void)
{
  u8_t data manufacturer=0x0, chipID=0x0;

	//check Flash ID
    Flash_Access_Enable();
      XBYTE[0x5555] = 0xAA;
      XBYTE[0x2AAA] = 0x55;
      XBYTE[0x5555] = 0x90;

    Delay_10ms(3);

	manufacturer=XBYTE[0x0];
	chipID=XBYTE[0x1];

      XBYTE[0x5555] = 0xAA;
      XBYTE[0x2AAA] = 0x55;
      XBYTE[0x5555] = 0xF0;
    Delay_10ms(3);

    Flash_Access_Disable();

//    printf("\nManufacturer=[%02x], chipID[%02x]", (unsigned short)manufacturer, (unsigned short)chipID);
    Global_flash_ID=((u16_t)manufacturer)<<8 | (u16_t)chipID;

	switch(Global_flash_ID)
	{
	    case 0xbfd7:
		           Global_erase_flash_wait_time=10;//100ms
		           printf("\nFlash SST39VLF040");
 			       break;
	    case 0x4003:
	    case 0x4063:
	    case 0x40A3:
		           Global_erase_flash_wait_time=10;//100ms
		           printf("\nFlash SyncMOS");
 			       break;
	    case 0xc2b6:
		           Global_erase_flash_wait_time=200;//2000ms
 		           printf("\nFlash MX29LV004CB");
 			       break;
	    case 0x1f13:
		           Global_erase_flash_wait_time=200;//2000ms
 		           printf("\nFlash AT49BV040B");
 			       break;
 	    default:
		        printf("\n Unknown Flash detected!FlashID[%04x]!!", (u16_t)Global_flash_ID);
		        break;
	}
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
  u8_t i, byte_data, flash_checksum;
  u8_t magic[4]={0x13, 0xf0, 0xf0, 0x13};

    printf("\nChecking MagicCode");
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
#endif//sorbica071008

    //check magic code
    for(i=0;i<4;i++)
	  {
        Flash_Switch_Bank(7);
        byte_data = Flash_Read_Byte(0xfffc+i);
		    if(byte_data!=magic[i])
		        break;
        Flash_Switch_Bank(4);
	  }
	  if(i!=4)
      {
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
        Stop_Mirror_Mode();
#endif//sorbica071008
	    return 0;
      } 

	  //calculate ap checksum
    printf("\nChecking Flash Checksum");
	Global_cal_chechsum=0;
    for(i=0;i<8;i++)
	  {
		WatchDogTimerReset();
        FirmwareUpdateFlashBankCheck(i);
	  }

    //compare checksum
    Flash_Switch_Bank(7);
    flash_checksum = Flash_Read_Byte(0xfffb);
	//printf("\n\r flash_checksum=%x Global_cal_chechsum=%x",(u16_t)flash_checksum,(u16_t)Global_cal_chechsum);
    Flash_Switch_Bank(4);

#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
    Stop_Mirror_Mode();
#endif//sorbica071008
	WatchDogTimerReset();
    printf("\nFlash [%02x], Cal [%02x]", (u16_t)flash_checksum, (u16_t)Global_cal_chechsum);
    if(flash_checksum!=Global_cal_chechsum)
	    return 0;

    return 1;

}
#if 0//for just test
u8_t Flash_Blank_Check(u8_t bank_index)
{
  u32_t ii, addr_limit=0xFFFF;
  u8_t byte_data;

    printf("\nFlash_Blank_Check(%x)", (u16_t) bank_index);
    if(bank_index==0) addr_limit=0x7FFF;
    
    Flash_Switch_Bank(bank_index);
    byte_data = FlashBlankCheck(addr_limit);
    Flash_Switch_Bank(4);
	return byte_data;
}
u8_t FirmwareUpdateBlankCheck(void)
{
  u8_t i, byte_data;
  u32_t j;

#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
#endif//sorbica071008

    for(i=0;i<0x8;i++)
	{
        if(Flash_Blank_Check(i)!=1)//if 0, not blank
        {
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
            Stop_Mirror_Mode();
#endif//sorbica071008
    	    return 0;
        }
	}
    Flash_Switch_Bank(4);

#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
    Stop_Mirror_Mode();
#endif//sorbica071008
    
    return 1;

}
#endif//#if 0//for just test

#ifdef  FLASHERASE_IN_WHILE//sorbica071101
#if 0//for just test
void Test_check_flash_write(void)
{
  u8_t i, byte_data;
  u32_t j;
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
#endif//sorbica071008
    for(i=0;i<0x8;i++)
	{
printf("\nW");
        Flash_Switch_Bank(i);
	    for(j=0;j<0x10000;j++)
		{
		    if(i==0 && j>=0x8000)
			    continue;

            Flash_Write_Byte(j, (u8_t)j+2);
		}
	}
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
    Stop_Mirror_Mode();
#endif//sorbica071008
}
#endif
#define FirmwareUpdateEraseFlash_S29C31004T_while FirmwareUpdateEraseFlash_SST39VLF040_while
void FirmwareUpdateEraseFlash_While(void)
{
	WatchDogTimerReset();
    if(Phase_Flash_Erase==128) return;
#if 0//for just test
    if(Phase_Flash_Erase==128)
    {
        printf("\n Blank Checking...");
        if(FirmwareUpdateBlankCheck()==1)
		{
		    Phase_Flash_Erase=130;
            printf("Pass ");        
		}
        else
            printf("Fail ");   
     Test_check_flash_write();
	 Phase_Flash_Erase=0;
	 Global_flag_restart=0;
	 next_erase_bank=0;
	 next_erase_sector=0;
        return;
    }
#endif
    IP210_Update_Timer_Counter();
    if (Delta_Time(last_timercounter) < Global_erase_flash_wait_time) 
        return;
    last_timercounter = timercounter;
	WatchDogTimerReset();
    switch(Global_flash_ID)
	{
	    case 0xbfd7:
		           FirmwareUpdateEraseFlash_SST39VLF040_while();
 			       break;
	    case 0x4003:
	    case 0x4063:
	    case 0x40A3:
		           FirmwareUpdateEraseFlash_S29C31004T_while();
 			       break;
	    case 0xc2b6:
		           FirmwareUpdateEraseFlash_MX29LV004CB_while();
 			       break;
	    case 0x1f13:
		           FirmwareUpdateEraseFlash_AT49BV040B_while();
 			       break;
 	    default:
		        break;
	}
	printf("\n FirmwareUpdateEraseFlash_While()");
}
#else
#define FirmwareUpdateEraseFlash_SyncMOS29C31004T FirmwareUpdateEraseFlash_SST39VLF040
void FirmwareUpdateEraseFlash(void)
{
    switch(Global_flash_ID)
	{
	    case 0xbfd7:
		           FirmwareUpdateEraseFlash_SST39VLF040();
 			       break;
	    case 0x4003:
	    case 0x4063:
	    case 0x40A3:
		           FirmwareUpdateEraseFlash_SyncMOS29C31004T();
 			       break;
	    case 0xc2b6:
		           FirmwareUpdateEraseFlash_MX29LV004CB();
 			       break;
	    case 0x1f13:
		           FirmwareUpdateEraseFlash_AT49BV040B();
 			       break;
 	    default:
		        break;
	}

}
#endif//#ifdef  FLASHERASE_IN_WHILE//sorbica071101


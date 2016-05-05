/******************************************************************************
*
*   Name:           main.C
*
*   Description:    Hardware initial main function
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
//20060110
#include <stdio.h>
#include <intrins.h>
#include <string.h>
#include "type_def.h"
#include "eepdef.h"
#include "MACdriver.h"
#include "public.h"
#include "flash.h"
#include "kernelsetting.h"
extern u16_t Global_flash_ID;
u8_t Flash_check_error_flag=0; // james 20090506

void bootmain(void);

void WatchDogTimerInit(void)
{
    //select timer width
    CKCON|=0xc0;	//26bits wait time (about 1.3sec if WDTWCYC is 0)

    //determine timer count interval by SFR:WDTWCYC
    WDTWCYC=16;
}
void WatchDogTimerReset(void)
{
    WDTRST=1;//reset WatchDog Timer
	WDTCLR=1; WDTCLR=0;//reset independent WatchDog Timer
}
/******************************************************************************
*
*  Function:    Check_MAC()
*
*  Description: Check MAC Address Invalid or Valid
*               
*  Parameters:  None
*               
*  Returns:     If Valid return 1. Otherwise, return 0
*               
*******************************************************************************/
u8_t All0_buf[size_EEMACID]={0x0,0x0,0x0,0x0,0x0,0x0};
u8_t Check_MAC()
{
//	u8_t AllF_buf[size_EEMACID];//={0xff,0xff,0xff,0xff,0xff,0xff};
//	u8_t All0_buf[size_EEMACID]={0x0,0x0,0x0,0x0,0x0,0x0};
	u8_t valid_flag=1;//,fcount=0,zcount=0,i;
    memset(All0_buf,0xFF,size_EEMACID);
	if(!memcmp(KSetting_Info.netif->MACID,All0_buf,size_EEMACID))
	{
		valid_flag=0;
	}
    memset(All0_buf,0x00,size_EEMACID);
	if(!memcmp(KSetting_Info.netif->MACID,All0_buf,size_EEMACID))
	{
		valid_flag=0;
	}
	if((KSetting_Info.netif->MACID[0])&0x01)
	{
		valid_flag=0;		
	}
	if(!valid_flag)
	{	
    	memcpy(KSetting_Info.netif->MACID, EE_Default.netif.MACID, size_EEMACID);
	}
	return valid_flag;
}

/******************************************************************************
*
*  Function:    boot_proc()
*
*  Description: Boot procedure
*               
*  Parameters:  None
*               
*  Returns:     None
*               
*******************************************************************************/
void boot_proc()
{
	//***********************************
//    Switch(Flash_Type)
//printf("boot_proc_1");
//printf("boot_proc_2");	
    Timercounter_Init();//sorbica071024
	EEPROMInit();//eeprom.h
//printf("boot_proc_3");		
	if(!Check_MAC())
	{
//		printf("\n\n\rInvalid MAC Address in EEPROM. Please check EEPROM!\n\rMAC Address use default value now!");
        printf("\n\rInvalid MAC Address, it will use default value!");
	}
//printf("boot_proc_4");	
	TRACE_TEST(("\n->EEPROMInit()\n~"));
	MAC_init();//MACdriver.h
  //20100831 chance add for filter not my mac packet
    IP210RegWrite( REG_RX_FILTER_0 , RF0_MY_MAC_EN);
    IP210RegWrite( REG_RX_FILTER_1 , 0x0 );
  //------------------------
//printf("boot_proc_5");	
	TRACE_TEST(("\n->MAC_init()\n~"));	
    bootmain();
	TRACE_TEST(("\n->bootmain()\n~"));
}
/******************************************************************************
*
*  Function:    IP210_SWReset
*
*  Description:  Software reset
*               
*  Parameters:  NONE
*               
*  Returns:     NONE
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
*  Function:    main
*
*  Description:  System initial
*               
*  Parameters:  NONE
*               
*  Returns:     NONE
*               
*******************************************************************************/
void main (void)
{	
	u8_t chk_tmp, count;

    KernelSettingInit();

	OUTPUTEN=0x42;
    //disable HW WDT
//sorbica    WDT_DIS=0x1;

	IP210_SWReset();//utility.h 

    WatchDogTimerInit();//sorbica 
//sorbica    WDTEN=0;//disable watch dog timer
    WDTEN=1;//sorbica enable watch dog timer

	Serial_Init();//serial.h
	TRACE_TEST(("\n->Serial_Init()~"));
	printf("\n\r%s",FW_CODE_VERSION);     
//sorbica Firmware Update check
//pressing 'f' followed by pressing 'u' will invoke Firmware Update
    //Resize_RX_Buf(0x50);//change rx buffer start address for cgi_firmwareupdate NETTMPBUFFER buffer use
    MAC_init();//MACdriver.h
  //20100831 chance add for filter not my mac packet
    IP210RegWrite( REG_RX_FILTER_0 , RF0_MY_MAC_EN);
	IP210RegWrite( REG_RX_FILTER_1 , 0x0 );
  //------------------------
	chk_tmp=DBYTE[SFR_IE_TEMP];
	TRACE_TEST(("\\n\r (%x)DBYTE[SFR_IE_TEMP]",(u16_t)chk_tmp));		
	//Mirro mode
//	printf("\n\r Before Mirror");
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
    FlashIdentification();
	Stop_Mirror_Mode();

//	printf("\n\r After Mirror");
	//Check flash code	
	if(chk_tmp==0xA6)
	{
		boot_proc();
	}
	else
	{
        count = 0;
		do{
//sorbica add WDT reset here
        //reset WDT in case WDT would cause a CPU Reset
        WatchDogTimerReset();
		    if(FirmwareUpdateFlashCheck())
			    break;
		    count++;	
		}
        while((count<3));  
		if(count >= 3)
		{
			printf("\nFlashCheck Error!");
			Flash_check_error_flag=1;// james 20090506
			boot_proc();
		}
		else
		{
			WatchDogTimerReset();
            printf("\nFlashCheck OK!");
			//***********************************
			//change to webcontrol code
			//***********************************
			//printf("\nRun Webcontrol code!"); 
			CPU_GOTO_0000H();
		}
	}
}
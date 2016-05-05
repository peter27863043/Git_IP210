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
//#include "utility.h"
#include "eepdef.h"
#include "MACdriver.h"
#include "public.h"
#include "version.h"
#include "flash.h"
extern uFW_EE_Info* ptr_EE_Info;
//#define  FU_ALL_RUN_IN_MIRROR

//volatile u8_t uip_acc32[4];//for macdriver
extern u16_t Global_flash_ID;

void bootmain(void);
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
u8_t Check_MAC()
{
	u8_t AllF_buf[size_EEMACID];//={0xff,0xff,0xff,0xff,0xff,0xff};
//	u8_t All0_buf[size_EEMACID]={0x0,0x0,0x0,0x0,0x0,0x0};
	u8_t valid_flag=1;//,fcount=0,zcount=0,i;
    memset(AllF_buf,0xFF,size_EEMACID);
	if(!memcmp(ptr_EE_Info->netif.MACID,AllF_buf,size_EEMACID))
	{
		valid_flag=0;
	}
    memset(AllF_buf,0x00,size_EEMACID);
	if(!memcmp(ptr_EE_Info->netif.MACID,AllF_buf,size_EEMACID))
	{
		valid_flag=0;
	}
	if((ptr_EE_Info->netif.MACID[0])&0x01)
	{
		valid_flag=0;		
	}
	if(!valid_flag)
	{	
    	memcpy(ptr_EE_Info->netif.MACID, EE_Default.netif.MACID, size_EEMACID);
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
    Timercounter_Init();//sorbica071024
	EEPROMInit();//eeprom.h
	TRACE_TEST(("\n->EEPROMInit()\n~"));
	MAC_init();//MACdriver.h	
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
	u8_t chk_tmp;

    //KernelSettingInit();

    //disable HW WDT
    WDT_DIS=0x1;

	IP210_SWReset();//utility.h 

    WDTEN=0;//disable watch dog timer

	Serial_Init();//serial.h
	TRACE_TEST(("\n->Serial_Init()~"));
	printf("\n\rStart FUU_v1.1_080814 ");     
	chk_tmp=DBYTE[SFR_IE_TEMP];
	TRACE_TEST(("\\n\r (%x)DBYTE[SFR_IE_TEMP]",(u16_t)chk_tmp));		
	//Mirro mode
//	printf("\n\r Before Mirror");
	FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
    FlashIdentification();
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
#else
	Stop_Mirror_Mode();
#endif//sorbica071008

//	printf("\n\r After Mirror");
	//Check flash code	
	boot_proc();
	printf("\n\r End of FUU ");
}
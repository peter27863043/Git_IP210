#include "eepdef.h"
#include "kernelsetting.h"
#include "utility.h"
#include "eeprom.h"
#include "switch.h"
extern pEE_Info ptr_EE_Info;
KernelSettingInfo KSetting_Info;
/******************************************************************************
*
*  Function:    KernelSettingInit()
*
*  Description: assign some runtime information
*
*  Parameters:  KSetting_Info
*
*  Returns:     KSetting_Info
*
******************************************************************************/

//**********Initial Setting values, modify kernelsetting.h for fine tuning******************
void KernelSettingInit(void)
{
    KSetting_Info.SystemClock=KS_SYSTEM_CLOCK_SETTING;
    KSetting_Info.EE_Interface=KS_EEPROM_SETTING;
    KSetting_Info.PHY_Setting=KS_PHY_SETTING;
    KSetting_Info.netif=(pEEConfigInfo)&(ptr_EE_Info->netif);
#ifndef FIRMWARE_UPDATE
    KSetting_Info.HW_LoadDefault_Option=KS_HW_LOAD_DEFAULT_OPTION;
    KSetting_Info.HW_LoadDefault_LEDFlash_time=KS_HW_LOAD_DEFAULT_FLASHTIME;
    KSetting_Info.MDCMDIO_Interface=KS_MDC_MDIO_INTERFACE;
    KSetting_Info.EEInfoSize=sizeof(uEE_Info);
    KSetting_Info.netif_offset=(u8_t*)KSetting_Info.netif-(u8_t*)ptr_EE_Info;
    KSetting_Info.wdt_Setting=KS_WDT_SETTING;
    KSetting_Info.wdtwcyc=KS_WDT_WAIT_CYCLE;
	KSetting_Info.EtherTag=KS_ETHTAG_VLAN;
	KSetting_Info.EtherTagLen=KS_ETHERTAG_LEN;
#endif
#ifdef MODULE_SPECIAL_TAG//sorbica071112
    KSetting_Info.SpecialTagLength=sizeof(uSpecialTagInfo);
#endif//#ifdef MODULE_SPECIAL_TAG
}
//**********Hardware Load Default***************

#ifndef FIRMWARE_UPDATE
u8_t UpdateEEPROMData(u8_t *ee_addr , u16_t len);
void flash1()
{
	LED_SW=1;
	Delay(12000);
	LED_SW=0;
	Delay(12000);
}
void flashLED()
{
	flash1();
}

void check_Load_Default()
{
	u8_t i;

	if(LOAD_DEF){return;}
	for(i=0;i<3;i++)
	  {	Delay(50000);
	  	if(LOAD_DEF){return;}
	  }

	//flashLED();
	printf("\r\n Loading Default Setting...");
    EEPROM_Enable();
	EEMemoryInit();//Load default value to memory
	if (!UpdateEEPROMData(EE_membuffer,sizeof(uEE_Info)))//save memory to eeprom
	{
		EE_result = 0;
        printf("\n\r Update EEPROM ERROR!");
    }
	else
    {
  	    printf("OK!!");
    }
//	LED_SW=1;
//	CPU_GOTO_FFFDH();
}
#endif
//***********************************************


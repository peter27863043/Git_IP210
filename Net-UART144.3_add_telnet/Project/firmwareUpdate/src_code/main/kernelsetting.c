#include "kernelsetting.h"
#include "eepdef.h"
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
    //KSetting_Info.netif=(pEEConfigInfo)&(ptr_EE_Info->netif);
	KSetting_Info.netif=&EEConfigInfo;
}


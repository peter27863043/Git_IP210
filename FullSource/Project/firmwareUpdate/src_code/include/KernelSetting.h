#ifndef _KERNELSETTING_H 
#define _KERNELSETTING_H

#include "eepdef.h"

//definition for Some Constants
#define KS_SYSTEM_CLOCK_59M               1
#define KS_SYSTEM_CLOCK_44M               0
#define KS_EEPROM_P4                      1
#define KS_EEPROM_P3                      0
#define KS_USE_INTERNAL_PHY               1
#define KS_USE_EXTERNAL_PHY               0
#define KS_HW_LOAD_DEFAULT_ENABLE         1
#define KS_HW_LOAD_DEFAULT_DISABLE        0
#define KS_MDC_MDIO_ENABLE                1             
#define KS_MDC_MDIO_DISABLE               0
#define KS_WDT_ENABLE                     1
#define KS_WDT_DISABLE                    0

//device configuration define for fine tuning
#define KS_SYSTEM_CLOCK_SETTING           KS_SYSTEM_CLOCK_59M
#define KS_EEPROM_SETTING                 KS_EEPROM_P4
#define KS_PHY_SETTING                    KS_USE_INTERNAL_PHY
#define KS_HW_LOAD_DEFAULT_OPTION         KS_HW_LOAD_DEFAULT_ENABLE
#define KS_MDC_MDIO_INTERFACE             KS_MDC_MDIO_ENABLE
#define KS_HW_LOAD_DEFAULT_FLASHTIME      100//the number of BYTE data for LED to be flashed once, the least is 1, which flashes LED per BYTE
#define KS_WDT_SETTING                    KS_WDT_ENABLE
#define KS_WDT_WAIT_CYCLE                 16//if WatchDog Timer is enabled, the larger KS_WDT_WAIT_CYCLE, the longer time allowing CPU to hang before WDT Reset
                                            //KS_WDT_WAIT_CYCLE=16, about 20 seconds
#define KS_ETHERTAG_LEN                   4  //default VLAN TAG length
#define KS_ETHTAG_VLAN 				  0x8100 //VLAN TAG
//*******************Hardware Load Default *************
sbit	LED_SW  =	P4^2;
sbit	LOAD_DEF=	P4^3;
//******************************************************
typedef struct _KernelSettingInfo{
    u8_t    SystemClock;
    u8_t    EE_Interface;
    u8_t    PHY_Setting;
	pEEConfigInfo    netif;
}KernelSettingInfo;
void KernelSettingInit(void);
extern KernelSettingInfo KSetting_Info;
void check_Load_Default();
void flashLED();
#endif

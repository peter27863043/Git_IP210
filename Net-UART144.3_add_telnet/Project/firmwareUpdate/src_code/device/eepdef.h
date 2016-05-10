/******************************************************************************
*
*   Name:           eepdef.h
*
*   Description:    EEPROM intialization Header
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Grace
*
*******************************************************************************/
#ifndef _EEPDEF_H_
#define _EEPDEF_H_
#include "public.h"
#include "type_def.h"
#include "switch.h"
// COMMON EEPROM definition
#define EE_OK               1
#define EE_FAIL             0

#define size_EEchip_id		2
#define size_EEapp_id		2
#define size_SWITCHDATA     0x200
#define size_EEBUFFER       2048
#define size_EEPROM         0x800 //24c08
//COMMON Network Information in EEPROM
#define size_EEMACID			   6
#define size_EEIPAddr			   4
#define size_EELoginName		   16
#define size_EELoginPassword       16
#define size_EEComment			   16
typedef struct _EEConfigInfo{
    u8_t  MACID[size_EEMACID];			        // 6
    u8_t  IPAddress[size_EEIPAddr];     	    // 4
    u8_t  SubnetMask[size_EEIPAddr];    	    // 4
    u8_t  Gateway[size_EEIPAddr];       	    // 4
    u8_t  DNSAddress[size_EEIPAddr];    	    // 4
    u8_t  DHCPEnable;                           // 1
    u8_t  LoginName[size_EELoginName];          // 16
    u8_t  LoginPassword[size_EELoginPassword];  // 16
    u8_t  Comment[size_EEComment];           	// 16
}uEEConfigInfo, *pEEConfigInfo;               //length 0x4A

typedef struct _EE_Info{
	struct _EEConfigInfo netif;       //0x84
}uEE_Info, *pEE_Info;

#define loc_EEnetif				size_SWITCHDATA  //ip1726 is 0x100
#define size_EEInfo				(u16_t)sizeof(uEE_Info)	//joe for all default size 20081017
#define size_EEnetif			(u16_t)sizeof(uEEConfigInfo)
#define size_EEDEVICEApp		(u16_t)sizeof(m_eedeviceapp)
#define size_EEDEVICEApp_two	(u16_t)sizeof(m_eedeviceapp_two)
#define size_EEDEVICErec		(u16_t)sizeof(m_eedevicerec)
#define size_EESTPSettings		(u16_t)sizeof(uEESTPSettings)
#define size_EELACPSettings		(u16_t)sizeof(uEELACPSettings)
#define size_EEEocSettings		(u16_t)sizeof(uEEEocSettings)
#define size_SNMPINFO          (u16_t)sizeof(m_SNMPINFO)//for SNMP
//netif area***************************************************************
#define	loc_EE_NETIF_BLOCK_ID  (loc_EEnetif+0)
#define	loc_EE_NETIF_BLOCK_LEN (loc_EEnetif+1)
#define loc_EEchip_id		   (loc_EEnetif+3)    
#define loc_EEapp_id		   (loc_EEnetif+5)  
#define loc_EEMACID			   (loc_EEnetif+7)	  
#define loc_EEIPAddress		   (loc_EEnetif+13)   
#define loc_EESubnetMask	   (loc_EEnetif+17)   
#define loc_EEGateway		   (loc_EEnetif+21)  
#define loc_EEDNSAddress	   (loc_EEnetif+25) 
#define loc_DHCPEnable   	   (loc_EEnetif+29) 
#define loc_EELoginName		   (loc_EEnetif+30)  
#define loc_EELoginPassword	   (loc_EEnetif+46) 
#define loc_EEComment		   (loc_EEnetif+62) 
 //***************************************************************************
#define EE_MAGIC_CODE_DATA      0x1235
#define EE_MAGIC_0				(u8_t)(EE_MAGIC_CODE_DATA>>8)
#define EE_MAGIC_1				(u8_t)(EE_MAGIC_CODE_DATA&0xff)

#define MAX_BLOCK_NUM 10
typedef struct _Block {
    u8_t  BlockID;                    
    u16_t BlockLength;               
    union
    {
      u16_t offset;
      u8_t  *poffset;
    }u_offset;
} ublock, *pblock;

// Global variables and functions
//variables
extern uEEConfigInfo EEConfigInfo;
extern uEE_Info	code EE_Default;
extern u8_t			 EE_result;
extern u8_t EE_membuffer[size_EEBUFFER];
// Prototype functions
extern u8_t EEPROMCheckDataAvailable(void);
extern void EEPROMInit(void);
extern void EEMemoryInit(void);
#define EEPROM_BUF uip_buf
#endif  //end of ifndef EEPDEF_H
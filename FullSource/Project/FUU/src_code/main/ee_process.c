/******************************************************************************
*
*   Name:           ee_process.c
*
*   Description:    Process EEPROM content includes Default eeprom restore and
*                   eeprom check.
*   support functions:
*					1. u8_t EEPROMCheckDataAvailable(void) -->
*							NO EEPROM - EE_Result=0
*							if (EE_Result==1) Check if EEPROM with valid ID
*					2. u8_t EEPROMLoadDefault(void) -->
*							Write default EEPROM data into EEPROM
*					3. void EEPROMInit(void) -->
*						    Enable EEPROM. call EEPROMCheckDataAvailable(void).
* 							If EE_Result==0, load default uEEConfigInfo into
*							    working EEConfigInfo.
*							If EE_Result==1 && ID is invalid,
*								EEPROMLoadDefault(void).
*							If EE_result==1 && ID is valid, load
*								EEPROM uEEConfigInfo into working EEConfigInfo
*					4. void Restore_Default_DataRate(void) -->
*							Write DataRate default value into EEPROM
*							DataRate.
*					5. void Restore_Default_VLANGroup(void) -->
*							Write VLANGroup default value into Switch
*							and EEPROM
*					6. void Restore_EEPROM_to_Reg(void) -->
*							Write Switch_Data from EEPROM into Switch registers
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Grace
*
*******************************************************************************/
// DEBUG defined
//#define DBG_EEPROM


//include data
#include "type_def.h"
#include "public.h"
#include "eeprom.h"
#include "eepdef.h"
#include "string.h"
#include "module.h"
#ifndef EE_OTPCODE
#include "switch.h"
#include "MDC_MDIO.h"
//*******************Function prototype area*********************
extern void Write_EEP(u8_t phyad,u8_t reg1,u8_t Regdata);
#endif

u8_t need_load_default=0;
//---------------------------------------------------
//constants

uEEConfigInfo EEConfigInfo;			// Global variable to save Network Info
u8_t EE_Result;						// =0 eeprom error, =1 eeprom existed

u8_t EE_Reserved_0[8] = {0x0, 0x1, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0};//EE_Reserved_0 area declaration

#ifdef EE_OTPCODE
SModuleInfo ModuleInfo;
/******************************************************************************
*
*  Function:    GetDeviceID
*
*  Description: Get device ID form EE_Default
*               
*  Parameters:  none
*               
*  Returns:     Device ID
*               
*******************************************************************************/
u16_t GetDeviceID()
{
	return CBYTE[(u16_t)&EE_Default.chip_id]*256+CBYTE[(u16_t)&EE_Default.chip_id+1];
}
/******************************************************************************
*
*  Function:    u8_t LoadEEPROM2ModuleInfo(void)
*
*  Description: Load EEPROM ModuleInfo into ModuleInfo working memory
*
*  Parameters: 	NONE
*  Global Variables:
*				SModuleInfo ModuleInfo; //working memory for ModuleInfo
*
*  Returns:   EEPROM access status
*             1: OK
*             0: Failed
*
*******************************************************************************/
static u8_t LoadEEPROM2ModuleInfo(void)
{
#define loc_ModuleInfo (loc_EEnetif+sizeof(uEEConfigInfo)+100)
	u16_t 	i;
	u8_t 	*p;

	p=(u8_t *) &ModuleInfo;
    // Read in EEConfigInfo
    for (i=0; i < sizeof (SModuleInfo); i++)
    {
    	if(EEPROM_Read_Byte(loc_ModuleInfo+i, (p+i)) == 0)
    	{
			TRACE_EEPROM(("ACCESS EEPROM ERROR"));
			return 0; 	// FAIL to Access EEPROM
    	}
	}
#ifdef MODULE_INFO_DUMP
  printf("\n\rEEPROM_Dump:");
  for(i=0;i<sizeof(struct _EE_Info);i++)
  {
  	  u8_t eedata;
  	  EEPROM_Read_Byte(i,&eedata);
  	  if(i%16==0)printf("\n\r");
  	  printf("%2.2x,",(u16_t)eedata);
  }
  printf("\n\r");
#endif
	return 1;
}

/******************************************************************************
*
*  Function:    u8_t SaveModuleInfo2EEPROM(void)
*
*  Description: Save ModuleInfo working memory into EEPROM ModuleInfo
*
*  Parameters: 	NONE
*  Global Variables:
*				SModuleInfo ModuleInfo; //working memory for ModuleInfo
*
*  Returns:   EEPROM access status
*             1: OK
*             0: Failed
*
*******************************************************************************/
u8_t SaveModuleInfo2EEPROM(void* pSubModule,u16_t size)
{
	u16_t i;
	u8_t* pu8SubMoudle=(u8_t*)pSubModule;
	u16_t loc_EESubMoudle=loc_EEModuleInfo+(u16_t)((u8_t*)pSubModule-(u8_t*)&ModuleInfo);
	for(i=0;i<size;i++)
		if(EEPROM_Write_Byte(loc_EESubMoudle+i,pu8SubMoudle[i])==0)return 0;
	return 1;
}
#endif //EE_OTPCODE
#ifdef FLASH_512
u8_t Setup_BLUpgrade(u8_t *uip_hostaddr)
{
	EEPROM_Write_Byte(2,((u8_t*)uip_hostaddr)[0]);
	EEPROM_Write_Byte(3,((u8_t*)uip_hostaddr)[1]);
	EEPROM_Write_Byte(6,((u8_t*)uip_hostaddr)[2]);
	EEPROM_Write_Byte(7,((u8_t*)uip_hostaddr)[3]);

    return 1;
}
#endif
/******************************************************************************
*
*  Function:    u8_t LoadEEPROM2EEConfig(void)
*
*  Description: Load EEPROM EECOnfigInfo into EECOnfigInfo working memory
*
*  Parameters: 	NONE
*  Global Variables:
*				uEEConfigInfo EEConfigInfo; //working memory for uEEConfigInfo
*
*  Returns:   EEPROM access status
*             1: OK
*             0: Failed
*
*******************************************************************************/
static
u8_t
LoadEEPROM2EEConfig(void)
{
	u16_t 	i;
	u8_t 	*p;

	p=(u8_t *) &EEConfigInfo;
    // Read in EEConfigInfo
    for (i=0; i < sizeof (uEEConfigInfo); i++)
    {
    	if(EEPROM_Read_Byte(loc_EEnetif+i, (p+i)) == 0)
    	{
			TRACE_EEPROM(("ACCESS EEPROM ERROR"));
			return 0; 	// FAIL to Access EEPROM
    	}
	}

#ifdef DBG_EEPROM
    TRACE_EEPROM(("MACID["));
	for(i=0;i<6;i++)
	{
        TRACE_EEPROM(("%02x ", (u8_t)EEConfigInfo.MACID[i]));
	}
    TRACE_EEPROM(("]\n"));

    TRACE_EEPROM(("IPAddress["));
	for(i=0;i<4;i++)
	{
        TRACE_EEPROM(("%02x ", (u8_t)EEConfigInfo.IPAddress[i]));
	}
    TRACE_EEPROM(("]\n"));

    TRACE_EEPROM(("SubnetMask["));
	for(i=0;i<4;i++)
	{
        TRACE_EEPROM(("%02x ", (u8_t)EEConfigInfo.SubnetMask[i]));
	}
    TRACE_EEPROM(("]\n"));

    TRACE_EEPROM(("Gateway["));
	for(i=0;i<4;i++)
	{
        TRACE_EEPROM(("%02x ", (u8_t)EEConfigInfo.Gateway[i]));
	}
    TRACE_EEPROM(("]\n"));

    TRACE_EEPROM(("DNSAddress["));
	for(i=0;i<4;i++)
	{
        TRACE_EEPROM(("%02x ", (u8_t)EEConfigInfo.DNSAddress[i]));
	}
    TRACE_EEPROM(("]\n"));

    TRACE_EEPROM(("LoginName["));
	for(i=0;i<16;i++)
	{
        TRACE_EEPROM(("%02x ", (u8_t)EEConfigInfo.LoginName[i]));
	}
    TRACE_EEPROM(("]\n"));
    TRACE_EEPROM(("\nLoginName=[%s] ", EEConfigInfo.LoginName));

    TRACE_EEPROM(("LoginPassward["));
	for(i=0;i<16;i++)
	{
        TRACE_EEPROM(("%02x ", (u8_t)EEConfigInfo.LoginPassword[i]));
	}
    TRACE_EEPROM(("]\n"));
    TRACE_EEPROM(("\nLoginPassward=[%s] \n", EEConfigInfo.LoginPassword));

    TRACE_EEPROM(("Comment["));
	for(i=0;i<12;i++)
	{
        TRACE_EEPROM(("%02x ", (unsigned short)EEConfigInfo.Comment[i]));
	}
    TRACE_EEPROM(("]\n"));
    TRACE_EEPROM(("\nComment=[%s] \n", EEConfigInfo.Comment));
#endif
	return 1;
}
/******************************************************************************
*
*  Function:    u8_t EEPROMCheckDataAvailable(void)
*
*  Description: check the EEPROM Status :
*				1. Any EEPROM Access problem -
*				   EE_Result =0 : Access error
*				   EE_Result =1 : Access Successful
*				2. Check if EEPROM with valid ID
*				   Valid ID - return 1
*				   Invalid ID or EE_Result=0 - return 0
*  Parameters:  none
*  Global Variables:
*				u8_t EE_Result - =0 EEPROM Access Failed
*								 =1 EEPROM Access OK
*
*  Returns:     EEPROM status
*  Return value:
*  1: EEPROM with Valid ID and Access ok.
*  0: EEPROM without Valid ID or Access Fail

*******************************************************************************/
u8_t EEPROMCheckDataAvailable(void)
{
    u8_t 	eebyte;
	u8_t	i;

    TRACE_EEPROM(("\n->EEPROMCheckDataAvailable()\n~"));

    EE_Result = 1;		// Set default EEPROM Access OK

#ifdef EE_SWITCH_178
    if(EEPROM_Read_Byte(loc_EEIP178id, &eebyte) == 0)
    {
		TRACE_EEPROM(("ACCESS EEPROM ERROR"));
		EE_Result=0;
		return 0; 	// FAIL to Access EEPROM
    }
    TRACE_EEPROM(("178id[0]=%02x ", (u8_t)eebyte));
    if (eebyte != EE_Default.IP178id[0])
    {
		return 0;   // invalid ID
	}

    if(EEPROM_Read_Byte(loc_EEIP178id+1, &eebyte) == 0)
    {
		TRACE_EEPROM(("ACCESS EEPROM ERROR"));
		EE_Result=0;
		return 0; 	// FAIL to Access EEPROM
    }
    TRACE_EEPROM(("178id[1]=%02x ", (u8_t)eebyte));
    if (eebyte != EE_Default.IP178id[1])
    {
		return 0;   // invalid ID
	}
#endif
	for (i=0; i < 8; i++)
	{
    	if(EEPROM_Read_Byte(i, &eebyte) == 0)
    	{
			TRACE_EEPROM(("ACCESS EEPROM ERROR"));
			EE_Result=0;
			return 0; 	// FAIL to Access EEPROM
    	}
    	TRACE_EEPROM(("CHIPID[%02x]=%02x ",i, (u8_t)eebyte));
    	if (eebyte != EE_Reserved_0[i])
    	{
            printf("\n EE check invalid!");
			return 0;   // invalid ID
		}
	}

    TRACE_EEPROM(("\n<-EEPROMCheckDataAvailable()~"));
	return 1; // EEPROM Access OK and ID Valid

}
#ifdef EE_OTPCODE
/******************************************************************************
*
*  Function:    EEPROMLoadDefault
*
*  Description: Write default EEPROM data into EEPROM
*
*  Parameters:  none
*
*  Returns:     EEPROM status
*  1: data is written successfully
*  0: at least one byte of data can't be written into eeprom
*
*******************************************************************************/
#include "wdg.h"
u8_t web_def=0;
void flashLed();
u8_t EEPROMLoadDefault(void)
{
	u16_t 	i; 		//i need to bigger than 1K when IP1726 or 1826
    u8_t 	*p;
	u8_t    *pEE_Reserved_0;

    TRACE_EEPROM(("\n->EEPROMLoadDefault()\n~"));
    pEE_Reserved_0 = (u8_t *)EE_Reserved_0;
	p = (u8_t *) &EE_Default;
	for (i=0; i < sizeof (uEE_Info); i++)
	{
		if(web_def)
		{ if(i>=loc_EEnetif&&i<loc_EEnetif+sizeof(uEEConfigInfo)-3)continue; }
		else
		{ if(i>=loc_EEnetif&&i<loc_EEnetif+6)continue; }
		if(i<8)
		{
			WatchDogTimerReset();
		    if(EEPROM_Write_Byte(loc_EEStartAddr+i,*(pEE_Reserved_0+i))!=1)
    		{
	    		TRACE_EEPROM(("Write ERROR"));
	            return 0;
    		}
		}else
		{
			WatchDogTimerReset();
		    if(EEPROM_Write_Byte(loc_EEStartAddr+i,*(p+i))!=1)
    		{
	    		TRACE_EEPROM(("Write ERROR"));
	            return 0;
    		}
		}
		flashLed();
	}

    TRACE_EEPROM(("\n<-EEPROMLoadDefault()~"));
	return 1;
}
#endif //EE_OTPCODE
/******************************************************************************
*
*  Function:    EEPROMInit
*
*  Description: Enable EEPROM.
*				call EEPROMCheckDataAvailable(void).
*				If EE_Result==0, load default uEEConfigInfo into working
*								 EEConfigInfo.
*				If EE_Result==1 && ID is invalid, EEPROMLoadDefault(void)
                                 and load default uEEConfigInfo into working
*								 EEConfigInfo.
*				If EE_Result==1 && ID is valid, load EEPROM uEEConfigInfo
*								 into working EEConfigInfo.
*
*  Parameters:  none
*
*  Returns: NONE
*
*******************************************************************************/
void EEPROMInit(void)
{
    TRACE_EEPROM(("\n->EEPROMInit()\n~"));

    EEPROM_Enable();

    if (EEPROMCheckDataAvailable())
    { // EE_Result==1 && ID is Valid
    	if (!LoadEEPROM2EEConfig())
    	{ // EEPROM ACCESS ERROR
    		EE_Result = 0; // EEPROM ACCESS Error
    		// Load EE_Default.netif into EEConfigInfo
    		memcpy(&EEConfigInfo, &EE_Default.netif, sizeof (uEEConfigInfo));
		}
#ifdef EE_OTPCODE
		if (!LoadEEPROM2ModuleInfo())
		{
			EE_Result = 0; // EEPROM ACCESS Error
    		// Load EE_Default.netif into EEConfigInfo
    		memcpy(&ModuleInfo, &EE_Default.ModuleInfo, sizeof (SModuleInfo));
		}
#endif //EE_OTPCODE
	} else
	{	// ID is invalid or EE_Result == 0
		// Load EE_Default.netif into EEConfigInfo
#ifdef EE_OTPCODE
		memcpy(&ModuleInfo, &EE_Default.ModuleInfo, sizeof (SModuleInfo));
#endif //EE_OTPCODE
    	memcpy(&EEConfigInfo, &EE_Default.netif, sizeof (uEEConfigInfo));
		if (EE_Result)
		{	// EEPROM Access OK but ID inValid
		#ifdef DBG_EEPROM
			printf("Write Default into EEPROM\n");
		#endif
		  need_load_default=1;
			/*if (!EEPROMLoadDefault())
			{
				EE_Result = 0;
			}*/
		}
	}
    TRACE_EEPROM(("\n<-EEPROMInit()\n~"));
}

#ifndef EE_OTPCODE
#ifdef EE_SWITCH_178
/******************************************************************************
*
*  Function:    Restore_Default_DataRate
*
*  Description: Restore DataRate to default value
*
*  Parameters:  None
*
*  Returns:     None
*
*******************************************************************************/
void Restore_Default_DataRate()
{
  	u8_t	i;

  	for(i=0;i<size_EEDataRate;i++)
  	{
		EEPROM_Write_Byte(loc_EEDataRate+i, EE_Default.DataRate[i]);
	}

}
#endif

/******************************************************************************
*
*  Function:    Restore_Default_VLANGroup
*
*  Description: Restore VLANGroup to default value
*
*  Parameters:  None
*
*  Returns:     None
*
*******************************************************************************/
void Restore_Default_VLANGroup()
{
	u16_t	i;

#ifdef EE_SWITCH_178
	for(i=0;i<MAX_PORT_NUM+1;i++)
	{
		Write_Reg(PHY_31,MII_POSTVLAN+i,0x1ff);
		Write_EEP(PHY_31,MII_POSTVLAN+i,0x1ff);
	}
#endif

	// Write VLANGroup and VLANMulti or VLAN_ftth into EEPROM
#ifdef EE_SWITCH_178
    for(i=0; i< (size_EEVLANGroup+size_EEVLANMulti); i++)
#endif
#ifdef EE_SWITCH_1726
	for(i=0; i < (size_EEVLANGroup+size_EEVLAN_ftth); i++)
#endif
	{
		EEPROM_Write_Byte(loc_EEVLANGroup+i,EE_Default.VLANGroup[i]);
	}

}

/******************************************************************************
*
*  Function:    Restore_EEPROM_to_Reg
*
*  Description: Set EEPROM value to Register
*
*  Parameters:  None
*
*  Returns:     None
*
*******************************************************************************/
#ifdef EE_SWITCH_178
void Restore_EEPROM_to_Reg()
{	u8_t	eep_val0,eep_val1;
	u16_t i;

	for(i=0;i<SWITCH_EEPROM_SIZE;i++)
	{
		if( (i>1)&&!(i%2) )
		{
    	   	EEPROM_Read_Byte(i  ,&eep_val0);
    	   	EEPROM_Read_Byte(i+1,&eep_val1);
		    Write_Reg(30+i/64,(i%64)/2 , ( ( (u16_t)eep_val1<<8 ) | eep_val0 ) );
		}
 	}
}
#endif

#endif


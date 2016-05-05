/******************************************************************************
*
*   Name:           ee_process.c
*
*   Description:    Process EEPROM content includes Default eeprom restore and
*                   eeprom check.
*   support functions:
*					1. u8_t EEPROMCheckDataAvailable(void) -->
*							NO EEPROM - EE_result=0
*							if (EE_result==1) Check if EEPROM with valid ID
*					3. void EEPROMInit(void) -->
*						    Enable EEPROM. call EEPROMCheckDataAvailable(void).
* 							If EE_result==0, load default uEEConfigInfo into
*							    working EEConfigInfo.
*							If EE_result==1 && ID is invalid,
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
//include data
#include "type_def.h"
#include "public.h"
#include "string.h"
#include "fm_web_com.h"
#include "eepdef.h"
#include "eeprom.h"
//*******************Function prototype area*********************

//*******************Global variable*********************
//---------------------------------------------------
//constants

//uEEConfigInfo EEConfigInfo;			// Global variable to save Network Info

u8_t EE_result;						// =0 eeprom error, =1 eeprom existed

//u8_t MODE_TYPE=0xff;    //Switch Mode=> bit 0:Fibet bit1:MII/RvMII
uEEConfigInfo EEConfigInfo;
uFW_EE_Info* ptr_EE_Info=&EEConfigInfo;
EE_Reserved_0[8] = {0x0, 0x1, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0};//EE_Reserved_0 area declaration
//u16_t loc_EEnetif=0x80;

/******************************************************************************
*
*  Function:    u8_t LoadEEPROM2MEMORY(void)
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
LoadEEPROM2MEMORY(void)
{
	u8_t 	i;
	u8_t 	*p;

	p=ptr_EE_Info;
    // Read in uFW_EE_Info
    for (i=0; i < sizeof (uFW_EE_Info); i++)
    {
    	if(EEPROM_Read_Byte(loc_EEnetif+i,(p+i)) == 0)
    	{
			TRACE_EEPROM(("ACCESS EEPROM ERROR"));
//printf(" access eeprom error!");
			return 0; 	// FAIL to Access EEPROM
    	}
//printf(" %02x",(u16_t)*(p+i));
	}
	return 1;
}

/******************************************************************************
*
*  Function:    u8_t EEPROMCheckDataAvailable(void)
*
*  Description: check the EEPROM Status :
*				1. Any EEPROM Access problem -
*				   EE_result =0 : Access error
*				   EE_result =1 : Access Successful
*				2. Check if EEPROM with valid ID
*				   Valid ID - return 1
*				   Invalid ID or EE_result=0 - return 0
*  Parameters:  none
*  Global Variables:
*				u8_t EE_result - =0 EEPROM Access Failed
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
    //check chip id dose match with default value
	for (i=0; i < 8; i++)
	{
        if( (i==2) || (i==3) || (i==6) || (i==7) )
            continue;
    	if(EEPROM_Read_Byte(i, &eebyte) == 0)
    	{
			TRACE_EEPROM(("ACCESS EEPROM ERROR"));
			EE_result=0;
			return 0; 	// FAIL to Access EEPROM
    	}
		//printf("\n\rCHIPID[%x]=%x ",(u16_t)i, (u16_t)eebyte);
   	    if (eebyte != EE_Reserved_0[i])
    	{
            EE_result=1;
			return 0;   // invalid ID
		}
	}
    EE_result=1;
    TRACE_EEPROM(("\n<-EEPROMCheckDataAvailable()~"));
	return 1; // EEPROM Access OK and ID Valid

}


u8_t EEWorkAroundV133(void)
{
  u8_t i, eebyte;
  u8_t IS_V133 = 1;
  u8_t EE_ID_V133[4] = {0x0, 0x1, 0x0, 0x1};

    //check EEPROM contents
	//check ID
	for (i=0; i < 4; i++)
	{
    	if(EEPROM_Read_Byte(i, &eebyte) == 0)
    	{
			TRACE_EEPROM(("ACCESS EEPROM ERROR"));
            IS_V133 = 0;
    	}
//		printf("\n\r  ee[%x]=%x ",(u16_t)i, (u16_t)eebyte);
   	    if (eebyte != EE_ID_V133[i])
    	{
            IS_V133 = 0;
		}
	}
	//check if the first 2 bytes of IP are IP mask
	for (i=0; i < 2; i++)
	{
    	if(EEPROM_Read_Byte(14+i, &eebyte) == 0)
    	{
			TRACE_EEPROM(("ACCESS EEPROM ERROR"));
            IS_V133 = 0;
    	}
//		printf("\n\r  ee[%x]=%x ",(u16_t)i, (u16_t)eebyte);
   	    if (eebyte != 255)
    	{
            IS_V133 = 0;
		}
	}
    //get ip info
	if(IS_V133 == 1)
	{
	    printf("\n V133 ");
	    //get MACID
	    for(i=0;i<6;i++)
		{
    	    EEPROM_Read_Byte(i+4, &eebyte);
            EEConfigInfo.MACID[i] = eebyte;
		}
	    //get IP
	    for(i=0;i<4;i++)
		{
    	    EEPROM_Read_Byte(i+10, &eebyte);
            EEConfigInfo.IPAddress[i] = eebyte;
		}
	    //get IP Mask
	    for(i=0;i<4;i++)
		{
    	    EEPROM_Read_Byte(i+14, &eebyte);
            EEConfigInfo.SubnetMask[i] = eebyte;
		}
	    //get Gateway
	    for(i=0;i<4;i++)
		{
    	    EEPROM_Read_Byte(i+18, &eebyte);
            EEConfigInfo.Gateway[i] = eebyte;
		}
	    //get DNS
	    for(i=0;i<4;i++)
		{
    	    EEPROM_Read_Byte(i+22, &eebyte);
            EEConfigInfo.DNSAddress[i] = eebyte;
		}
	}

	return IS_V133;
}


u8_t EEWorkAroundV135(void)
{
  u8_t i, eebyte;
  u8_t IS_V135 = 1;
  u8_t EE_ID_V135[8] = {0x0, 0x1, 0x00, 0x00, 0x0, 0x1, 0x00, 0x00};

    //check EEPROM contents
	//check ID
	for (i=0;i<8;i++)
	{
    	if(EEPROM_Read_Byte(i, &eebyte) == 0)
    	{
			TRACE_EEPROM(("ACCESS EEPROM ERROR"));
            IS_V135 = 0;
    	}
		printf("\n\r  ee[%x]=%x ",(u16_t)i, (u16_t)eebyte);
   	    if (eebyte != EE_ID_V135[i])
    	{
            IS_V135 = 0;
		}
	}

    //get ip info
	if(IS_V135 == 1)
	{
	    printf("\n V135 ");
	    //get MACID
	    for(i=0;i<6;i++)
		{
    	    EEPROM_Read_Byte(i+8, &eebyte);
            EEConfigInfo.MACID[i] = eebyte;
		}
	    //get IP
	    for(i=0;i<4;i++)
		{
    	    EEPROM_Read_Byte(i+14, &eebyte);
            EEConfigInfo.IPAddress[i] = eebyte;
		}
	    //get IP Mask
	    for(i=0;i<4;i++)
		{
    	    EEPROM_Read_Byte(i+18, &eebyte);
            EEConfigInfo.SubnetMask[i] = eebyte;
		}
	    //get Gateway
	    for(i=0;i<4;i++)
		{
    	    EEPROM_Read_Byte(i+22, &eebyte);
            EEConfigInfo.Gateway[i] = eebyte;
		}
	    //get DNS
	    for(i=0;i<4;i++)
		{
    	    EEPROM_Read_Byte(i+26, &eebyte);
            EEConfigInfo.DNSAddress[i] = eebyte;
		}
	}

	return IS_V135;
}
/******************************************************************************
*
*  Function:    EEMeoryInit
*
*  Description: Initial EEPROM data in memory 
*				Copy EEPROM Default value to EEPROM memory buffer
*
*  Parameters:  none
*
*  Returns: NONE
*
*******************************************************************************/
void EEMemoryInit(void)
{
	memcpy(ptr_EE_Info, &EE_Default, sizeof (uFW_EE_Info));
}
/******************************************************************************
*
*  Function:    EEPROMInit
*
*  Description: Enable EEPROM.
*				call EEPROMCheckDataAvailable(void).
*				If EE_result==0, load default uEEConfigInfo into working
*								 EEConfigInfo.
*				If EE_result==1 && ID is invalid, EEPROMLoadDefault(void)
                                 and load default uEEConfigInfo into working
*								 EEConfigInfo.
*				If EE_result==1 && ID is valid, load EEPROM uEEConfigInfo
*								 into working EEConfigInfo.
*
*  Parameters:  none
*
*  Returns: NONE
*
*******************************************************************************/
void EEPROMInit(void)
{
	u16_t i=0;
	u8_t eebyte=0;

    TRACE_EEPROM(("\n->EEPROMInit()\n~"));
    //Step 1
    EEPROM_Enable();
	//Step 2 copy default value to memory buffer
	EEMemoryInit();
    //check eeprom exist check switch id chip id app id
    if(EEPROMCheckDataAvailable())
    {    // EE_result==1 && ID is Valid           
	       //eep to mem
  	    printf("\n EEPROM check OK ");
	    //get MACID
	    for(i=0;i<6;i++)
		{
    	    EEPROM_Read_Byte(i+8, &eebyte);
            EEConfigInfo.MACID[i] = eebyte;
		}
	    //get IP
	    for(i=0;i<4;i++)
		{
    	    EEPROM_Read_Byte(i+14, &eebyte);
            EEConfigInfo.IPAddress[i] = eebyte;
		}
	    //get IP Mask
	    for(i=0;i<4;i++)
		{
    	    EEPROM_Read_Byte(i+18, &eebyte);
            EEConfigInfo.SubnetMask[i] = eebyte;
		}
	    //get Gateway
	    for(i=0;i<4;i++)
		{
    	    EEPROM_Read_Byte(i+22, &eebyte);
            EEConfigInfo.Gateway[i] = eebyte;
		}
	    //get DNS
	    for(i=0;i<4;i++)
		{
    	    EEPROM_Read_Byte(i+26, &eebyte);
            EEConfigInfo.DNSAddress[i] = eebyte;
		}
/*
		if(EEWorkAroundV133()==0)
		{
  	        printf("\n Not v133 ");
	    //Not v133 //get IP
            //check if v135
            if(EEWorkAroundV135() == 0)
            {//if not v135
    	        EEPROM_Read_Byte(2, &(EEConfigInfo.IPAddress[0]));
        	    EEPROM_Read_Byte(3, &(EEConfigInfo.IPAddress[1]));
        	    EEPROM_Read_Byte(6, &(EEConfigInfo.IPAddress[2]));
    	        EEPROM_Read_Byte(7, &(EEConfigInfo.IPAddress[3]));
    			EEPROM_Write_Byte(2, 0);
	    		EEPROM_Write_Byte(3, 0);
		     	EEPROM_Write_Byte(6, 0);
			    EEPROM_Write_Byte(7, 0);
            }
		}
//	       if(!LoadEEPROM2MEMORY())
//           {
//         		EE_result = 0; // EEPROM ACCESS Error                
//           }
	    //}
*/
	}
	else
	{
	    printf("\n EERPOM ID mismatch ");
	    //check and process if NetUART_v1.33
		EEWorkAroundV133();
	}
}


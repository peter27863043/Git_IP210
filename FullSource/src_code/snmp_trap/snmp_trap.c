/******************************************************************************
*
*   Name:           snmp_trap.c
*
*   Description:    SNMP Trap protocol
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
#include "module.h"
#ifdef MODULE_SNMP_TRAP
#include "snmp_trap.h"
#include "../net_uart/udp.h"
#include "eepdef.h"
#include "uip.h"
#include <string.h>

#define MIB_INTERNET              0x2b,0x06,0x01  //1.3.6.1
#define MIB_PRI_ICPLUS            0x04,0x01,0x80,0x71
#define MIB_PRI_ICPLUS_END        0x01,0x01
#define ENTERPRISE_SPECIFIC       0x06
#define SPECIAL_TRAP              0x00,0x11,0x31
//-----------------------------------------------------------------
#define SNMP_START                0x30  //0
#define SNMP_LEN                  0x00  

#define SNMP_VER_ID               0x02  //2
#define SNMP_VER_LEN              0x01  
#define SNMP_VER_STR              0x00  

#define SNMP_COM_ID               0x04  //5
#define SNMP_COM_LEN              0x06  
#define SNMP_COM_STR              'p','u','b','l','i','c'

#define SNMP_PDU_TYPE_TRAP        0xa4  //13
#define SNMP_PDU_LEN              0x00

#define SNMP_ENT_ID               0x06  //15
#define SNMP_ENT_LEN              0x09
#define SNMP_ENT_STR              MIB_INTERNET,MIB_PRI_ICPLUS,MIB_PRI_ICPLUS_END

#define SNMP_AGN_IP_ID            0x40
#define SNMP_AGN_IP_LEN           0x04  //18+SNMP_ENT_LEN
#define SNMP_AGN_IP_STR           0x00,0x00,0x00,0x00

#define SNMP_GEN_TRAP_ID          0x02
#define SNMP_GEN_TRAP_LEN         0x01
#define SNMP_GEN_TRAP_STR         ENTERPRISE_SPECIFIC

#define SNMP_SPC_TRAP_ID          0x02  
#define SNMP_SPC_TRAP_LEN         0x03
#define SNMP_SPC_TRAP_STR         SPECIAL_TRAP

#define SNMP_TIME_ID              0x43
#define SNMP_TIME_LEN             0x01
#define SNMP_TIME_STR             0x00

#define SNMP_SEQ_VARBIND_LIST_ID  0x30
#define SNMP_SEQ_VARBIND_LIST_LEN 0x00

#define SNMP_SEQ_VARBIND_1_ID     0x30
#define SNMP_SEQ_VARBIND_1_LEN    0x00

#define SNMP_ODI_1_NAME_ID        0x06
#define SNMP_ODI_1_NAME_LEN       0x07
#define SNMP_ODI_1_NAME_STR       MIB_INTERNET,MIB_PRI_ICPLUS

#define SNMP_ODI_1_VALUE_ID       0x04
#define SNMP_ODI_1_VALUE_LEN      0x00
#define SNMP_ODI_1_VALUE_STR      0x00



#define loc_SNMP_LEN                  01
#define loc_SNMP_PDU_LEN              14
#define loc_SNMP_ENT_ID				  15
#define loc_SNMP_ENT_LEN              16
#define loc_SNMP_AGN_IP_LEN           (loc_SNMP_ENT_LEN     +2+SNMP_ENT_LEN)
#define loc_SNMP_AGN_IP_STR           (loc_SNMP_AGN_IP_LEN  +1)
#define loc_SNMP_GEN_TRAP_LEN         (loc_SNMP_AGN_IP_LEN  +2+SNMP_AGN_IP_LEN)
#define loc_SNMP_SPC_TRAP_LEN         (loc_SNMP_GEN_TRAP_LEN+2+SNMP_GEN_TRAP_LEN)
#define loc_SNMP_TIME_LEN             (loc_SNMP_SPC_TRAP_LEN+2+SNMP_SPC_TRAP_LEN)
#define loc_SNMP_SEQ_VARBIND_LIST_LEN (loc_SNMP_TIME_LEN    +2+SNMP_TIME_LEN)
#define loc_SNMP_SEQ_VARBIND_1_LEN    (loc_SNMP_SEQ_VARBIND_LIST_LEN+2)
#define loc_SNMP_ODI_1_NAME_LEN       (loc_SNMP_SEQ_VARBIND_1_LEN+2)
#define loc_SNMP_ODI_1_VALUE_LEN      (loc_SNMP_ODI_1_NAME_LEN+2+SNMP_ODI_1_NAME_LEN)
#define loc_SNMP_ODI_1_VALUE_STR      (loc_SNMP_ODI_1_VALUE_LEN+1)

/******************************************************************************
*
*  Function:    SNMP_TRAP
*
*  Description: SNMP TRAP Protocol
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void SNMP_TRAP()
{
	u8_t* pSNMP=UdpData.p_DataPkt->udpload;
	char SNMP_TRAP_TEMP[]={	
		SNMP_START,SNMP_LEN,
		SNMP_VER_ID,SNMP_VER_LEN,SNMP_VER_STR,
		SNMP_COM_ID,SNMP_COM_LEN,SNMP_COM_STR,
		SNMP_PDU_TYPE_TRAP,SNMP_PDU_LEN,
		SNMP_ENT_ID,SNMP_ENT_LEN,SNMP_ENT_STR,
		SNMP_AGN_IP_ID,SNMP_AGN_IP_LEN,SNMP_AGN_IP_STR,
		SNMP_GEN_TRAP_ID,SNMP_GEN_TRAP_LEN,SNMP_GEN_TRAP_STR,
		SNMP_SPC_TRAP_ID,SNMP_SPC_TRAP_LEN,SNMP_SPC_TRAP_STR,
		SNMP_TIME_ID,SNMP_TIME_LEN,SNMP_TIME_STR,
		SNMP_SEQ_VARBIND_LIST_ID,SNMP_SEQ_VARBIND_LIST_LEN,
		SNMP_SEQ_VARBIND_1_ID,SNMP_SEQ_VARBIND_1_LEN,
		SNMP_ODI_1_NAME_ID,SNMP_ODI_1_NAME_LEN,SNMP_ODI_1_NAME_STR,
		SNMP_ODI_1_VALUE_ID,SNMP_ODI_1_VALUE_LEN,SNMP_ODI_1_VALUE_STR
	};
	 memcpy(pSNMP,SNMP_TRAP_TEMP,sizeof(SNMP_TRAP_TEMP));
	 
	 ((u16_t*)(pSNMP+loc_SNMP_AGN_IP_STR))[0]=uip_hostaddr[0];
	 ((u16_t*)(pSNMP+loc_SNMP_AGN_IP_STR))[1]=uip_hostaddr[1];
	 
	 pSNMP[loc_SNMP_ODI_1_VALUE_LEN]=strlen(EEConfigInfo.Comment);
	 memcpy(pSNMP+loc_SNMP_ODI_1_VALUE_STR,EEConfigInfo.Comment,pSNMP[loc_SNMP_ODI_1_VALUE_LEN]);
	 
	 pSNMP[loc_SNMP_SEQ_VARBIND_1_LEN]=pSNMP[loc_SNMP_ODI_1_NAME_LEN]+4+pSNMP[loc_SNMP_ODI_1_VALUE_LEN];
	 
	 pSNMP[loc_SNMP_SEQ_VARBIND_LIST_LEN]=pSNMP[loc_SNMP_SEQ_VARBIND_1_LEN]+2;
	 
	 pSNMP[loc_SNMP_LEN]=sizeof(SNMP_TRAP_TEMP)+pSNMP[loc_SNMP_ODI_1_VALUE_LEN]-1-2;
	 
	 pSNMP[loc_SNMP_PDU_LEN]=pSNMP[loc_SNMP_LEN]-loc_SNMP_ENT_ID;
	 	 
 	 //UDP settings:
   	 UdpData.SrcIPAddr[0] =uip_hostaddr[0];
  	 UdpData.SrcIPAddr[1] =uip_hostaddr[1];
  	 UdpData.DestIPAddr[0]=0xffff;
	 UdpData.DestIPAddr[1]=0xffff;
	 UdpData.SrcPort=162;
	 UdpData.DestPort=1001;
	 udp_send(pSNMP[loc_SNMP_LEN]+2);
}

#endif //MODULE_SNMP_TRAP

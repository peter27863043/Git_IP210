/******************************************************************************
*
*   Name:           net_cmd.c
*
*   Description:    Net command protocol
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
#include "net.h"
#include "net_cmd.h"
/******************************************************************************
*
*  Function:    LocalCmd
*
*  Description: LocalCmd process function
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void LocalCmd()
{
//  u16_t pkt_len=uip_len;
    uip_len=0;
#ifdef LocalCmd_debug
           printf("LocalCmd");
#endif
	if(LCmd->EtherType!=ETHER_TYPE_ARP){return;}
#ifdef LocalCmd_debug
           printf("ETHER_TYPE_ARP");
#endif
	if(LCmd->Protocol!=ARP_USER_DEFINE){return;}
#ifdef LocalCmd_debug
           printf("ARP_USER_DEFINE");
#endif
	if(LCmd->HWType!=ARP_HARDWARE_TYPE){return;}
#ifdef LocalCmd_debug
           printf("ARP_HARDWARE_TYPE");
#endif
	NetCmd();
}
/******************************************************************************
*
*  Function:    LocalCmdTxPrefix
*
*  Description: Init LocalCmd Transmit packet
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void LocalCmdTxPrefix()
{
	u8_t i;
	for(i=0;i<6;i++)
	{
		LCmd->DA[i]=LCmd->SA[i];
		LCmd->SA[i]=uip_ethaddr.addr[i];
	}
    LCmd->EtherType=0x0806;
	LCmd->Protocol=0xf000;
	LCmd->HWType=1;

	uip_len=LCMD_CmdData_OFFSET;
}
/******************************************************************************
*
*  Function:    NetCmd
*
*  Description: NetCmd process function. After NetCmdOnTCPRx() and LocalCmd, 
*               this function will be call.
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void NetCmd()
{
	u8_t *ProjectName,*option,i;	
	SSearchDevAck* SearchDevAck;
	if(LNetCmdPkt->CmdOP==SEARCH_DEVICE_REQ)
	{
			LocalCmdTxPrefix();

			SearchDevAck=(SSearchDevAck*)LNetCmdPkt->CmdData;
			LNetCmdPkt->CmdOP=SEARCH_DEVICE_ACK;
			LNetCmdPkt->MajorVersion=1;
			LNetCmdPkt->MinorVersion=0;
			SearchDevAck->DevID=GetDeviceID();
			for(i=0;i<6;i++)SearchDevAck->MAC[i]=uip_ethaddr.addr[i];
			SearchDevAck->IP[0]=uip_hostaddr[0];
			SearchDevAck->IP[1]=uip_hostaddr[1];
			SearchDevAck->Netmask[0]=uip_arp_netmask[0];
			SearchDevAck->Netmask[1]=uip_arp_netmask[1];
			SearchDevAck->Getway[0]=uip_arp_draddr[0];
			SearchDevAck->Getway[1]=uip_arp_draddr[1];
			uip_len+=LCMD_DEV_ACK_NICKNAME_OFFSET;
			strcpy(SearchDevAck->Nickname,"IP210 FIRMWARE UPDATE");
			uip_len+=strlen(SearchDevAck->Nickname)+1;//include '\0'
			ProjectName=SearchDevAck->Nickname+strlen(SearchDevAck->Nickname)+1;
			GetProjectName(ProjectName);
			uip_len+=strlen(ProjectName)+1;//include '\0'
			option=ProjectName+strlen(ProjectName)+1;
			i=0;
			uip_len+=i;
			LNetCmdPkt->CmdLen=uip_len-sizeof(SLCmd);		
	}
}
void GetProjectName(char* ProjectName)
{
	strcpy(ProjectName,"1.0.0");
}
u16_t GetDeviceID()
{
	return 0x01;
}

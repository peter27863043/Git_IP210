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
#include "net_cmd.h"
#include "net_uart.h"
#include "option.h"
#ifdef MODULE_SIGNAL_GPIO
	#include "dido.h"
#endif
#ifdef MODULE_NET_CMD
/*Support Command list:*/
//#define SUPPORT_GPIO_COMMAND

#include "udp.h" //joe 20080702 NetCmdonUDP
#include "uip.h"
#include "eepdef.h"
#include "dhcpc.h"
#include "timer.h"
#include<string.h>
#include<stdio.h>
#include "eeprom.h"
#ifdef SUPPORT_GPIO_COMMAND
#include "../net_uart/gpio.h"
#endif //SUPPORT_GPIO_COMMAND

#ifdef MODULE_NET_CMD_EEP_CFG
#include "net_cmd_eep_cfg.h"
#endif //MODULE_NET_CMD_EEP_CFG

#ifndef MODULE_NET_UART
SNetUARTData NetUARTData;
CTelnetData TelnetData;
#endif

extern void udp_send(u16_t length); //joe 20080702 NetCmdonUDP
extern void WatchDogTimerReset();
#ifdef MODULE_SIGNAL_GPIO
	extern u8_t current_channel;
	extern u16_t adc_convert(void);
#endif
void MPProcess(void);
#include<public.h>
#include <stdlib.h>
void NetCmd();
//encryption
u16_t GetDeviceID();
u32_t CRC_operation();
u32_t crc(u8_t *ip, u32_t AuthenticKey);
u8_t crc_buf[16]=0;
void CRC_info();
#define WIDTH	32
//login:
void OnLoginRequest();
void OnLoginAccount();
void SendLoginMD5Key(u8_t Result);
void SendLoginResult(u8_t result,u32_t ConnID);
//GPIO:
void OnAccessGPIOReq();
void SendAccessGPIOAck();
void eep_read_req();
void eep_write_req();
void reset_device_req();
#ifdef MODULE_FLASH512 
//HW Loop Back
void en_loopback();
void dis_loopback();
extern u8_t lb_flag;
#endif
extern u8_t force_reset_countdown;
#ifdef MODULE_SIGNAL_GPIO
	void OngpioRequest();
	void OnchannelRequest();
	void OngpioRequestAck(u8_t gpioresult, u8_t gpiovalue);
	void OnchannelRequestAck(u8_t channel_result, u8_t old_value, u16_t channel_value);
#endif
//==========================================================================
#define LCmd ((SLCmd*)uip_buf)
#define PktSA (uip_buf+6)
#define LNetCmdPkt ((SNetCmdPkt*)(uip_buf+LOCAL_CMD_ARP_HEADER_SIZE))
#define TNetCmdPkt ((SNetCmdPkt*)uip_appdata)
#define	UNetCmdPkt ((SNetCmdPkt*)UdpData.p_DataPkt->udpload)	//joe 20080702 NetCmdonUDP
SNetCmdData NetCmdData;
SSearchAckOP* SearchAckOP;

/******************************************************************************
*
*  Function:    NetCmdInit
*
*  Description: Init net command module
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void NetCmdInit()
{
	u8_t i;
	for(i=0;i<MAX_NET_CMD_CONN;i++)
	{
		NetCmdData.NetCmdConn[i].ConnID=EMPTY_CONN_ID;
	}
	NetCmdData.TCmdData.Port=99;
	uip_listen(NetCmdData.TCmdData.Port);
#ifdef MODULE_NET_CMD_EEP_CFG	
	init_eep_cfg();
#endif //MODULE_NET_CMD_EEP_CFG	
}

/******************************************************************************
*
*  Function:    FindSA
*
*  Description: Get Connection Array Index by source address
*               
*  Parameters:  SA: source address
*               StartIdx: Start Array Index
*  Returns:     CONN_ID_NOT_FIND or connection ID
*               
*******************************************************************************/
u8_t FindSA(u8_t* SA,u8_t StartIdx)
{
	u8_t i,j;
	for(i=StartIdx;i<MAX_NET_CMD_CONN;i++)
	{
		for(j=0;j<6;j++)
		{
			if(SA[j]!=NetCmdData.NetCmdConn[i].SA[j])break;
		}
		if(j==6)return i;
	}
	return CONN_ID_NOT_FIND;
}

/******************************************************************************
*
*  Function:    CheckConnID
*
*  Description: Check if this connect had be established
*               
*  Parameters:  
*               
*  Returns:     CONN_ID_NOT_FIND or connection ID
*               
*******************************************************************************/
u8_t CheckConnID()
{
	u8_t ConnIdx=0;
	ConnIdx=FindSA(PktSA,ConnIdx);
	while(ConnIdx!=CONN_ID_NOT_FIND)
	{	
		if(NetCmdData.NetCmdConn[ConnIdx].ConnID==NetCmdData.NetCmdPkt->ConnID)
		{
			return ConnIdx;
		}
		ConnIdx=FindSA(PktSA,ConnIdx+1);
	}
	return CONN_ID_NOT_FIND;
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
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
	uip_len=LCMD_CmdData_OFFSET;
}

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
    u16_t pkt_len=uip_len;
    uip_len=0;
	if(LCmd->EtherType!=ETHER_TYPE_ARP)return;
	if(LCmd->Protocol!=ARP_USER_DEFINE)return;
	if(LCmd->HWType!=ARP_HARDWARE_TYPE)return;

	NetCmdData.CurrentPktType=TypeLCmd;
	NetCmdData.NetCmdPkt=LNetCmdPkt;
	NetCmd();
}

/******************************************************************************
*
*  Function:    NetCmdOnTCPRx
*
*  Description: NetCmd process function, callback function for uip
*               
*  Parameters:  
*               
*  Returns:     1: This is my packet, and had processed
*               0: Not my packet
*               
*******************************************************************************/
u8_t NetCmdOnTCPRx()
{
  if(uip_conn->lport==NetCmdData.TCmdData.Port)
  {
  	  NetCmdData.CurrentPktType=TypeTCmd;  	
	  NetCmdData.NetCmdPkt=TNetCmdPkt;
/*     if (uip_connected()) {	

	   }
*/

/*	   if (uip_acked()) {

      }
*/
  
      if (uip_newdata()) {
      	printf(".");//jc_db
         NetCmd();
      }
  
/*USE: Retransmit by PC-site application
	  if (uip_rexmit()) {
		
      }
*/

/*  
      if (uip_newdata() || uip_acked()) {
    	
      } else if(uip_poll()) {

      }
*/

/*USE: Clear by AgingNetCmdConnTable
      if (uip_closed() || uip_aborted()) {
      }
*/
        		
  		return 1;
  }
  return 0;
}

/******************************************************************************
*
*  Function:    NetCmdSend
*
*  Description: Send NetCmd packet out
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void NetCmdSend()
{
	if(EEConfigInfo.DHCPEnable!=sDHCP_DISABLE)	//joe 20081117 for can't get DHCP IP we use hostaddr
	{
		if(UdpData.SrcIPAddr[0]==0)
		{
			UdpData.SrcIPAddr[0]=uip_hostaddr[0];
	  		UdpData.SrcIPAddr[1]=uip_hostaddr[1];
		}
	}
	NetCmdData.NetCmdPkt->CmdLen=uip_len;
	if(NetCmdData.CurrentPktType==TypeUCmd){ udp_send(uip_len); return;	} //joe 20080702 NetCmdonUDP
	if(NetCmdData.CurrentPktType==TypeLCmd){	uip_len+=LOCAL_CMD_ARP_HEADER_SIZE;	}
	uip_slen=uip_len;
}


/******************************************************************************
*
*  Function:    CRC operation
*
*  Description: pass Target IP and AuthenticKey get the CRC result
*               
*  Parameters:  
*               
*  Returns:    CRC result
*              
*               
*******************************************************************************/
/***********************************************************
0x80, 0x00, 0x00, 0x00	//For Planet
***********************************************************/
u32_t CRC_operation()
{
	u8_t i=0;
	u32_t key=0;
	u8_t vendor[4]={0x13, 0xF0, 0x80, 0x40};	//For Standard

	typedef struct _SCRCcheck
	{
		u16_t	Devid;
		u8_t	OP;
		u8_t	len;
		u8_t	key[4];
	}SCRCcheck;

	SCRCcheck* CRCcheck=(SCRCcheck*)NetCmdData.NetCmdPkt->CmdData;

	if(CRCcheck->OP==6)
	{
		for(i=0;i<4;i++)
		{
			key=(key<<8)|*(CRCcheck->key+i);
		}
		CRC_info();

		memcpy(&crc_buf[4], vendor, 4);

		return crc(crc_buf, key);
	}
	return 0;
}

u32_t crc(u8_t *ip, u32_t AuthenticKey)
{
	u8_t i, j; 
	u32_t remainder = AuthenticKey;

    for(i=0;i<8;++i) //Data num
	{
		remainder^=( (u32_t)ip[i]<<(WIDTH-8) );
		for(j=8;j>0;--j)
		{
	        if(remainder&0x80000000)
				remainder=(remainder<<1)^AuthenticKey;
        	else
				remainder=(remainder<<1);
        }
	}
    return remainder;
}
/******************************************************************************
*
*  Function:    CRC info
*
*  Description: CRC operation information
*               
*  Parameters:  
*               
*  Returns:  	   
*               
*******************************************************************************/
void CRC_info()
{
	u8_t i;

	for(i=0;i<4;i++)
	{ crc_buf[i]=((u8_t*)uip_hostaddr)[i]; }

	for(i=0;i<4;i++)
	{ crc_buf[4+i]=((u8_t*)uip_arp_netmask)[i]; }

	for(i=0;i<4;i++)
	{ crc_buf[8+i]=((u8_t*)uip_arp_draddr)[i]; }

	for(i=0;i<4;i++)
	{ crc_buf[12+i]=((u8_t*)uip_dnsaddr)[i]; }
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
#define UDP_HEAD_LEN	4
#ifdef MODULE_FLASH512
	#define OP_COUNT		6
#else
	#define OP_COUNT		4	//no include version info
#endif
void NetCmd()
{
	u8_t i,ConnIndex,j=0,count=0,index=0,offset=0;
	u8_t* ptr=(u8_t*)ModuleInfo.UDPTelInfo.remote;
	u8_t re_ip[4]=0;
	u8_t *v_info=WEB_CODE_VERSION;
	u8_t v_len=0;
#ifdef MODULE_FLASH512
	u8_t name_buff[31]=0;
	u8_t ee_value;
#endif
	u32_t result=0;
	

	result=CRC_operation();

	if(NetCmdData.NetCmdPkt->CmdOP>=NOT_NEED_LOGIN_OP)
	{
		ConnIndex=CheckConnID();
		if(ConnIndex==CONN_ID_NOT_FIND)
		{
		    //send Connection invalid ack packet
			NetCmdData.NetCmdPkt->CmdOP=CONN_INVALID_ACK;
			uip_len=sizeof(SNetCmdPkt)-1;
			NetCmdSend();
			return;	
		}
	}
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
	switch(NetCmdData.NetCmdPkt->CmdOP)
	{
		case SEARCH_DEVICE_REQ:
			uip_len=sizeof(SNetCmdPkt)-1;	//joe debug for search device 20081103
			if(NetCmdData.CurrentPktType!=TypeUCmd)	//joe 20080702 NetCmdonUDP
			{LocalCmdTxPrefix();}
			{
				char *ProjectName,*option;
				SSearchDevAck* SearchDevAck=(SSearchDevAck*)NetCmdData.NetCmdPkt->CmdData;
				NetCmdData.NetCmdPkt->CmdOP=SEARCH_DEVICE_ACK;
				NetCmdData.NetCmdPkt->MajorVersion=1;
				NetCmdData.NetCmdPkt->MinorVersion=0;
				SearchDevAck->DevID=GetDeviceID();
				for(i=0;i<6;i++)SearchDevAck->MAC[i]=uip_ethaddr.addr[i];
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
				SearchDevAck->SIP[0]=uip_hostaddr[0];
				SearchDevAck->SIP[1]=uip_hostaddr[1];
				SearchDevAck->Netmask[0]=uip_arp_netmask[0];
				SearchDevAck->Netmask[1]=uip_arp_netmask[1];
				SearchDevAck->Getway[0]=uip_arp_draddr[0];
				SearchDevAck->Getway[1]=uip_arp_draddr[1];
				uip_len+=LCMD_DEV_ACK_NICKNAME_OFFSET;
				strcpy(SearchDevAck->Nickname,EEConfigInfo.Comment);
				uip_len+=strlen(SearchDevAck->Nickname)+1;//include '\0'
				ProjectName=SearchDevAck->Nickname+strlen(SearchDevAck->Nickname)+1;
				strcpy(ProjectName,"NetUART");
				uip_len+=strlen(ProjectName)+1;//include '\0'
				option=ProjectName+strlen(ProjectName)+1;
	
				SearchAckOP=(SSearchAckOP*)option;
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif	
				SearchAckOP->mode_op=4;
				SearchAckOP->mode_len=1;
				SearchAckOP->mode_id=TelnetData.pEEPROM_Telnet->op_mode;
#ifdef MODULE_FLASH512
				SearchAckOP->ctrl_op=5;
				SearchAckOP->ctrl_len=2;
				SearchAckOP->ctrl_port=NetUARTData.pTelnetData->pEEPROM_Telnet->ctrl_port;
#endif
#ifdef MODULE_VLAN_TAG
				SearchAckOP->vlan_op=8;
				SearchAckOP->vlan_len=3;
				SearchAckOP->vlan_en=ModuleInfo.VLANINFO.vlan;
				SearchAckOP->vlan_id[0]=ModuleInfo.VLANINFO.value_id[0];
				SearchAckOP->vlan_id[1]=ModuleInfo.VLANINFO.value_id[1];
#endif
				SearchAckOP->tcmd_op=2;
				SearchAckOP->tcmd_len=2;
				SearchAckOP->tcmd_port=NetCmdData.TCmdData.Port;
	
				SearchAckOP->tel_op=1;
				SearchAckOP->tel_len=2;
				SearchAckOP->tel_port=NetUARTData.pTelnetData->pEEPROM_Telnet->TelentPort;
	
				SearchAckOP->udp_op=3;
				SearchAckOP->udp_lp=ModuleInfo.UDPTelInfo.local_port;
#ifdef MODULE_FLASH512
				SearchAckOP->udp_len=0;
				for(i=0;i<10;i++)
				{
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
					if(ModuleInfo.dido_info.remote[i].ip_demo==0)	//IP
					{
						name_buff[0]=0;
						for(j=0;j<4;j++)
						{
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
							EEPROM_Read_Byte(loc_EEUDPTelInfo+(2+i*6+j) , &ee_value);
							*(SearchAckOP->ip_port+SearchAckOP->udp_len+j+1)=ee_value;
							if(ee_value!=0){ name_buff[0]=1; }
						}
						*(SearchAckOP->ip_port+SearchAckOP->udp_len)=0;	//Type
						EEPROM_Read_Byte(loc_EEUDPTelInfo+(2+i*6+4) , &ee_value);
						*(SearchAckOP->ip_port+SearchAckOP->udp_len+5)=ee_value;
						EEPROM_Read_Byte(loc_EEUDPTelInfo+(2+i*6+5) , &ee_value);
						*(SearchAckOP->ip_port+SearchAckOP->udp_len+6)=ee_value;
						if(name_buff[0]==1)
						{
							SearchAckOP->udp_len+=7;
							count++;
						}
					}
					else
					{
						memset(name_buff, 0, 31);
						for(j=0;j<30;j++)
						{
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
							EEPROM_Read_Byte(loc_EEDIDOInfo+(8+(u16_t)i*31+j) , &name_buff[j]);
						}
						if(strlen(name_buff)>1)
						{
							*(SearchAckOP->ip_port+SearchAckOP->udp_len)=1;	//Type
							memcpy((SearchAckOP->ip_port+SearchAckOP->udp_len+1), name_buff, strlen(name_buff));
							*(SearchAckOP->ip_port+SearchAckOP->udp_len+strlen(name_buff)+1)=0;
							SearchAckOP->udp_len+=strlen(name_buff)+2;
							EEPROM_Read_Byte(loc_EEUDPTelInfo+(2+i*6+4) , &ee_value);
							*(SearchAckOP->ip_port+SearchAckOP->udp_len)=ee_value;
							EEPROM_Read_Byte(loc_EEUDPTelInfo+(2+i*6+5) , &ee_value);
							*(SearchAckOP->ip_port+SearchAckOP->udp_len+1)=ee_value;
							SearchAckOP->udp_len+=2;
							count++;
						}
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
					}
				}
				SearchAckOP->udp_rp=count;
#else
				for(i=0;i<50;i++)
				{
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
					index=i/5;
					offset=i%5;
					if(offset!=4)	//ip
					{
						re_ip[offset]=ptr[index*6+offset];
						if(re_ip[offset]==0)
						{	j++;	}
					}
					else		//port
					{
						if(j!=4)
						{
							for(j=0;j<4;j++)
							{	*(SearchAckOP->ip_port+(j+(count*6)))=ptr[index*6+j];	}
							*(SearchAckOP->ip_port+j+(count*6))=(*(u16_t*)(ptr+index*6+j))>>8&0xff;
							*(SearchAckOP->ip_port+j+1+(count*6))=(*(u16_t*)(ptr+index*6+j))&0xff;
							count++;
						}
						j=0;
					}
				}
				SearchAckOP->udp_rp=count;
				SearchAckOP->udp_len=(count*6);
#endif
//------------------------------------Versino info--------------------------------------
#ifdef MODULE_FLASH512
				*(SearchAckOP->ip_port+SearchAckOP->udp_len)=9;
				v_len=strlen(v_info);
				*(SearchAckOP->ip_port+SearchAckOP->udp_len+1)=v_len+1;	//string length
				for(i=0;i<v_len;i++)
				{ *(SearchAckOP->ip_port+SearchAckOP->udp_len+2+i)=*(v_info+i); }
				*(SearchAckOP->ip_port+SearchAckOP->udp_len+2+v_len)=0;	//end string
				v_len+=3;
#endif
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
//------------------------------------CRC Check-----------------------------------------
				if(result!=0)
				{
					SearchAckOP->op_num=OP_COUNT+1;
					*(SearchAckOP->ip_port+SearchAckOP->udp_len+v_len)=7;
					*(SearchAckOP->ip_port+SearchAckOP->udp_len+v_len+1)=4;
					for(i=0;i<4;i++)
					{	*(SearchAckOP->ip_port+SearchAckOP->udp_len+v_len+2+i)=(result>>8*(3-i))&0xff;	}
//					*(SearchAckOP->ip_port+SearchAckOP->udp_len+6)=0;	//End of Packet
					uip_len+=(sizeof(SSearchAckOP)-1)+SearchAckOP->udp_len+6+v_len;
					SearchAckOP->udp_len+=UDP_HEAD_LEN;
				}
				else
				{
					SearchAckOP->op_num=OP_COUNT;
//					*(SearchAckOP->ip_port+SearchAckOP->udp_len)=0;	//End of Packet
					uip_len+=(sizeof(SSearchAckOP)-1)+SearchAckOP->udp_len+v_len;
					SearchAckOP->udp_len+=UDP_HEAD_LEN;
				}
//--------------------------------------------------------------------------------------
#ifdef MODULE_VLAN_TAG
				SearchAckOP->op_num+=1;
#endif
				NetCmdData.NetCmdPkt->CmdLen=uip_len;
	
				if(NetCmdData.CurrentPktType!=TypeUCmd) //joe 20080702 NetCmdonUDP
				{NetCmdData.NetCmdPkt->CmdLen=uip_len-sizeof(SLCmd);}
			}
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
			if(NetCmdData.CurrentPktType==TypeUCmd) //joe 20080702 NetCmdonUDP
			{NetCmdSend();}
			break;
		case LOGIN_REQUEST:
			OnLoginRequest();
			break;
		case LOGIN_ACCOUNT:
			OnLoginAccount();
			break;
		case ACCESS_GPIO_REQ:
			OnAccessGPIOReq();
			break;
		case MP_PROCESS_REQ:
			MPProcess();
			break;
#ifdef MODULE_NET_CMD_EEP_CFG	
		case WOWNLOAD_EEP_REQ: got_download_eep_req(); break;
	    case UPLOAD_EEP_REQ:   got_upload_eep_req();   break;
    	case LOAD_DEF_EEP_REQ: got_load_def_eep_req(); break;
#endif //MODULE_NET_CMD_EEP_CFG	
#ifdef MODULE_SIGNAL_GPIO		
		case SINGLE_PIN_GPIO_REQ:
			OngpioRequest();
			break;
		case SINGLE_CHANNEL_REQ:
			OnchannelRequest();
			break;
#endif
//-------------------------EEPRom Read & Write---------------------20081021
		case EEP_READ_REQ:
			eep_read_req();
			break;
		case EEP_WRITE_REQ:
			eep_write_req();
			break;
//-------------------------------------------------------------------------
		case RESET_DEVICE_REQ:
			reset_device_req();
			break;
#ifdef MODULE_FLASH512
//--------------------HW Loop Back-----------------------------------------
		case EN_LOOP_REQ:
			en_loopback();
			break;
		case DIS_LOOP_REQ:
			dis_loopback();
			break;
#endif
	}
}

/******************************************************************************
*
*  Function:    SearchEmptyConnTable
*
*  Description: Search empty space of connection table 
*               
*  Parameters:  
*               
*  Returns:     CONN_ID_NOT_FIND: space full
*               other value:      empty index
*               
*******************************************************************************/
u8_t SearchEmptyConnTable()
{
	u8_t SA[6];
	memset(SA,0,6);
	return FindSA(SA,0);
}

/******************************************************************************
*
*  Function:    OnLoginRequest
*
*  Description: Process Login Request packet
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void OnLoginRequest()
{
	u8_t ConnIdx=0;
	u8_t Result=0;//success
	//Check if login ID exist
	ConnIdx=FindSA(PktSA,ConnIdx);
	while(ConnIdx!=CONN_ID_NOT_FIND)
	{
		if(NetCmdData.NetCmdConn[ConnIdx].LoginID==*((u32_t*)NetCmdData.NetCmdPkt->CmdData))break;
		ConnIdx=FindSA(PktSA,ConnIdx+1);
	}
	if(ConnIdx==CONN_ID_NOT_FIND)
	{
		//find a empty conn for use
		ConnIdx=SearchEmptyConnTable();
		if(ConnIdx==SearchEmptyConnTable()==CONN_FULL)
		{
			Result=1;
		}
		else
		{
			//Save SA to this conn
			memcpy(NetCmdData.NetCmdConn[ConnIdx].SA,PktSA,6);
			//Save LoginID to this conn
			NetCmdData.NetCmdConn[ConnIdx].LoginID=*((u32_t*)NetCmdData.NetCmdPkt->CmdData);
			//SaveTime
			IP210_Update_Timer_Counter();
			NetCmdData.NetCmdConn[ConnIdx].Time=timercounter;
		}
	}
	else
	{
		//aready exist, update information
		//Save LoginID to this conn
		NetCmdData.NetCmdConn[ConnIdx].LoginID=*((u32_t*)NetCmdData.NetCmdPkt->CmdData);
		//SaveTime
		IP210_Update_Timer_Counter();
		NetCmdData.NetCmdConn[ConnIdx].Time=timercounter;
	}
	//Send MD5 Key reply
	SendLoginMD5Key(Result);
}

/******************************************************************************
*
*  Function:    SendLoginMD5Key
*
*  Description: send LoginMD5key packet out
*               
*  Parameters:  Result: Login result
*               
*  Returns:     
*               
*******************************************************************************/
void SendLoginMD5Key(u8_t Result)
{
	typedef struct _SMD5PKT{
		u32_t LoginID;
		u8_t  Result;
		u8_t  MD5Len;
		u8_t  MD5_KEY[1];
	}SMD5Pkt;
 
	SMD5Pkt* MD5Pkt=(SMD5Pkt*)NetCmdData.NetCmdPkt->CmdData;
	NetCmdData.NetCmdPkt->CmdOP=LOGIN_MD5_KEY;
	MD5Pkt->Result=Result;
	MD5Pkt->MD5Len=0;//Not need MD5
	uip_len=((sizeof(SMD5Pkt)-1)+MD5Pkt->MD5Len)+(sizeof(SNetCmdPkt)-1);
	NetCmdSend();
}

/******************************************************************************
*
*  Function:    GenConnID
*
*  Description: Generate a unique connection ID
*               
*  Parameters:  
*               
*  Returns:     Connection ID
*               
*******************************************************************************/
u32_t GenConnID()
{
	u32_t ConnID;
	u16_t *p16ConnID=&ConnID;
	u8_t i;
	while(1)
	{
		p16ConnID[0]=rand();
		p16ConnID[1]=rand();
		for(i=0;i<MAX_NET_CMD_CONN;i++)
		{
			if(NetCmdData.NetCmdConn[i].ConnID==ConnID)break;
		}
		if(i==MAX_NET_CMD_CONN)return ConnID;
	}
}

/******************************************************************************
*
*  Function:    SendLoginResult
*
*  Description: Send login result
*               
*  Parameters:  result: login result
*               ConnID: Connection ID for Client
*  Returns:     
*               
*******************************************************************************/
void SendLoginResult(u8_t result,u32_t ConnID)
{
	typedef struct _SLoginResult
	{
		u32_t LoginID;
		u8_t  Result;
	}SLoginResult;
	SLoginResult* LoginResult=(SLoginResult*)NetCmdData.NetCmdPkt->CmdData;
	NetCmdData.NetCmdPkt->ConnID=ConnID;
	NetCmdData.NetCmdPkt->CmdOP=LOGIN_RESULT;
	LoginResult->Result=result;
	uip_len=sizeof(SLoginResult)+(sizeof(SNetCmdPkt)-1);
	NetCmdSend();
}

/******************************************************************************
*
*  Function:    OnLoginAccount
*
*  Description: Process LoginAccount packet
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void OnLoginAccount()
{
	typedef struct _SACCOUNT{
	u32_t LoginID;
	u8_t  NameLen;
	u8_t  PWLen;
	u8_t  Data[1];
	}SAccount;

	char buf[21];
	u8_t result=0;//0:pass, 1:fail
	u8_t ConnIdx=0;

	SAccount* Account=(SAccount*)NetCmdData.NetCmdPkt->CmdData;
	if(Account->NameLen<20)memcpy(buf,Account->Data,Account->NameLen);
	buf[Account->NameLen]=0;//end of string
	if(strcmp(buf,EEConfigInfo.LoginName)!=0)result=1;

	if(Account->PWLen<20)memcpy(buf,Account->Data+Account->NameLen,Account->PWLen);
	buf[Account->PWLen]=0;//end of string
	if(strcmp(buf,EEConfigInfo.LoginPassword)!=0)result=1;

	if(!result)
	{
		ConnIdx=0;
		//Search exist login ID
		ConnIdx=FindSA(PktSA,ConnIdx);
		while(ConnIdx!=CONN_ID_NOT_FIND)
		{
			if(NetCmdData.NetCmdConn[ConnIdx].LoginID==Account->LoginID)break;
			ConnIdx=FindSA(PktSA,ConnIdx+1);
		}

		if(ConnIdx!=CONN_ID_NOT_FIND)
		{
			//Save ConnID
			NetCmdData.NetCmdConn[ConnIdx].ConnID=GenConnID();
			//SaveTime
			IP210_Update_Timer_Counter();
			NetCmdData.NetCmdConn[ConnIdx].Time=timercounter;
		}
		else
		{
			result=1;
		}
	}

	SendLoginResult(result,NetCmdData.NetCmdConn[ConnIdx].ConnID);
}
void AgingNetCmdConnTable()
{
	u8_t i;
	for(i=0;i<MAX_NET_CMD_CONN;i++)
		if(Delta_Time(NetCmdData.NetCmdConn[i].Time)>NET_CMD_TIMEOUT)
	{	memset(NetCmdData.NetCmdConn[i].SA,0,6);	}
}
void NetCmdTimer()
{
	AgingNetCmdConnTable();
#ifdef MODULE_NET_CMD_EEP_CFG	
	timer_eep_cfg();
#endif //MODULE_NET_CMD_EEP_CFG
}

/******************************************************************************
*
*  Function:    SendAccessGPIOAck
*
*  Description: Send the Ack of AccessGPIO
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void SendAccessGPIOAck()
{
}

/******************************************************************************
*
*  Function:    OnAccessGPIOReq
*
*  Description: Process AccessGPIOReq packet
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void OnAccessGPIOReq()
{
#ifdef SUPPORT_GPIO_COMMAND

#define OP_WRITE 0x80
#define OP_RESULT_SUCCESS 0
	u8_t i;
	typedef struct _SGPIOCmd
	{
		u8_t RegOp;
		u8_t AccessMask;
		u8_t RegData;
		u8_t WritOp;
		u16_t Delay;
	}SGPIOCmd;
	typedef struct _SAccGPIOReq
	{
		u8_t  CmdTotal;
		SGPIOCmd GPIOCmd[255];
	}SAccGPIOReq;
	typedef struct _SGPIOResult
	{
		u8_t RegOp;
		u8_t AccessMask;
		u8_t RegData;
		u8_t OpResult;
		u16_t Reserve;
	}SGPIOResult;
	typedef struct _SAccGPIOAck
	{
		u8_t  CmdTotal;
		SGPIOResult GPIOResult[255];
	}SAccGPIOAck;

	SAccGPIOReq* AccGPIOReq=(SAccGPIOReq*)NetCmdData.NetCmdPkt->CmdData;
	SAccGPIOAck* AccGPIOAck=(SAccGPIOAck*)NetCmdData.NetCmdPkt->CmdData;
	NetCmdData.NetCmdPkt->CmdOP=ACCESS_GPIO_ACK;
	for(i=0;i<AccGPIOReq->CmdTotal;i++)
	{
		if(AccGPIOReq->GPIOCmd[i].RegOp&OP_WRITE)
		{//Write cmd
			WriteGPIO(AccGPIOReq->GPIOCmd[i].RegOp&0x07,
			          AccGPIOReq->GPIOCmd[i].AccessMask,
					  AccGPIOReq->GPIOCmd[i].WritOp,
					  &AccGPIOAck->GPIOResult[i].RegData);
			AccGPIOAck->GPIOResult[i].OpResult=OP_RESULT_SUCCESS;
		}
		else
		{//Read cmd
			ReadGPIO(AccGPIOReq->GPIOCmd[i].RegOp&0x07,
			         AccGPIOReq->GPIOCmd[i].AccessMask,
					&AccGPIOAck->GPIOResult[i].RegData);
			AccGPIOAck->GPIOResult[i].OpResult=OP_RESULT_SUCCESS;
		}
		//Delay();//JC_STOP
	}
	NetCmdSend();
#endif //SUPPORT_GPIO_COMMAND
}

/******************************************************************************
*
*  Function:    MPProcess
*
*  Description: Process MP_Process_Req packet
*               
*  Parameters:  NetCommand data in uip_buffer
*               
*  Returns:  send back MP_Process_Ack packet
*               
*******************************************************************************/
//#define DB_MP
u8_t MP_flag=1;
extern u8_t load_default_pin_pressed;
extern u16_t reboot_check ;
u16_t special_id_counter;
void MPProcess(void)//have to place this function at Bank0 so that it can update MPFlag
{
  u16_t i;
  u8_t TargetMACID[6];
  u8_t update_status=0;//0:ok, 1:fail
  SNetCmdPkt* NetCMD_Frame=NetCmdData.NetCmdPkt;
  u8_t *pMACIDentry;

#ifdef DB_MP
	printf("\n\r MPProcess(): enter");
#endif //DB_MP

  if((MP_flag==0)||(load_default_pin_pressed==0))
  {
  	uip_len=0;
#ifdef DB_MP
	printf("\n\r MPProcess(): MP_flag=%d, load_default_pin_pressed=%d",(u16_t)MP_flag,(u16_t)load_default_pin_pressed);
#endif //DB_MP  	

	if(special_id_counter!=10)
		special_id_counter++;

	if((MP_flag==1)&&(reboot_check!=0x1234)&&(special_id_counter!=10)){
		reboot_check=0000;
	}
	else
  	   return;
  }
  memcpy(uip_buf,uip_buf+6,6);//copy SA to DA
  memcpy(uip_buf+6,EEConfigInfo.MACID,6);//copy my MAC to SA
  
    pMACIDentry=NetCMD_Frame->CmdData;

	uip_len=LCMD_CmdData_OFFSET+1;
	for(i=0;i<6;i++)
    {	TargetMACID[i]=0x0;	}
#ifdef DB_MP
	printf("\n\rTargetMACID=");
	for(i=0;i<6;i++)
	{ printf("%02x ",(u16_t)TargetMACID[i]); }
	printf("\n\rpMACIDentry=");
	for(i=0;i<6;i++)
	{ printf("%02x ",(u16_t)*(pMACIDentry+i)); }
#endif
	if(memcmp(TargetMACID,pMACIDentry, 6)==0)
	{//just a query, not really a request
        if(update_status!=2)
		{update_status=3;}
#ifdef DB_MP
	printf("\n\r MPProcess(): Just request");
#endif //DB_MP		
	}
#ifdef DB_MP
	printf("\n\rupdate_status=%d",(u16_t)update_status);
#endif
	if(update_status==2 || update_status==3)
	{//send ack&return
        NetCMD_Frame->CmdOP=MP_PROCESS_ACK;
		NetCMD_Frame->CmdData[0]=update_status;
        return;
	}
	if(load_default_pin_pressed==0){
		printf("MP Counter=%d\n",special_id_counter)  ;
		reboot_check=0x1289;
	}	
  	MP_flag=0;
    //get MACID info from request data
    memcpy(TargetMACID, NetCMD_Frame->CmdData, 6); 

    //load default EEPROM into EEPROM
	EEPROMLoadDefault();
    //write MACID back to EEPROM
  	for(i=0;i<6;i++)
	{
	    if(!EEPROM_Write_Byte(loc_EEnetif+i,TargetMACID[i]))
		{
		    break;
		}
	}
	if(i!=6)
	{
        update_status=1;
	}

	//read and check if the MACID in EEPROM is updated correctly
    for(i=0;i<6;i++)
	{
	    EEPROM_Read_Byte(loc_EEnetif+i,&TargetMACID[i]);
	    if(TargetMACID[i]!=NetCMD_Frame->CmdData[i])
		{
            update_status=1;  
		    break;
		}
	}

	//prepare the MP_Process_ACK packet
  NetCMD_Frame->CmdOP=MP_PROCESS_ACK;
	NetCMD_Frame->CmdData[0]=update_status;	
#ifdef DB_MP
	printf("\n\r MPProcess(): SUCCESS");
#endif //DB_MP	
        return;
}
#endif //MODULE_NET_CMD



/******************************************************************************
*
*  Function:    OngpioReqest
*
*  Description: Read or write value to GPIO
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
#ifdef MODULE_SIGNAL_GPIO
void OngpioRequest()
{
	u8_t value=0x0, gping=0x0, common=0x0, result=0x0;

	common = (NetCmdData.NetCmdPkt->CmdData[0])>>7 & 0x1;	//read or write
	value = (NetCmdData.NetCmdPkt->CmdData[0])>>6 & 0x1;	//value 
	gping = (NetCmdData.NetCmdPkt->CmdData[0]) & 0x3f;		//GPIO ping
	NetCmdData.NetCmdPkt->CmdOP=SINGLE_PIN_GPIO_ACK;

	if(!common)	//read
	{
		if(gping>0x2)
			result=1;
		else if(gping==0x1)
			value = DIDO_0;
		else 
			value = DIDO_1;
	}
	else	//write
	{
		if(gping>0x2)
			result=1;
		else if(gping==0x1)
			DIDO_0 = value;
		else 
			DIDO_1 = value;		
	}
	value = (common<<7)|(value<<6)|gping;
	OngpioRequestAck(result, value);
}
#endif
/******************************************************************************
*
*  Function:    OngpioReqestAck
*
*  Description: Send the Ack of AccessGPIO
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
#ifdef MODULE_SIGNAL_GPIO
void OngpioRequestAck(u8_t gpioresult, u8_t gpiovalue)
{
	typedef struct _SUSERGPIO
	{
		u8_t result;
		u8_t value;
	}SUSERGPIO;

	SUSERGPIO* USERGPIO=(SUSERGPIO*)NetCmdData.NetCmdPkt->CmdData;
	USERGPIO->result = gpioresult;
	USERGPIO->value = gpiovalue;
	uip_len=sizeof(SUSERGPIO)+(sizeof(SNetCmdPkt)-1);
	NetCmdSend();	
}
#endif
/******************************************************************************
*
*  Function:    OnchannelRequest
*
*  Description: Check Data of AccessChannel
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
#ifdef MODULE_SIGNAL_GPIO
void OnchannelRequest()
{
	u8_t result=0x0, value=0x0;
	u16_t chan_value=0x0;
	NetCmdData.NetCmdPkt->CmdOP=SINGLE_CHANNEL_ACK;
	value = NetCmdData.NetCmdPkt->CmdData[0];	
	if(chan_value>0x8)
		result = 1;
	current_channel=value;
	chan_value = adc_convert();
	OnchannelRequestAck(result, value, chan_value);
}
#endif
/******************************************************************************
*
*  Function:    OnchannelRequest
*
*  Description: Send the Ack of AccessChannel
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
#ifdef MODULE_SIGNAL_GPIO
void OnchannelRequestAck(u8_t channel_result, u8_t old_value, u16_t channel_value)
{
	typedef struct _SUSERADC
	{
		u8_t result;
		u8_t value;
		u16_t chan_value;
	}SUSERADC;

	SUSERADC* USERADC=(SUSERADC*)NetCmdData.NetCmdPkt->CmdData;
	USERADC->result = channel_result;
	USERADC->value = old_value;
	USERADC->chan_value = channel_value;
	uip_len=sizeof(SUSERADC)+(sizeof(SNetCmdPkt)-1);
	NetCmdSend();
}
#endif

/******************************************************************************
*
*  Function:    eep_reqd_req
*
*  Description: read eeprom
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
//NetCmdData.NetCmdPkt->CmdData
//EEPROM_Read_Byte
#ifdef MODULE_NET_CMD
void eep_read_req()
{
	u8_t result;
	u16_t i;

	typedef struct _SEEPREAD
	{
		u16_t star_addr;
		u16_t addr_len;
		u8_t read_data[1];
	}SEEPREAD;

	SEEPREAD* EEPREAD=(SEEPREAD*)NetCmdData.NetCmdPkt->CmdData;

	NetCmdData.NetCmdPkt->CmdOP=EEP_READ_ACK;
	if(EEPREAD->addr_len<=1024)
	{
		for(i=EEPREAD->star_addr;i<EEPREAD->addr_len+EEPREAD->star_addr;i++)
		{
			WatchDogTimerReset();
			if(EEPROM_Read_Byte(i, EEPREAD->read_data+(i-EEPREAD->star_addr))) //succeeded
			{	}
			else
			{	result=1;	}
		}
		if(result)
		{	EEPREAD->addr_len=0;	}
	}
	else
	{	EEPREAD->addr_len=0;	}

	uip_len=sizeof(SEEPREAD)+(sizeof(SNetCmdPkt)-1)+(EEPREAD->addr_len-1);
	NetCmdSend();
}

/******************************************************************************
*
*  Function:    eep_write_req
*
*  Description: write eeprom
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
//NetCmdData.NetCmdPkt->CmdData
//EEPROM_Write_Byte
void eep_write_req()
{
	u8_t an=1;
	u16_t i;

	typedef struct _SEEPWRITE
	{
		u16_t star_addr;
		u16_t addr_len;
		u8_t write_data[1];
	}SEEPWRITE;
	
	SEEPWRITE* EEPWRITE=(SEEPWRITE*)NetCmdData.NetCmdPkt->CmdData;

	NetCmdData.NetCmdPkt->CmdOP=EEP_WRITE_ACK;
	for(i=EEPWRITE->star_addr;i<EEPWRITE->addr_len+EEPWRITE->star_addr;i++)
	{
		WatchDogTimerReset();
		if(EEPROM_Write_Byte(i, *(EEPWRITE->write_data+(i-EEPWRITE->star_addr))))	//succeeded
		{/*	printf(" %02x",(u16_t)*(EEPWRITE->write_data+(i-EEPWRITE->star_addr)));	*/}
		else
		{	an=0; EEPWRITE->star_addr=0x1<<8;	break;	}
	}
	if(an==1)
	{	EEPWRITE->star_addr=0;	}

	uip_len=sizeof(SNetCmdPkt);
	NetCmdSend();	
}


/******************************************************************************
*
*  Function:    reset_device_req
*
*  Description: reset device
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void reset_device_req()
{
	NetCmdData.NetCmdPkt->CmdOP=RESET_DEVICE_ACK;
	if(NetCmdData.NetCmdPkt->CmdData[0]==0xaa)
	{
		NetCmdData.NetCmdPkt->CmdData[0]=0;
		force_reset_countdown=1;
	}
	else
	{	NetCmdData.NetCmdPkt->CmdData[0]=1;	}
	uip_len=sizeof(SNetCmdPkt);
	NetCmdSend();
}
#ifdef MODULE_FLASH512
/******************************************************************************
*
*  Function:    en loop back
*
*  Description: enable loop back for vcom
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void en_loopback()
{
	NetCmdData.NetCmdPkt->CmdOP=EN_LOOP_ACK;

	lb_flag=1;
	RingInit(UartRxBuf,RxBuf,max_uart_rx_buf_len);

	if(lb_flag)
	{ NetCmdData.NetCmdPkt->CmdData[0]=0; }
	else
	{ NetCmdData.NetCmdPkt->CmdData[0]=1; }

	uip_len=sizeof(SNetCmdPkt);
	NetCmdSend();
}

/******************************************************************************
*
*  Function:    dis loop back
*
*  Description: disable loop back for vcom
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void dis_loopback()
{
	NetCmdData.NetCmdPkt->CmdOP=DIS_LOOP_ACK;

	lb_flag=0;
	RingInit(UartRxBuf,RxBuf,max_uart_rx_buf_len);

	if(lb_flag)
	{ NetCmdData.NetCmdPkt->CmdData[0]=1; }
	else
	{ NetCmdData.NetCmdPkt->CmdData[0]=0; }

	uip_len=sizeof(SNetCmdPkt);
	NetCmdSend();
}
#endif
/******************************************************************************
*
*  Function:    NetCom on UDP
*
*  Description: 
*               
*  Parameters:  
*               
*  Returns:  
*               
*******************************************************************************/
u8_t borcast_flag=0;
void udp_net_cmd() //joe 20080702 NetCmdonUDP
{
	u8_t borcast[6]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8_t i=0;

#ifdef MODULE_SIGNAL_GPIO
	if( (UdpData.p_DataPkt->destport!=8000) && (UdpData.p_DataPkt->destport!=9000) )return;
#else
	if( (UdpData.p_DataPkt->destport!=8000) )return;
#endif
//-------------------Different LAN for Search Device------------------------------------------
	for(i=0;i<6;i++)
	{	if(uip_buf_real[i]!=borcast[i])break;	}

	UdpData.DestIPAddr[0] = UdpData.p_DataPkt->srcipaddr[0]; //save in coming packet srcipaddr
	UdpData.DestIPAddr[1] = UdpData.p_DataPkt->srcipaddr[1];
/*
	if(i!=6)	//Not brocast & in different LAN sent to gateway
	{
		if( (UdpData.SrcIPAddr[0]&uip_arp_netmask[0])!=(UdpData.p_DataPkt->srcipaddr[0]&uip_arp_netmask[0])||
			(UdpData.SrcIPAddr[1]&uip_arp_netmask[1])!=(UdpData.p_DataPkt->srcipaddr[1]&uip_arp_netmask[1])
		   )
		{
			UdpData.DestIPAddr[0] = uip_arp_draddr[0]; //sent packet to gateway
			UdpData.DestIPAddr[1] = uip_arp_draddr[1];				
		}
	}
	else
*/
	if(i==6)
	{ borcast_flag=1; }
//--------------------------------------------------------------------------------------------
	UdpData.SrcPort = UdpData.p_DataPkt->srcport;  //save in coming packet srcport & destport
	UdpData.DestPort = UdpData.p_DataPkt->destport;
	NetCmdData.CurrentPktType=TypeUCmd;
	NetCmdData.NetCmdPkt=UNetCmdPkt;
#ifdef MODULE_SIGNAL_GPIO
	if(UdpData.p_DataPkt->destport==8000)
		NetCmd();	
	else if( (NetCmdData.NetCmdPkt->CmdOP==SINGLE_PIN_GPIO_REQ) || 
			 (NetCmdData.NetCmdPkt->CmdOP==SINGLE_CHANNEL_REQ) ||
			 (NetCmdData.NetCmdPkt->CmdOP==LOGIN_REQUEST) ||
			 (NetCmdData.NetCmdPkt->CmdOP==LOGIN_ACCOUNT) )
		NetCmd();
	return;
#else
	NetCmd();
#endif
}
#endif	//MODULE_NET_CMD 

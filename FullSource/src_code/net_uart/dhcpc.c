/******************************************************************************
*
*   Name:           dhcpc.c
*
*   Description:    DHCP client protocol
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#include <module.h>
#ifdef MODULE_DHCPC
#include "dhcpc.h"
#include "uip.h"
#include "option.h"
#include <stdio.h>
#include <string.h>
SDHCPCInfo DHCPCInfo;
#include "eepdef.h"

#include "net_uart.h"

void CatNetworkConfigMsg(char* Msg,char* endl);//Cat NetworkConfigMsg to Msg
#ifdef DB_FIFO
extern u8_t rx_over;
#endif
/******************************************************************************
*
*  Function:    DHCPCInit
*
*  Description: DHCPC init
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void DHCPCInit()
{
	if(EEConfigInfo.DHCPEnable==sDHCP_DISABLE)		
		DHCPCInfo.Status=sDHCP_DISABLE;
	else
		DHCPCInfo.Status=sDHCPC_NEEDDHCP;

	DHCPCInfo.TimeoutCounter=0;
	DHCPCInfo.PktInterval=0;
	DHCPCInfo.OneSecondCounter=0;
}

/******************************************************************************
*
*  Function:    DHCPCApplyIP
*
*  Description: Apply IP form DHCP server to host IP address
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void DHCPCApplyIP()
{
	*((u32_t*)uip_hostaddr)=DHCPCInfo.MyIPAddr;
	*((u32_t*)uip_arp_netmask)=DHCPCInfo.SubnetMask;
	*((u32_t*)uip_arp_draddr)=DHCPCInfo.GetwayAddr;
	*((u32_t*)uip_dnsaddr)=DHCPCInfo.DNSAddr;
	UdpData.SrcIPAddr[0]=uip_hostaddr[0];	//joe 20080715 when get DHCP SrcIPAddr will be clear so must be regive
	UdpData.SrcIPAddr[1]=uip_hostaddr[1];
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r25");rx_over=0;}
#endif
#endif
}

/******************************************************************************
*
*  Function:    DHCPCFoundTag
*
*  Description: Found tag form DHCPC packet.
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
u8_t* DHCPCFoundTag(u8_t tag)
{
	u8_t* Options=((SDHCPPkt*)UdpData.p_DataPkt->udpload)->Options;
	int i;
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r26");rx_over=0;}
#endif
#endif	
	//check cookie
	if(*((u32_t*)Options)!=MagicCookie)return 0;	
	
	for(i=4;i<UdpData.RxLength-DHCPHeaderLen;)
	{
		if(Options[i]==DHCP_OPTION_END_OF_LIST)return 0;
		if(Options[i]==tag)return &Options[i];
		i+=Options[i+1]+2;//2=tag 1 byte + len 1 byte
	}
	return 0;
	
}

/******************************************************************************
*
*  Function:    DHCPCltPktInit
*
*  Description: Init a packet for DHCPC to send out
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void DHCPCCltPktInit()
{
	SDHCPPkt *DHCPPkt=UdpData.p_DataPkt->udpload;
	DHCPPkt->op=DHCP_BOOTREQUEST;
	DHCPPkt->htype=1;
	DHCPPkt->hlen=6;
	DHCPPkt->hops=0;
	DHCPPkt->xid=0xb1323d11;//any value
	DHCPPkt->secs=0x0001;
	DHCPPkt->flags=0;
	DHCPPkt->ciaddr=0;
	DHCPPkt->yiaddr=0;
	DHCPPkt->siaddr=0;
	DHCPPkt->giaddr=0;
	DHCPPkt->chaddr[0]=0;
	DHCPPkt->chaddr[1]=0;
	DHCPPkt->chaddr[2]=0;
	DHCPPkt->chaddr[3]=0;	
	memcpy(DHCPPkt->chaddr,uip_ethaddr.addr,6);	
	memset(DHCPPkt->sname,0,64);
	memset(DHCPPkt->file,0,128);

    *((u32_t*)DHCPPkt->Options)=MagicCookie;
	DHCPCInfo.OptionCount=4;
}


/******************************************************************************
*
*  Function:    DHCPCSendDiscover
*
*  Description: Send DHCP Discover packet out
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void DHCPCSendDiscover()
{
	SDHCPPkt *DHCPPkt=UdpData.p_DataPkt->udpload;
	u8_t DiscoverTemplate[]={
		  DHCP_OPTION_MESSAGE_TYPE,LEN_OF_DHCP_OPTION_MESSAGE_TYPE,DHCP_DISCOVER,
//		  DHCP_OPTION_PARAMETER_REQUEST_LIST,0x0a,DHCP_OPTION_SUBNET_MASK,0x0f,DHCP_OPTION_GETWAY_ADDRESS,0x06,0x2c,0x2e,0x2f,0x1f,0x21,0x2b, //0a: len of DHCP_OPTION_PARAMETER_REQUEST_LIST
		  DHCP_OPTION_PARAMETER_REQUEST_LIST,0x0b,DHCP_OPTION_SUBNET_MASK,0x0f,DHCP_OPTION_GETWAY_ADDRESS,0x06,0x2c,0x2e,0x2f,0x1f,0x21,0xf9,0x2b,0x2b,0x02,0xdc,0x00, //0a: len of DHCP_OPTION_PARAMETER_REQUEST_LIST
		  DHCP_OPTION_END_OF_LIST,0x00,0x00,0x00,0x00,0x00
	};

	DHCPCCltPktInit();
   	DHCPPkt->flags=0x8000;
	memcpy(&DHCPPkt->Options[DHCPCInfo.OptionCount],DiscoverTemplate,sizeof(DiscoverTemplate));
	DHCPCInfo.OptionCount+=sizeof(DiscoverTemplate);
	

//UDP settings:
	UdpData.SrcIPAddr[0] =0x0000;
  	UdpData.SrcIPAddr[1] =0x0000;
  	UdpData.DestIPAddr[0]=0xffff;
 	UdpData.DestIPAddr[1]=0xffff;; 
	UdpData.SrcPort=67;
	UdpData.DestPort=68;
	udp_send(DHCPHeaderLen+DHCPCInfo.OptionCount);
}

/******************************************************************************
*
*  Function:    DHCPCSendRequest
*
*  Description: Send DHCP Request packet out
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void DHCPCSendRequest(u8_t reply_offer)
{
	SDHCPPkt *DHCPPkt=UdpData.p_DataPkt->udpload;
	u16_t len;
	u8_t RequestTemplate[]={
              DHCP_OPTION_MESSAGE_TYPE,LEN_OF_DHCP_OPTION_MESSAGE_TYPE,DHCP_REQUEST,
/*offset:03*/ DHCP_OPTION_REQUESTED_IP,LEN_OF_DHCP_OPTION_REQUESTED_IP,0x00,0x00,0x00,0x00, //need to rewrite IP
/*offset:09*/ DHCP_OPTION_SERVER_ID,LEN_OF_DHCP_OPTION_SERVER_ID,0x00,0x00,0x00,0x00, //need to rewrite IP
		        DHCP_OPTION_END_OF_LIST,0x00,0x00,0x00,0x00,0x00
	};   
   
	DHCPCCltPktInit();  
	len=sizeof(RequestTemplate);
	if(reply_offer==0)
	{  
		DHCPPkt->ciaddr=DHCPCInfo.MyIPAddr;
		RequestTemplate[3]=DHCP_OPTION_END_OF_LIST;
		len-=12;//req_ip+server id
	}
	else
	{
		 *((u32_t*)&RequestTemplate[3+2])=DHCPCInfo.MyIPAddr;
		*((u32_t*)&RequestTemplate[9+2])=DHCPCInfo.SrvIPAddr;
	}
 	
	memcpy(&DHCPPkt->Options[DHCPCInfo.OptionCount],RequestTemplate,sizeof(RequestTemplate));
	DHCPCInfo.OptionCount+=len;
	

//UDP settings:
    if(reply_offer)
    {
		UdpData.SrcIPAddr[0] =0x0000;
 		UdpData.SrcIPAddr[1] =0x0000;
 		UdpData.DestIPAddr[0]=0xffff;
 		UdpData.DestIPAddr[1]=0xffff;;  
 	}
 	else
 	{
		*((u32_t*)UdpData.SrcIPAddr) =DHCPCInfo.MyIPAddr;
 		*((u32_t*)UdpData.DestIPAddr)=DHCPCInfo.SrvIPAddr;
 	}
	UdpData.SrcPort=67;
	UdpData.DestPort=68;
	udp_send(DHCPHeaderLen+DHCPCInfo.OptionCount);
	
	DHCPCInfo.Status=sDHCPC_WAIT_ACK;
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r27");rx_over=0;}
#endif
#endif
}

/******************************************************************************
*
*  Function:    DHCPCCheckInfo
*
*  Description: check information in offer and ack
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void DHCPCCheckInfo()
{
	u8_t* Tag;
	u32_t* pValue32;
	Tag=DHCPCFoundTag(DHCP_OPTION_SUBNET_MASK);//subnet mask
	if(Tag!=0)
	{
		pValue32=(u32_t*)&Tag[2];
		DHCPCInfo.SubnetMask=*pValue32;
	}	
	Tag=DHCPCFoundTag(DHCP_OPTION_GETWAY_ADDRESS);//Getway/router address
	if(Tag!=0)
	{
		pValue32=(u32_t*)&Tag[2];
		DHCPCInfo.GetwayAddr=*pValue32;
	}
	Tag=DHCPCFoundTag(LEN_OF_DHCP_OPTION_DNS);//DNS
	if(Tag!=0)
	{
		pValue32=(u32_t*)&Tag[2];
		DHCPCInfo.DNSAddr=*pValue32;
	}
	Tag=DHCPCFoundTag(DHCP_OPTION_LEASE_TIME);//lease time
	if(Tag!=0)
	{
		pValue32=(u32_t*)&Tag[2];
		DHCPCInfo.LeaseConuntdown=*pValue32-(*pValue32/5);//send request if there are only left 1/5 time
	}
	Tag=DHCPCFoundTag(DHCP_OPTION_SERVER_ID);
	if(Tag!=0){
		pValue32=(u32_t*)&Tag[2];
		DHCPCInfo.SrvIPAddr=*pValue32;
	}
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r28");rx_over=0;}
#endif
#endif	
}
/******************************************************************************
*
*  Function:    DHCPCReceiveOffer
*
*  Description: Process DHCP Offer
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void DHCPCReceiveOffer()
{
	SDHCPPkt *DHCPPkt=UdpData.p_DataPkt->udpload;

	DHCPCInfo.MyIPAddr=DHCPPkt->yiaddr;    
//	DHCPCInfo.SrvIPAddr=*(u32_t*)&UdpData.p_DataPkt->srcipaddr;
	DHCPCInfo.SrvIPAddr=DHCPPkt->siaddr;
	DHCPCCheckInfo();	
}

/******************************************************************************
*
*  Function:    DHCPCReceiveAck
*
*  Description: Process DHCP Ack
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
u8_t DHCP_get_ip=0;
void DHCPCReceiveAck()
{
	/*u8_t* Tag;
	u32_t* pValue32;
	Tag=DHCPCFoundTag(DHCP_OPTION_LEASE_TIME);//lease time
	if(Tag!=0)
	{
		pValue32=(u32_t*)&Tag[2];
		DHCPCInfo.LeaseConuntdown=*pValue32-(*pValue32/5);//send request if there are only left 1/5 time
	}*/
	DHCPCCheckInfo();
	DHCPCInfo.Status=sDHCPC_DONE;
    DHCPCApplyIP();
	if(DHCP_get_ip==0)
	{
#ifdef MODULE_FULL_MSG	
		strcpy(uip_appdata,"\n\rDHCP Info:");
#endif
		CatNetworkConfigMsg(uip_appdata,"\n\r");
		printf(uip_appdata);
		DHCP_get_ip=1;
		printf("\n\r");
	}
}

/******************************************************************************
*
*  Function:    DHCPCReceiveNAck
*
*  Description: Process DHCP NAck
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void DHCPCReceiveNAck()
{
	DHCPCInfo.Status=sDHCPC_NEEDDHCP;
}

/******************************************************************************
*
*  Function:    DHCPCTimer
*
*  Description: Process when timer callback
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
u8_t dhcp_flag=0;
void DHCPCTimer()
{
	//Enter until one second
	if(DHCPCInfo.OneSecondCounter++<ONE_SECOND_COUNT)return;
	DHCPCInfo.OneSecondCounter=0;
	
	switch(DHCPCInfo.Status)
	{
		case sDHCPC_NEEDDHCP:
			DHCPCSendDiscover();
			break;	     
		case sDHCPC_WAIT_ACK:
			DHCPCInfo.TimeoutCounter++;
			if(dhcp_flag)
			{
				if( (DHCPCInfo.TimeoutCounter==DHCP_IF_ARP)||(DHCPCInfo.TimeoutCounter==DHCP_IF_ARP*2) )
					DHCPCSendRequest(0);
			}
			if(DHCPCInfo.TimeoutCounter>=DHCP_WAIT_ACK_TIMEOUT)
				DHCPCInfo.Status=sDHCPC_NEEDDHCP;
			break;
		case sDHCPC_DONE:
			if(!(DHCPCInfo.LeaseConuntdown--))
			{
				DHCPCSendRequest(0);
				dhcp_flag=1;
			}
			break;
	}
}

/******************************************************************************
*
*  Function:    DHCP_UDP_RECEIVE
*
*  Description: Callback function for UDP
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void DHCPC_UDP_RECEIVE()
{
	SDHCPPkt *DHCPPkt=UdpData.p_DataPkt->udpload;
	u8_t* Tag;
	if(!((UdpData.p_DataPkt->srcport==67)&&(UdpData.p_DataPkt->destport==68)&&(DHCPPkt->op==DHCP_BOOTREPLY)))return;
	Tag=DHCPCFoundTag(DHCP_OPTION_MESSAGE_TYPE);
	if(Tag==0)return;
	switch(Tag[2])
	{
		case DHCP_OFFER:
			     if(DHCPCInfo.Status!=sDHCPC_NEEDDHCP)break;
			     DHCPCReceiveOffer();			     
			     DHCPCSendRequest(1);			     			     
			     break;
		case DHCP_ACK:
			     if(DHCPCInfo.Status!=sDHCPC_WAIT_ACK)break;
			     DHCPCReceiveAck();
			     DHCPCApplyIP();
				 dhcp_flag=0;
				 DHCPCInfo.TimeoutCounter=0;
			     break;
		case DHCP_NAK:
			     if(DHCPCInfo.Status!=sDHCPC_WAIT_ACK)break;
			     DHCPCReceiveNAck();
			     break;
	}
	
}
#endif //MODULE_DHCPC

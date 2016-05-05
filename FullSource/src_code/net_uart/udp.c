/******************************************************************************
*
*   Name:           Small UDP
*
*   Description:    UDP protocol
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#include <module.h>
#ifdef MODULE_SMALL_UDP
#include "udp.h"
#include "dhcpc.h"
#include <stdio.h>
#include "dns.h"

#include "net_uart.h"

void udp_tel_rx();
//#define CHECKSUM_ENABLE
//#define UDP_DEMO_APPCALL
#ifdef CHECKSUM_ENABLE
u16_t uip_udpchksum(void);
u16_t uip_ipchksum(void);
#endif

#ifdef MODULE_VLAN_TAG
extern u16_t uip_ipchksum(void);
extern u16_t uip_tcpchksum(void);
extern u16_t uip_udpchksum(void);
extern u8_t eth_vlan;
#endif

CUdpData UdpData;
#ifdef DB_FIFO
extern u8_t rx_over;
#endif
/******************************************************************************
*
*  Function:    UDP_RECEIVE
*
*  Description: UDP Rx packet process function
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
#ifdef UDP_DEMO_APPCALL
void UDP_RECEIVE()//example UDP_RECEIVE() = PingPongServer()
{		
		UdpData.DestIPAddr[0]=UdpData.p_DataPkt->srcipaddr[0];
		UdpData.DestIPAddr[1]=UdpData.p_DataPkt->srcipaddr[1];
		UdpData.SrcPort=UdpData.p_DataPkt->srcport;
		UdpData.DestPort=UdpData.p_DataPkt->destport;
	
		udp_send(UdpData.RxLength);	
}
#else //UDP_DEMO_APPCALL

#endif //UDP_DEMO_APPCALL

/******************************************************************************
*
*  Function:    udp_init
*
*  Description: init small UDP
*               
*  Parameters:  p_DataPkt: point to packet buffer (uip_buf)
*               p_uip_len: point to uip_len
*               hostaddr:  point to host IP address
*  Returns:     
*               
*******************************************************************************/
void udp_init(u8_t* p_DataPkt,u16_t* p_uip_len, u16_t* hostaddr)
{
	UdpData.IP_ID=0;
	UdpData.p_DataPkt=(udpip_hdr*)p_DataPkt;
	UdpData.p_uip_len=p_uip_len;
	UdpData.SrcIPAddr[0]=hostaddr[0];
	UdpData.SrcIPAddr[1]=hostaddr[1];
}

u8_t* udp_set_buf(u8_t* p_DataPkt)
{
	UdpData.old_buf=(u8_t*)UdpData.p_DataPkt;
	UdpData.p_DataPkt=(udpip_hdr*)p_DataPkt;
	return UdpData.old_buf;
}
/******************************************************************************
*
*  Function:    udp_process
*
*  Description: udp main function
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void udp_process()
{	
		if(UdpData.p_DataPkt->proto==UIP_PROTO_UDP)
		{			
#ifdef CHECKSUM_ENABLE
			u16_t ipchksum, udpchksum;
			ipchksum =UdpData.p_DataPkt->ipchksum;			
			UdpData.p_DataPkt->ipchksum=0;				  		
	  		if(ipchksum!=~(uip_ipchksum()))
	  		{
	  			UdpData.p_DataPkt->ipchksum=ipchksum;
				return;
	  		}
	  		UdpData.p_DataPkt->ipchksum=ipchksum;
	  		
	  		udpchksum=UdpData.p_DataPkt->udpchksum;
	  		UdpData.p_DataPkt->udpchksum=0;
	  		if(udpchksum!=~(uip_udpchksum()))
	  		{
				UdpData.p_DataPkt->udpchksum=udpchksum;
				return;
	  		}
			UdpData.p_DataPkt->udpchksum=udpchksum;
#endif			
			UdpData.RxLength=((u16_t)UdpData.p_DataPkt->len[0]<<8 | UdpData.p_DataPkt->len[1])-28;
			*UdpData.p_uip_len=0;
			UDP_RECEIVE();
		}
		else
		{
			*UdpData.p_uip_len=0;
		}
}

/******************************************************************************
*
*  Function:    udp_send
*
*  Description: send udp packet out
*               
*  Parameters:  length: data len
*               
*  Returns:     
*               
*******************************************************************************/
#ifdef MODULE_VLAN_TAG
extern u8_t udp_send_pkt;
#endif
void udp_send(u16_t length)
{		
		if(length==0)
		{
			*UdpData.p_uip_len=0;
			return;
		}
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
		*UdpData.p_uip_len=length+28;//20:IP header length, 8:UDP header length
		UdpData.p_DataPkt->srcipaddr[0] = UdpData.SrcIPAddr[0];
  		UdpData.p_DataPkt->srcipaddr[1] = UdpData.SrcIPAddr[1];
  		UdpData.p_DataPkt->destipaddr[0] = UdpData.DestIPAddr[0];
 	 	UdpData.p_DataPkt->destipaddr[1] = UdpData.DestIPAddr[1]; 	 	
 	 			
		UdpData.p_DataPkt->vhl = 0x45;
  		UdpData.p_DataPkt->tos = 0;
  		UdpData.p_DataPkt->ipoffset[0] = UdpData.p_DataPkt->ipoffset[1] = 0;
  		UdpData.p_DataPkt->ttl  = UIP_TTL;
  		UdpData.IP_ID++;
  		UdpData.p_DataPkt->ipid[0] = UdpData.IP_ID >> 8;
  		UdpData.p_DataPkt->ipid[1] = UdpData.IP_ID & 0xff;
		UdpData.p_DataPkt->proto = UIP_PROTO_UDP;
		UdpData.p_DataPkt->srcport = UdpData.DestPort;
		UdpData.p_DataPkt->destport	= UdpData.SrcPort;
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif	
		UdpData.p_DataPkt->len[0] = (*UdpData.p_uip_len >> 8);
  		UdpData.p_DataPkt->len[1] = (*UdpData.p_uip_len & 0xff);
		/* Calculate IP checksum. */
  		UdpData.p_DataPkt->ipchksum	= 0;
/*
#ifdef CHECKSUM_ENABLE
  		UdpData.p_DataPkt->ipchksum	= ~(uip_ipchksum());
#endif
*/
#ifdef MODULE_VLAN_TAG
		if( (eth_vlan==1)||(udp_send_pkt==1) )
		{ UdpData.p_DataPkt->ipchksum=~(uip_ipchksum()); }
#endif

   		UdpData.p_DataPkt->udplen = htons(*UdpData.p_uip_len-20);//20:IP header size.
  		/* Calculate UDP checksum. */
   		UdpData.p_DataPkt->udpchksum = 0;
/*
#ifdef CHECKSUM_ENABLE
  		UdpData.p_DataPkt->udpchksum = ~(uip_udpchksum());
  		if(UdpData.p_DataPkt->udpchksum == 0)
		{	UdpData.p_DataPkt->udpchksum = 0xffff;	}	
#endif
*/
#ifdef MODULE_VLAN_TAG
		if( (eth_vlan==1)||(udp_send_pkt==1) )
		{
	  		UdpData.p_DataPkt->udpchksum = ~(uip_udpchksum());
	  		if(UdpData.p_DataPkt->udpchksum == 0)
			{ UdpData.p_DataPkt->udpchksum = 0xffff; }
		}
#endif

  		//*UdpData.p_uip_len+=14; //14:eth_header
}
#endif //MODULE_SMALL_UDP

	/******************************************************************************
*
*   Name:           muip.c
*
*   Description:    multiple buffer for increase uip transmit throughput
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#include "module.h"
#include "uip.h"
#include "muip.h"
#include "public.h"
#include <stdio.h>
#include "string.h"
#include "MacDriver.h"
#include "telnet.h"
#include "net_uart.h"

#ifdef MODULE_MUIP
extern u16_t ipid;
#define BUF ((uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])
struct uip_conn* muip_conn;
u8_t muip_buf[MAX_MBUF][UIP_BUFSIZE+2+4];
u8_t muip_buf_cnt;                      //total muip_buf need to send
u8_t muip_need_send;
u16_t muip_len[MAX_MBUF];
u8_t muip_snd_nxt0[4];
u8_t muip_snd_nxt1[MAX_MBUF][4];
u8_t muip_first_ack=0;
u16_t uip_window_size=1400;
#ifdef MODULE_VLAN_TAG
extern u8_t muip_check_flag;
extern u8_t eth_vlan;
extern u16_t uip_ipchksum(void);
extern u16_t uip_tcpchksum(void);
extern u16_t uip_udpchksum(void);
#endif
/******************************************************************************
*
*  Function:    muip_init
*
*  Description: Initialize muip module
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void muip_init()
{
	muip_need_send=0;
	muip_conn=0;
}
/******************************************************************************
*
*  Function:    muip_send
*
*  Description: send multiple buffer of muip out
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
#ifdef DB_FIFO
extern u8_t rx_over;
#endif
void muip_send()
{
	u8_t i;
	u16_t len;
	uip_tcpip_hdr* MBUF;
	//check if need to send multi packet
	if(muip_need_send==0)return;
	//mark this as send

#ifdef MODULE_VLAN_TAG
	if(eth_vlan)
	{ muip_check_flag=1; }
#endif

	muip_need_send=0;
	
	muip_snd_nxt0[0]=muip_conn->snd_nxt[0];
    muip_snd_nxt0[1]=muip_conn->snd_nxt[1];
    muip_snd_nxt0[2]=muip_conn->snd_nxt[2];
    muip_snd_nxt0[3]=muip_conn->snd_nxt[3];
#ifdef DEBUG_MUIP
    if((muip_buf_cnt==MAX_MBUF)&&(muip_len[muip_buf_cnt-1]>1400))
	{
		printf("\n\rmuip overflow, cnt=%d, last_len=%d",(u16_t)muip_buf_cnt,muip_len[muip_buf_cnt-1]);
	}
#endif//DEBUG_MUIP
	for(i=0;i<muip_buf_cnt;i++)
	{
		//copy header from uip_buf to muip_buf[i]
		CLEAR_UART_RX_BUF('4');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r71");rx_over=0;}
#endif
#ifdef MODULE_VLAN_TAG
		memcpy(&muip_buf[i][4],uip_buf,MUIP_APPDATA);
#else
		memcpy(muip_buf[i],uip_buf,MUIP_APPDATA);
#endif
#ifdef DEBUG_MUIP
		//hind index in IP.TTL
        muip_buf[i][22]=0x10+i;
#endif //DEBUG_MUIP		
		//caculate seq num
		if(i==0)
		{
			uip_add32(muip_snd_nxt0,muip_conn->len);
		}
		else
		{
			uip_add32(muip_snd_nxt1[i-1],muip_len[i-1]);
		}
		//fill seq num to variable
		muip_snd_nxt1[i][0]=uip_acc32[0];
		muip_snd_nxt1[i][1]=uip_acc32[1];
		muip_snd_nxt1[i][2]=uip_acc32[2];
		muip_snd_nxt1[i][3]=uip_acc32[3];
		
		//fill seq num to pkt
#ifdef MODULE_VLAN_TAG
		MBUF=(uip_tcpip_hdr*)&muip_buf[i][UIP_LLH_LEN+4];
#else
		MBUF=(uip_tcpip_hdr*)&muip_buf[i][UIP_LLH_LEN];
#endif
		MBUF->seqno[0]=muip_snd_nxt1[i][0];
		MBUF->seqno[1]=muip_snd_nxt1[i][1];
		MBUF->seqno[2]=muip_snd_nxt1[i][2];
		MBUF->seqno[3]=muip_snd_nxt1[i][3];
		//Fill header
    	len=muip_len[i]+40;//IP header len, TCP header len
		MBUF->len[0] = (len >> 8);
        MBUF->len[1] = (len & 0xff);
		++ipid;
		MBUF->ipid[0] = ipid >> 8;
	    MBUF->ipid[1] = ipid & 0xff;
		MBUF->ipchksum=0;
		MBUF->tcpchksum=0;
#ifdef MODULE_VLAN_TAG
		if(eth_vlan)
		{
			MBUF->ipchksum= ~(uip_ipchksum());
			MBUF->tcpchksum= ~(uip_tcpchksum());
		}
#endif
		
		len+=14;//Ethernet header len
		wait_packet_sent();
    	//uip_arp_out(); copy ether header from uip_buf
		EthernetSend(&muip_buf[i],len);
		uip_len=0;
	}
#ifdef MODULE_VLAN_TAG
	eth_vlan=0;
	muip_check_flag=0;
#endif
	wait_packet_sent();
}
/******************************************************************************
*
*  Function:    muip_process
*
*  Description: check if TCP ack number match muip or not
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void muip_process()
{
  if(muip_conn==0)return; 
  if(muip_buf_cnt==0)return;

  /* Check validity of the IP header. */
  if(BUF->vhl != 0x45)	return;

  /* Check if the packet is destined for our IP address. */  
  if(BUF->destipaddr[0] != uip_hostaddr[0]) return;
  if(BUF->destipaddr[1] != uip_hostaddr[1]) return;

  /* Check if the protocol is TCP*/
  if(BUF->proto != UIP_PROTO_TCP) return;
  
  /* Check if port number match */
  if(muip_conn->rport!=BUF->srcport)return;
  if(muip_conn->lport!=BUF->destport)return;
   //if match first packet:
   uip_add32(muip_snd_nxt0,muip_conn->len);
   if(BUF->ackno[0] == uip_acc32[0] &&
      BUF->ackno[1] == uip_acc32[1] &&
      BUF->ackno[2] == uip_acc32[2] &&
      BUF->ackno[3] == uip_acc32[3])
   {
   	   //force sequence num error, let uip ignore this ack
//   	   uip_conn->snd_nxt[0]=muip_snd_nxt0[0]+1;
		muip_first_ack=1;
   }
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r54");rx_over=0;}
#endif
#endif         
   //if match last packet:
   uip_add32(muip_snd_nxt1[muip_buf_cnt-1],muip_len[muip_buf_cnt-1]);
   if(BUF->ackno[0] == uip_acc32[0] &&
      BUF->ackno[1] == uip_acc32[1] &&
      BUF->ackno[2] == uip_acc32[2] &&
      BUF->ackno[3] == uip_acc32[3])
   {
   	   //update uip seq num, let uip to accept this ack
   	   muip_conn->len=muip_len[muip_buf_cnt-1];
   	   muip_conn->snd_nxt[0]=muip_snd_nxt1[muip_buf_cnt-1][0];
   	   muip_conn->snd_nxt[1]=muip_snd_nxt1[muip_buf_cnt-1][1];
   	   muip_conn->snd_nxt[2]=muip_snd_nxt1[muip_buf_cnt-1][2];
   	   muip_conn->snd_nxt[3]=muip_snd_nxt1[muip_buf_cnt-1][3];
   	   muip_buf_cnt=0;
	   muip_first_ack=0;
   }
}

/*
void UIP_APPCALL()
{
//1. when send new data:
//(1) check uip_pkt_acked, countinue to send packet after uip_pkt_acked
//(2) copy 1st packet to uip_buf
//(3) copy other packet to muip_buf
//(4) set muip_buf_cnt
//2. when retransmit:
//(1)check uip_pkt_acked, countinue to send packet after uip_pkt_acked
}
*/
#endif //MODULE_MUIP
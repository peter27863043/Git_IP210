/******************************************************************************
*
*   Name:           uip_arp
*
*   Description:    ARP process
*
*   Copyright:      
*
*******************************************************************************/
#include<stdio.h>
#define MODIFY_FOR_IP210_UIP_ARP_C
#ifdef MODIFY_FOR_IP210_UIP_ARP_C //==================================================================================  
	#define HTONS
#else //MODIFY_FOR_IP210_UIP_ARP_C //================================================================================== 
#endif //MODIFY_FOR_IP210_UIP_ARP_C //================================================================================== 
/**
 * \addtogroup uip
 * @{
 */

/**
 * \defgroup uiparp uIP Address Resolution Protocol
 * @{
 * 
 * The Address Resolution Protocol ARP is used for mapping between IP
 * addresses and link level addresses such as the Ethernet MAC
 * addresses. ARP uses broadcast queries to ask for the link level
 * address of a known IP address and the host which is configured with
 * the IP address for which the query was meant, will respond with its
 * link level address.
 *
 * \note This ARP implementation only supports Ethernet.
 */
 
/**
 * \file
 * Implementation of the ARP Address Resolution Protocol.
 * \author Adam Dunkels <adam@dunkels.com>
 *
 */

/*
 * Copyright (c) 2001-2003, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: uip_arp.c,v 1.7.2.3 2003/10/06 22:42:30 adam Exp $
 *
 */


#include "uip_arp.h"
#include "eepdef.h"
#include <string.h>

u8_t arpbuf[MAX_ARP_BUF_LEN];
u16_t arp_len;

struct arp_hdr {
	struct uip_eth_hdr ethhdr;
	u16_t hwtype;
	u16_t protocol;
	u8_t hwlen;
	u8_t protolen;
	u16_t opcode;
	struct uip_eth_addr shwaddr;
	u16_t sipaddr[2];
	struct uip_eth_addr dhwaddr;
	u16_t dipaddr[2]; 
};

struct ethip_hdr {
  struct uip_eth_hdr ethhdr;
  /* IP header. */
  u8_t vhl,
    tos,          
    len[2],       
    ipid[2],        
    ipoffset[2],  
    ttl,          
    proto;     
  u16_t ipchksum;
  u16_t srcipaddr[2], 
    destipaddr[2];
};

#define ARP_REQUEST 1
#define ARP_REPLY   2
#define ARP_HWTYPE_ETH 1

struct arp_entry {
  u16_t ipaddr[2];
  struct uip_eth_addr ethaddr;
  u8_t time;
  //u8_t send_timer;
  //u8_t resend_times;  
};
#ifdef MODIFY_FOR_IP210_UIP_ARP_C //==================================================================================  
struct uip_eth_addr uip_ethaddr;
#else //MODIFY_FOR_IP210_UIP_ARP_C //================================================================================== 
struct uip_eth_addr uip_ethaddr = {{UIP_ETHADDR0,
				    UIP_ETHADDR1,
				    UIP_ETHADDR2,
				    UIP_ETHADDR3,
				    UIP_ETHADDR4,
				    UIP_ETHADDR5}};
#endif //MODIFY_FOR_IP210_UIP_ARP_C //================================================================================== 

static struct arp_entry arp_table[UIP_ARPTAB_SIZE];
static u16_t ipaddr[2];
static u8_t i, c;

static u8_t arptime;
static u8_t tmpage;
//20100122 chance add for new arp issue
m_Gateway_Info m_gatewayinfo;
void send_arp_req();
u32_t os_cbk_arp_timeout();
extern u8_t All0_buf[size_EEMACID];

#ifdef MODIFY_FOR_IP210_UIP_ARP_C //==================================================================================  
	#define ARPBUF   ((struct arp_hdr *)&arpbuf[0])//20100908 chance add for independ arp buf	
	#define BUF   ((struct arp_hdr *)&uip_buf[0])
	#define IPBUF ((struct ethip_hdr *)&uip_buf[0])
	#define rx_eth   ((struct ETH_HDR *)&uip_buf[0])
#else //MODIFY_FOR_IP210_UIP_ARP_C //==================================================================================
  #define ARPBUF   ((struct arp_hdr *)&arpbuf[0])//20100908 chance add for independ arp buf
	#define BUF   ((struct arp_hdr *)&uip_buf[0])
	#define IPBUF ((struct ethip_hdr *)&uip_buf[0])
#endif //MODIFY_FOR_IP210_UIP_ARP_C //================================================================================== 

/*-----------------------------------------------------------------------------------*/
/**
 * Initialize the ARP module.
 *
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_init(void)
{
#ifdef MODIFY_FOR_IP210_UIP_ARP_C //==================================================================================  
#else //MODIFY_FOR_IP210_UIP_ARP_C //================================================================================== 
#endif //MODIFY_FOR_IP210_UIP_ARP_C //================================================================================== 
  for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {
    memset(&arp_table[i], 0, sizeof(struct arp_entry));//20080819 chance modified
  }
  //chance 20100120 add
  memset(&m_gatewayinfo,0,sizeof(m_Gateway_Info));
  m_gatewayinfo.gip[0]=uip_arp_draddr[0];
  m_gatewayinfo.gip[1]=uip_arp_draddr[1];
	//20100122 chance add for new arp gateway method
	//os_cbk_arp_timeout();
	//*******************  
  m_gatewayinfo.timeout=os_cbk_arp_timeout();
  //***********************  
  //arp_flag=0;
}
/*-----------------------------------------------------------------------------------*/
/**
 * Periodic ARP processing function.
 *
 * This function performs periodic timer processing in the ARP module
 * and should be called at regular intervals. The recommended interval
 * is 10 seconds between the calls.
 *
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_timer(void)
{
  struct arp_entry *tabptr;
  
  ++arptime;
  m_gatewayinfo.counters++;//100 msec add once
  if(!m_gatewayinfo.init_flag)
  {
    //printf("\n\r arp1");
    //send query gateway arp
    send_arp_req();
    m_gatewayinfo.init_flag=1;
  }
  else
  {
    if(m_gatewayinfo.counters>m_gatewayinfo.timeout)
    {
      m_gatewayinfo.counters=0;
      //send query gateway arp
      send_arp_req();
    }
  }
  for(i = 0; i < UIP_ARPTAB_SIZE; ++i) 
  {
    tabptr = &arp_table[i];
  	/*if(tabptr->resend_times!=0)
  	{
  		tabptr->send_timer++; //every 100 ms plus 1
  		if(tabptr->send_timer>UIP_ARP_SEND_MAXAGE)// send delta time bigger than 2 second
  		{
		  		if( tabptr->resend_times >= UIP_ARP_RESEND_MAXTIME )//if resend times bigger than or equal 3 times clear this entry in table
		  		{
		  			memset(&arp_table[i], 0, sizeof(struct arp_entry ));//20080819 chance modified
		  		}
		  		else
		  		{  			
		  			tabptr->send_timer=0;
					send_arp_req(tabptr);					
				}
  		}
  	}*/    
    if((tabptr->ipaddr[0] | tabptr->ipaddr[1]) != 0 &&
       (ARP_Delta_Time(arptime,tabptr->time)>=UIP_ARP_MAXAGE)//20080819 chance modified
       //arptime - tabptr->time >= UIP_ARP_MAXAGE
       ) 
       {
       	    //check this ip address never resend 20080819
       	    //if(tabptr->resend_times==0)
       	    //{
	       	    //send ARP request to check alive chance add 20080819
	       	  //  send_arp_req(tabptr);
	       	   // tabptr->resend_times++;
       	    //}
      		memset(tabptr->ipaddr, 0, 4); //chance modified 20080819
       }
  }

}
/*-----------------------------------------------------------------------------------*/
static void
uip_arp_update(u16_t *in_ipaddr, struct uip_eth_addr *ethaddr)
{
    //chance add 20100120
    if(in_ipaddr[0] == m_gatewayinfo.gip[0] &&
       in_ipaddr[1] == m_gatewayinfo.gip[1]) 
    {     
      if(memcmp(ethaddr->addr,All0_buf,6))//if gateway mac is not all zero
      {
      
        memcpy(m_gatewayinfo.gmac,ethaddr->addr,6);
    /*printf("\n\r gu2 %02X %02X %02X %02X %02X %02X",(u16_t)m_gatewayinfo.gmac[0]
                                                    ,(u16_t)m_gatewayinfo.gmac[1]
                                                    ,(u16_t)m_gatewayinfo.gmac[2]
                                                    ,(u16_t)m_gatewayinfo.gmac[3]
                                                    ,(u16_t)m_gatewayinfo.gmac[4]
                                                    ,(u16_t)m_gatewayinfo.gmac[5]);   
    */
      }                                                                                                       
      return;
    }  
  //register struct arp_entry *tabptr;
  /* Walk through the ARP mapping table and try to find an entry to
     update. If none is found, the IP -> MAC address mapping is
     inserted in the ARP table. */
  for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {

    //tabptr = &arp_table[i];
    /* Only check those entries that are actually in use. */
    if(arp_table[i].ipaddr[0] != 0 &&
       arp_table[i].ipaddr[1] != 0) {

      /* Check if the source IP address of the incoming packet matches
         the IP address in this ARP table entry. */
      if(in_ipaddr[0] ==arp_table[i].ipaddr[0] &&
	 in_ipaddr[1] ==arp_table[i].ipaddr[1]) {
	 
	/* An old entry found, update this and return. */
	memcpy(arp_table[i].ethaddr.addr, ethaddr->addr, 6);
	arp_table[i].time = arptime;
	//chance 20080820
    //arp_table[i].send_timer = 0; 
    //arp_table[i].resend_times=0;
    //************************
	return;
      }
    }
  }

  /* If we get here, no existing ARP table entry was found, so we
     create one. */

  /* First, we try to find an unused entry in the ARP table. */
  for(i = 0; i < UIP_ARPTAB_SIZE; ++i) {
    //tabptr = &arp_table[i];
    if(arp_table[i].ipaddr[0] == 0 &&
      arp_table[i].ipaddr[1] == 0) {
      break;
    }
  }

  /* If no unused entry is found, we try to find the oldest entry and
     throw it away. */
  if(i == UIP_ARPTAB_SIZE) 
  {
  	//printf("\n\r O");
	    tmpage = 0;
	    c = 0;
	    for(i = 0; i < UIP_ARPTAB_SIZE; ++i) 
	    {
		      //tabptr = &arp_table[i];
		      //if(arptime - tabptr->time > tmpage) 
		      if(ARP_Delta_Time(arptime, arp_table[i].time) > tmpage) //chance 20080819 modified
		      {
					tmpage = arptime - arp_table[i].time;
					c = i;
		      }
	    }
	    i = c;
  }

  /* Now, i is the ARP table entry which we will fill with the new
     information. */
  //tabptr = &arp_table[i];
  memset(&arp_table[i], 0, sizeof(struct arp_entry ));//20080819 chance modified
  memcpy(arp_table[i].ipaddr, in_ipaddr, 4);
  memcpy(arp_table[i].ethaddr.addr, ethaddr->addr, 6);
  arp_table[i].time = arptime;
}
/*-----------------------------------------------------------------------------------*/
/**
 * ARP processing for incoming IP packets
 *
 * This function should be called by the device driver when an IP
 * packet has been received. The function will check if the address is
 * in the ARP cache, and if so the ARP cache entry will be
 * refreshed. If no ARP cache entry was found, a new one is created.
 *
 * This function expects an IP packet with a prepended Ethernet header
 * in the uip_buf[] buffer, and the length of the packet in the global
 * variable uip_len.
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_ipin(void)
{
  uip_len -= sizeof(struct uip_eth_hdr);
	
  /* Only insert/update an entry if the source IP address of the
     incoming IP packet comes from a host on the local network. */
  if( (IPBUF->destipaddr[0]!=uip_hostaddr[0])||
      (IPBUF->destipaddr[1]!=uip_hostaddr[1])
    )
  {
  	return;
  }
  if((IPBUF->srcipaddr[0] & uip_arp_netmask[0]) !=
     (uip_hostaddr[0] & uip_arp_netmask[0])) {
    return;
  }
  if((IPBUF->srcipaddr[1] & uip_arp_netmask[1]) !=
     (uip_hostaddr[1] & uip_arp_netmask[1])) {
    return;
  }
//chance add 20100115 do not update all zero or all ff source ip
  if(((IPBUF->srcipaddr[0]==0)&&(IPBUF->srcipaddr[1]==0))||
     ((IPBUF->srcipaddr[0]==0xFFFF)&&(IPBUF->srcipaddr[1]==0xFFFF)) 
    )
  {
    return;
  } 
//***************************************************************  
  //printf("\n\r  ipin %x %x ",(u16_t)IPBUF->srcipaddr[0],(u16_t)IPBUF->srcipaddr[1]);
  uip_arp_update(IPBUF->srcipaddr, &(IPBUF->ethhdr.src));
  
  return;
}
/*-----------------------------------------------------------------------------------*/
/**
 * ARP processing for incoming ARP packets.
 *
 * This function should be called by the device driver when an ARP
 * packet has been received. The function will act differently
 * depending on the ARP packet type: if it is a reply for a request
 * that we previously sent out, the ARP cache will be filled in with
 * the values from the ARP reply. If the incoming ARP packet is an ARP
 * request for our IP address, an ARP reply packet is created and put
 * into the uip_buf[] buffer.
 *
 * When the function returns, the value of the global variable uip_len
 * indicates whether the device driver should send out a packet or
 * not. If uip_len is zero, no packet should be sent. If uip_len is
 * non-zero, it contains the length of the outbound packet that is
 * present in the uip_buf[] buffer.
 *
 * This function expects an ARP packet with a prepended Ethernet
 * header in the uip_buf[] buffer, and the length of the packet in the
 * global variable uip_len.
 */
/*-----------------------------------------------------------------------------------*/
void
uip_arp_arpin(void)
{ 

  if(uip_len < sizeof(struct arp_hdr)) {
    uip_len = 0;
    return;
  }

  uip_len = 0;
  
  switch(BUF->opcode) {
  case HTONS(ARP_REQUEST):
    /* ARP request. If it asked for our address, we send out a
       reply. */
    if(BUF->dipaddr[0] == uip_hostaddr[0] &&
       BUF->dipaddr[1] == uip_hostaddr[1]) {
      /* The reply opcode is 2. */
      BUF->opcode = HTONS(2);

      memcpy(BUF->dhwaddr.addr, BUF->shwaddr.addr, 6);
      memcpy(BUF->shwaddr.addr, uip_ethaddr.addr, 6);
      memcpy(BUF->ethhdr.src.addr, uip_ethaddr.addr, 6);
      memcpy(BUF->ethhdr.dest.addr, BUF->dhwaddr.addr, 6);
      
      BUF->dipaddr[0] = BUF->sipaddr[0];
      BUF->dipaddr[1] = BUF->sipaddr[1];
      BUF->sipaddr[0] = uip_hostaddr[0];
      BUF->sipaddr[1] = uip_hostaddr[1];

      BUF->ethhdr.type = HTONS(UIP_ETHTYPE_ARP);      
      uip_len = sizeof(struct arp_hdr);
    }      
    break;
  case HTONS(ARP_REPLY):
    /* ARP reply. We insert or update the ARP table if it was meant
       for us. */
       //chance add 20100120
    /*if(BUF->dipaddr[0] == m_gatewayinfo.gip[0] &&
       BUF->dipaddr[1] == m_gatewayinfo.gip[1]) 
    {
      memcpy(m_gatewayinfo.gmac,BUF->shwaddr.addr,6);
    }
    else
    { */   
      if(BUF->dipaddr[0] == uip_hostaddr[0] &&
         BUF->dipaddr[1] == uip_hostaddr[1]) 
      {
        //printf("\n\r  arprep %x %x ",(u16_t)BUF->sipaddr[0],(u16_t)BUF->sipaddr[1]);
        uip_arp_update(BUF->sipaddr, &BUF->shwaddr);
      }
    //}
    break;
  }

  return;
}
void arp_packet_init()
{
  	    /* The destination address was not in our ARP table, so we
  	       overwrite the IP packet with an ARP request. */
  	    memset(arpbuf,0,MAX_ARP_BUF_LEN);//chance add 20100908 for independent arp buffer
  	    arp_len=0;
  	    memset(ARPBUF->ethhdr.dest.addr, 0xff, 6);
  	    memset(ARPBUF->dhwaddr.addr, 0x00, 6);
  	    memcpy(ARPBUF->ethhdr.src.addr, uip_ethaddr.addr, 6);
  	    memcpy(ARPBUF->shwaddr.addr, uip_ethaddr.addr, 6);
  	    
  	    ARPBUF->dipaddr[0] = ipaddr[0];
  	    ARPBUF->dipaddr[1] = ipaddr[1];
  	    ARPBUF->sipaddr[0] = uip_hostaddr[0];
  	    ARPBUF->sipaddr[1] = uip_hostaddr[1];
  	    ARPBUF->opcode = HTONS(ARP_REQUEST); /* ARP request. */
  	    ARPBUF->hwtype = HTONS(ARP_HWTYPE_ETH);
  	    ARPBUF->protocol = HTONS(UIP_ETHTYPE_IP);
  	    ARPBUF->hwlen = 6;
  	    ARPBUF->protolen = 4;
  	    ARPBUF->ethhdr.type = HTONS(UIP_ETHTYPE_ARP);
  	
  	    //uip_appdata = &uip_buf[40 + UIP_LLH_LEN];    
  	    //uip_len = sizeof(struct arp_hdr);
        arp_len = sizeof(struct arp_hdr);    
	      UipSend(arpbuf,arp_len);	
	      uip_len=0;
}
/*-----------------------------------------------------------------------------------*/
/**
 * Prepend Ethernet header to an outbound IP packet and see if we need
 * to send out an ARP request.
 *
 * This function should be called before sending out an IP packet. The
 * function checks the destination IP address of the IP packet to see
 * what Ethernet MAC address that should be used as a destination MAC
 * address on the Ethernet.
 *
 * If the destination IP address is in the local network (determined
 * by logical ANDing of netmask and our IP address), the function
 * checks the ARP cache to see if an entry for the destination IP
 * address is found. If so, an Ethernet header is prepended and the
 * function returns. If no ARP cache entry is found for the
 * destination IP address, the packet in the uip_buf[] is replaced by
 * an ARP request packet for the IP address. The IP packet is dropped
 * and it is assumed that they higher level protocols (e.g., TCP)
 * eventually will retransmit the dropped packet.
 *
 * If the destination IP address is not on the local network, the IP
 * address of the default router is used instead.
 *
 * When the function returns, a packet is present in the uip_buf[]
 * buffer, and the length of the packet is in the global variable
 * uip_len.
 */
/*-----------------------------------------------------------------------------------*/
u8_t uip_arp_out(void)
{
  struct arp_entry *tabptr;
  /* Find the destination IP address in the ARP table and construct
     the Ethernet header. If the destination IP addres isn't on the
     local network, we use the default router's IP address instead.

     If not ARP table entry is found, we overwrite the original IP
     packet with an ARP request for the IP address. */

  //if (BUF->ethhdr.type != HTONS(UIP_ETHTYPE_IP))return;

  /* Check if the destination address is on the local network. */
  if((IPBUF->destipaddr[0] & uip_arp_netmask[0]) !=
     (uip_hostaddr[0] & uip_arp_netmask[0]) ||
     (IPBUF->destipaddr[1] & uip_arp_netmask[1]) !=
     (uip_hostaddr[1] & uip_arp_netmask[1])) {
    /* Destination address was not on the local network, so we need to
       use the default router's IP address instead of the destination
       address when determining the MAC address. */
    ipaddr[0] = uip_arp_draddr[0];
    ipaddr[1] = uip_arp_draddr[1];
  } else {
    /* Else, we use the destination IP address. */
    ipaddr[0] = IPBUF->destipaddr[0];
    ipaddr[1] = IPBUF->destipaddr[1];
  }
  if(!memcmp((u8_t*)&ipaddr,(u8_t*)&uip_arp_draddr,4))
  {
/*    printf("\n\r gmac %02X %02X %02X %02X %02X %02X",(u16_t)m_gatewayinfo.gmac[0]
                                                    ,(u16_t)m_gatewayinfo.gmac[1]
                                                    ,(u16_t)m_gatewayinfo.gmac[2]
                                                    ,(u16_t)m_gatewayinfo.gmac[3]
                                                    ,(u16_t)m_gatewayinfo.gmac[4]
                                                    ,(u16_t)m_gatewayinfo.gmac[5]);
*/
    if(IPBUF->destipaddr[0]==0xffff&&IPBUF->destipaddr[1]==0xffff)
    {
    	  	//memset(BUF->ethhdr.dest.addr, 0xff, 6);
    	  	memset(IPBUF->ethhdr.dest.addr, 0xff, 6);
    }
    else
    {
      if(!memcmp(m_gatewayinfo.gmac,All0_buf,6))//if gateway da is all zero
      {
        //send arp query gateway packet
        send_arp_req();
    //printf("\n\ra3");      
        return 1;
      }
      else
      {
        memcpy(IPBUF->ethhdr.dest.addr, m_gatewayinfo.gmac, 6);
      }
    }                                                        
  }
  else
  {    
    for(i = 0; i < UIP_ARPTAB_SIZE; ++i) 
    {
      tabptr = &arp_table[i];
      if(ipaddr[0] == tabptr->ipaddr[0] &&
         ipaddr[1] == tabptr->ipaddr[1])
      {
        //if(!arp_flag)
        //{
        		tabptr->time = arptime;
  				  //chance 20080827
  			    //tabptr->send_timer = 0; 
  			    //tabptr->resend_times=0;
  			    //************************
        //}
        break;
      }
    }
    if(!(IPBUF->destipaddr[0]==0xffff&&IPBUF->destipaddr[1]==0xffff))
    {
  	  if( i == UIP_ARPTAB_SIZE)// || (arp_flag==1) ) 
  	  {
  	     arp_packet_init();
  	    /* The destination address was not in our ARP table, so we
  	       overwrite the IP packet with an ARP request. */
  	    /*   
  	    memset(BUF->ethhdr.dest.addr, 0xff, 6);
  	    memset(BUF->dhwaddr.addr, 0x00, 6);
  	    memcpy(BUF->ethhdr.src.addr, uip_ethaddr.addr, 6);
  	    memcpy(BUF->shwaddr.addr, uip_ethaddr.addr, 6);
  	    
  	    BUF->dipaddr[0] = ipaddr[0];
  	    BUF->dipaddr[1] = ipaddr[1];
  	    BUF->sipaddr[0] = uip_hostaddr[0];
  	    BUF->sipaddr[1] = uip_hostaddr[1];
  	    BUF->opcode = HTONS(ARP_REQUEST); // ARP request. 
  	    BUF->hwtype = HTONS(ARP_HWTYPE_ETH);
  	    BUF->protocol = HTONS(UIP_ETHTYPE_IP);
  	    BUF->hwlen = 6;
  	    BUF->protolen = 4;
  	    BUF->ethhdr.type = HTONS(UIP_ETHTYPE_ARP);
  	
  	    uip_appdata = &uip_buf[40 + UIP_LLH_LEN];    
  	    uip_len = sizeof(struct arp_hdr);      	    
  	    */
  //printf("\n\ra2");  	    
  	    return 1;
  	  }
    } 
    /* Build an ethernet header. */  
    if(IPBUF->destipaddr[0]==0xffff&&IPBUF->destipaddr[1]==0xffff)
    {
    	  	memset(IPBUF->ethhdr.dest.addr, 0xff, 6);
    }
    else
    {
    		memcpy(IPBUF->ethhdr.dest.addr, tabptr->ethaddr.addr, 6);
    }
  }
  memcpy(IPBUF->ethhdr.src.addr, uip_ethaddr.addr, 6);  
  IPBUF->ethhdr.type = HTONS(UIP_ETHTYPE_IP);
  uip_len += sizeof(struct uip_eth_hdr);
  return 0;
  //printf("\n\ra1");
}
//active send arp request packet to query mac address 20100120 chance
void send_arp_req()
{
    ipaddr[0]=m_gatewayinfo.gip[0] ;
    ipaddr[1]=m_gatewayinfo.gip[1];
    //arp_flag=1;
	//uip_arp_out();
	arp_packet_init();
	//UipSend(arpbuf,arp_len);	
	//uip_len=0;
//	tabptr->resend_times++;
}

/*-----------------------------------------------------------------------------------*/

/** @} */
/** @} */

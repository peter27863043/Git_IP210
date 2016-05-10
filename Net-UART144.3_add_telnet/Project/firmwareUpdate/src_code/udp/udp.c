#include "udp.h"
#include <stdio.h>
#include <string.h>

extern u8_t *uip_fw_buf;//sorbica071122
//20051021
/******************************************************************************
*
*   Name:           udp.c
*
*   Description:    User Datagram Protocol
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/


/******************************************************************************
*
*  Function:    udp_process
*
*  Description:  while system get the udp packet, it will send here to process
*               
*  Parameters:   None       
*               
*  Returns:     None
*               
*******************************************************************************/
/*void udp_process()
{	
			
	u16_t ipchksum, udpchksum;
	ipchksum =iphdr->ipchksum;			
	iphdr->ipchksum=0;				  		
	if(ipchksum!=~(uip_ipchksum()))
	{
		iphdr->ipchksum=ipchksum;
		return;
	}
	iphdr->ipchksum=ipchksum;
	
	udpchksum=udph->chksum;
	udph->chksum=0;
	if(udpchksum!=~(uip_udpchksum()))
	{
		udph->chksum=udpchksum;
		return;
	}
	udph->chksum=udpchksum;
	UDP_APPCALL();
}
*/
/******************************************************************************
*
*  Function:    udp_send
*
*  Description:  Use UDP send out data, set ip header and udp header
*               
*  Parameters:   length : The length of the data which needs to send
*               
*  Returns:     None
*               
*******************************************************************************/
void udp_send(u16_t length)
{
		if(length==0)
		{
			uip_len=0;
			return;
		}
		//Ethernet Header***************
		//Set Remote MAC address
	    memcpy(rx_eth->dest_mac, rx_eth->src_mac, 6);
		//Set My MAC address
		memcpy(rx_eth->src_mac, icp_netinfo.MY_MAC, 6);	
		//Type at the same with echo request packet

		//IP Header********************
		//
		//set remote ip address
		memcpy(iphdr->destipaddr, iphdr->srcipaddr, 4);
        //set my ip address
		iphdr->srcipaddr[0]=uip_hostaddr[0];
		iphdr->srcipaddr[1]=uip_hostaddr[1];
		//set ipid
		ipid++;
		iphdr->ipid[0]=ipid>>8;
		iphdr->ipid[1]=ipid & 0xff;
        // set the ip packet length
		iphdr->len=htons(IP_HEADER_LENGTH+UDP_HEADER_LENGTH+length);
        //set ip header checksum
		iphdr->ipchksum=0;
		//iphdr->ipchksum=(~(uip_ipchksum()));
        //set udp remote port
		udph->destport=udph->srcport;
        //set udp my port
		if(udph->destport == 8000)//joe 20080711
		udph->srcport=8000;
		else
		udph->srcport=TFTP_Port;
	  	//set udp header length
   		udph->len = htons(UDP_HEADER_LENGTH+length);
   		udph->chksum = 0;
  		/* Calculate UDP checksum. */
  		//udph->chksum = ~(uip_udpchksum());
  		if(udph->chksum == 0) {
    		udph->chksum= 0xffff;
  		}	
  		uip_len=iphdr->len+UIP_LLH_LEN; //14:eth_header

}
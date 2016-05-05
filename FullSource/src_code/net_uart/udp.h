/******************************************************************************
*
*   Name:           udp.h
*
*   Description:    
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#ifndef UDP_H
#define UDP_H
#include "type_def.h"
#define UIP_PROTO_UDP   17
#define UIP_TTL         255
void UDP_RECEIVE();
/* The UDP and IP headers. */
typedef struct {
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
  
  /* UDP header. */
  u16_t srcport,
    destport;
  u16_t udplen;
  u16_t udpchksum;
  u8_t udpload[1];//udp load data
} udpip_hdr;

#define UDP_LOAD_OFFSET
typedef struct {	
//public data members:
	u16_t SrcIPAddr[2];
	u16_t SrcPort;
	u16_t DestPort;
	u16_t DestIPAddr[2];	
	u16_t RxLength;		//read only

//private data members:
	u16_t IP_ID;
	udpip_hdr *p_DataPkt;
	u16_t *p_uip_len;
	u8_t* old_buf;
	
}CUdpData;
//public function members:
void udp_process();
void udp_init(u8_t* p_DataPkt,u16_t* p_uip_len, u16_t* hostaddr);
void udp_send(u16_t length);
u8_t* udp_set_buf(u8_t* p_DataPkt);
//1472=1514-ETH(14)-IP(20)-UDP(8)
#define udp_mss() 1472 
extern CUdpData UdpData;
extern u8_t EnVlanCheck();
/*---------------------------------------------------------------------------*/
/* This is where you configure if your CPU is big or little endian.          */
/*---------------------------------------------------------------------------*/
#ifdef _MSC_VER
#define BIG_ENDIAN   0
#else
#define BIG_ENDIAN   1
#endif

#if BIG_ENDIAN
    #define htons(n) (n)
    #define ntohs(n) (n)
    #define htonl(n) (n)
    #define ntohl(n) (n)
#else
    #define htons(n) ((((u16_t)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))
    #define ntohs(n) ((((u16_t)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))
    #define htonl(n)	(((((u32_t)n)& 0x000000ff)<<24)+((((u32_t)n)& 0x0000ff00)<<8) \
                         +((((u32_t)n)& 0x00ff0000)>>8)+((((u32_t)n)& 0xff000000)>>24))
    #define ntohl(n)	htonl(n)
#endif

#endif //UDP_H
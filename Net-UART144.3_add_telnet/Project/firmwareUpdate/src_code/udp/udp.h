#ifndef UDP_H
#define UDP_H

#include "..\net.h"//chance

#define UDP_APPCALL 	TFTP_Process

/*
typedef struct {

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
  

  u16_t srcport,
    destport;
  u16_t udplen;
  u16_t udpchksum;
} udpip_hdr;

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
	
}CUdpData;
*/
//public function members:
void udp_process();
//void udp_init(u8_t* p_DataPkt,u16_t* p_uip_len, u16_t* hostaddr);
void udp_send(u16_t length);

//extern CUdpData UdpData;
#endif

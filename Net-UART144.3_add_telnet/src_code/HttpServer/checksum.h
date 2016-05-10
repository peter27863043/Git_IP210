//#include "uip_arp.h"
#ifndef _checksum_h_
#define _checksum_h_
#include "net.h"//chance
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
  
  /* TCP header. */
  u16_t srcport,
    destport;
  u8_t seqno[4],  
    ackno[4],
    tcpoffset,
    flags,
    wnd[2];     
  u16_t tcpchksum;
  u8_t urgp[2];
  u8_t optdata[4];
}uip_tcpip_hdr;
#define Checksum_BUF ((uip_tcpip_hdr *)&uip_fw_buf[UIP_LLH_LEN])
#endif

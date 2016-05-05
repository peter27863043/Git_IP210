#ifndef __NET_H__
#define __NET_H__

#include "type_def.h"
#include "MACdriver.h"
#include "IP210reg.h"
#include "flash.h"
#include "fm_web_com.h"
//*********net information
#define UIP_ETHTYPE_ARP 0x0806
#define UIP_ETHTYPE_IP  0x0800
#define UIP_PROTO_ICMP   1
#define UIP_PROTO_UDP   17
#define UIP_TTL         255
#define TFTP_Port 69
#define Ethernet_CRC_LEN 4
#define UIP_LLH_LEN    (sizeof(ETH_HDR))//14   //length of ethernet header
#define IP_HEADER_LENGTH   (sizeof(IP_HEADER)) //20 length of IP Header
#define UDP_HEADER_LENGTH  (sizeof(UDP_HEADER)) //8 length of UDP Header
#define UIP_BUFSIZE      1520
#define TFTP_DATA_LEN 0x200
#define TFTP_Timeout 1000000
typedef struct
{
	u8_t  MY_MAC[6];
	u8_t  MY_IP[4];
	u8_t  MY_MASK[4];
	u8_t  MY_GW[4];
} NET_INFO;
extern  NET_INFO icp_netinfo;
//******************************
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

#define HTONS htons
//************Ethernet Header******************
/*typedef struct
{
    u8_t   dest_mac[6];
    u8_t   src_mac[6];
    u16_t  type;
} ETH_HDR;*/

//**********IPHeader***************
typedef struct{
    u8_t  vhl;
    u8_t  tos;
    u16_t  len;
    u8_t  ipid[2];
    u8_t  ipoffset[2];
    u8_t  ttl;
    u8_t  proto;
    u16_t ipchksum;
    u16_t srcipaddr[2], destipaddr[2];
/*
    u32_t srcipaddr;
    u32_t destipaddr;
*/
} IP_HEADER;
//*************UDP header*************
typedef struct {
  u16_t srcport, destport; 
  u16_t len, chksum;
}UDP_HEADER ;
//**************************************

extern volatile u16_t uip_len;
extern u16_t uip_hostaddr[2];

#define rx_eth  ((ETH_HDR *)&uip_fw_buf[0])
#define iphdr  ((IP_HEADER *)&uip_fw_buf[UIP_LLH_LEN])
#define udph  ((UDP_HEADER *)&uip_fw_buf[UIP_LLH_LEN+IP_HEADER_LENGTH])

//ICMP 
//ipid
extern u16_t ipid;  
//TFTP 
extern u16_t blok_num; 
extern u8_t  tftp_last_flag;
extern u16_t Rx_data_len;
extern u8_t  tftp_start_flag; //for last packet flag
extern u32_t tftp_counter;

//function
//u16_t uip_chksum(u16_t *sdata, u16_t len);
//u16_t uip_ipchksum(void);
//u16_t uip_udpchksum(void);
void TFTP_Process();
void udp_send(u16_t length);

#endif


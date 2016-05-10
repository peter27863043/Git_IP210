/******************************************************************************
*
*   Name:           dhcpc.h
*
*   Description:    
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#ifndef DHCPC_H
#define DHCPC_H
#include "type_def.h"
#include "udp.h"
#define MAX_OPTION_LEN 312
#define DHCPHeaderLen (sizeof(SDHCPPkt)-MAX_OPTION_LEN)
typedef struct _SDHCPPkt
{
	u8_t  op;
	u8_t  htype;
	u8_t  hlen;
	u8_t  hops;
	u32_t xid;
	u16_t secs;
	u16_t flags;
	u32_t ciaddr;
	u32_t yiaddr;
	u32_t siaddr;
	u32_t giaddr;
	u32_t chaddr[4];
	u8_t  sname[64];
	u8_t  file[128];
	   
	u8_t Options[MAX_OPTION_LEN];
}SDHCPPkt;
enum {sDHCP_DISABLE,sDHCPC_DONE,sDHCPC_NEEDDHCP,sDHCPC_WAIT_ACK};
typedef struct _SDHCPCInfo
{
	u16_t OptionCount;
	u32_t MyIPAddr;
	u32_t SrvIPAddr;
	u32_t SubnetMask;
	u32_t GetwayAddr;
	u32_t DNSAddr;
	u8_t  Status;
	u32_t TimeoutCounter;
	u32_t PktInterval;
	u32_t OneSecondCounter;
	u32_t LeaseConuntdown;
}SDHCPCInfo;
extern SDHCPCInfo DHCPCInfo;
//------------------------------------------------------------------------------------------
#define DHCP_BOOTREQUEST 1
#define DHCP_BOOTREPLY 2

#define DHCP_CLIENT_PORT 68  
#define DHCP_SERVER_PORT 67

#define DHCP_BROADCAST_FLAG 15
#define DHCP_BROADCAST_MASK (1 << DHCP_FLAG_BROADCAST)
#define MagicCookie 0x63825363
//------------------------------------------------------------------------------------------
//DHCP_OPTION_MESSAGE_TYPE values:
             //options:   50,51,52,53,54,55,57,61 //**:must, xx:not, mm:may
//for server:         
#define DHCP_OFFER    2 //xx,**,mm,**,**,xx,xx,xx
#define DHCP_ACK      5 //xx,**,mm,**,**,xx,xx,xx
#define DHCP_NAK      6 //xx,xx,xx,**,**,xx,xx,mm
//for client:
#define DHCP_DISCOVER 1 //mm,mm,mm,**,xx,mm,mm,mm
#define DHCP_REQUEST  3 //*x,mm,mm,**,*x,mm,mm,mm //50*:in selecting or init-reboot.
                                                  //50x:in bound or renewing may.
                                                  //54*:after selecting.
                                                  //54x:after init-reboot,bound,renewing or rebinding.
#define DHCP_DECLINE  4 //**,xx,mm,**,**,xx,xx,mm
#define DHCP_RELEASE  7 //xx,xx,mm,**,**,xx,xx,mm
#define DHCP_INFORM   8 //xx,xx,mm,**,xx,mm,mm,mm

//------------------------------------------------------------------------------------------
//DHCP options
#define DHCP_OPTION_SUBNET_MASK            01
#define DHCP_OPTION_GETWAY_ADDRESS         03
#define DHCP_OPTION_REQUESTED_IP           50 //RFC 2132 9.1, requested IP address
#define LEN_OF_DHCP_OPTION_REQUESTED_IP    04
#define LEN_OF_DHCP_OPTION_DNS             06
#define DHCP_OPTION_LEASE_TIME             51 //RFC 2132 9.2, time in seconds, in 4 bytes
#define DHCP_OPTION_OVERLOAD               52 //RFC2132 9.3, use file and/or sname field for options
#define DHCP_OPTION_MESSAGE_TYPE           53 //RFC 2132 9.6, important for DHCP
#define LEN_OF_DHCP_OPTION_MESSAGE_TYPE    01
#define DHCP_OPTION_SERVER_ID              54 //RFC 2132 9.7, server IP address
#define LEN_OF_DHCP_OPTION_SERVER_ID       04
#define DHCP_OPTION_PARAMETER_REQUEST_LIST 55 //RFC 2132 9.8, requested option types
#define DHCP_OPTION_MAX_MSG_SIZE           57 //RFC 2132 9.10, message size accepted >= 576
#define DHCP_OPTION_MAX_MSG_SIZE_LEN       02
#define DHCP_OPTION_CLIENT_ID              61
#define DHCP_OPTION_END_OF_LIST            255
//------------------------------------------------------------------------------------------
void DHCPCTimer();
void DHCPCInit();
void DHCPC_UDP_RECEIVE();
#define DHCP_PKT_INTERVAL 30
#define DHCP_WAIT_ACK_TIMEOUT 5
#define DHCP_IF_ARP 2
#define ONE_SECOND_COUNT 10
//------------------------------------------------------------------------------------------
#endif
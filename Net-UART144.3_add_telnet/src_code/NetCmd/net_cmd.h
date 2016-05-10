/******************************************************************************
*
*   Name:           net_cmd.h
*
*   Description:    
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
#include "module.h"
#ifdef MODULE_NET_CMD
#ifndef NET_CMD_H
#define NET_CMD_H
#include "type_def.h"
#define ETHER_TYPE_ARP    0x0806
#define ARP_USER_DEFINE   0xf000
#define ARP_HARDWARE_TYPE 1
#define EMPTY_CONN_ID     0xffff
#define CONN_ID_NOT_FIND  255
#define CONN_FULL         255
#define NET_CMD_TIMEOUT   30000 //Unit: 10ms
//==========================Local command===============================
#define LCMD_CmdData_OFFSET   (sizeof(SLCmd)+sizeof(SNetCmdPkt)-1)
#define LOCAL_CMD_ARP_HEADER_SIZE sizeof(SLCmd)
typedef struct _SLCmd
{
	u8_t  DA[6];		//0
	u8_t  SA[6];		//6
	u16_t EtherType;	//12
	u16_t HWType;		//14
	u16_t Protocol;		//16
}SLCmd;

typedef struct _SLCmdData
{
	u32_t ConnID;
}SLCmdData;
typedef struct _SSearchDevAck
{
	u16_t DevID;//0
    u8_t MAC[6];//2
    u16_t SIP[2];//8
	u16_t Netmask[2];//12
	u16_t Getway[2];//16
	u8_t Nickname[1];//20
}SSearchDevAck;

typedef struct _SSearchAckOP
{
	u8_t op_num;
	u8_t mode_op;	//Uart mode
	u8_t mode_len;
	u8_t mode_id;
#ifdef MODULE_FLASH512 
	u8_t ctrl_op;	//Control port
	u8_t ctrl_len;
	u16_t ctrl_port;
#endif
	u8_t tcmd_op;	//TCP Command port
	u8_t tcmd_len;
	u16_t tcmd_port;
	u8_t tel_op;	//Telnet port
	u8_t tel_len;
	u16_t tel_port;
#ifdef MODULE_VLAN_TAG
	u8_t vlan_op;	//VLAN TAG
	u8_t vlan_len;
	u8_t vlan_en;
	u8_t vlan_id[2];
#endif
	u8_t udp_op;	//UDP port
	u8_t udp_len;
	u16_t udp_lp;
	u16_t udp_rp;
	u8_t ip_port[1];
}SSearchAckOP;

#define LCMD_DEV_ACK_NICKNAME_OFFSET (sizeof(SSearchDevAck)-1)

void LocalCmd();
//==========================TCP command===============================
#define TCmd SNetCmdPkt

typedef struct _STCmdData
{
	u32_t ConnID;
	u16_t Port;
}STCmdData;
u8_t NetCmdOnTCPRx();
//==========================Net command OP===============================
#define NOT_NEED_LOGIN_OP 0x00ff

#define CONN_INVALID_ACK  	0x0000
#define SEARCH_DEVICE_REQ 	0x0001
#define SEARCH_DEVICE_ACK 	0x0002
#define LOGIN_REQUEST     	0x0005
#define LOGIN_MD5_KEY     	0x0006
#define LOGIN_ACCOUNT     	0x0007
#define LOGIN_RESULT      	0x0008
#define MP_PROCESS_REQ    	0x0009
#define MP_PROCESS_ACK    	0x000A
#define EEP_READ_REQ		0x0105
#define EEP_READ_ACK		0x0106
#define EEP_WRITE_REQ		0x0107
#define EEP_WRITE_ACK		0x0108
#define ACCESS_GPIO_REQ   	0x0109
#define ACCESS_GPIO_ACK   	0x010A
#define WOWNLOAD_EEP_REQ  	0x011B
#define WOWNLOAD_EEP_ACK  	0x011C
#define UPLOAD_EEP_REQ    	0x011D
#define UPLOAD_EEP_ACK    	0x011E
#define LOAD_DEF_EEP_REQ  	0x011F
#define LOAD_DEF_EEP_ACK  	0x0120
#define SINGLE_PIN_GPIO_REQ	0x0121
#define SINGLE_PIN_GPIO_ACK	0x0122
#define SINGLE_CHANNEL_REQ	0x0123
#define SINGLE_CHANNEL_ACK	0x0124
#define RESET_DEVICE_REQ	0x0125
#define RESET_DEVICE_ACK	0x0126
#define EN_LOOP_REQ			0x1007
#define EN_LOOP_ACK			0x1008
#define DIS_LOOP_REQ		0x1009
#define DIS_LOOP_ACK		0x100A
//==========================Net command==================================
#define MAX_NET_CMD_CONN 10
#define TypeLCmd 0 //Local command
#define TypeTCmd 1 //TCP command
#define TypeUCmd 2 //joe 20080702 NetCmdonUDP UDP command
typedef struct _SNetCmdConn
{
	u8_t SA[6];
	u32_t ConnID;
	u32_t Time;
	u32_t LoginID;
}SNetCmdConn;
typedef struct _SNetCmdPkt
{
    u8_t  MajorVersion; 
    u8_t  MinorVersion; 
	u16_t CmdLen;       
    u32_t ConnID;		
	u16_t CmdOP;		
	u8_t  CmdData[1];	
}SNetCmdPkt;
typedef struct _SNetCmdData
{
	SLCmdData LCmdData;
	STCmdData TCmdData;
	SNetCmdConn NetCmdConn[MAX_NET_CMD_CONN];
	SNetCmdPkt* NetCmdPkt;
	u8_t CurrentPktType;//TypeLCmd,TypeTCmd
}SNetCmdData;
extern SNetCmdData NetCmdData;
#define NetCmdPkt_SA (uip_buf+6)
void NetCmdInit();
void NetCmdTimer();

#endif
#endif //MODULE_NET_CMD

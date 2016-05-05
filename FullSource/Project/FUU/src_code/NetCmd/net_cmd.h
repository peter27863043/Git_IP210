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
#ifndef NET_CMD_H
#define NET_CMD_H
#include<string.h>
#include<stdio.h>
#include "type_def.h"
#include "kernelsetting.h"
#define ETHER_TYPE_ARP    0x0806
#define ARP_USER_DEFINE   0xf000
#define ARP_HARDWARE_TYPE 1
#define SEARCH_DEVICE_REQ 0x0001
#define SEARCH_DEVICE_ACK 0x0002
//==========================Local command===============================
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
  u16_t IP[2];//8
	u16_t Netmask[2];//12
	u16_t Getway[2];//16
	u8_t Nickname[1];//20
}SSearchDevAck;
typedef struct _SNetCmdPkt
{
  u8_t  MajorVersion; 
  u8_t  MinorVersion; 
	u16_t CmdLen;       
  u32_t ConnID;		
	u16_t CmdOP;		
	u8_t  CmdData[1];	
}SNetCmdPkt;
//extern u8_t *uip_fw_buf;//sorbica071122

#define LOCAL_CMD_ARP_HEADER_SIZE sizeof(SLCmd)
#define LCmd  ((SLCmd *)&uip_fw_buf[0])
#define LNetCmdPkt ((SNetCmdPkt*)(uip_fw_buf+LOCAL_CMD_ARP_HEADER_SIZE))
#define LCMD_CmdData_OFFSET   (sizeof(SLCmd)+sizeof(SNetCmdPkt)-1)
#define LCMD_DEV_ACK_NICKNAME_OFFSET (sizeof(SSearchDevAck)-1)
void LocalCmd();
void NetCmd();
void LocalCmdTxPrefix();
u16_t GetDeviceID();
void GetProjectName(char*);
#endif//NET_CMD_H

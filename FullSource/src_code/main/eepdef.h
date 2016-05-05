#include "module.h"
#define PROJECT_NUM 0x00001000
#define EEPROM_VER  0x00000001
#ifdef CUSTOMER_EEPROM
#include "..\..\src_code_customer\device\main\eepdef.h"
#else //CUSTOMER_EEPROM
#ifndef EEPDEF_H
#define EEPDEF_H
#include "type_def.h"
/******************************************************************************
*
*   Name:           eepdef.h
*
*   Description:    EEPROM intialization Header
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Grace
*
*******************************************************************************/
// Define application type
#define EE_OTPCODE

// COMMON EEPROM definition
#define size_EEchip_id			2
#define size_EEapp_id			  2
#define loc_EEStartAddr			0

//COMMON Network Information in EEPROM
#define size_EEMACID			6
#define size_EEIPAddr			4
#define size_EELoginName		16
#define size_EELoginPassword	16
#define size_EEComment			12
typedef struct _EEConfigInfo{
    u8_t MACID[size_EEMACID];			// 0-5
    u8_t IPAddress[size_EEIPAddr];     	// 6-9
    u8_t SubnetMask[size_EEIPAddr];    	// 10-13
    u8_t Gateway[size_EEIPAddr];       	// 0x0E - 0x11 0x96-0x99
    u8_t DNSAddress[size_EEIPAddr];    	// 0x12 - 0x15
    u8_t DHCPEnable;                    //0x16
    u8_t LoginName[size_EELoginName];   // 0x17 - 0x26
    u8_t LoginPassword[size_EELoginPassword];// 0x27 - 0x36
    u8_t Comment[size_EEComment];      	// 0x37 - 0x42
} uEEConfigInfo, *pEEConfigInfo;

// IP1726 and IP1718 use same EEPROM structure - 1024BYTE EEPROM
// OTP CODE 128Bytes EEPROM
#define loc_EEchip_id		0x00
#define loc_EEapp_id		0x04
#define loc_EEnetif			0x08

#define UART_INFO_DILIMITER_EN_CHAR1  0x1
#define UART_INFO_DILIMITER_EN_CHAR2  0x2
#define UART_INFO_DILIMITER_EN_SILENT 0x4
#define UART_INFO_OP_M_RS232 0
#define UART_INFO_OP_M_RS422 1
#define UART_INFO_OP_M_RS485 2
typedef struct _SUARTInfo
{
	u8_t OP_Mode;//0:RS232, 1:RS422 2:RS485
	u8_t Baudrate;
	u8_t Character_Bits;
    u8_t Parity_Type;
	u8_t Stop_Bit;
	u8_t HW_Flow_Control;
//--------extra parameter---------
	u8_t mode;//0:RS232, 1:RS485
	u8_t delimiter_en;//bit0:char1, bit1:char2, bit2:silent
	u8_t delimiter_drop;//1:drop char
	u8_t delimiter[3]; //0:char1, 1:char2, 2:time
	u8_t rd_io;	//1:input 0:output
	u8_t reserve[99];
}SUARTInfo;
#define DEFAULT_UARTINFO \
0x00,\
0x08,\  
0x03,\
0x00,\
0x00,\
0x00,\
0x00,\
0x00,\
0x00,\
0x00,0xff,0x05,\
1,\
""
//----DEFAULT_UARTINFO.end----
typedef struct _STelnetInfo
{
	//u8_t  IsClient;//server or client port
	u8_t op_mode;
	u16_t TelentPort;
	u16_t RemoteIPAddr[2];
	u16_t ClientTimeout;
	u16_t ctrl_port;
	u8_t enable_reverse_telnet;
	u16_t tcptimeout;
	u16_t OLD_CLI_RemoteIPAddr[2];
	u16_t CLI_RemotePort;
	u16_t CLI_timeout;
	u8_t cli_en;
	u16_t s_connect_time;
	u8_t ctr_en;
	u8_t CLI_RemoteIPAddr[30];
	u8_t RemoteDNSIPAddr[30];
	u8_t ip_demo;
	u8_t reserve[21];
}STelnetInfo;
#define DEFAULT_TELNETINFO \
0x01,\	
23,\          
0xD2C8,0xb566,\
20,\
6000,\
0,\
60,\
0x0000,0x0000,\
0x00,\
10,\
0,\
10,\
1,\
"0",\
"0",\
0,\
""
//----DEFAULT_TELNETINFO.end----
typedef struct _SNetCmdGroup
{
	u8_t LoginName[16];
	u8_t LoginPassword[16];
	u32_t FunctionEnable;	
}SNetCmdGroup;
// Grace

#define MAX_TCPCMD_GROUP_NUM 1  // 36bytes -- 128-(36+82)=10bytes for SMTP

typedef struct _SNetCmdInfo
{
	u8_t   GroupNum;
	u16_t  Port;
	SNetCmdGroup Group[MAX_TCPCMD_GROUP_NUM];
	u8_t reserve[100];
}SNetCmdInfo;
#define DEFAULT_NETCMDINFO \
0x01,\
0x13f0,\
"admin",\
"system",\
0x00000000,\
""
//----DEFAULT_NETCMDINFO.end----
typedef struct _udp_tel_rm_t
{
	u16_t ip[2];
	u16_t port;
}udp_tel_rm_t;
#define UDP_TEL_MAX_IP_RANGE 10
typedef struct _udp_tel_info_t
{
	u16_t local_port;
	udp_tel_rm_t remote[UDP_TEL_MAX_IP_RANGE];
	u8_t dido_value;
	u8_t dido_echo;
	u8_t dido_name[40];
	u16_t dido_port;
	u8_t reserve[66];
}udp_tel_info_t;
#define DEFAULT_UDP_TELNET \
21,\
0x0000, 0x0000, 0,\
0x0000, 0x0000, 0,\
0x0000, 0x0000, 0,\
0x0000, 0x0000, 0,\
0x0000, 0x0000, 0,\
0x0000, 0x0000, 0,\
0x0000, 0x0000, 0,\
0x0000, 0x0000, 0,\
0x0000, 0x0000, 0,\
0x0000, 0x0000, 0,\
0,\
1,\
'D', '1', 0x0, 0x0, 0x0,\
'D', '2', 0x0, 0x0, 0x0,\
'D', '3', 0x0, 0x0, 0x0,\
'D', '4', 0x0, 0x0, 0x0,\
'D', '5', 0x0, 0x0, 0x0,\
'D', '6', 0x0, 0x0, 0x0,\
'D', '7', 0x0, 0x0, 0x0,\
'D', '8', 0x0, 0x0, 0x0,\
5000,\
""
//----DEFAULT_NETCMDINFO.end----
typedef struct _udp_tel_dm_info
{
	u8_t dnsip[30];
	u8_t ip_demo; //0:IP 1:Demo
}udp_tel_dm_info;

typedef struct _dido_info_t
{
	u8_t direction[8];//[0]: dirction of DIDO_0(2=inout,0=out,1=in)
	udp_tel_dm_info remote[UDP_TEL_MAX_IP_RANGE];
	u8_t reserve[9];//100+219
}dido_info_t;
#define DEFAULT_DIDO \
1,1,1,0,0,0,0,0,\
"0", 0,\
"0", 0,\
"0", 0,\
"0", 0,\
"0", 0,\
"0", 0,\
"0", 0,\
"0", 0,\
"0", 0,\
"0", 0,\
""

#define loc_ModuleInfo (loc_EEnetif+sizeof(uEEConfigInfo)+100)
#include "../smtp/smtp_ee.h"
#define DEFAULT_SMTPINFO \
0,\
0,\
"username",\
"password",\
25,\
"umail.hinet.net",\
"ip210@icplus.com.tw;ip210@icplus.com.tw;ip210@icplus.com.tw;ip210@icplus.com.tw",\
"ip210@icplus.com.tw",\
"GPIO 01","GPIO 10","GPIO 11",\
"GPIO 01 body","GPIO 10 body","GPIO 11 body"
//----DEFAULT_SMTPINFO.end----
typedef struct _vlan_info
{
	u8_t vlan;
	u8_t value_id[2];
	u8_t reserve[10];
}vlan_info;
#define DEFAULT_VLANINFO \
0,\
0, 1,\
""



typedef struct _SModuleInfo 
{
	SUARTInfo   UARTInfo;
	STelnetInfo TelnetInfo;
	SNetCmdInfo NetCmdInfo;
	udp_tel_info_t UDPTelInfo;
  	dido_info_t dido_info;
	SSMTPInfo SMTPInfo;
	vlan_info VLANINFO;
}SModuleInfo;
typedef struct _EE_Info {
	u8_t chip_id[size_EEchip_id]; 			//0x00- must be 0xAA, 0x55
	u8_t reserve0[2];
	u8_t app_id[size_EEapp_id]; 			//0x02- Eth2RS232=0x0001
	u8_t reserve1[2];
	struct _EEConfigInfo netif;             //0x04
	u8_t reserve[100];
	SModuleInfo ModuleInfo;
	}	uEE_Info, *pEE_Info;
#define loc_EEModuleInfo (size_EEchip_id+2+size_EEapp_id+2+sizeof(struct _EEConfigInfo)+100)
#define loc_EEUDPTelInfo (loc_EEModuleInfo+sizeof(struct _SUARTInfo)+sizeof(struct _STelnetInfo)+sizeof(struct _SNetCmdInfo))
#define loc_EEDIDOInfo (loc_EEModuleInfo+sizeof(struct _SUARTInfo)+sizeof(struct _STelnetInfo)+sizeof(struct _SNetCmdInfo)+sizeof(struct _udp_tel_info_t))

typedef struct _uFW_EE_Info {
	struct _EEConfigInfo netif;             //0x04
    }uFW_EE_Info;

#define loc_EEMACID			(loc_EEnetif+0)		// 0-5
#define loc_EEIPAddress		(loc_EEnetif+6)     // 6-9
#define loc_EESubnetMask	(loc_EEnetif+10)   	// 10-13
#define loc_EEGateway		(loc_EEnetif+0xe)  	// 0x0E - 0x11
#define loc_EEDNSAddress	(loc_EEnetif+0x12) 	// 0x12 - 0x15
#define loc_EEDHCPAddress	(loc_EEnetif+0x16) 	// 0x12 - 0x15
#define loc_EELoginName		(loc_EEnetif+0x17)  // 0x17 - 0x26
#define loc_EELoginPassword	(loc_EEnetif+0x27)  // 0x27 - 0x36
#define loc_EEComment		(loc_EEnetif+0x37) 	// 0x37 - 0x42
#define loc_EEVlan			(loc_EEnetif+0x43) 	// 0x43

// Global variables and functions
//variables
extern uEEConfigInfo EEConfigInfo;

#ifndef FIRMWARE_UPDATE
extern uEE_Info	code EE_Default;
#else
extern uFW_EE_Info	code EE_Default;
#endif

extern u8_t			 EE_Result;
extern SModuleInfo ModuleInfo;

// Prototype functions
extern u8_t EEPROMCheckDataAvailable(void);
extern void EEPROMInit(void);
extern u8_t EEPROMLoadDefault(void);
extern void Restore_Default_VLANGroup();

extern u8_t SaveModuleInfo2EEPROM(void* pSubModule,u16_t size);

#endif //EEPDEF_H
#endif //CUSTOMER_EEPROM
/******************************************************************************
*
*   Name:           cbk2.c
*
*   Description:    call back routine
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/
//*******************Include files area**************************
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <intrins.h>
#include "timer.h"
#include "wdg.h"
#include "reg52.h"
#include "type_def.h"
#include "fs.h"
#include "httpd.h"
#include "uip.h"
#include "net_uart.h"
#include "..\sys\eeprom.h"
#include "..\main\eepdef.h"
#include "..\net_uart\dhcpc.h"
#include "IP210Reg.h"
#include "option.h"
#include "CommandLine.h"
#include "timer.h"
#ifdef MODULE_COMMAND_LINE
//-----------------------------------------
u8_t *CMD="CMD>";
u8_t flag=0;
u8_t code_flag=1;
u8_t result=0;
u8_t echo_flag=1;		//1:ON 0:OFF
u8_t cmd_flag=0;		//1:ON 0:OFF
u16_t code rs[3]={232,422,485};
void up2lwr(u8_t *buf, u8_t len);
u8_t *get_cmd2(u8_t *str);
u8_t ATD_flag=0;
u8_t ATD_close=0;
u32_t ATD_timer=0;
u8_t tmp[50]={0x0};
u8_t code car[14]="NO CARRIER\n\r";
u8_t code car1[5]="2\n\r";
u8_t listen_back=0;
u8_t back_2_CLI=1;
u8_t ip_info=0;
u8_t code ans[13]="NO ANSWER\n\r";
u8_t code ans1[5]="3\n\r";
//=======================================
SCLT code CLT[Command_NUM]={
#ifdef MODULE_NET_UART
ATA,		"ata",
ATD_T,		"ats7=",
S_TIMEOUT,	"ats8=",
LOADDEFAULT,"at&f",				// 2 (loaddefault)
T_CLIEND,	"atd ",				// 3 (change TCP to client)
F_CONTROL,	"at&k",				// 4 (flow control)
DIS_CON,	"ath",
#endif
ATSET,		"at+set",
EXIT,		"ato",				// 0 (exit)
R_CODE,		"atv",				// 1 (result code)
RESET,		"atz",				// 5 (reset)
ECHO,		"ate",				// 6 (echo) 
ATINFO,		"at+info",
ATFRESH,	"at+fresh",
AT,			"at",
U_MANE,		"username=",		// 7 
P_WORD,		"password=",		// 8 
IP,			"ip=",				// 9 
SUBNET,		"subnet=",			//10 
GATEWAY,	"gateway=",			//11 
DNS,		"dns=",				//12 
DHCP,		"dhcp=",			//13 
#ifdef MODULE_NET_UART
TELNET,		"opmode=",			//15 
R_TELNET,	"reverse telnet=",	//16 
D_PORT,		"data port=",		//17 
C_PORT,		"control port=",	//18 
SERVER_IP,	"server ip=",		//19 
TIMEOUT,	"timeout=",			//20 
T_TIMEOUT,	"tcp timeout=",
#ifdef MODULE_CLI_UDP
L_PORT,		"local port=",		//22 
R_IP,		"remote ip=",		//23 
R_PORT,		"remote port=",		//24 
#endif
UART_MODE,	"uart mode=",	 	//25 
CBAUDRATE,	"baudrate=",		//26 
CHARA,		"character=",		//27
CPARITY,	"parity=",			//28 
STOP,		"stop=",			//29 
D_CHARA1,	"delimiter1=",
D_CHARA2,	"delimiter2=",
D_TIME,		"silent=",
D_DROP,		"drop=",
#endif
N_NAME,		"nickname="
};

void hex_2_asc(u16_t value)
{
	u8_t ipset[6]={0};
	u32_t i;
	u8_t  pos=0,loop=0;
	if(value==0)
	{	ipset[0]=0x30;	}
	else
	{
		for(i=100000;i>=1;i=i/10)
		{
			if( ((value/i)>0)||(loop==1) )
			{
				ipset[pos]=0x30+value/i;
				value=value%i;
				loop=1;
				pos++;
				if(i==1)
				{	break;	}
			}
		}
	}
	UART_Transmit_Multi(ipset, (pos+1));
	return;
}

u8_t *cmd2;
u8_t z=0, za=1;
u8_t ATH_flag=0;
void command_type()
{ 
	u8_t *cmd=tmp;
	u8_t i=0, tu=0, len=0, Kont=0, Cont=0, atd=0;
#ifdef MODULE_NET_UART
	u8_t remote_ip[4]={0,0,0,0};
#endif
	u16_t rip=0;
	u8_t *loaddef="\n\rLoad Default....";	
	za=1;
	result=0;
	if(CLI_flag)
	{
		cmd2 = cmd+7;		//for at+set
		for(i=0;i<15;i++)
		{
			up2lwr(cmd, strlen(CLT[i].Name));
			if(!strncmp(cmd, CLT[i].Name, strlen(CLT[i].Name)))
			{
				if( (i==0)&&(listen_wait!=1) )
				{	break;	}
				else
				{	tu=1;	break;	}
			}
		} 
		if(tu)
		{
			switch(CLT[i].number_count)
			{
#ifdef MODULE_NET_UART
				case ATA:
					len=strlen(cmd);
					if(len==3)
					{
						ip210_isr_ctrl(0);
						RingInit(UartRxBuf,RxBuf,max_uart_rx_buf_len);
						RingInit(UartTxBuf,TxBuf,max_uart_tx_buf_len);
						ip210_isr_ctrl(1);
						CLI_flag=0;
						conn_flag=0;
						listen_wait=0;
						z=1;
					}
					else
					{	result=1;	z=0;	}
					break;
				case ATD_T:
					if(!strncmp(cmd+5, "?", 1))
					{
						len=strlen(cmd+5);
						if(len==1)
						{
							UART_Transmit_Multi(end, 4);
							hex_2_asc(NetUARTData.pTelnetData->pEEPROM_Telnet->CLI_timeout);
							z=1;
						}
						else
						{	result=1;	z=0;	}
					}
					else 
					{
						len=atoi(cmd+5);
						if(len==0 || len>255)
						{	result=1;	}
						else
						{	NetUARTData.pTelnetData->pEEPROM_Telnet->CLI_timeout=len;	result=0;	}
					}
					break;
				case S_TIMEOUT:
					if(!strncmp(cmd+5, "?", 1))
					{
						len=strlen(cmd+5);
						if(len==1)
						{
							UART_Transmit_Multi(end, 4);
							hex_2_asc(NetUARTData.pTelnetData->pEEPROM_Telnet->s_connect_time);
							z=1;
						}
						else
						{	result=1;	z=0;	}
					}
					else 
					{
						len=atoi(cmd+5);
						if(len==0 || len>255)
						{	result=1;	}
						else
						{	NetUARTData.pTelnetData->pEEPROM_Telnet->s_connect_time=len;	result=0;	}
					}
					break;
				case DIS_CON:
					if(TelnetData.ConnectStatus==tcConnected)
					{
						ATH_flag=1;
						if(code_flag){	UART_Transmit_Multi(car, 14);	}
						else{	UART_Transmit_Multi(car1, 5);	}
						TelnetData.ConnectStatus=tcDisconnected;
						conn_flag = 0;
						z=1;	za=0;
					}
/*
#ifdef MODULE_FLASH512
					else if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS)||
							 (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR) )
#else
					else if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP)
#endif
					{
						if(code_flag){	UART_Transmit_Multi(car, 14);	}
						else{	UART_Transmit_Multi(car1, 5);	}
						NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode=NU_OP_MODE_SERVER;
						SaveModuleInfo2EEPROM(&ModuleInfo.UDPTelInfo,sizeof(udp_tel_info_t));
					    SaveModuleInfo2EEPROM(TelnetData.pEEPROM_Telnet,sizeof(STelnetInfo));
						force_reset_countdown=1;
						z=1;	za=0;
					}
*/
					else
					{	result=1;	z=0;	}
					break;
				case T_CLIEND:		//ATD
					if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_SERVER)
					{
						len = strlen(cmd+4);
						if(len<9)
						{	result=1;	}	
						else
						{
							for(i=0;i<len;i++)
							{
								if(*(cmd+4+i) == ':')
								{
									memset(NetUARTData.pTelnetData->pEEPROM_Telnet->CLI_RemoteIPAddr, 0, 30);
									memcpy(NetUARTData.pTelnetData->pEEPROM_Telnet->CLI_RemoteIPAddr, cmd+4, i);
									NetUARTData.pTelnetData->pEEPROM_Telnet->CLI_RemotePort = (u16_t)atoi(cmd+4+i+1);
										break;
									}
								}
							if(i==len){
								result=1;
								break;
							}
							if(NetUARTData.pTelnetData->pEEPROM_Telnet->CLI_RemotePort>=0 && NetUARTData.pTelnetData->pEEPROM_Telnet->CLI_RemotePort<=65535)
							{
								z=1;
								ATD_flag=1;		//for change mode to connecting
							}
							else
							{	result=1;	}
						}
					}
					else
					{	result=1;	}
					break;
				case F_CONTROL:
					len = strlen(cmd+4);
					if(len==1)
					{
						i=*(cmd+4);	
						if(!strncmp(cmd+4, "?", 1))
						{
							if(UART_Settings.HW_Flow_Control==1)
							{
								UART_Transmit_Multi(end, 4);
								UART_Transmit_Multi(fc_n[0], 8);
								z=1;								
							}
							else
							{
								UART_Transmit_Multi(end, 4);
								UART_Transmit_Multi(fc_n[1], 4);	
								z=1;
							}
						}
						else if(i!=0x30&&i!=0x31)
						{	result=1;	z=0;	}
						else
						{	
							NetUARTData.pTelnetData->pEEPROM_UART->HW_Flow_Control = (i-0x30);
							UART_Settings.HW_Flow_Control = (i-0x30);
							result=0;	z=0;
						}
#ifdef MODULE_RS485
	    				if(NetUARTData.pTelnetData->pEEPROM_UART->OP_Mode!=UART_INFO_OP_M_RS232)
	    				{
	    					NetUARTData.pTelnetData->pEEPROM_UART->HW_Flow_Control=0;
	    					UART_Settings.HW_Flow_Control=0;
	    				}
#endif //MODULE_RS485
						if(result==0){
							SaveModuleInfo2EEPROM(NetUARTData.pTelnetData->pEEPROM_UART,sizeof(SUARTInfo));
						}
					}
					else
					{	result=1;	z=0;	}
					break;
#endif	//MODULE_NET_UART
				case AT:
					len=strlen(cmd);
					if(len==2)
					{	result=0;	z=0;	}
					else
					{	result=1;	z=0;	}
					break;
				case EXIT:
					if(strlen(cmd)>3)
					{	result=1;	}
#ifdef MODULE_FLASH512
					else if( (TelnetData.ConnectStatus == tcConnected)||(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS)||
							 (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR) )
#else
					else if( (TelnetData.ConnectStatus == tcConnected)||(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP) )
#endif
					{
#ifdef MODULE_NET_UART
						ip210_isr_ctrl(0);
						RingInit(UartRxBuf,RxBuf,max_uart_rx_buf_len);
						RingInit(UartTxBuf,TxBuf,max_uart_tx_buf_len);
						ip210_isr_ctrl(1);
						CLI_flag=0;
#ifdef MODULE_FLASH512
						if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS)||
							 (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR) )
#else
						if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP) )
#endif
						{
							if(TelnetData.RexmitLen)RingIncRi(UartRxBuf, TelnetData.RexmitLen);
							TelnetData.RexmitLen=0;
							udp_tel_data.initial_flag=0;
						}
#endif
						z=1;
					}
					else
					{	result=1;	}
					break;
				case R_CODE:
					len = strlen(cmd+3);
					i=*(cmd+3);
					if(len!=1)
					{	result=1;	}
					else if(i==0x30)
					{	
						code_flag = 0;
						result = 0;
					}
					else if(i==0x31)
					{
						code_flag = 1;
						result = 0;
					}
					else
					{	result = 1;	}
					break;
				case LOADDEFAULT:
					UART_Transmit_Multi(loaddef, strlen(loaddef));
					EEPROMLoadDefault();
					force_reset_countdown=1;
					result=0;
					break;
				case RESET:
					force_reset_countdown=1;	result=0;
					break;
				case ECHO:
					len = strlen(cmd+3);
					i = *(cmd+3);
					if(len==1)
					{
						if(i!=0x31 && i!=0x30 && i!=0x32 && i!=0x33)
						{	result=1;	z=0;	}
						else if(i==0x30 || i==0x31)
						{	echo_flag=(i-0x30);	result=0;	z=0;	}
						else
						{	cmd_flag=(i-0x32);	result=0;	z=0;	}
					}
					else
					{	result=1;	z=0;	}
					break;
				case ATSET:
					command_setting();
					break;
				case ATINFO:
					command_info();
					break;
				case ATFRESH:
					z=0;
					result=1;
					if(strlen(cmd+9)==3)
					{
						if(!strncmp(cmd, "at+fresh dns", 12))
						{
#ifdef MODULE_DNS
							udp_dns=5;
							udp_get_dns();
#endif
							result=0;
						}
					}
					break;
			}
			if(!z)
			{	re_rorn();	}
			else
			{	if(za){UART_Transmit_Multi(end, 4);}	}
			z=0;
		}
		else
		{
			if(strlen(cmd)>0){	result=1;	re_rorn();	}
			else{	UART_Transmit_Multi(end, 4);	}
		}   
	}//if(flag==1)
}
//---------------------------------------------------------------
void command_setting()
{

	u8_t i=0,Cont=0, Kont=0, len=0, c_name=0, tu=0;
	u8_t buf_udp[30]=0;
	u8_t ipset[3]={0,0,0}, re_ip[4]={0,0,0,0}, re=0, re_num=0;
	u8_t eep_val;
	u16_t re_port=0;
#ifdef MODULE_NET_UART
	u8_t *ser="TCP" ,*cli="UDP";
#ifdef MODULE_FLASH512
	u8_t *u_l="UDP Listen", *u_n="UDP Normal";
#endif
#endif
	u8_t *point=".";
	u8_t *on="on", *off="off";

	udp_tel_rm_t *ptr=(u8_t*)ModuleInfo.UDPTelInfo.remote;
	udp_tel_dm_info *ptd=(u8_t*)ModuleInfo.dido_info.remote;

	memcpy(buf_udp, cmd2, strlen(cmd2));
	for(c_name=15;c_name<Command_NUM;c_name++)
	{
		len = strlen(CLT[c_name].Name);
		up2lwr(buf_udp, len);
		if(!strncmp(buf_udp, CLT[c_name].Name, len))
		{	tu=1;	break;	}
	}
	memset(buf_udp, 0, 30);
	cmd2 += len;
	len = strlen(cmd2);
	if(tu)
	{
		z=1;
		switch(CLT[c_name].number_count)
		{
			case U_MANE:
				up2lwr(cmd2, len);
				if(len>15)
				{	result=1; z=0;	}
				else if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						UART_Transmit_Multi(EEConfigInfo.LoginName, 15);	
					}
					else
					{	result=1;	z=0;	}
				}
				else 
				{
					for(i=0;i<len;i++){
						if( ((cmd2[i]>='a')&&(cmd2[i]<='z'))||((cmd2[i]>='0')&&(cmd2[i]<='9'))||(cmd2[i]=='-')||(cmd2[i]=='_') ){
						}
						else{
							result=1;
							z=0;
							break;
						}
					}
					if(result==1){
						break;
					}
					memset(EEConfigInfo.LoginName, 0, size_EELoginName);
					memcpy(EEConfigInfo.LoginName, cmd2, len);
					for(i=0;i<len;i++)
					{	EEPROM_Write_Byte(loc_EELoginName+i, cmd2[i]);	}
					for(; i< size_EELoginName; i++)
					{	EEPROM_Write_Byte(loc_EELoginName+i,0x0);	}
					result=0;	z=0;
				}
				break;
			case P_WORD:
				up2lwr(cmd2, len);
				if(len>15)
				{	result=1; z=0; }
				else if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						UART_Transmit_Multi(EEConfigInfo.LoginPassword, 15);	
					}
					else
					{	result=1;	z=0;	}
				}
				else
				{
					for(i=0;i<len;i++){
						if( ((cmd2[i]>='a')&&(cmd2[i]<='z'))||((cmd2[i]>='0')&&(cmd2[i]<='9'))||(cmd2[i]=='-')||(cmd2[i]=='_') ){
						}
						else{
							result=1;
							z=0;
							break;
						}
					}
					if(result==1){
						break;
					}
					memset(EEConfigInfo.LoginPassword, 0, size_EELoginPassword);
					memcpy(EEConfigInfo.LoginPassword, cmd2, len);
					for(i=0;i<len;i++)
					{	EEPROM_Write_Byte(loc_EELoginPassword+i, cmd2[i]);	}
					for(; i< size_EELoginPassword; i++)
					{	EEPROM_Write_Byte(loc_EELoginPassword+i,0x0);	}
					result=0;	z=0;
				}
				break;
			case IP:
				if(len==0)
				{ result=1; z=0; }
				else if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{					
						UART_Transmit_Multi(end, 4);
						for(i=0;i<4;i++)
						{
							if (EE_Result == 1)
							{	EEPROM_Read_Byte(loc_EEIPAddress + i , &eep_val);	}
							else
							{	eep_val = EEConfigInfo.IPAddress[i];	}
							hex_2_asc((u16_t)eep_val);
							if(i!=3){	UART_Transmit_Multi(point, strlen(point));	}
						}
					}
					else
					{	result=1;	z=0;	}
				}
				else if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)
				{	result=1;	z=0;	}
				else
				{
					for(i=0;i<len;i++){
						if( ((cmd2[i]>='0')&&(cmd2[i]<='9'))||cmd2[i]=='.' ){
						}
						else{
							result=1;
							z=0;
							break;
						}
					}
					if(result==1){
						break;
					}
					for(i=0;i<len;i++)
					{	
						memset(ipset, 0, 3);
						if((cmd2[i] == '.'))
						{
							if(((i-Kont)==0)||((i-Kont)>3)){
								result=1;
							}
							memcpy(ipset, cmd2+Kont, i-Kont);
							Kont = i+1;
							if((atoi(ipset)>255)||(Kont==len)){
								result=1;
							}
							else{
							re_ip[Cont]=(u8_t)atoi(ipset);
							Cont++;
						}
					}
					}
					if((atoi(cmd2+Kont)>255)||(strlen(cmd2+Kont)>3)){
						result=1;
					}
					else{
					re_ip[Cont]=(u8_t)atoi(cmd2+Kont);
					}
					if( (Cont!=3)||(result==1) )
					{	result=1;	z=0;	}
					else
					{
						for(i=0;i<4;i++)
						{	EEPROM_Write_Byte(loc_EEIPAddress+i,(u8_t)re_ip[i]);	}
						result=0;	z=0;
					}
				}
				break;
			case SUBNET:
				if(len==0)
				{ result=1; z=0; }
				else if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						for(i=0;i<4;i++)
						{
							if (EE_Result == 1)
							{	EEPROM_Read_Byte(loc_EESubnetMask + i , &eep_val);	}
							else
							{	eep_val = EEConfigInfo.SubnetMask[i];	}
							hex_2_asc((u16_t)eep_val);
							if(i!=3){	UART_Transmit_Multi(point, strlen(point));	}
						}
					}
					else
					{	result=1;	z=0;	}
				}
				else if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)
				{	result=1;	z=0;	}
				else
				{
					for(i=0;i<len;i++){
						if( ((cmd2[i]>='0')&&(cmd2[i]<='9'))||cmd2[i]=='.' ){
						}
						else{
							result=1;
							z=0;
							break;
						}
					}
					for(i=0;i<len;i++)
					{
						memset(ipset, 0, 3);	
						if((cmd2[i] == '.'))
						{
							if(((i-Kont)==0)||((i-Kont)>3)){
								result=1;
							}
							memcpy(ipset, cmd2+Kont, i-Kont);
							Kont = i+1;
							if((atoi(ipset)>255)||(Kont==len)){
								result=1;
							}
							else{
							re_ip[Cont]=(u8_t)atoi(ipset);
							Cont++;
						}
					}
					}
					if((atoi(cmd2+Kont)>255)||(strlen(cmd2+Kont)>3)){
						result=1;
					}
					else{
					re_ip[Cont]=(u8_t)atoi(cmd2+Kont);
					}
					if( (Cont!=3)||(result==1) )
					{	result=1;	z=0;	}
					else
					{
						for(i=0;i<4;i++)
						{	EEPROM_Write_Byte(loc_EESubnetMask+i,(u8_t)re_ip[i]);	}
						result=0;	z=0;
					}
				}
				break;
			case GATEWAY:
				if(len==0)
				{ result=1; z=0; }
				else if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						for(i=0;i<4;i++)
						{
							if (EE_Result == 1)
							{	EEPROM_Read_Byte(loc_EEGateway + i , &eep_val);	}
							else
							{	eep_val = EEConfigInfo.Gateway[i];	}
							hex_2_asc((u16_t)eep_val);
							if(i!=3){	UART_Transmit_Multi(point, strlen(point));	}
						}
					}
					else
					{	result=1;	z=0;	}
				}
				else if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)
				{	result=1;	z=0;	}
				else
				{
					for(i=0;i<len;i++){
						if( ((cmd2[i]>='0')&&(cmd2[i]<='9'))||cmd2[i]=='.' ){
						}
						else{
							result=1;
							z=0;
							break;
						}
					}
					for(i=0;i<len;i++)
					{	
						memset(ipset, 0, 3);
						if((cmd2[i] == '.'))
						{
							if(((i-Kont)==0)||((i-Kont)>3)){
								result=1;
							}
							memcpy(ipset, cmd2+Kont, i-Kont);
							Kont = i+1;
							if((atoi(ipset)>255)||(Kont==len)){
								result=1;
							}
							else{
							re_ip[Cont]=(u8_t)atoi(ipset);
							Cont++;
						}
					}
					}
					if((atoi(cmd2+Kont)>255)||(strlen(cmd2+Kont)>3)){
						result=1;
					}
					else{
					re_ip[Cont]=(u8_t)atoi(cmd2+Kont);
					}
					if( (Cont!=3)||(result==1) )
					{	result=1;	z=0;	}
					else
					{
						for(i=0;i<4;i++)
						{	EEPROM_Write_Byte(loc_EEGateway+i,(u8_t)re_ip[i]);	}
						result=0;	z=0;
					}
				}
				break;
			case DNS:
				if(len==0)
				{ result=1; z=0; }
				else if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						for(i=0;i<4;i++)
						{
							if (EE_Result == 1)
							{	EEPROM_Read_Byte(loc_EEDNSAddress + i , &eep_val);	}
							else
							{	eep_val = EEConfigInfo.DNSAddress[i];	}
							hex_2_asc((u16_t)eep_val);
							if(i!=3){	UART_Transmit_Multi(point, strlen(point));	}
						}
					}
					else
					{	result=1;	z=0;	}
				}
				else if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)
				{	result=1;	z=0;	}
				else
				{
					for(i=0;i<len;i++){
						if( ((cmd2[i]>='0')&&(cmd2[i]<='9'))||cmd2[i]=='.' ){
						}
						else{
							result=1;
							z=0;
							break;
						}
					}
					for(i=0;i<len;i++)
					{	
						memset(ipset, 0, 3);
						if((cmd2[i] == '.'))
						{
							if(((i-Kont)==0)||((i-Kont)>3)){
								result=1;
							}
							memcpy(ipset, cmd2+Kont, i-Kont);
							Kont = i+1;
							if((atoi(ipset)>255)||(Kont==len)){
								result=1;
							}
							else{
							re_ip[Cont]=(u8_t)atoi(ipset);
							Cont++;
						}
					}
					}
					if((atoi(cmd2+Kont)>255)||(strlen(cmd2+Kont)>3)){
						result=1;
					}
					else{
					re_ip[Cont]=(u8_t)atoi(cmd2+Kont);
					}
					if( (Cont!=3)||(result==1) )
					{	result=1;	z=0;	}
					else
					{
						for(i=0;i<4;i++)
						{	EEPROM_Write_Byte(loc_EEDNSAddress+i,(u8_t)re_ip[i]);	}
						result=0;	z=0;
					}
				}
				break;
			case DHCP:
				up2lwr(cmd2, len);
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						if(EEConfigInfo.DHCPEnable==sDHCP_DISABLE)
						{	UART_Transmit_Multi(off, strlen(off));	}
						else
						{	UART_Transmit_Multi(on, strlen(on));	}
					}
					else
					{	result=1;	z=0;	}
				}
				else if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)
				{	result=1;	z=0;	}
				else if(!strncmp(cmd2,"on", 2))
				{
					if(len==2)
					{
						EEConfigInfo.DHCPEnable=sDHCPC_NEEDDHCP;	
						EEPROM_Write_Byte(loc_EEDHCPAddress,(u8_t)EEConfigInfo.DHCPEnable);
						result=0;	z=0;
					}
					else
					{	result=1;	z=0;	}
				}
				else if(!strncmp(cmd2, "off", 3))
				{
					if(len==3)
					{
						EEConfigInfo.DHCPEnable=sDHCP_DISABLE;	
						EEPROM_Write_Byte(loc_EEDHCPAddress,(u8_t)EEConfigInfo.DHCPEnable);
						result=0;	z=0;
					}
					else
					{	result=1;	z=0;	}
				}
				else
				{	result=1;	z=0;	}
				break;
			case N_NAME:
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						UART_Transmit_Multi(EEConfigInfo.Comment, 12);	
					}
					else
					{	result=1;	z=0;	}
				}
				else if(len>0&&len<13)
				{
					for(i=0;i<len;i++){
						if( ((cmd2[i]>='A')&&(cmd2[i]<='Z'))||((cmd2[i]>='a')&&(cmd2[i]<='z'))||((cmd2[i]>='0')&&(cmd2[i]<='9'))||(cmd2[i]=='-')||(cmd2[i]=='_') ){
						}
						else{
							result=1;
							z=0;
							break;
						}
					}
					if(result==1){
						break;
					}
					memset(EEConfigInfo.Comment, 0, size_EEComment);
					memcpy(EEConfigInfo.Comment, cmd2, len);
					for (i=0; i < len; i++)
					{	EEPROM_Write_Byte(loc_EEComment+i, cmd2[i]);	}
					for (; i < size_EEComment; i++)
					{	EEPROM_Write_Byte(loc_EEComment+i, 0);	}
					result=0;	z=0;
				}
				else
				{	result=1;	z=0;	}
				break;
#ifdef MODULE_NET_UART
			case TELNET:
				up2lwr(cmd2, len);
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
#ifdef MODULE_FLASH512
						if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP_LIS)&&
							(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP_NOR) )
#else
						if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP)
#endif
						{	UART_Transmit_Multi(ser, strlen(ser));	}
#ifdef MODULE_FLASH512
						else if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS)
						{ UART_Transmit_Multi(u_l, strlen(u_l)); }
						else if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR)
						{ UART_Transmit_Multi(u_n, strlen(u_n));}
#else
						else if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP)
						{	UART_Transmit_Multi(cli, strlen(cli));	}
#endif
					}
					else
					{	result=1;	z=0;	}
				}
				else if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)
				{	result=1;	z=0;	}
				else if(!strncmp(cmd2, "tcp", 3))
				{
					if(len==3){
						NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode=NU_OP_MODE_SERVER;
						result=0;
						z=0;
						force_reset_countdown=1;
					}
					else{	result=1;	z=0;	}
				}
#ifdef MODULE_FLASH512
				else if(!strncmp(cmd2, "udp listen", 10))
				{
					if(len==10)
					{
						NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode=NU_OP_MODE_UDP_LIS;
						force_reset_countdown=1;
						result=0;
						z=1;	
					}
					else{	result=1;	z=0;	}
				}
				else if(!strncmp(cmd2, "udp normal", 10))
				{
					if(len==10)
					{
						NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode=NU_OP_MODE_UDP_NOR;
						force_reset_countdown=1;
						result=0;
						z=1;	
					}
					else{	result=1;	z=0;	}
				}				
#else
				else if(!strncmp(cmd2, "udp", 3))
				{
					if(len==3)
					{
						NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode=NU_OP_MODE_UDP;
						force_reset_countdown=1;
						result=0;
						z=1;	
					}
					else{	result=1;	z=0;	}
				}
#endif
				else
				{	result=1;	z=0;	}
				if(result==0){
					SaveModuleInfo2EEPROM(NetUARTData.pTelnetData->pEEPROM_Telnet,sizeof(SUARTInfo));
				}
				break;
	#ifdef MODULE_REVERSE_TELNET
			case R_TELNET:
				up2lwr(cmd2, len);
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{	
						UART_Transmit_Multi(end, 4);
						if(NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet)
						{	UART_Transmit_Multi(on, 2);	}
						else
						{	UART_Transmit_Multi(off, 3);	}
					}
					else
					{	result=1;	z=0;	}
				}
				else if(!strncmp(cmd2, "on", 2))
				{
					if(len==2){	NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet=1;	result=0;	z=0;}
					else{	result=1;	z=0;	}
				}
				else if(!strncmp(cmd2, "off", 3))
				{
					if(len==3){	NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet=0;	result=0;	z=0;}
					else{	result=1;	z=0;	}
				}
				else
				{	result=1;	z=0;}
				if(result==0){
					SaveModuleInfo2EEPROM(NetUARTData.pTelnetData->pEEPROM_Telnet,sizeof(SUARTInfo));
				}				
				break;
	#endif
			case D_PORT:
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						hex_2_asc((u16_t)NetUARTData.pTelnetData->pEEPROM_Telnet->TelentPort);	
					}
					else
					{	result=1;	z=0;	}
				}
				else if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)
				{	result=1;	z=0;	}
				else if((len>0)&&(len<6))
				{
					for(i=0;i<len;i++){
						if((cmd2[i]>='0')&&(cmd2[i]<='9')){
						}
						else{
							result=1;
							z=0;
							break;
						}
					}
					if(len==5){
						if(cmd2[0]>='7'){
							result=1;
							z=0;
						}
					}
					if((atol(cmd2)<0)||(atol(cmd2)>65536)){
						result=1;
						z=0;
					}
					if(result==1){
						break;
					}
					NetUARTData.pTelnetData->pEEPROM_Telnet->TelentPort = atoi(cmd2);
					result=0;	z=0;
					if(result==0){
						SaveModuleInfo2EEPROM(NetUARTData.pTelnetData->pEEPROM_Telnet,sizeof(SUARTInfo));
					}
				}
				else
				{	result=1;	z=0;	}
				break;
			case C_PORT:
				if(!strncmp(cmd2, "?", 1))	
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						hex_2_asc((u16_t)NetUARTData.pTelnetData->pEEPROM_Telnet->ctrl_port);	
					}
					else
					{	result=1;	z=0;	}
				}
				else if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)
				{	result=1;	z=0;	}
				else if((len>0)&&(len<6))
				{
					for(i=0;i<len;i++){
						if( ((cmd2[i]>='0')&&(cmd2[i]<='9'))&&(atoi(cmd2)<=65536) ){
						}
						else{
							result=1;
							z=0;
							break;
						}
					}
					if(len==5){
						if(cmd2[0]>='7'){
							result=1;
							z=0;
						}
					}
					if((atol(cmd2)<0)||(atol(cmd2)>65536)){
						result=1;
						z=0;
					}
					if(result==1){
						break;
					}
					NetUARTData.pTelnetData->pEEPROM_Telnet->ctrl_port = atoi(cmd2);
					result=0;	z=0;
					if(result==0){
						SaveModuleInfo2EEPROM(NetUARTData.pTelnetData->pEEPROM_Telnet,sizeof(SUARTInfo));
					}
				}
				else
				{	result=1;	z=0;	}
				break;
			case SERVER_IP:
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						for(i=0;i<4;i++)
						{
							eep_val = ((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[i];
							hex_2_asc((u16_t)eep_val);
							if(i!=3){	UART_Transmit_Multi(point, strlen(point));	}
						}
					}
					else
					{	result=1;	z=0;	}
				}
				else if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)
				{	result=1;	z=0;	}
				else if(len==0)
				{ result=1; z=0; }
				else
				{
					for(i=0;i<len;i++){
						if( ((cmd2[i]>='0')&&(cmd2[i]<='9'))||cmd2[i]=='.' ){
						}
						else{
							result=1;
							z=0;
							break;
						}
					}
					if(result==1){
						break;
					}
					for(i=0;i<len;i++)
					{	
						memset(ipset, 0, 3);
						if((cmd2[i] == '.'))
						{
							if(((i-Kont)==0)||((i-Kont)>3)){
								result=1;
							}
							memcpy(ipset, cmd2+Kont, i-Kont);
							Kont = i+1;
							if((atoi(ipset)>255)||(Kont==len)){
								result=1;
							}
							else{
								re_ip[Cont]=(u8_t)atoi(ipset);
							Cont++;
						}
					}
					}
					if((atoi(cmd2+Kont)>255)||(strlen(cmd2+Kont)>3)){
						result=1;
					}
					else{
						re_ip[Cont]=(u8_t)atoi(cmd2+Kont);
					}
					if( (Cont!=3)||(result==1) )
					{	result=1;	z=0;	}
					else
					{
						for(i=0;i<4;i++)
						{	((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[i]=re_ip[i];	}
						NetUARTData.pTelnetData->pEEPROM_Telnet->ip_demo=0;
						result=0;	z=0;
					}
					if(result==0){
						SaveModuleInfo2EEPROM(NetUARTData.pTelnetData->pEEPROM_Telnet,sizeof(SUARTInfo));
					}
				}
				break;
			case TIMEOUT:
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						hex_2_asc((u16_t)NetUARTData.pTelnetData->pEEPROM_Telnet->ClientTimeout);	
					}
					else
					{	result=1;	z=0;	}
				}
				else if(len>0)
				{
					NetUARTData.pTelnetData->pEEPROM_Telnet->ClientTimeout=atoi(cmd2);
					result=0;
					z=0;
					SaveModuleInfo2EEPROM(NetUARTData.pTelnetData->pEEPROM_Telnet,sizeof(SUARTInfo));
				}
				else
				{	result=1;	z=0;	}
				break;
			case T_TIMEOUT:
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						hex_2_asc(NetUARTData.pTelnetData->pEEPROM_Telnet->tcptimeout);
					}
					else
					{	result=1;	z=0;	}
				}
				else if(len>0)
				{
					NetUARTData.pTelnetData->pEEPROM_Telnet->tcptimeout=atoi(cmd2);
					result=0;
					z=0;
					SaveModuleInfo2EEPROM(NetUARTData.pTelnetData->pEEPROM_Telnet,sizeof(SUARTInfo));
				}
				else
				{	result=1;	z=0;	}
				break;
#ifdef MODULE_CLI_UDP
//----------------------------------------Star UDP---------------------------------------
			case L_PORT:
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						hex_2_asc((u16_t)ModuleInfo.UDPTelInfo.local_port);	
					}
					else
					{	result=1;	z=0;	}
				}
				else if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)
				{	result=1;	z=0;	}
				else if((len>0)&&(len<6))
				{
					for(i=0;i<len;i++){
						if( ((cmd2[i]>='0')&&(cmd2[i]<='9'))&&(atoi(cmd2)<=65536) ){
						}
						else{
							result=1;
							z=0;
							break;
						}
					}
					if(len==5){
						if(cmd2[0]>='7'){
							result=1;
							z=0;
						}
					}
					if((atol(cmd2)<0)||(atol(cmd2)>65536)){
						result=1;
						z=0;
					}
					if(result==1){
						break;
					}
					ModuleInfo.UDPTelInfo.local_port = atoi(cmd2);
					SaveModuleInfo2EEPROM(&ModuleInfo.UDPTelInfo,sizeof(udp_tel_info_t));
					result=0;
					z=0;
				}
				else
				{	result=1;	z=0;}
				break;
			case R_IP:
				up2lwr(cmd2, len);
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						for(i=0;i<UDP_TEL_MAX_IP_RANGE;i++)
						{
							re_ip[0]=0x31+i; re_ip[1]=0x3A;
						UART_Transmit_Multi(end, 4);
							if(i==9)
						{
								re_ip[0]=0x31; re_ip[1]=0x30; re_ip[2]=0x3A;
								UART_Transmit_Multi(re_ip, 3);
							}
							else
							{ UART_Transmit_Multi(re_ip, 2); }
							UART_Transmit_Multi(ptd[i].dnsip, strlen(ptd[i].dnsip));
						}
					}
					else
					{	result=1;	z=0;	}
				}
				else if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)
				{	result=1;	z=0;	}
				else if(len==0)
				{	result=1;	z=0;}
				else
				{
					for(i=0;i<len;i++)
					{	
						if(cmd2[i] == ':')
						{
							if((strlen(cmd2+i+1)>2)||(i>30)){
								result=1;
							}
							re_num=(u16_t)(atoi(cmd2+i+1)-1);
								break;
							}
						}
					if((i==len)||(result==1)){
						result=1;
						z=0;
						break;
					}
					if(re_num>=0 && re_num<=9)
					{
						memset(ptd[re_num].dnsip, 0, 30);
						memcpy(ptd[re_num].dnsip, cmd2, i);
						ptd[re_num].ip_demo=1;
						result=0;
						z=0;
						SaveModuleInfo2EEPROM(&ModuleInfo.dido_info,sizeof(dido_info_t));
					}
					else
					{	result=1;	z=0;}
				}
				break;
			case R_PORT:
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						for(i=0;i<UDP_TEL_MAX_IP_RANGE;i++)
						{
							re_ip[0]=0x31+i; re_ip[1]=0x3A;
							UART_Transmit_Multi(end, 4);
							if(i==9)
							{ 
								re_ip[0]=0x31; re_ip[1]=0x30; re_ip[2]=0x3A;
								UART_Transmit_Multi(re_ip, 3);
							}
							else
							{ UART_Transmit_Multi(re_ip, 2); }
							hex_2_asc(ptr[i].port);
						}
					}
					else
					{	result=1;	z=0;	}
				}
				else if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)
				{	result=1;	z=0;	}
				else if(len<3)
				{	result=1;	z=0;}
				else
				{
					for(i=0;i<len;i++)
					{	
						if(cmd2[i] == ':')
						{
							re=1;
							memcpy(ipset, cmd2, i);
							Kont = i+1;	
							if(i>6){
								re=0;
							}
							else if(i==5){
								if(ipset[0]>='7'){
									re=0;
								}
							}
							if((atol(ipset)<65536)&&( (atoi(cmd2+Kont)>=1)&&(atoi(cmd2+Kont)<=10) )){
							re_port = (u16_t)atoi(ipset);
							}
							else{
								re=0;
							}
							break;
						}

					}
					if((Kont==len)||(Kont==0))
					{ re=0; }
					if(re)
					{
						re_num = (u16_t)(atoi(cmd2+Kont)-1);
						ptr[re_num].port = re_port;
						result=0;
						z=0;
						SaveModuleInfo2EEPROM(&ModuleInfo.UDPTelInfo,sizeof(udp_tel_info_t));
					}
					else
					{	result=1;	z=0;}
				}
				break;
//----------------------------------------------------------------------------------------------
#endif
			case UART_MODE:
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						hex_2_asc((u16_t)rs[NetUARTData.pTelnetData->pEEPROM_UART->OP_Mode]);
					}
					else
					{	result=1;	z=0;	}
				}
				else if(!strncmp(cmd2, "232", 3))
				{
					if(len==3)
					{
						NetUARTData.pTelnetData->pEEPROM_UART->OP_Mode = 0;
						P4_0=1;
						P4_5=1;
						ET0=0x0;
						result = 0;	z=0;
					}
					else
					{	result=1;	z=0;	}
				}
				else if(!strncmp(cmd2, "422", 3))
				{
					if(len==3)
					{
						NetUARTData.pTelnetData->pEEPROM_UART->OP_Mode = 1;
						P4_0=0;
						P4_5=0;
						ET0=0x0;
						result = 0;	z=0;
					}
					else
					{	result=1;	z=0;	}
				}
				else if(!strncmp(cmd2, "485", 3))
				{
					if(len==3)
					{
						NetUARTData.pTelnetData->pEEPROM_UART->OP_Mode = 2;
						P4_0=0;
						P4_5=0;
						result = 0;	z=0;
					}
					else
					{	result=1;	z=0;	}
				}
				else
				{	result = 1;	z=0;	}
				if(result==0){
					SaveModuleInfo2EEPROM(NetUARTData.pTelnetData->pEEPROM_UART,sizeof(SUARTInfo));
				}
				break;
			case CBAUDRATE:
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						UART_Transmit_Multi(BaudrateTable[NetUARTData.pTelnetData->pEEPROM_UART->Baudrate].Name, strlen(BaudrateTable[NetUARTData.pTelnetData->pEEPROM_UART->Baudrate].Name));		
					}
					else
					{	result=1;	z=0;	}
				}
				else if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)
				{	result=1;	z=0;	}
				else if(len>0)
				{
					result=1;	z=0;
					for(i=0;i<BAUDRATE_NUM;i++)
					{
						if(!strcmp(cmd2, BaudrateTable[i].Name))
						{
							NetUARTData.pTelnetData->pEEPROM_UART->Baudrate=i;
							UART_Settings.divisor = BaudrateTable[TelnetData.pEEPROM_UART->Baudrate].Divisor;
							result=0;
							SaveModuleInfo2EEPROM(NetUARTData.pTelnetData->pEEPROM_UART,sizeof(SUARTInfo));
						}
					}
				}
				else
				{	result=1;	z=0;}
				break;
			case CHARA:
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						hex_2_asc((u16_t)NetUARTData.pTelnetData->pEEPROM_UART->Character_Bits+5);
					}
					else
					{	result=1;	z=0;	}
				}
				else if(len>1)
				{	result=1;	z=0;}
				else
				{
					eep_val=atoi(cmd2);
					if(eep_val>=5 && eep_val<=8)
					{
						NetUARTData.pTelnetData->pEEPROM_UART->Character_Bits=eep_val-5;
						UART_Settings.Character_Bits=eep_val-5;
						result=0;	z=0;
						SaveModuleInfo2EEPROM(NetUARTData.pTelnetData->pEEPROM_UART,sizeof(SUARTInfo));
					}
					else
					{	result=1;	z=0;	}
				}
				break;
			case CPARITY:
				up2lwr(cmd2, len);
				if(!strncmp(cmd2, "?", 1))		
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						for(i=0;i<5;i++)
						{	if(pr_v[i]==NetUARTData.pTelnetData->pEEPROM_UART->Parity_Type){ break;	}	}	
						UART_Transmit_Multi(pr_n[i], strlen(pr_n[i]));					
					}
					else
					{	result=1;	z=0;	}
				}
				else if(len>0)
				{
					result=1;	z=0;
					for(i=0;i<5;i++)
					{	if(!strcmp(cmd2, pr_n[i]))
						{	result=0;	break;	}
					}
					if(result==0)
					{
						NetUARTData.pTelnetData->pEEPROM_UART->Parity_Type = pr_v[i];
						SaveModuleInfo2EEPROM(NetUARTData.pTelnetData->pEEPROM_UART,sizeof(SUARTInfo));
					}
				}
				else
				{	result=1;	z=0;}
				break;
			case STOP:
				up2lwr(cmd2, len);
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						UART_Transmit_Multi(sb_n[NetUARTData.pTelnetData->pEEPROM_UART->Stop_Bit], strlen(sb_n[NetUARTData.pTelnetData->pEEPROM_UART->Stop_Bit]));
					}
					else
					{	result=1;	z=0;	}
				}
				else if(len==1)
				{
					eep_val=atoi(cmd2);
					if(eep_val==1 || eep_val==2)
					{	
						NetUARTData.pTelnetData->pEEPROM_UART->Stop_Bit=eep_val-1;
						UART_Settings.Stop_Bit=eep_val-1;
						SaveModuleInfo2EEPROM(NetUARTData.pTelnetData->pEEPROM_UART,sizeof(SUARTInfo));
						result=0;	z=0;
					}
					else
					{	result=1;	z=0;	}
				}
				else
				{	result=1;	z=0;}
				break;
			case D_CHARA1:
				up2lwr(cmd2, len);
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);	
						if( (TelnetData.pEEPROM_UART->delimiter_en&0x1)==0 )
						{	UART_Transmit_Multi(off, strlen(off));	}
						else
						{
							re_ip[0]=0x3a;
							UART_Transmit_Multi(on, strlen(on));
							hex_to_asc(TelnetData.pEEPROM_UART->delimiter[0],re_ip+1);
							UART_Transmit_Multi(re_ip, 3);
						}
					}
					else
					{	result=1;	z=0;	}
				}
				else if(!strncmp(cmd2, "off", 3))
				{
					if(len==3)
					{	TelnetData.pEEPROM_UART->delimiter_en&=~0x1;	result=0;	z=0;	}
					else
					{	result=1;	z=0;	}						
				}
				else if(!strncmp(cmd2, "on:", 3))
				{
					if(len>3&&len<6)
					{
						TelnetData.pEEPROM_UART->delimiter_en|=0x1;
						TelnetData.pEEPROM_UART->delimiter[0]=asc_to_hex(cmd2+3);	result=0;	z=0;	
					}
					else
					{	result=1;	z=0;	}					
				}
				else
				{	result=1;	z=0;	}
				if(result==0){
					SaveModuleInfo2EEPROM(TelnetData.pEEPROM_UART,sizeof(SUARTInfo));
				}
				break;
			case D_CHARA2:
				up2lwr(cmd2, len);
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{	
						UART_Transmit_Multi(end, 4);
						if( (TelnetData.pEEPROM_UART->delimiter_en&0x2)==0 )
						{	UART_Transmit_Multi(off, 3);	}
						else
						{
							re_ip[0]=0x3a;
							UART_Transmit_Multi(on, strlen(on));
							hex_to_asc(TelnetData.pEEPROM_UART->delimiter[1],re_ip+1);
							UART_Transmit_Multi(re_ip, 3);
						}
					}
					else
					{	result=1;	z=0;	}
				}
				else if(!strncmp(cmd2, "off", 3))
				{
					if(len==3)
					{	TelnetData.pEEPROM_UART->delimiter_en&=~0x2;	result=0;	z=0;	}
					else
					{	result=1;	z=0;	}						
				}
				else if(!strncmp(cmd2, "on:", 3))
				{
					if(len>3&&len<6)
					{
						TelnetData.pEEPROM_UART->delimiter_en|=0x2;
						TelnetData.pEEPROM_UART->delimiter[1]=asc_to_hex(cmd2+3);	result=0;	z=0;	
					}
					else
					{	result=1;	z=0;	}					
				}
				else
				{	result=1;	z=0;	}
				if(result==0){
					SaveModuleInfo2EEPROM(TelnetData.pEEPROM_UART,sizeof(SUARTInfo));
				}
				break;
			case D_TIME:
				up2lwr(cmd2, len);
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{	
						UART_Transmit_Multi(end, 4);
						if( (TelnetData.pEEPROM_UART->delimiter_en&0x4)==0 )
						{	UART_Transmit_Multi(off, strlen(off));	}
						else
						{
							re_ip[0]=0x3a;
							UART_Transmit_Multi(on, strlen(on));
							UART_Transmit_Multi(re_ip, 1);
							hex_2_asc(TelnetData.pEEPROM_UART->delimiter[2]);
							
						}
					}
					else
					{	result=1;	z=0;	}
				}
				else if(!strncmp(cmd2, "off", 3))
				{
					if(len==3)
					{	TelnetData.pEEPROM_UART->delimiter_en&=~0x4;	result=0;	z=0;	}
					else
					{	result=1;	z=0;	}						
				}
				else if(!strncmp(cmd2, "on:", 3))
				{
					if(len>3&&len<7)
					{
						TelnetData.pEEPROM_UART->delimiter_en|=0x4;
						eep_val=atoi(cmd2+3);
						if(eep_val>=1 && eep_val<=255)
						{	TelnetData.pEEPROM_UART->delimiter[2]=eep_val;	result=0;	z=0;	}
						else
						{	result=1;	z=0;	}
					}
					else
					{	result=1;	z=0;	}					
				}
				else
				{	result=1;	z=0;	}
				if(result==0){
					SaveModuleInfo2EEPROM(TelnetData.pEEPROM_UART,sizeof(SUARTInfo));
				}
				break;
			case D_DROP:
				up2lwr(cmd2, len);
				if(!strncmp(cmd2, "?", 1))
				{
					if(len==1)
					{
						UART_Transmit_Multi(end, 4);
						if(TelnetData.pEEPROM_UART->delimiter_drop&0x1)
						{	UART_Transmit_Multi(on, strlen(on));	}
						else
						{	UART_Transmit_Multi(off, strlen(off));	}
					}
					else
					{	result=1;	z=0;	}
				}
				else if(!strncmp(cmd2, "off", 3))
				{
					if(len==3)
					{	TelnetData.pEEPROM_UART->delimiter_drop&=~0x1;	result=0;	z=0;	}
					else
					{	result=1;	z=0;	}
				}
				else if(!strncmp(cmd2, "on", 2))
				{
					if(len==2)
					{	TelnetData.pEEPROM_UART->delimiter_drop|=0x1;	result=0;	z=0;	}
					else
					{	result=1;	z=0;	}
				}
				else
				{	result=1;	z=0;	}
				if(result==0){
					SaveModuleInfo2EEPROM(TelnetData.pEEPROM_UART,sizeof(SUARTInfo));
				}
				break;
#endif	//MODULE_NET_UART
			default:
				result=1;	z=0;
				break;
		}
#ifdef MODULE_NET_UART
	  	UART_Set_Property();
		UART_Clear_RTS();
#endif
	}
	else
	{	z=0;	result=1;	}
}

void command_info()
{
	u8_t c_name=0, tu=0, len=0;
	u8_t information[20];
	u8_t *end="\n\r";
	if(cmd2[0]!=0x20){
		z=0;
		result=1;
		return;
	}
	cmd2=cmd2+1;
	if(strlen(cmd2)==0)	//just at+info
	{ z=0; result=1; return; }
	for(c_name=17;c_name<21;c_name++)
	{
		len = strlen(CLT[c_name].Name);
		up2lwr(cmd2, len);
		if(!strncmp(cmd2, CLT[c_name].Name, (len-1)))
		{	tu=1;	break;	}
	}
	z=1;
	cmd2+=(len-1);
	if( (strlen(cmd2)!=0)||(c_name==21) )
	{ z=0; result=1; return; }
	if(tu)
	{
		switch(CLT[c_name].number_count)
		{
			case IP:
				sprintf(information,"%s%d.%d.%d.%d",end,(u16_t)((u8_t*)uip_hostaddr)[0],(u16_t)((u8_t*)uip_hostaddr)[1],(u16_t)((u8_t*)uip_hostaddr)[2],(u16_t)((u8_t*)uip_hostaddr)[3]);
				UART_Transmit_Multi(information, strlen(information));
				break;
			case GATEWAY:
	    		sprintf(information,"%s%d.%d.%d.%d",end,(u16_t)((u8_t*)uip_arp_draddr)[0],(u16_t)((u8_t*)uip_arp_draddr)[1],(u16_t)((u8_t*)uip_arp_draddr)[2],(u16_t)((u8_t*)uip_arp_draddr)[3]);
				UART_Transmit_Multi(information, strlen(information));
				break;
			case SUBNET:
	    		sprintf(information,"%s%d.%d.%d.%d",end,(u16_t)((u8_t*)uip_arp_netmask)[0],(u16_t)((u8_t*)uip_arp_netmask)[1],(u16_t)((u8_t*)uip_arp_netmask)[2],(u16_t)((u8_t*)uip_arp_netmask)[3]);
				UART_Transmit_Multi(information, strlen(information));
				break;
			case DNS:
				sprintf(information,"%s%d.%d.%d.%d",end,(u16_t)((u8_t*)uip_dnsaddr)[0],(u16_t)((u8_t*)uip_dnsaddr)[1],(u16_t)((u8_t*)uip_dnsaddr)[2],(u16_t)((u8_t*)uip_dnsaddr)[3]);	
				UART_Transmit_Multi(information, strlen(information));
				break;
		}
	}

}
//-------------------------------------------------------
void re_rorn()
{
	u8_t *error="\n\rERROR\n\r";
	u8_t *ok="\n\rOK\n\r";
	u8_t *ok1="\n\r0\n\r";
	u8_t *error1="\n\r1\n\r";

	if(code_flag)
	{
		if(result)
		{	UART_Transmit_Multi(error, strlen(error));	}
		else
		{	UART_Transmit_Multi(ok, strlen(ok));	}
	}
	else
	{
		if(result)
		{	UART_Transmit_Multi(error1, strlen(error1));	}
		else
		{	UART_Transmit_Multi(ok1, strlen(ok1));	}
	}
}
//-------------------------------------------------------
void up2lwr(u8_t *buf, u8_t len)
{	
	u8_t i=0;

	for(i=0;i<len;i++){
		if(buf[i]>0x40 && buf[i]<0x5b){
			buf[i]+=0x20;
		}
	}
}
//--------------------------------------------------------
extern 	u8_t back_2_CLI;
void ATD_loop()
{
#ifdef MODULE_NET_UART
	if(ATD_flag)
	{
		if(TelnetData.ConnectStatus==tcConnected)
		{
			if(NetUARTData.pTelnetData->uip_conn_last!=uip_conn)
			{
				conn_flag = 0;
				TelnetData.ConnectStatus=tcDisconnected;
				TelnetData.WaitAck=0;
				ip210_isr_ctrl(0);
				RingInit(UartRxBuf,RxBuf,max_uart_rx_buf_len);
				RingInit(UartTxBuf,TxBuf,max_uart_tx_buf_len);
				ip210_isr_ctrl(1);
			}
		}
		TelnetData.ConnectStatus=tcDisconnected;
		ATD_flag=0;
		ATD_close=1;
		back_2_CLI=1;

		IP210_Update_Timer_Counter();
		ATD_timer = timercounter;
		IP210_Update_Timer_Counter();
	}
#endif
}

void help()
{

}

void CLI_init()
{
#ifdef MODULE_NET_UART
	#ifndef MODULE_CLI_CUSTOMIAZE
		if(NetUARTData.pTelnetData->pEEPROM_Telnet->cli_en)
	#endif
		{
#ifdef MODULE_FLASH512
			if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP_NOR)&&
				(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP_LIS) )
#else
			if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP)
#endif
			{
				NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode=NU_OP_MODE_SERVER;
			}
		}
	#ifdef MODULE_CLI_CUSTOMIAZE
		NetUARTData.pTelnetData->pEEPROM_UART->Baudrate=8;
		UART_Settings.divisor = BaudrateTable[8].Divisor;
	  	UART_Set_Property();
		UART_Clear_RTS();
	#endif
		SaveModuleInfo2EEPROM(TelnetData.pEEPROM_UART,sizeof(SUARTInfo));
		NetUartInit();
#endif
}

void CLI_Timer(){
#ifdef MODULE_NET_UART
		ATD_loop();
		WatchDogTimerReset();
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r2");rx_over=0;}
#endif
#endif
		if(ATD_close == 1)
		{
			if( Delta_Time(ATD_timer)>=(100*NetUARTData.pTelnetData->pEEPROM_Telnet->CLI_timeout) )
			{
				if(TelnetData.ConnectStatus == tcConnected)
				{	back_2_CLI=0;	}
				else
				{
					TelnetData.WaitAck=0;	CLI_flag=1;
					ATD_close=0;	conn_flag=0;	
					if(back_2_CLI==1)
					{
						if(code_flag){	UART_Transmit_Multi(ans, 13);	}
						else{	UART_Transmit_Multi(ans1, 5);	}
					}	
					else
					{
						if(code_flag){	UART_Transmit_Multi(car, 14);	}
						else{	UART_Transmit_Multi(car1, 5);	}
						back_2_CLI=1;	
					}
					if(cmd_flag)
					{	UART_Transmit_Multi(CMD, strlen(CMD));	}
				}
			}
		}
#ifndef MODULE_CLI_CUSTOMIAZE
		else if(NetUARTData.pTelnetData->pEEPROM_Telnet->cli_en)
#else
		else
#endif
		{
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r3");rx_over=0;}
#endif
#endif
			if(CLI_flag==0)
			{
#ifdef MODULE_FLASH512
				if( (TelnetData.ConnectStatus==tcConnected)||(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS)||(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR) )
#else
				if( (TelnetData.ConnectStatus==tcConnected)||(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP) )
#endif
				{	back_2_CLI=0;	}
				else
				{	
					TelnetData.WaitAck=0;	CLI_flag=1;
					conn_flag=0;
					if(back_2_CLI==0)
					{
						if(code_flag){	UART_Transmit_Multi(car, 14);	}
						else{	UART_Transmit_Multi(car1, 5);	}
						back_2_CLI=1;
					}
					if(cmd_flag)
					{	UART_Transmit_Multi(CMD, strlen(CMD));	}
				}
			}
		}
		if(listen_wait)
		{
			listen_back=1;
			if( Delta_Time(s_con_timer)>=(100*NetUARTData.pTelnetData->pEEPROM_Telnet->s_connect_time) )
			{	listen_wait=0;	conn_flag=0;	}
		}
#endif
}
#endif

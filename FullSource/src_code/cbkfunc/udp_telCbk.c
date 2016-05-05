#include <module.h>
#ifdef MODULE_CLI_CUSTOMIAZE
#else
#include "type_def.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>								  
#include "type_def.h"
#include "fs.h"
#include "httpd.h"
#include "uip.h"
#include "..\sys\eeprom.h"
#include "..\main\eepdef.h"
///#ifdef MODULE_NET_UART		//20081215 joe for module netuart testing
#include "net_uart.h"
//#endif //MODULE_NET_UART
#ifdef MODULE_UDP_TELNET
#include "..\HtmCfiles\udp_tel_htm.h"
#endif //MODULE_UDP_TELNET
extern u8_t errmsgflag ;
void eepromerr();
extern struct httpd_info *hs;
#define IPB1 0
#define IPB2 1
#define IPB3 2
#define IPB4 3
#define IPE1 4
#define IPE2 5
#define IPE3 6
#define IPE4 7
#define IPP1 8
u8_t ssi_udp_tel(u8_t varid, data_value *vp)
{
#ifdef MODULE_UDP_TELNET
	u8_t index=hs->row_num-1;
#ifdef MODULE_FLASH512
	u8_t ee_value=0, i=0;
	u16_t ee_port=0;
	u8_t name_buf[31]=0;
#endif
	if(hs->row_num>UDP_TEL_MAX_IP_RANGE)return ERROR;
	vp->type=digits_3_int;
	switch(varid)
	{
#ifdef MODULE_FLASH512
		case CGI_UDP_TEL_LPORT:
				EEPROM_Read_Byte(loc_EEUDPTelInfo, &ee_value);
				ee_port=ee_value;
				ee_port=ee_port<<8;
				EEPROM_Read_Byte(loc_EEUDPTelInfo+1, &ee_value);
				ee_port|=ee_value;
				vp->value.digits_3_int=ee_port;
				break;
		case CGI_TEL_UDP_LIS:
			   vp->value.string ="";
			   vp->type = string;
	       	   if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS)vp->value.string ="Checked";
				break;
		case CGI_TEL_UDP_NOR:
			   vp->value.string ="";
			   vp->type = string;
	       	   if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR)vp->value.string ="Checked";
				break;
		case CGI_TEL_UDP_DIS:
			   vp->value.string ="";
			   vp->type = string;
				if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP_LIS)&&
					(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP_NOR)
				  )
				{ vp->value.string ="Checked"; }
				break;
#else
		case CGI_UDP_TEL_LPORT:
				vp->value.digits_3_int=ModuleInfo.UDPTelInfo.local_port;
				break;
		case CGI_TEL_UDP_EN:
			   vp->value.string ="";
			   vp->type = string;
	       	   if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP)vp->value.string ="Checked";
				break;
		case CGI_TEL_UDP_DIS:
			   vp->value.string ="";
			   vp->type = string;
				if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP)vp->value.string ="Checked";
				break;
#endif
#ifdef MODULE_FLASH512
		case CGI_UDP_CHECK:
				vp->value.digits_3_int=hs->row_num;
				vp->type=digits_3_int;
				break;
		case CGI_UDP_NUM0:
		case CGI_UDP_NUM1:
		case CGI_UDP_NUM2:
		case CGI_UDP_NUM3:
		case CGI_UDP_NUM4:
		case CGI_UDP_NUM5:
				vp->value.digits_3_int=(6*(hs->row_num-1))+((varid-CGI_UDP_NUM0)/2);
				vp->type=digits_3_int;
				break;
#endif
	}
#ifdef MODULE_FLASH512
	if(varid==CGI_UDP_TEL_DEMO)
	{
		for(i=0;i<30;i++)
		{ EEPROM_Read_Byte(loc_EEDIDOInfo+(8+(u16_t)index*31+i) , &name_buf[i]); }
		vp->value.string=name_buf;
		vp->type=string;
	}
	else if(varid==CGI_UDP_TEL_RPORT)
	{
		EEPROM_Read_Byte(loc_EEUDPTelInfo+(2+index*6+4) , &ee_value);
		ee_port=ee_value;
		ee_port=ee_port<<8;
		EEPROM_Read_Byte(loc_EEUDPTelInfo+(2+index*6+5) , &ee_value);
		ee_port|=ee_value;
		vp->value.digits_3_int=ee_port;
		vp->type=digits_3_int;
	}
	else if(varid==CGI_UDP_TEL_IP1||varid==CGI_UDP_TEL_IP2||varid==CGI_UDP_TEL_IP3||varid==CGI_UDP_TEL_IP4)
	{
		u8_t offset=(varid-CGI_UDP_TEL_IP1)/2;

		EEPROM_Read_Byte(loc_EEUDPTelInfo+(2+index*6+offset) , &ee_value);
		vp->value.digits_3_int=ee_value;
		vp->type=digits_3_int;
	}
	else if(varid==CGI_UDP_IP)
	{
		if(ModuleInfo.dido_info.remote[index].ip_demo==0) // 0:IP 1:demo
		{ vp->value.string="checked"; }
		else
		{ vp->value.string=""; }
		vp->type=string;
	}
	else if(varid==CGI_UDP_DEMO)
	{
		if(ModuleInfo.dido_info.remote[index].ip_demo==1) // 0:IP 1:demo
		{ vp->value.string="checked"; }
		else
		{ vp->value.string=""; }
		vp->type=string;
	}
#else
	if(varid>CGI_UDP_TEL_LPORT)
	{
		u8_t index=hs->row_num-1;
		u8_t offset=(varid-1-CGI_UDP_TEL_LPORT)%5;
		u8_t* ptr=(u8_t*)ModuleInfo.UDPTelInfo.remote;
		if(offset!=4)
		{	vp->value.digits_3_int=ptr[index*6+offset];		}	//ip
		else
		{	vp->value.digits_3_int=*((u16_t*)(ptr+index*6+offset));		}	//port
	}
#endif
#else //MODULE_UDP_TELNET
int temp=(int)varid;
temp=(int)vp;
#endif //MODULE_UDP_TELNET
    return OK;
}
//extern void CGIstrcpy(char* str,char* cgistr);
u8_t cgi_udp_tel(u8_t cnt, void *vptr)
{
#ifdef MODULE_UDP_TELNET
	u8_t i=0,begin=0, j=0, count=0;
#ifdef MODULE_FLASH512
	u8_t ee_value=0;
	u16_t ee_port=0;
#endif
	req_data* vp2=vptr;
	NetUartTelnetClose();
	for(i=0;i<cnt;i++)
	{
		if(!strcmp(vp2[i].item,"en_radio"))
		{
#ifdef MODULE_FLASH512
			if(!strcmp(vp2[i].value, "udp_lis"))
				NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode=NU_OP_MODE_UDP_LIS;
			if(!strcmp(vp2[i].value, "udp_nor"))
				NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode=NU_OP_MODE_UDP_NOR;
#else
			if(!strcmp(vp2[i].value, "udp_en"))
				NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode=NU_OP_MODE_UDP;
#endif
			if(!strcmp(vp2[i].value, "udp_dis"))
				NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode=NU_OP_MODE_DISABLE;				
		}		
		if(!strcmp(vp2[i].item,"LPORT"))
		{
#ifndef MODULE_DNS
			begin=i+1;
#endif
			ModuleInfo.UDPTelInfo.local_port=atoi(vp2[i].value);
		}
#ifdef MODULE_DNS
		if(!strncmp(vp2[i].item, "ip_demo", 7))
		{
			if(!strcmp(vp2[i].value, "s_ip"))
			{ ModuleInfo.dido_info.remote[begin].ip_demo=0; }
			else
			{ ModuleInfo.dido_info.remote[begin].ip_demo=1; }
			begin++;
		}
#endif
	}
#ifdef MODULE_DNS
	for(i=0;i<cnt;i++)
	{
		if(!strcmp(vp2[i].item, "A"))
		{
			ModuleInfo.UDPTelInfo.remote[j].ip[0]=0;
			ModuleInfo.UDPTelInfo.remote[j].ip[1]=0;
			{
				u8_t* ptr=(u8_t*)ModuleInfo.UDPTelInfo.remote;
				for(count=0;count<4;count++)
				{ ptr[j*6+count]=(u8_t)atoi(vp2[i+count].value); }
			}
		}
		if(!strcmp(vp2[i].item, "E"))
		{
			ModuleInfo.dido_info.remote[j].dnsip[0]=0x30;
			memset(&ModuleInfo.dido_info.remote[j].dnsip[1], 0, 29);
//			if(ModuleInfo.UDPTelInfo.ip_demo[j]==1)
			{ memcpy(ModuleInfo.dido_info.remote[j].dnsip, vp2[i].value, strlen(vp2[i].value)); }
		}
		if(!strcmp(vp2[i].item, "P"))
		{
			ModuleInfo.UDPTelInfo.remote[j].port=(u16_t)atoi(vp2[i].value);
			j++;
		}
	}
#else
	for(i=0;i<5*UDP_TEL_MAX_IP_RANGE;i++)
	{
		u8_t index=i/5;
		u8_t offset=i%5;
		u8_t* ptr=(u8_t*)ModuleInfo.UDPTelInfo.remote;
		if(offset!=4)ptr[index*6+offset]=(u8_t)atoi(vp2[begin+i].value);
		else *((u16_t*)(ptr+index*6+offset))=atoi(vp2[begin+i].value);
	}
#endif
//	NetUartInit();
	SaveModuleInfo2EEPROM(&ModuleInfo.UDPTelInfo, sizeof(udp_tel_info_t));
	SaveModuleInfo2EEPROM(TelnetData.pEEPROM_Telnet, sizeof(STelnetInfo));
	SaveModuleInfo2EEPROM(&ModuleInfo.dido_info, sizeof(ModuleInfo.dido_info));
	//httpd_init_file(fs_open("udp_tel.htm",&hs->op_index));
	hs->errormsg = "RESET to effect";
	errmsgflag = ERRORMSG_SETOK;
	httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
#else //MODULE_UDP_TELNET
int temp=(int)cnt;
temp=(int)vptr;
#endif //MODULE_UDP_TELNET
    return OK;
}
#endif

#include <module.h>
#ifdef MODULE_64K_HTM	//for CLI 64K
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
#include "..\HtmCfiles\telnet_htm.h"
#ifdef MODULE_NET_UART
#include "net_uart.h"
#endif //MODULE_NET_UART
extern u8_t errmsgflag ;
void eepromerr();
extern struct httpd_info *hs;

u8_t ssi_telnet(u8_t varid, data_value *vp)
{
#ifdef MODULE_NET_UART

#ifdef MODULE_NET_UART_CTRL	//Net Uart & RFC2217
	u8_t code SCdis[36]="style='background:#CCCCCC' disabled";
#endif
	vp->value.string ="";
    vp->type = string;
    switch (varid)
    {
        case CGI_TELNET_CHK_S:
        	   if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_SERVER)vp->value.string ="Checked";
               break;
        case CGI_TELNET_CHK_C:
        	   if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_CLIENT)vp->value.string ="Checked";
               break;
        case CGI_TELNET_CHK_D:
        	   if((NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_CLIENT)&&(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_SERVER))vp->value.string ="Checked";
               break;
        case CGI_TELNET_PORT:
       	       vp->value.digits_3_int=NetUARTData.pTelnetData->pEEPROM_Telnet->TelentPort;
   			   vp->type=digits_3_int;
               break;
#ifdef MODULE_DNS
		case CGI_IP:
				if(NetUARTData.pTelnetData->pEEPROM_Telnet->ip_demo)
				{ vp->value.string=""; }
				else
				{ vp->value.string="checked"; }
				vp->type=string;
				break;
		case CGI_DEMO:
				if(NetUARTData.pTelnetData->pEEPROM_Telnet->ip_demo)
				{ vp->value.string="checked"; }
				else
				{ vp->value.string=""; }
				vp->type=string;
				break;
        case CGI_TELNET_DEMO:
        	   vp->value.string=NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteDNSIPAddr;
   			   vp->type=string;
               break;
#endif
        case CGI_TELNET_IP0:
        	   vp->value.digits_3_int=((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[0];
   			   vp->type=digits_3_int;
               break;
        case CGI_TELNET_IP1:
        	   vp->value.digits_3_int=((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[1];
   			   vp->type=digits_3_int;
               break;
        case CGI_TELNET_IP2:
        	   vp->value.digits_3_int=((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[2];
   			   vp->type=digits_3_int;
               break;
        case CGI_TELNET_IP3:
        	   vp->value.digits_3_int=((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[3];
   			   vp->type=digits_3_int;
               break;
//#else
/*
        case CGI_TELNET_IP0:
        	      vp->value.digits_3_int=((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[0];
   			   vp->type=digits_3_int;
               break;
        case CGI_TELNET_IP1:
        	      vp->value.digits_3_int=((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[1];
   			   vp->type=digits_3_int;
               break;
        case CGI_TELNET_IP2:
        	      vp->value.digits_3_int=((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[2];
   			   vp->type=digits_3_int;
               break;
        case CGI_TELNET_IP3:
        	   vp->value.digits_3_int=((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[3];
   			   vp->type=digits_3_int;
               break;
*/
        case CGI_TELNET_TIMEOUT:
			   vp->value.digits_3_int=NetUARTData.pTelnetData->pEEPROM_Telnet->ClientTimeout;
   			   vp->type=digits_3_int;
               break;
        case CGI_TCP_TIMEOUT:
				if( (NetUARTData.pTelnetData->pEEPROM_Telnet->tcptimeout<0)||(NetUARTData.pTelnetData->pEEPROM_Telnet->tcptimeout>99) )
				{	NetUARTData.pTelnetData->pEEPROM_Telnet->tcptimeout = EE_Default.ModuleInfo.TelnetInfo.tcptimeout;	}
				vp->value.digits_3_int=NetUARTData.pTelnetData->pEEPROM_Telnet->tcptimeout;
				vp->type=digits_3_int;
               break;
#ifdef MODULE_NET_UART_CTRL	//Net Uart & RFC2217
		case CGI_TELNET_CPORT:
				vp->value.digits_3_int=NetUARTData.pTelnetData->pEEPROM_Telnet->ctrl_port;
   			   	vp->type=digits_3_int;
			break;
		case CGI_CTR_DIS:
				if(NetUARTData.pTelnetData->pEEPROM_Telnet->ctr_en!=1)			
				{vp->value.string=SCdis;}
			break;
		case CGI_T2CHK:		//RFC2217 on
			if(NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet == 1)
			{	vp->value.string="disabled";}
			if(NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet == 2)
			{	vp->value.string="checked";	}
			break;
		case CGI_T2CHKR:	//Control port
			if(NetUARTData.pTelnetData->pEEPROM_Telnet->ctr_en == 1)vp->value.string="checked";
			break;
#endif//MODULE_NET_UART_CTRL
#ifdef MODULE_REVERSE_TELNET
		case CGI_T1CHK:	//reverse telnet on
			if(NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet == 1)vp->value.string="checked";
			break;
		case CGI_T1CHKR:	//reverse telnet off
			if(NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet != 1)vp->value.string="checked";
			break;
#endif //MODULE_REVERSE_TELNET
#ifdef MODULE_COMMAND_LINE
		case CGI_TELNET_CLI:
			if(NetUARTData.pTelnetData->pEEPROM_Telnet->cli_en == 1)vp->value.string="checked";
			break;
#endif
#ifdef MODULE_COMMAND_LINE
		case CGI_CLI_L:
			vp->value.string="";
			vp->type=string;
			break;
#else
			vp->value.string="<!--";
			vp->type=string;
			break;
#endif
#ifdef MODULE_COMMAND_LINE
		case CGI_CLI_R:
			vp->value.string="";
			vp->type=string;
			break;
#else
			vp->value.string="-->";
			vp->type=string;
			break;
#endif
    }
#else //MODULE_NET_UART
int temp=(int)varid;
temp=(int)vp;
#endif //MODULE_NET_UART
    return OK;
}

u8_t cgi_telnet(u8_t cnt, void *vptr)
{
	u8_t en_re_rf=0, count=0;
#ifdef MODULE_NET_UART
	req_data *vp = vptr;
	int i;	
	NetUartTelnetClose();
#ifdef MODULE_REVERSE_TELNET
	NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet=0;
#endif //MODULE_REVERSE_TELNET	
#ifdef MODULE_COMMAND_LINE
	NetUARTData.pTelnetData->pEEPROM_Telnet->cli_en=0;
#endif
#ifdef MODULE_NET_UART_CTRL
	NetUARTData.pTelnetData->pEEPROM_Telnet->ctr_en=1;
#endif
	for(i=0;i<cnt;i++)
	{
		if(!strcmp(vp[i].value, "s_ip"))
		{ NetUARTData.pTelnetData->pEEPROM_Telnet->ip_demo=0; }
		if(!strcmp(vp[i].value, "s_demo"))
		{ NetUARTData.pTelnetData->pEEPROM_Telnet->ip_demo=1; }
	}
    for (i=0; i < cnt; i++, vp++)
    {        		
			if(!strcmp(vp->item, "sc_radio"))
			{
				if(!strcmp(vp->value, "server"))
					NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode=NU_OP_MODE_SERVER;
				if(!strcmp(vp->value, "client"))
					NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode=NU_OP_MODE_CLIENT;				
				if(!strcmp(vp->value, "disable"))
					NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode=NU_OP_MODE_DISABLE;				
			}
#ifdef MODULE_REVERSE_TELNET			
			if(!strcmp(vp->item, "t1"))
			{
				en_re_rf = atoi(vp->value);
				if(en_re_rf == 1)
				{	NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet = 1;	}
			}			
#endif //MODULE_REVERSE_TELNET
			if(!strcmp(vp->item, "port"))
			{
				NetUARTData.pTelnetData->pEEPROM_Telnet->TelentPort=atoi(vp->value);
			}
#ifdef MODULE_NET_UART_CTRL		//RFC2217 & Net Uart Control
			if(!strcmp(vp->item, "cport"))
			{
				NetUARTData.pTelnetData->pEEPROM_Telnet->ctrl_port=atoi(vp->value);
			}
			if(!strcmp(vp->item, "t2"))
			{
				en_re_rf=atoi(vp->value);
				if(en_re_rf==2)
				{
					if(NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet!=1)
					{
						NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet=2;
						NetUARTData.pTelnetData->pEEPROM_Telnet->ctr_en=0;
					}
				}
			}
#endif //MODULE_NET_UART_CTRL
#ifdef MODULE_DNS
			if(!strcmp(vp->item, "demo"))
			{
				memset(NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteDNSIPAddr, 0, 30);
				if(NetUARTData.pTelnetData->pEEPROM_Telnet->ip_demo)
				{ memcpy(NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteDNSIPAddr, vp->value, strlen(vp->value)); }
			}
			if(!strcmp(vp->item, "IP0"))
			{
				NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr[0]=0;
				NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr[1]=0;
				if(NetUARTData.pTelnetData->pEEPROM_Telnet->ip_demo==0)
				{
					for(count=0;count<4;count++)
					{ ((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[count]=atoi((vp+count)->value); }
				}
			}
/*
			if(!strcmp(vp->item, "IP1"))
			{
				if(NetUARTData.pTelnetData->pEEPROM_Telnet->ip_demo==0)
				{ ((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[1]=atoi(vp->value); }
			}
			if(!strcmp(vp->item, "IP2"))
			{
				if(NetUARTData.pTelnetData->pEEPROM_Telnet->ip_demo==0)
				{ ((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[2]=atoi(vp->value); }
			}
			if(!strcmp(vp->item, "IP3"))
			{
				if(NetUARTData.pTelnetData->pEEPROM_Telnet->ip_demo==0)
				{ ((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[3]=atoi(vp->value); }
			}
*/
#else
			if(!strcmp(vp->item, "IP0"))
			{
				((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[0]=atoi(vp->value);
			}
			if(!strcmp(vp->item, "IP1"))
			{
				((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[1]=atoi(vp->value);
			}
			if(!strcmp(vp->item, "IP2"))
			{
				((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[2]=atoi(vp->value);
			}
			if(!strcmp(vp->item, "IP3"))
			{
				((u8_t*)NetUARTData.pTelnetData->pEEPROM_Telnet->RemoteIPAddr)[3]=atoi(vp->value);
			}
#endif
			if(!strcmp(vp->item, "c_timeout"))
			{
				NetUARTData.pTelnetData->pEEPROM_Telnet->ClientTimeout=atoi(vp->value);
			}
			if(!strcmp(vp->item, "t_timeout"))
			{
				NetUARTData.pTelnetData->pEEPROM_Telnet->tcptimeout=atoi(vp->value);
			}
#ifdef MODULE_COMMAND_LINE
			if(!strcmp(vp->item, "CLI"))
			{
				NetUARTData.pTelnetData->pEEPROM_Telnet->cli_en=1;
			}
#endif
    }
	NetUartInit();
    SaveModuleInfo2EEPROM(TelnetData.pEEPROM_Telnet,sizeof(STelnetInfo));
    //httpd_init_file(fs_open("telnet.htm",&hs->op_index));
		hs->errormsg = "RESET to effect";
		errmsgflag = ERRORMSG_SETOK;
		httpd_init_file(fs_open("errormsg.htm",&hs->op_index));    
#else //MODULE_NET_UART
int temp=(int)cnt;
temp=(int)vptr;
#endif //MODULE_NET_UART
    return OK;
}
#endif


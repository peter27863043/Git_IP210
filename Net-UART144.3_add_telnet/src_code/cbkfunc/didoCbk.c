#include <module.h>
#ifdef MODULE_DIDO
#include "type_def.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>								  
#include "type_def.h"
#include "public.h"
#include "fs.h"
#include "httpd.h"
#include "uip.h"
#include "..\sys\eeprom.h"
#include "..\main\eepdef.h"
#include "..\HtmCfiles\dido_htm.h"
#include "dido.h"
#include "..\net_uart\dhcpc.h"

extern struct httpd_info *hs;
extern char selected[];
extern u8_t errmsgflag;


void eepromerr();


u8_t ssi_dido(u8_t varid, data_value *vp)
{
#ifdef MODULE_TCP_DIDO
	u8_t name_buf[5]=0;
	u8_t i=0;
#endif
	u8_t eep_val;
    u8_t dido_value=0, direction=0;
    vp->value.string="";
    vp->type = string;

	switch(varid)
	{

		case CGI_IP_STATIC:
			if(EEConfigInfo.DHCPEnable==sDHCP_DISABLE)vp->value.string="checked";
			break;
			
		case CGI_IP_DHCP:
			if(EEConfigInfo.DHCPEnable!=sDHCP_DISABLE)vp->value.string="checked";
			break;
		
		case CGI_DIDO_DNS1:
		case CGI_DIDO_DNS2:
		case CGI_DIDO_DNS3:
		case CGI_DIDO_DNS4:
			if (EE_Result == 1)
			{	EEPROM_Read_Byte(loc_EEDNSAddress + (varid-CGI_DIDO_DNS1), &eep_val);	}
			else
			{	eep_val = EEConfigInfo.DNSAddress[(varid-CGI_DIDO_DNS1)];	}
			vp->value.digits_3_int=eep_val;
			vp->type=digits_3_int;
			break;
		
		
		case CGI_DIDO_GW1:
		case CGI_DIDO_GW2:
		case CGI_DIDO_GW3:
		case CGI_DIDO_GW4:
			if (EE_Result == 1)
			{	EEPROM_Read_Byte(loc_EEGateway+ (varid-CGI_DIDO_GW1), &eep_val); }
			else
			{	eep_val = EEConfigInfo.Gateway[(varid-CGI_DIDO_GW1)];	}
			vp->value.digits_3_int=eep_val;
			vp->type=digits_3_int;
			break;
			
		case CGI_DIDO_SM1:
		case CGI_DIDO_SM2:
		case CGI_DIDO_SM3:
		case CGI_DIDO_SM4:
			if (EE_Result == 1)
			{	EEPROM_Read_Byte(loc_EESubnetMask + (varid-CGI_DIDO_SM1), &eep_val); }
			else
			{	eep_val = EEConfigInfo.SubnetMask[(varid-CGI_DIDO_SM1)]; }
			vp->value.digits_3_int=eep_val;
			vp->type=digits_3_int;
			break;
		
		case CGI_DIDO_IP1:
		case CGI_DIDO_IP2:
		case CGI_DIDO_IP3:
		case CGI_DIDO_IP4:
			if (EE_Result == 1)
			{	EEPROM_Read_Byte(loc_EEIPAddress + (varid-CGI_DIDO_IP1) , &eep_val); }
			else
			{	eep_val = EEConfigInfo.IPAddress[(varid-CGI_DIDO_IP1)];	}
			vp->value.digits_3_int=eep_val;
			vp->type=digits_3_int;
			break;
		 

	
	}
	return OK;
}
u8_t cgi_dido(u8_t cnt, void *vptr)
{
	u8_t i;
	u8_t chr;
	req_data* vp2=vptr;
	if (EE_Result == 1)
	{
		for (i=0; i < cnt; i++, vp2++)
    	{        		
    		if(!strcmp(vp2->item, "ip_from"))
    		{
				if(!strcmp(vp2->value, "2"))
				{
					if(EEConfigInfo.DHCPEnable!=sDHCP_DISABLE)
					{
						EEConfigInfo.DHCPEnable=sDHCP_DISABLE;
						//NetworkConfig();//restore IP from eeprom_info
					}
				}	
				if(!strcmp(vp2->value, "1"))
				{
					if(EEConfigInfo.DHCPEnable==sDHCP_DISABLE)
					{
						EEConfigInfo.DHCPEnable=sDHCPC_NEEDDHCP;

						// peter add the following statements for dhcp function
						//DHCPCInfo.Status=sDHCPC_NEEDDHCP;
						//DHCPCInfo.TimeoutCounter=0;
						//DHCPCInfo.PktInterval=0;
						//DHCPCInfo.OneSecondCounter=0;
						
						//DHCPCInfo.Status=sDHCPC_NEEDDHCP;
					}				
				}
			}	
		}
		vp2=vptr;
		for(i=1;i<17;i++)
		{
			if((atoi(vp2[i].value)>255) || (atoi(vp2[i].value)<0))
			{
				{httpd_init_file(fs_open("dido.htm",&hs->op_index));return OK;	}
			}
		}

		// for test , don't save the eeprom data
		//httpd_init_file(fs_open("dido,htm",&hs->op_index));
		//return OK;

		 EEPROM_Write_Byte(loc_EEDHCPAddress,EEConfigInfo.DHCPEnable);
		
		EEPROM_Write_Byte(loc_EEchip_id, 0x00);

		//write back IP,SM,GW,DNS
		for(i=1;i<17;i++)
			{EEPROM_Write_Byte((loc_EEIPAddress+i-1),(u8_t)atoi(vp2[i].value) );}

		
		for(i=1;i<17;i++)
		{
		    EEPROM_Read_Byte((loc_EEIPAddress+i-1),&chr) ;
		    if(i<4)EEConfigInfo.IPAddress[i]=chr;
		    if(chr!=(char)atoi(vp2[i].value))
		   	{
			    hs->errormsg = "Check EEPROM!";
			    errmsgflag = ERRORMSG_SETNO;
			    httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
				return OK;
			}
		}
			//write check byte
			EEPROM_Write_Byte(loc_EEchip_id, EE_Default.chip_id[0]);
			hs->errormsg = "RESET to effect";
			errmsgflag = ERRORMSG_SETOK;
			httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
	}
	else
	{
			eepromerr();
	}
	return OK;
	
}
#endif //MODULE_DIDO

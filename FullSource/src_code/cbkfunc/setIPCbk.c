#include <module.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef MODULE_CLI_CUSTOMIAZE
#else
#include "type_def.h"
#include "..\sys\eeprom.h"
#include "..\main\eepdef.h"							  
#include "fs.h"
#include "httpd.h"
#include "uip.h"
#include "setIP_htm.h"
#ifdef MODULE_DHCPC
#include "..\net_uart\dhcpc.h"
#endif //MODULE_DHCPC
#ifdef MODULE_FLASH512
u16_t asc_to_hex(char asc[]);
extern u8_t errmsgflag ;
void eepromerr();
extern struct httpd_info *hs;
#ifdef MODULE_VLAN_TAG
extern void hex_to_asc(u8_t hex, s8_t *buf);
extern u16_t asc_to_hex(char asc[]);
#endif
/******************************************************************************
*
*  Function:    ssi_setip
*
*  Description: Handle IP Html tag information
*               
*  Parameters:  varid: SSI Tag ID, vp: SSI value and type.
*               
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*               
*******************************************************************************/
u8_t ip_chged=0;
u8_t ssi_setip(u8_t varid, data_value *vp)
{	
	
	u8_t eep_val;
  	ip_chged=1;
	// Save field by field in case the CGI sequence is different from
	// IPAddr->NetMask->Gateway
	eep_val=0;
	if (varid >= CGI_SET_IP1 && varid < (CGI_SET_IP1 + size_EEIPAddr))
	{ // valid varid
		if (EE_Result == 1)
		{
			EEPROM_Read_Byte(loc_EEIPAddress + (varid-CGI_SET_IP1) , &eep_val);
		} else
		{
			eep_val = EEConfigInfo.IPAddress[(varid-CGI_SET_IP1)];
		}
	} else
	if (varid >= CGI_SET_SM1 && varid < (CGI_SET_SM1 + size_EEIPAddr))
	{ // valid varid
		if (EE_Result == 1)
		{
			EEPROM_Read_Byte(loc_EESubnetMask + (varid-CGI_SET_SM1), &eep_val);
		} else
		{
			eep_val = EEConfigInfo.SubnetMask[(varid - CGI_SET_SM1)];
		}
	}else
	if (varid >= CGI_SET_GW1 && varid < (CGI_SET_GW1 + size_EEIPAddr))
	{ // valid varid
		if (EE_Result == 1)
		{
			EEPROM_Read_Byte(loc_EEGateway + (varid-CGI_SET_GW1), &eep_val);
		} else
		{
			eep_val = EEConfigInfo.Gateway[(varid-CGI_SET_GW1)];
		}
	}
#ifdef MODULE_DHCPC
		else if(varid==CGI_IP_STATIC)
		{
			if(EEConfigInfo.DHCPEnable==sDHCP_DISABLE)vp->value.string="checked";
			else vp->value.string="";
		}
		else if(varid==CGI_IP_DHCP)
		{
			if(EEConfigInfo.DHCPEnable==sDHCP_DISABLE)vp->value.string="";
			else vp->value.string="checked";
		}
#endif //MODULE_DHCPC	

#ifdef MODULE_DNS
	else if (varid >= CGI_SET_DNS1 && varid < (CGI_SET_DNS1 + size_EEIPAddr))
	{ // valid varid
		if (EE_Result == 1)
		{
			EEPROM_Read_Byte(loc_EEDNSAddress + (varid-CGI_SET_DNS1), &eep_val);
		} else
		{
			eep_val = EEConfigInfo.DNSAddress[(varid-CGI_SET_DNS1)];
		}
	}
#endif //MODULE_DNS
	if((varid==CGI_IP_STATIC)||(varid==CGI_IP_DHCP))
	{
		vp->type=string;		
	}
	else
	{
		vp->value.digits_3_int=eep_val;
		vp->type=digits_3_int;
	}
#ifdef MODULE_VLAN_TAG
	if(varid==CGI_VLAN_EN)
	{
		if(ModuleInfo.VLANINFO.vlan==1)
		{ vp->value.string="checked"; }
		else
		{ vp->value.string=""; }
		vp->type=string;
	}
	if(varid==CGI_VLAN_DIS)
	{
		if(ModuleInfo.VLANINFO.vlan==0)
		{ vp->value.string="checked"; }
		else
		{ vp->value.string=""; }
		vp->type=string;
	}
	if(varid==CGI_VLAN_ID)
	{
		u8_t buf[4]={0},i;
		for(i=0;i<2;i++)
		{ hex_to_asc(ModuleInfo.VLANINFO.value_id[i], &buf[i*2]); }
		vp->value.string=buf;
		vp->type=string;
	}
#endif
	return OK;
}
/******************************************************************************
*
*  Function:    cgi_setip
*
*  Description: Handle IP request from client
*               
*  Parameters:  cnt: number of the cgi data, vp: CGI item and value.
*               
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*               
*******************************************************************************/
void NetworkConfig();
u8_t cgi_setip(u8_t cnt, req_data *vp)
{
	u8_t i;
	u8_t chr;
	req_data* vp2=vp;
	if (EE_Result == 1)
	{
#ifdef MODULE_DHCPC
		for (i=0; i < cnt; i++, vp2++)
    	{        		
    		if(!strcmp(vp2->item, "ip_from"))
    		{
				if(!strcmp(vp2->value, "ip_static"))
				{
					if(EEConfigInfo.DHCPEnable!=sDHCP_DISABLE)
					{
						EEConfigInfo.DHCPEnable=sDHCP_DISABLE;
						//NetworkConfig();//restore IP from eeprom_info
					}
				}	
				if(!strcmp(vp2->value, "ip_dhcp"))
				{
					if(EEConfigInfo.DHCPEnable==sDHCP_DISABLE)
					{
						EEConfigInfo.DHCPEnable=sDHCPC_NEEDDHCP;
						//DHCPCInfo.Status=sDHCPC_NEEDDHCP;
					}				
				}
			}	
#ifdef MODULE_VLAN_TAG
			if(!strcmp(vp2->item, "vlan"))
			{
				if(!strcmp(vp2->value, "vlan_dis"))
				{ ModuleInfo.VLANINFO.vlan=0; }
				else
				{ ModuleInfo.VLANINFO.vlan=1; }
			}
			if(!strcmp(vp2->item, "vid"))
			{
				u16_t value=0;
				value=asc_to_hex(vp2->value);
				ModuleInfo.VLANINFO.value_id[0]=value>>8;
				ModuleInfo.VLANINFO.value_id[1]=value&0xff;
			}
#endif	
	  }
	  EEPROM_Write_Byte(loc_EEDHCPAddress,EEConfigInfo.DHCPEnable);
#ifdef MODULE_VLAN_TAG
		SaveModuleInfo2EEPROM(&ModuleInfo.VLANINFO, sizeof(vlan_info));
#endif
#else
   {
     u8_t x=cnt;
   }
#endif //MODULE_DHCPC
		for(i=0;i<16;i++)
		{
			if((atoi(vp[i].value)>255) || (atoi(vp[i].value)<0))
			{
				{httpd_init_file(fs_open("setIP.htm",&hs->op_index));return OK;	}
			}
		}
		EEPROM_Write_Byte(loc_EEchip_id, 0x00);

		//write back IP,SM,GW,DNS
		for(i=0;i<16;i++)
		{EEPROM_Write_Byte(loc_EEIPAddress+i,(u8_t)atoi(vp[i].value) );}
		//comfirm write successfully
		for(i=0;i<16;i++)
		{
		    EEPROM_Read_Byte(loc_EEIPAddress+i,&chr) ;
		    if(i<4)EEConfigInfo.IPAddress[i]=chr;
		    if(chr!=(char)atoi(vp[i].value))
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
#endif
#endif

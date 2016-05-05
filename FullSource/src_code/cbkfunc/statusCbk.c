#include "type_def.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "type_def.h"
#include "..\sys\eeprom.h"
#include "..\main\eepdef.h"
#include "fs.h"
#include "httpd.h"
#include "uip.h"
#include "status_htm.h"
#include "net_uart.h"
#include "option.h"

#ifdef MODULE_CLI_CUSTOMIAZE
#else

void hex_to_asc(u8_t hex, s8_t *buf);  //send 2 bytes buffer to save u8_t hex
extern u8_t errmsgflag ;
void eepromerr();
extern struct httpd_info *hs;
/******************************************************************************
*
*  Function:    ssi_status
*
*  Description: Handle Status Html tag information
*
*  Parameters:  varid: SSI Tag ID, vp: SSI value and type.
*
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*
*******************************************************************************/
#ifdef MODULE_FLASH512
u8_t ssi_status(u8_t varid, data_value *vp)
{
	u8_t i;
	s8_t MACAddr[18];
	switch(varid)
	{
		case CGI_SET_MAC:
			for(i=0; i<6; i++)
			  {
				 hex_to_asc(EEConfigInfo.MACID[i], &MACAddr[i*3]);
				 MACAddr[i*3+2]=':';
			  }
			MACAddr[i*3-1]=0x0;
			vp->value.string=MACAddr;
			break;
		case CGI_NICKNAME:
		  if (EE_Result == 1) // EEPROM is ok
		  {
			  for (i=0; i < size_EEComment; i++)
			  {
				EEPROM_Read_Byte(loc_EEComment+i, &MACAddr[i]);
			  }
			  MACAddr[size_EEComment] = 0;
			  vp->value.string=MACAddr;
		   }
		  else
		   {vp->value.string="EEPROM error";}
			break;
		default:
			vp->value.string="";
			break;
		case CGI_K_VER:
			vp->value.string=WEB_CODE_VERSION;
			break;
	}
	vp->type=string;
	return OK;
}
/******************************************************************************
*
*  Function:    cgi_setDef
*
*  Description: Handle Status setting command from client
*
*  Parameters:  cnt: number of the cgi data, vp: CGI name and value.
*
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*
*******************************************************************************/
u8_t cgi_status(u8_t cnt, req_data *vp)
{
	u8_t i, len;
    if(cnt){}
	if (EE_Result == 1) // EEPROM is ok
	{
		if(strncmp(vp[0].value,"Update",6))
		{
			len = strlen(vp[0].value);
			for (i=0; i < len; i++)
				EEPROM_Write_Byte(loc_EEComment+i,vp[0].value[i]);
			for (; i < size_EEComment; i++)
				EEPROM_Write_Byte(loc_EEComment+i, 0);
		
			httpd_init_file(fs_open("Status.htm",&hs->op_index));
		}
	
	}
	else
	{
		eepromerr();
	}
	return OK;
}
#else
#ifdef MODULE_DHCPC
#include "..\net_uart\dhcpc.h"
#endif //MODULE_DHCPC
extern u8_t web_def;
u8_t ip_chged=0;
u8_t ssi_status(u8_t varid, data_value *vp)
{
	u8_t i;
	s8_t MACAddr[18];
	u8_t eep_val;
	s8_t username[size_EELoginName];
	s8_t password[size_EELoginPassword];

  	ip_chged=1;
	// force username and password must have a 0-ending
	username[size_EELoginName-1] = 0;
	password[size_EELoginPassword-1] =0;

	switch(varid)
	{
		case CGI_SET_MAC:
			for(i=0; i<6; i++)
			  {
				 hex_to_asc(EEConfigInfo.MACID[i], &MACAddr[i*3]);
				 MACAddr[i*3+2]=':';
			  }
			MACAddr[i*3-1]=0x0;
			vp->value.string=MACAddr;
			break;
		case CGI_NICKNAME:
		  if (EE_Result == 1) // EEPROM is ok
		  {
			  for (i=0; i < size_EEComment; i++)
			  {
				EEPROM_Read_Byte(loc_EEComment+i, &MACAddr[i]);
			  }
			  MACAddr[size_EEComment] = 0;
			  vp->value.string=MACAddr;
		   }
		  else
		   {vp->value.string="EEPROM error";}
			break;
		case CGI_K_VER:
			vp->value.string=WEB_CODE_VERSION;
			break;
		case CGI_SET_IP1:
		case CGI_SET_IP2:
		case CGI_SET_IP3:
		case CGI_SET_IP4:
			if (EE_Result == 1)
			{	EEPROM_Read_Byte(loc_EEIPAddress + (varid-CGI_SET_IP1) , &eep_val);	}
			else
			{	eep_val = EEConfigInfo.IPAddress[(varid-CGI_SET_IP1)];	}
			break;
		case CGI_SET_SM1:
		case CGI_SET_SM2:
		case CGI_SET_SM3:
		case CGI_SET_SM4:
			if (EE_Result == 1)
			{	EEPROM_Read_Byte(loc_EESubnetMask + (varid-CGI_SET_SM1), &eep_val);	}
			else
			{	eep_val = EEConfigInfo.SubnetMask[(varid-CGI_SET_SM1)];	}
			break;
		case CGI_SET_GW1:
		case CGI_SET_GW2:
		case CGI_SET_GW3:
		case CGI_SET_GW4:
			if (EE_Result == 1)
			{	EEPROM_Read_Byte(loc_EEGateway + (varid-CGI_SET_GW1), &eep_val);}
			else
			{	eep_val = EEConfigInfo.Gateway[(varid-CGI_SET_GW1)];	}
			break;
#ifdef MODULE_DHCPC
		case CGI_IP_STATIC:
			if(EEConfigInfo.DHCPEnable==sDHCP_DISABLE)vp->value.string="checked";
			else vp->value.string="";
			break;
		case CGI_IP_DHCP:
			if(EEConfigInfo.DHCPEnable==sDHCP_DISABLE)vp->value.string="";
			else vp->value.string="checked";
			break;
#endif //MODULE_DHCPC
		case CGI_SET_UN:
			if (EE_Result == 1)
			{
				for(i=0;i<size_EELoginName-1;i++)
				{	EEPROM_Read_Byte(loc_EELoginName+i,username+i);	}
				vp->value.string=username;
			}
			else
			{	vp->value.string=EEConfigInfo.LoginName;	}
			break;
		case CGI_SET_PW:
			if (EE_Result == 1)
			{
				for(i=0;i<size_EELoginPassword-1;i++)
				{	EEPROM_Read_Byte(loc_EELoginPassword+i,password+i);	}
				vp->value.string=password;
			}
			else
			{	vp->value.string=EEConfigInfo.LoginPassword;}
			break;
	}
	if((varid>=CGI_SET_IP1)&&(varid<=CGI_SET_GW4))
	{
		vp->value.digits_3_int=eep_val;
		vp->type=digits_3_int;
	}
	else
	{	vp->type=string;	}
	return OK;
}
u8_t cgi_status(u8_t cnt, req_data *vp)
{
	u8_t i, len;
	s8_t chr;

    if(cnt){}
	if (EE_Result == 1) // EEPROM is ok
	{
		if(strcmp(vp[15].value,vp[16].value))
		{// not equal
			httpd_init_file(fs_open("Status.htm",&hs->op_index));return OK;
		}
		if(!strncmp(vp[17].value, "Update", 6))
		{
			if(!strncmp(vp[0].item, "comment", 7))
			{
				len = strlen(vp[0].value);
				for (i=0; i < len; i++)
					EEPROM_Write_Byte(loc_EEComment+i,vp[0].value[i]);
				for (; i < size_EEComment; i++)
					EEPROM_Write_Byte(loc_EEComment+i, 0);
			}
#ifdef MODULE_DHCPC
    		if(!strcmp(vp[13].item, "ip_from"))
    		{
				if(!strcmp(vp[13].value, "ip_static"))
				{
					if(EEConfigInfo.DHCPEnable!=sDHCP_DISABLE)
					{
						EEConfigInfo.DHCPEnable=sDHCP_DISABLE;
						//NetworkConfig();//restore IP from eeprom_info
					}
				}	
				if(!strcmp(vp[13].value, "ip_dhcp"))
				{
					if(EEConfigInfo.DHCPEnable==sDHCP_DISABLE)
					{
						EEConfigInfo.DHCPEnable=sDHCPC_NEEDDHCP;
						//DHCPCInfo.Status=sDHCPC_NEEDDHCP;
					}				
				}
				EEPROM_Write_Byte(loc_EEDHCPAddress,EEConfigInfo.DHCPEnable);
			}
#endif
			for(i=0;i<12;i++)
			{
				if((atoi(vp[i+1].value)>255) || (atoi(vp[i+1].value)<0))
				{
					httpd_init_file(fs_open("Status.htm",&hs->op_index));
					return OK;	
				}
			}
			EEPROM_Write_Byte(loc_EEchip_id, 0x00);
			//write back IP,SM,GW,DNS
			for(i=0;i<12;i++)
			{EEPROM_Write_Byte(loc_EEIPAddress+i,(u8_t)atoi(vp[i+1].value) );}
			//comfirm write successfully
			for(i=0;i<12;i++)
			{
				EEPROM_Read_Byte(loc_EEIPAddress+i,&chr) ;
			   	if(i<4)EEConfigInfo.IPAddress[i]=chr;
			   	if(chr!=(char)atoi(vp[i+1].value))
			   	{
				    hs->errormsg = "Check EEPROM!";
				    errmsgflag = ERRORMSG_SETNO;
				    httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
					return OK;
				}
			}

			//write back password
			len= strlen(vp[15].value);
			for (i=0; i < len; i++)
				EEPROM_Write_Byte(loc_EELoginPassword+i,vp[15].value[i]);
			for(; i < size_EELoginPassword; i++)
				EEPROM_Write_Byte(loc_EELoginPassword+i, 0);

			//write back username
			len= strlen(vp[14].value);
			for (i=0; i < len; i++)
				EEPROM_Write_Byte(loc_EELoginName+i,vp[14].value[i]);
			for (; i< size_EELoginName; i++)
				EEPROM_Write_Byte(loc_EELoginName+i,0x0);

			// read back username and check if eeprom is same as user set
			// len is username string length
			for(i=0;i< len;i++)
			{
			   EEPROM_Read_Byte(loc_EELoginName+i,&chr) ;
			   if(chr!=vp[14].value[i])
			   {
					hs->errormsg = "FAILL";
					errmsgflag = ERRORMSG_SETNO;
					httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
					return OK;
			   }
			}

			// read back password and check if eeprom is same as user set
			len = strlen(vp[15].value);
			for(i=0;i<len;i++)
			{
			   EEPROM_Read_Byte(loc_EELoginPassword+i,&chr) ;
			   if(chr!=vp[15].value[i])
			   {
					hs->errormsg = "FAILL";
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
		else if(!strncmp(vp[17].value, "Load", 4))
		{
			web_def=1;
			EEPROMLoadDefault();
		    hs->errormsg = "RESET";
		    errmsgflag = ERRORMSG_SETOK;
		    httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
		}
	}
	else
	{	eepromerr();	}
	return OK;
}
#endif
#endif


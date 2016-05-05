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
extern struct httpd_info *hs;
extern char selected[];
extern u8_t errmsgflag;
u8_t ssi_dido(u8_t varid, data_value *vp)
{
#ifdef MODULE_TCP_DIDO
	u8_t name_buf[5]=0;
	u8_t i=0;
#endif
    u8_t dido_value=0, direction=0;
    vp->value.string="";
    vp->type = string;
#ifdef MODULE_TCP_DIDO
	if(hs->row_num>MAX_DODI_PIN){return ERROR;}

	for(i=0;i<MAX_DODI_PIN;i++)
	{ direction|=(ModuleInfo.dido_info.direction[i]&0x01)<<i; }
	dido_value=dido_read(direction);
#endif
	switch(varid)
	{
#ifdef MODULE_TCP_DIDO
		case CGI_PIN:
			vp->value.digits_3_int=hs->row_num;
			vp->type=digits_3_int;
			break;
		case CGI_DIDO_NAME:
			memset(name_buf, 0, 5);
			for(i=0;i<5;i++)
			{ name_buf[i]=ModuleInfo.UDPTelInfo.dido_name[5*(hs->row_num-1)+i]; }
			vp->value.string=name_buf;
			vp->type=string;
			break;
		case CGI_DIDO_PORT:
			vp->value.digits_3_int=ModuleInfo.UDPTelInfo.dido_port;
			vp->type=digits_3_int;
			break;
#endif
		case CGI_D0V:
#ifdef MODULE_TCP_DIDO
		     vp->value.digits_3_int=(dido_value>>hs->row_num-1)&0x1;
#else
		     vp->value.digits_3_int=dido_value&0x1;
#endif
  		 	 vp->type=digits_3_int;
		     break;
#ifndef MODULE_FLASH512
		case CGI_D1V:
			 vp->value.digits_3_int=(dido_value>>1)&0x1;
		 	 vp->type=digits_3_int;
		     break;
#endif
		case CGI_D0D1:
#ifdef MODULE_TCP_DIDO
		     if(ModuleInfo.dido_info.direction[hs->row_num-1]==1)
			 { vp->value.string=selected; }
#else
		     if(ModuleInfo.dido_info.direction[0]==1)
			 { vp->value.string=selected; }
#endif
		     break;
		case CGI_D0D2:
#ifdef MODULE_TCP_DIDO
		     if(ModuleInfo.dido_info.direction[hs->row_num-1]==0)
			 { vp->value.string=selected; }
#else
		     if(ModuleInfo.dido_info.direction[0]==0)
			 { vp->value.string=selected; }
#endif
		     break;
#ifndef MODULE_FLASH512
		case CGI_D1D1:
		     if(ModuleInfo.dido_info.direction[1]==1)
			 {
		     	vp->value.string=selected;
			 }
		     break;
		case CGI_D1D2:
		     if(ModuleInfo.dido_info.direction[1]==0)
			 {
		     	vp->value.string=selected;
			 }
		     break;
#endif
	}
	return OK;
}
u8_t cgi_dido(u8_t cnt, void *vptr)
{
	req_data *vp = vptr;
	int i;
	u16_t port_num=0;
	u8_t direction=0, port_flag=0;
#ifdef MODULE_TCP_DIDO
	u8_t count=0, len=0, value=0;
#endif
	u8_t dido_value=0;
#ifdef MODULE_TCP_DIDO
	for(i=0;i<cnt;i++)
	{
		if(!strncmp(vp[i].item, "Reflash", 7))
		{
			httpd_init_file(fs_open("dido.htm",&hs->op_index));
			return OK;
		}
	}
#endif
	for (i=0; i < cnt; i++, vp++)
	{
#ifdef MODULE_TCP_DIDO
		if(!strncmp(vp->item, "name", 4))
		{
			len=strlen(vp->value);
			memcpy(&ModuleInfo.UDPTelInfo.dido_name[5*count], vp->value, len);
			memset(&ModuleInfo.UDPTelInfo.dido_name[5*count+len], 0, 5-len);
		}
		if(!strncmp(vp->item, "inout", 5))
		{
		    if(!strcmp(vp->value, "in"))
			{
            	ModuleInfo.dido_info.direction[count]=1;
			}
			else
			{
				ModuleInfo.dido_info.direction[count]=0;
			}
			count+=1;
		}
		if(!strncmp(vp->item, "port", 4))
		{ port_num=atoi(vp->value);	}
		if(!strncmp(vp->item, "Submit", 6))
		{ port_flag=1; }
#endif
		if(!strcmp(vp->item, "d0v"))
		{
#ifdef MODULE_TCP_DIDO
			value=atoi(vp->value)&0x01;
			dido_value|=value<<count;
#else
		   dido_value|=atoi(vp->value)&0x1;
#endif
		}
#ifndef MODULE_TCP_DIDO
		if(!strcmp(vp->item, "d1v"))
		{
			dido_value|=(atoi(vp->value)<<1)&0x2;
		}
		if(!strcmp(vp->item, "d0d"))
		{
		    if(!strcmp(vp->value, "in"))
			{
            	ModuleInfo.dido_info.direction[0]=1;
			}
			else
			{
				ModuleInfo.dido_info.direction[0]=0;
			}
		}
		if(!strcmp(vp->item, "d1d"))
		{
		    if(!strcmp(vp->value, "in"))
			{
            	ModuleInfo.dido_info.direction[1]=1;
			}
			else
			{
				ModuleInfo.dido_info.direction[1]=0;
			}
		}
#endif
	}
#ifdef MODULE_TCP_DIDO
	if(port_flag)
	{
		ModuleInfo.UDPTelInfo.dido_port=port_num;
		SaveModuleInfo2EEPROM(&ModuleInfo.UDPTelInfo,sizeof(ModuleInfo.UDPTelInfo));
		hs->errormsg = "RESET to effect";
		errmsgflag = ERRORMSG_SETOK;
		httpd_init_file(fs_open("errormsg.htm",&hs->op_index));  
	}
	else
#endif
	{

		ModuleInfo.UDPTelInfo.dido_value=dido_value;
		for(i=0;i<8;i++)
		{ direction|=(ModuleInfo.dido_info.direction[i]&0x01)<<i; }
		dido_write(dido_value, direction);
		SaveModuleInfo2EEPROM(&ModuleInfo.dido_info, sizeof(ModuleInfo.dido_info));
		SaveModuleInfo2EEPROM(&ModuleInfo.UDPTelInfo, sizeof(udp_tel_info_t));
    	httpd_init_file(fs_open("dido.htm",&hs->op_index));
	}
	return OK;
	
}
#endif //MODULE_DIDO
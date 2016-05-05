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
#include "setPASS_htm.h"
#include "net_uart.h"
#ifdef MODULE_FLASH512
extern u8_t errmsgflag;
#ifdef MODULE_CLI_CUSTOMIAZE
#else

void eepromerr();
extern struct httpd_info *hs;

/******************************************************************************
*
*  Function:    ssi_setpass
*
*  Description: Handle Password Html tag information
*
*  Parameters:  varid: SSI Tag ID, vp: SSI value and type.
*
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*
*******************************************************************************/
u8_t ssi_setpass(u8_t varid, data_value *vp)
{
	u8_t i;
	s8_t username[size_EELoginName];
	s8_t password[size_EELoginPassword];

	// force username and password must have a 0-ending
	username[size_EELoginName-1] = 0;
	password[size_EELoginPassword-1] =0;
		switch(varid)
		{
			case CGI_SET_UN:
				if (EE_Result == 1)
				{
					for(i=0;i<size_EELoginName-1;i++)
						{EEPROM_Read_Byte(loc_EELoginName+i,username+i);}
					vp->value.string=username;
				}
				else
				{
					vp->value.string=EEConfigInfo.LoginName;
				}
				break;
			case CGI_SET_PW:
				if (EE_Result == 1)
				{
					for(i=0;i<size_EELoginPassword-1;i++)
						{EEPROM_Read_Byte(loc_EELoginPassword+i,password+i);}
					vp->value.string=password;
				}
				else
				{
					vp->value.string=EEConfigInfo.LoginPassword;
				}
				break;

			default:
				vp->value.string="";
				break;
		}
		vp->type=string;
		return OK;
}
/******************************************************************************
*
*  Function:    cgi_setpass
*
*  Description: Handle Password request from client
*
*  Parameters:  cnt: number of the cgi data,
*				vp:  CGI name and value.
*					 vp[0] - username
*					 vp[1] - password one
*					 vp[2] - password two
*
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*
*******************************************************************************/
u8_t cgi_setpass(u8_t cnt, req_data *vp)
{
	u8_t i;
	u8_t len;
	s8_t chr;
	if(cnt){}
	if (EE_Result == 1)
	{
			//compare if password1==password2
			if(strcmp(vp[1].value,vp[2].value))
			{// not equal
				httpd_init_file(fs_open("setPASS.htm",&hs->op_index));return OK;
			}

			EEPROM_Write_Byte(loc_EEchip_id, 0x00);

			//write back password
			len= strlen(vp[1].value);
			for (i=0; i < len; i++)
				EEPROM_Write_Byte(loc_EELoginPassword+i,vp[1].value[i]);
			for(; i < size_EELoginPassword; i++)
				EEPROM_Write_Byte(loc_EELoginPassword+i, 0);

			//write back username
			len= strlen(vp[0].value);
			for (i=0; i < len; i++)
				EEPROM_Write_Byte(loc_EELoginName+i,vp[0].value[i]);
			for (; i< size_EELoginName; i++)
				EEPROM_Write_Byte(loc_EELoginName+i,0x0);

			// read back username and check if eeprom is same as user set
			// len is username string length
			for(i=0;i< len;i++)
			{
			   EEPROM_Read_Byte(loc_EELoginName+i,&chr) ;
			   if(chr!=vp[0].value[i])
			   {
		       hs->errormsg = "FAILL";
		       errmsgflag = ERRORMSG_SETNO;
		       httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
				 return OK;
			   }
			}

			// read back password and check if eeprom is same as user set
			len = strlen(vp[1].value);
			for(i=0;i<len;i++)
			{
			   EEPROM_Read_Byte(loc_EELoginPassword+i,&chr) ;
			   if(chr!=vp[1].value[i])
			   {
		       hs->errormsg = "FAILL";
		       errmsgflag = ERRORMSG_SETNO;
		       httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
				 return OK;
			   }
			}

			//write check byte
			EEPROM_Write_Byte(loc_EEchip_id, EE_Default.chip_id[0]);

		    hs->errormsg = "RESET";
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

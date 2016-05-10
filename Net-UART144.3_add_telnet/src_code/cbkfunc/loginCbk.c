/******************************************************************************
*
*   Name:           loginCbk.c
*
*   Description:    Handle login call back function
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
//*******************Include files area**************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IP210reg.h"
#include "type_def.h"
#include "eeprom.h"
#include "eepdef.h"
#include "fs.h"
#include "uip.h"
#include "httpd.h"
#include "login2_htm.h"
#include "errormsg_htm.h"
#include "module.h"
#include "cgi.h"
#include "httpd_app.h"
#ifndef	EE_OTPCODE
#include "switch.h"
#endif
//#define Login_DEBUG			1
//*******************Function Prototype************************
#ifdef MODULE_CLI_CUSTOMIAZE
#else
void httpd_login_pass();
void httpd_login_fail();
void httpd_ChallengeError();
//*****************global data******************
s8_t ipbuf[16];
//*******************extern value**************************
extern COOKIE_Type challenge_data;
/******************************************************************************
*
*  Function:    ssi_login
*
*  Description: Handle login Html tag information
*
*  Parameters:  varid: SSI Tag ID, vp: SSI value and type.
*
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*
*******************************************************************************/
extern u8_t force_reset_countdown;
u8_t ssi_login(u8_t varid, data_value *vp)
{
    u8_t *ipa;
    switch (varid)
    {
        case CGI_DEVICE_IP:
            ipa =(u8_t*)uip_hostaddr;
            if(force_reset_countdown)
            { ipa=EEConfigInfo.IPAddress; }
            sprintf(ipbuf, "%bu.%bu.%bu.%bu", ipa[0], ipa[1], ipa[2], ipa[3]);
            vp->value.string = ipbuf;
            vp->type = string;
            break;
#ifdef DIS_CHALLENGE
#else        	
            case CGI_CHALLENGE_VALUE:
            vp->value.string = httpd_set_challenge();
            vp->type = string;
	        break;
#endif            

    }
    return OK;
}
/******************************************************************************
*
*  Function:    cgi_login
*
*  Description: Handle login request from client
*
*  Parameters:  cnt: number of the cgi data, vp: CGI name and value.
*
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*
*******************************************************************************/
u8_t cgi_login(u8_t cnt, void *vptr)
{
    u8_t i;
    u8_t *username=NULL, *passwd=NULL, *resp=NULL;//,*challenge_value=NULL; 
    //02/20/2006 Grace char name[16],pass[16];
	s8_t name[size_EELoginName],pass[size_EELoginPassword];
    req_data *vp = vptr;
    u8_t login_valid=0,challenge_valid = 0;
    //chance 20070823
	//u8_t *indataptr;
  //u8_t *indataendptr;    
    //******************
	memset(name,0x0,size_EELoginName);		//add by DeNa for LOAD from EEPROM
	memset(pass,0x0,size_EELoginPassword);	//add by DeNa for LOAD from EEPROM 20040714

  if(cnt==0)
  {
		hs->state=HTTP_MULTIPACKET;
		if(hs->multipacket_count==0)
	    {
	       return OK;
	    }   
		if(hs->multipacket_count==1)
		{
			if(!strncmp(vp[0].item,"MULTIPACKET",11))
			{
				hs->INpacket.http_cgiInfo=strfind(vp[0].value,"Username");     
	            hs->INpacket.http_cgiInfo-=8;
				cnt=querystr_func();								
			}			
		}
  }   


	if (EE_Result == 1)
	{
	#ifdef Login_DEBUG
		printf("EEUsername(%x):", loc_EELoginName);
	#endif
		for(i=0;i<(size_EELoginName-1);i++)
		{
			EEPROM_Read_Byte(loc_EELoginName+i,&name[i]);
	#ifdef Login_DEBUG
			printf("%x ", name[i]);
	#endif
		}
	#ifdef Login_DEBUG
		printf("EEPassword:");
	#endif
		for(i=0;i<(size_EELoginPassword-1);i++)
		{
			EEPROM_Read_Byte(loc_EELoginPassword+i,pass+i);
	#ifdef Login_DEBUG
			printf("%x ", *(pass+i));
	#endif
		}
	}
	else
	{
		strcpy(name,EEConfigInfo.LoginName);
		strcpy(pass,EEConfigInfo.LoginPassword);
	}

#ifdef	Login_DEBUG
	TXD=1;printf("\n\r n=%s p=%s",name,pass);
#endif

    for (i=0; i < cnt; i++, vp++)
    {
        if (!strcmp(vp->item, "Response"))
		{
            resp = vp->value;
		}
        else if(!strcmp(vp->item, "Username"))
        {
			username = vp->value;
			  //printf("\n\r username_cgi=%s",username);
        }
        else if(!strcmp(vp->item, "Password"))
        {
			passwd = vp->value;
        }
#ifdef DIS_CHALLENGE
		challenge_valid = 1;
#else		
		if(!strcmp(vp->item, "Challenge"))//add by chance
		{
			//challenge_value=vp->value;
            if (httpd_parse_challenge(vp->value) == OK)
            {
	            if (strlen(vp->value) == strlen(challenge_data.value) && !strcmp(vp->value, challenge_data.value) )//modify by DeNa 20040720
	            {
					challenge_valid = 1;
				}
            }
		}

#endif

    }
	if (strlen(passwd) == strlen(pass) && !strcmp(passwd, pass) )//modify by DeNa 20040720
	{
		if (strlen(username) == strlen(name) && !strcmp(username, name) )//modify by Jesse 20070518
		{
			login_valid = 1;
		}
	}    
    if(login_valid&&challenge_valid)
	{
   		 httpd_login_pass();
	}
	else
	{
		if(login_valid==0)
		{
			httpd_login_fail();
		}
#ifdef DIS_CHALLENGE		
		if(challenge_valid==0)
		{
			httpd_ChallengeError();
		}
#endif
	}
    return OK;
}
#endif

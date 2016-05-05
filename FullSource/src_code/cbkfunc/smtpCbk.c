/*update asc_to_hex()*/
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
#include "eepdef.h"
#include "wdg.h"
#ifdef MODULE_SMTP
#include "..\HtmCfiles\smtp_htm.h"
#include "..\smtp\smtp.h"
#endif //MODULE_SMTP

#ifdef MODULE_SMTP
extern struct httpd_info *hs;

char *itoah(u8_t bv);
u16_t asc_to_hex(char asc[]);
void ReplaceDoubleQuotes(char* src,char* des)
{
	u8_t i,j,len=strlen(src);
	for(i=0,j=0;i<len;i++)
	{
		WatchDogTimerReset();
		if(src[i]&0x80)
		{
			des[j++]=src[i];
			des[j++]=src[++i];
		}
		else if(src[i]=='\"')
		{
			des[j]='\0';
			strcat(des,"&quot\;");
			j=strlen(des);
		}
		else
		{
			des[j++]=src[i];
		}
	}
	des[j]='\0';
}
void CGIstrcpy(char* str,char* cgistr)
{
	char buf[3]="00";
	u16_t i,j,len=strlen(cgistr);

	for(i=0,j=0;i<len;i++)
	{
		WatchDogTimerReset();
		if(cgistr[i]&0x80)
		{
			str[j++]=cgistr[i];
			str[j++]=cgistr[++i];
		}
		else if(cgistr[i]=='+')str[j++]=' ';
		else if(cgistr[i]=='%')
		{
			buf[0]=cgistr[i+1];
			buf[1]=cgistr[i+2];
			i+=2;
			str[j++]=asc_to_hex(buf);
		}
		else
		{
			str[j++]=cgistr[i];
		}
	}
	str[j]='\0';
}
#endif //MODULE_SMTP
u8_t ssi_smtp(u8_t varid, data_value *vp)
{
#ifdef MODULE_SMTP
   char buf[100];
   char code title[3][3]={"01","10","11"};
   if(hs->row_num>3)return ERROR;
   vp->type = string;
   vp->value.string =buf;
   switch (varid)
   {   	
   	case CGI_SMTP_ENABLE:
   		  if(SMTPData.pEEPROM_SMTP->SMTPEnable&SMTP_ENABLE_MASK)
   		  {
   		     vp->value.string ="checked";
   		  }
   		  break;
   	case CGI_SMTP_PORT:
   		 vp->value.digits_3_int=SMTPData.pEEPROM_SMTP->SMTPPort;
   		 vp->type=digits_3_int;
 		 break;   	
   	case CGI_SMTP_SERVER:
   		     vp->value.string =SMTPData.pEEPROM_SMTP->SMTPServer;
	  		  break;
	  		  
	   case CGI_LOGIN_ENABLE:
   		  if(SMTPData.pEEPROM_SMTP->AuthEnable&SMTP_ENABLE_MASK)
   		  {
   		     vp->value.string ="checked";
   		  }
	   	  break;
	  		  
	   case CGI_LOGIN_ID:
	   		  vp->value.string =SMTPData.pEEPROM_SMTP->AuthID;
	   	  break;
   	
	   case CGI_LOGIN_PW:
	   		  vp->value.string =SMTPData.pEEPROM_SMTP->AuthPW;
	   	  break;
	   	     	
   	case CGI_SMTP_MAILT:
   			  vp->value.string =SMTPData.pEEPROM_SMTP->MAIL_TO;
   		  break;  	
   	
   	case CGI_SMTP_MAILFROM:
   		     vp->value.string =SMTPData.pEEPROM_SMTP->MAIL_FROM;
   		  break;
   	case CGI_SMTP_TITLE:
   			 vp->value.string =title[hs->row_num-1];
 		 break;

   	case CGI_SMTP_SUB:
			 ReplaceDoubleQuotes(SMTPData.pEEPROM_SMTP->MAIL_SUBJ[hs->row_num-1],buf);
   		  break;
   	
   	case CGI_SMTP_BODY:				
 			 ReplaceDoubleQuotes(SMTPData.pEEPROM_SMTP->MAIL_MSG[hs->row_num-1],buf);
   		  break;
#ifdef MODULE_FLASH512
	case CGI_SMTP_NUM0:
		vp->value.digits_3_int=hs->row_num-1;
		vp->type=digits_3_int;
		break;
	case CGI_SMTP_NUM1:
		vp->value.digits_3_int=hs->row_num-1;
		vp->type=digits_3_int;
		break;
#endif
   }
#else //MODULE_SMTP
int temp=(int)varid;
temp=(int)vp;
#endif //MODULE_SMTP
    return OK;
}

u8_t cgi_smtp(u8_t cnt, void *vptr)
{
#ifdef MODULE_SMTP
	req_data *vp = vptr;
	u8_t i=0;
	u8_t j=0;
	u8_t smtp_cbk_cnt=0;
	u16_t totallen=0;
	const char NameArray[][13]=
	{"login_id"                         //0
	,"login_pw"                         //1
	,"smtp_srv"                         //2
	,"mail_to"                          //3
	,"mail_from"                        //4
	,"gpio_sub"                         //5
	,"gpio_body"};                      //6

	const u8_t* ValuePtrArray[]=
	{ModuleInfo.SMTPInfo.AuthID      //0
	,ModuleInfo.SMTPInfo.AuthPW      //1
	,ModuleInfo.SMTPInfo.SMTPServer  //2
	,ModuleInfo.SMTPInfo.MAIL_TO     //3
	,ModuleInfo.SMTPInfo.MAIL_FROM   //4
	};

	SMTPData.pEEPROM_SMTP->SMTPEnable=0;
	SMTPData.pEEPROM_SMTP->AuthEnable=0;

    for (i=0; i < cnt; i++, vp++)
    {
      if(!strcmp(vp->item,"smtp_enable")){ SMTPData.pEEPROM_SMTP->SMTPEnable=SMTP_ENABLE_MASK; }
      if(!strcmp(vp->item,"login_enable")){ SMTPData.pEEPROM_SMTP->AuthEnable=AUTH_ENABLE_MASK; }
      if(!strcmp(vp->item,"smtp_port")){ SMTPData.pEEPROM_SMTP->SMTPPort=atoi(vp->value); }

      for(j=0;j<7;j++)
      {
         if(!strncmp(vp->item, NameArray[j], 8))
				 {
				 	if(j<5) CGIstrcpy(ValuePtrArray[j],vp->value);
					else if(j==5)
					{
						CGIstrcpy(ModuleInfo.SMTPInfo.MAIL_SUBJ[smtp_cbk_cnt],vp->value);
					}
					else if(j==6)
					{
						CGIstrcpy(ModuleInfo.SMTPInfo.MAIL_MSG[smtp_cbk_cnt++],vp->value);
					}
				 }
      }
    }
    SaveModuleInfo2EEPROM(SMTPData.pEEPROM_SMTP,sizeof(SSMTPInfo));
    httpd_init_file(fs_open("smtp.htm",&hs->op_index));

#else //MODULE_SMTP
int temp=(int)cnt;
temp=(int)vptr;
#endif //MODULE_SMTP
    return OK;
}
#endif

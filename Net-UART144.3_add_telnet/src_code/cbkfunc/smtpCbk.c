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

#include "..\HttpServer\application.h"
extern u8_t errmsgflag ;


#ifdef MODULE_SMTP
extern struct httpd_info *hs;
extern  u8_t  m_src_mode_data;

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

u16_t m_rtc_year	;
u8_t  m_rtc_month	;
u8_t  m_rtc_date	;
u8_t  m_rtc_hour	;
u8_t  m_rtc_min		;
u8_t  tmier1[9];
u8_t  tmier2[9];
u8_t  tmier3[9];
u8_t  tmier4[9];


u8_t ssi_smtp(u8_t varid, data_value *vp)
{
#ifdef MODULE_SMTP
//   char buf[100];
//   char code title[3][3]={"01","10","11"};
   
   u8_t select_chk;
   u16_t bit_shift;
//   u16_t test;

   if(hs->row_num>3)return ERROR;
   vp->type = string;
   vp->value.string ="";
   switch (varid)
   {   	
	   //RTC
	   case CGI_SMTP_RTC_Y:	vp->type=digits_3_int; vp->value.digits_3_int=m_rtc_year;	break;
	   case CGI_SMTP_RTC_M:	vp->type=digits_3_int; vp->value.digits_3_int=m_rtc_month;	break;
	   case CGI_SMTP_RTC_D:	vp->type=digits_3_int; vp->value.digits_3_int=m_rtc_date;	break;
	   case CGI_SMTP_RTC_H:	vp->type=digits_3_int; vp->value.digits_3_int=m_rtc_hour;	break;
	   case CGI_SMTP_RTC_MM:vp->type=digits_3_int; vp->value.digits_3_int=m_rtc_min;	break;

	   // Timer 1 	
	   case CGI_SMTP_TM1_ON_H:	vp->type=digits_3_int; vp->value.digits_3_int=tmier1[0];break;
	   case CGI_SMTP_TM1_ON_M:	vp->type=digits_3_int; vp->value.digits_3_int=tmier1[1];break;
	   case CGI_SMTP_TM1_OFF_H:	vp->type=digits_3_int; vp->value.digits_3_int=tmier1[2];break;
	   case CGI_SMTP_TM1_OFF_M:	vp->type=digits_3_int; vp->value.digits_3_int=tmier1[3];break;
	   
	   case CGI_SMTP_TM1_SUN:
	   case CGI_SMTP_TM1_MON:	
	   case CGI_SMTP_TM1_TUE:
	   case CGI_SMTP_TM1_WED:
	   case CGI_SMTP_TM1_THU:
	   case CGI_SMTP_TM1_FRI:
	   case CGI_SMTP_TM1_SAT:
	   case CGI_SMTP_TM1_EVE:	
	   	   bit_shift=varid-CGI_SMTP_TM1_SUN;
		   select_chk= (tmier1[4]& (0x01<<bit_shift));
		   if(select_chk!=0) { vp->value.string ="checked"; }   
		   break;

	   case CGI_SMTP_TM1_EN:  	if(tmier1[5]==1) { vp->value.string ="checked";  }   break;
	   case CGI_SMTP_TM1_ON_EN:	if(tmier1[6]==1) { vp->value.string ="checked";  }   break;
	   case CGI_SMTP_TM1_OFF_EN:if(tmier1[7]==1) { vp->value.string ="checked";  }   break;

	   case CGI_SMTP_TM1_SRC1:	   
	   case CGI_SMTP_TM1_SRC2:
	   case CGI_SMTP_TM1_SRC3:
	   case CGI_SMTP_TM1_SRC4:
	   case CGI_SMTP_TM1_SRC5:
	   case CGI_SMTP_TM1_SRC6:
	   case CGI_SMTP_TM1_SRC7:	
		   if((m_src_mode_data==1)||((m_src_mode_data==0)&&(varid!=CGI_SMTP_TM1_SRC4))) {
			   select_chk= varid-CGI_SMTP_TM1_SRC1;
			   if(select_chk==tmier1[8]) { 
				   vp->value.string ="Selected"; 
			   }   
		   }
		   else
			   vp->value.string ="hidden"; 
		   break;
 
		   

	   // Timer 2 	
	   case CGI_SMTP_TM2_ON_H:	vp->type=digits_3_int; vp->value.digits_3_int=tmier2[0];break;
	   case CGI_SMTP_TM2_ON_M:	vp->type=digits_3_int; vp->value.digits_3_int=tmier2[1];break;
	   case CGI_SMTP_TM2_OFF_H:	vp->type=digits_3_int; vp->value.digits_3_int=tmier2[2];break;
	   case CGI_SMTP_TM2_OFF_M:	vp->type=digits_3_int; vp->value.digits_3_int=tmier2[3];break;
	   
	   case CGI_SMTP_TM2_SUN:
	   case CGI_SMTP_TM2_MON:	
	   case CGI_SMTP_TM2_TUE:
	   case CGI_SMTP_TM2_WED:
	   case CGI_SMTP_TM2_THU:
	   case CGI_SMTP_TM2_FRI:
	   case CGI_SMTP_TM2_SAT:
	   case CGI_SMTP_TM2_EVE:	
	   	   bit_shift=varid-CGI_SMTP_TM2_SUN;
		   select_chk= (tmier2[4]& (0x01<<bit_shift));
		   if(select_chk!=0) { vp->value.string ="checked"; }   
		   break;

	   case CGI_SMTP_TM2_EN:   	if(tmier2[5]==1) { vp->value.string ="checked";  }	 break;
	   case CGI_SMTP_TM2_ON_EN:	if(tmier2[6]==1) { vp->value.string ="checked";  }   break;
	   case CGI_SMTP_TM2_OFF_EN:if(tmier2[7]==1) { vp->value.string ="checked";  }   break;
	   
	   case CGI_SMTP_TM2_SRC1:	   
	   case CGI_SMTP_TM2_SRC2:
	   case CGI_SMTP_TM2_SRC3:
	   case CGI_SMTP_TM2_SRC4:
	   case CGI_SMTP_TM2_SRC5:
	   case CGI_SMTP_TM2_SRC6:
	   case CGI_SMTP_TM2_SRC7:	
		   if((m_src_mode_data==1)||((m_src_mode_data==0)&&(varid!=CGI_SMTP_TM2_SRC4))) {
			   select_chk= varid-CGI_SMTP_TM2_SRC1;
			   if(select_chk==tmier2[8]) { 
				   vp->value.string ="Selected"; 
			   }   
		   }
		   else
			   vp->value.string ="hidden"; 
		   break;

	   
	   // Timer 3 	
	   case CGI_SMTP_TM3_ON_H:	vp->type=digits_3_int; vp->value.digits_3_int=tmier3[0];break;
	   case CGI_SMTP_TM3_ON_M:	vp->type=digits_3_int; vp->value.digits_3_int=tmier3[1];break;
	   case CGI_SMTP_TM3_OFF_H:	vp->type=digits_3_int; vp->value.digits_3_int=tmier3[2];break;
	   case CGI_SMTP_TM3_OFF_M:	vp->type=digits_3_int; vp->value.digits_3_int=tmier3[3];break;
	   
	   case CGI_SMTP_TM3_SUN:
	   case CGI_SMTP_TM3_MON:	
	   case CGI_SMTP_TM3_TUE:
	   case CGI_SMTP_TM3_WED:
	   case CGI_SMTP_TM3_THU:
	   case CGI_SMTP_TM3_FRI:
	   case CGI_SMTP_TM3_SAT:
	   case CGI_SMTP_TM3_EVE:	
	   	   bit_shift=varid-CGI_SMTP_TM3_SUN;
		   select_chk= (tmier3[4]& (0x01<<bit_shift));
		   if(select_chk!=0) { vp->value.string ="checked"; }   
		   break;

	   case CGI_SMTP_TM3_EN:   	if(tmier3[5]==1) { vp->value.string ="checked";  }	 break;
	   case CGI_SMTP_TM3_ON_EN:	if(tmier3[6]==1) { vp->value.string ="checked";  }   break;
	   case CGI_SMTP_TM3_OFF_EN:if(tmier3[7]==1) { vp->value.string ="checked";  }   break;
	   
	   case CGI_SMTP_TM3_SRC1:	   
	   case CGI_SMTP_TM3_SRC2:
	   case CGI_SMTP_TM3_SRC3:
	   case CGI_SMTP_TM3_SRC4:
	   case CGI_SMTP_TM3_SRC5:
	   case CGI_SMTP_TM3_SRC6:
	   case CGI_SMTP_TM3_SRC7:	
		   if((m_src_mode_data==1)||((m_src_mode_data==0)&&(varid!=CGI_SMTP_TM3_SRC4))) {
			   select_chk= varid-CGI_SMTP_TM3_SRC1;
			   if(select_chk==tmier3[8]) { 
				   vp->value.string ="Selected"; 
			   }   
		   }
		   else
			   vp->value.string ="hidden"; 
		   break;

	  
	  // Timer 4   
	  case CGI_SMTP_TM4_ON_H:  vp->type=digits_3_int; vp->value.digits_3_int=tmier4[0];break;
	  case CGI_SMTP_TM4_ON_M:  vp->type=digits_3_int; vp->value.digits_3_int=tmier4[1];break;
	  case CGI_SMTP_TM4_OFF_H: vp->type=digits_3_int; vp->value.digits_3_int=tmier4[2];break;
	  case CGI_SMTP_TM4_OFF_M: vp->type=digits_3_int; vp->value.digits_3_int=tmier4[3];break;
	  
	  case CGI_SMTP_TM4_SUN:
	  case CGI_SMTP_TM4_MON:   
	  case CGI_SMTP_TM4_TUE:
	  case CGI_SMTP_TM4_WED:
	  case CGI_SMTP_TM4_THU:
	  case CGI_SMTP_TM4_FRI:
	  case CGI_SMTP_TM4_SAT:
	  case CGI_SMTP_TM4_EVE:   
		  bit_shift=varid-CGI_SMTP_TM4_SUN;
		  select_chk= (tmier4[4]& (0x01<<bit_shift));
		  if(select_chk!=0) { vp->value.string ="checked"; }   
		  break;
	  
	  case CGI_SMTP_TM4_EN:    if(tmier4[5]==1) { vp->value.string ="checked";	}	break;
	  case CGI_SMTP_TM4_ON_EN: if(tmier4[6]==1) { vp->value.string ="checked";	}	break;
	  case CGI_SMTP_TM4_OFF_EN:if(tmier4[7]==1) { vp->value.string ="checked";	}	break;
	  
	  case CGI_SMTP_TM4_SRC1:	  
	  case CGI_SMTP_TM4_SRC2:
	  case CGI_SMTP_TM4_SRC3:
	  case CGI_SMTP_TM4_SRC4:
	  case CGI_SMTP_TM4_SRC5:
	  case CGI_SMTP_TM4_SRC6:
	  case CGI_SMTP_TM4_SRC7:
		  if((m_src_mode_data==1)||((m_src_mode_data==0)&&(varid!=CGI_SMTP_TM4_SRC4))) {
			  select_chk= varid-CGI_SMTP_TM4_SRC1;
			  if(select_chk==tmier4[8]) { 
				  vp->value.string ="Selected"; 
			  }   
		  }
		  else
			  vp->value.string ="hidden"; 
		  break;

	  
	  default:
	  	  break;
   }
#else //MODULE_SMTP
int temp=(int)varid;
temp=(int)vp;
#endif //MODULE_SMTP
    return OK;
}

u8_t cgi_smtp(u8_t cnt, void *vptr)
{
	req_data *vp = vptr;
	u8_t i=0;
	u16_t j=0;
	
	u8_t smtp_cbk_cnt=0;
	u16_t totallen=0;
	eSCALER_ST eCMDStatus=GetTVState();
    if(eCMDStatus==eScaler_PowerOn){
	for(i=0;i<cnt;i++,vp++)
	{				
		if(!strcmp(vp->item, "rtc_y")){
				m_rtc_year= atoi(vp->value);
				eCMDStatus=SendTVSetCommand('s',TV_CMD_RTC_YEAR,(m_rtc_year-2000));
		}	
		
		if(!strcmp(vp->item, "rtc_m")){
				m_rtc_month= atoi(vp->value);
				eCMDStatus=SendTVSetCommand('s',TV_CMD_RTC_MONTH,(m_rtc_month));
		}	
		
		if(!strcmp(vp->item, "rtc_d")){
				m_rtc_date= atoi(vp->value);
				eCMDStatus=SendTVSetCommand('s',TV_CMD_RTC_DAY,(m_rtc_date));
		}	
		
		if(!strcmp(vp->item, "rtc_h")){
				m_rtc_hour= atoi(vp->value);
				eCMDStatus=SendTVSetCommand('s',TV_CMD_RTC_HOUR,(m_rtc_hour));
		}	
		
		if(!strcmp(vp->item, "rtc_mm")){
				m_rtc_min= atoi(vp->value);
				eCMDStatus=SendTVSetCommand('s',TV_CMD_RTC_MINUTE,(m_rtc_min));
				tmier1[4]=0;
		}	

		// Timer1
		if(!strcmp(vp->item, "no1_1st"))     {tmier1[5]= 0; tmier1[6]=0; tmier1[7]=0;	}	
		if(!strcmp(vp->item, "no1_en"))      {tmier1[5]= 1;}	
		if(!strcmp(vp->item, "no1_on"))      {tmier1[6]= 1;}	
		if(!strcmp(vp->item, "no1_off"))     {tmier1[7]= 1;}	
		if(!strcmp(vp->item, "tm1_on_h"))    {tmier1[0]= atoi(vp->value);}	
		if(!strcmp(vp->item, "tm1_on_m"))    {tmier1[1]= atoi(vp->value);}	
		if(!strcmp(vp->item, "tm1_off_h"))   {tmier1[2]= atoi(vp->value);}	
		if(!strcmp(vp->item, "tm1_off_m"))   {tmier1[3]= atoi(vp->value);}	
		if(!strcmp(vp->item, "no1_sun"))     {tmier1[4] |=1;}	
		if(!strcmp(vp->item, "no1_mon"))     {tmier1[4] |=2;}	
		if(!strcmp(vp->item, "no1_tue"))     {tmier1[4] |=4;}	
		if(!strcmp(vp->item, "no1_wed"))     {tmier1[4] |=8;}	
		if(!strcmp(vp->item, "no1_thu"))     {tmier1[4] |=0x10;}	
		if(!strcmp(vp->item, "no1_fri"))     {tmier1[4] |=0x20;}	
		if(!strcmp(vp->item, "no1_sat"))     {tmier1[4] |=0x40;}	
		if(!strcmp(vp->item, "no1_eve"))     {tmier1[4] =0x80;}	
		if(!strcmp(vp->item, "no1_src_sel")) {tmier1[8]= atoi(vp->value);}	
		if(!strcmp(vp->item, "no1_last"))    {eCMDStatus=SendTVSetTimerCommand(1,&tmier1[0]);}	
		
		
		// Timer2
		if(!strcmp(vp->item, "no2_1st"))     {tmier2[5]= 0; tmier2[6]=0; tmier2[7]=0; }	
		if(!strcmp(vp->item, "no2_en"))      {tmier2[5]= 1; }	
		if(!strcmp(vp->item, "no2_on"))      {tmier2[6]= 1; }	
		if(!strcmp(vp->item, "no2_off"))     {tmier2[7]= 1; }	
		if(!strcmp(vp->item, "tm2_on_h"))    {tmier2[0]= atoi(vp->value);}	
		if(!strcmp(vp->item, "tm2_on_m"))    {tmier2[1]= atoi(vp->value);}	
		if(!strcmp(vp->item, "tm2_off_h"))   {tmier2[2]= atoi(vp->value);}	
		if(!strcmp(vp->item, "tm2_off_m"))   {tmier2[3]= atoi(vp->value);}	
		if(!strcmp(vp->item, "no2_sun"))     {tmier2[4] |=1;}	
		if(!strcmp(vp->item, "no2_mon"))     {tmier2[4] |=2;}	
		if(!strcmp(vp->item, "no2_tue"))     {tmier2[4] |=4;}	
		if(!strcmp(vp->item, "no2_wed"))     {tmier2[4] |=8;}	
		if(!strcmp(vp->item, "no2_thu"))     {tmier2[4] |=0x10;}	
		if(!strcmp(vp->item, "no2_fri"))     {tmier2[4] |=0x20;}	
		if(!strcmp(vp->item, "no2_sat"))     {tmier2[4] |=0x40;}	
		if(!strcmp(vp->item, "no2_eve"))     {tmier2[4] =0x80;}	
		if(!strcmp(vp->item, "no2_src_sel")) {tmier2[8]= atoi(vp->value);}	
		if(!strcmp(vp->item, "no2_last"))    {eCMDStatus=SendTVSetTimerCommand(2,&tmier2[0]);}	

		
		// Timer3
		if(!strcmp(vp->item, "no3_1st"))     {tmier3[5]= 0; tmier3[6]=0; tmier3[7]=0;	}	
		if(!strcmp(vp->item, "no3_en"))      {tmier3[5]= 1;}	
		if(!strcmp(vp->item, "no3_on"))      {tmier3[6]= 1;}	
		if(!strcmp(vp->item, "no3_off"))     {tmier3[7]= 1;}	
		if(!strcmp(vp->item, "tm3_on_h"))    {tmier3[0]= atoi(vp->value);}	
		if(!strcmp(vp->item, "tm3_on_m"))    {tmier3[1]= atoi(vp->value);}	
		if(!strcmp(vp->item, "tm3_off_h"))   {tmier3[2]= atoi(vp->value);}	
		if(!strcmp(vp->item, "tm3_off_m"))   {tmier3[3]= atoi(vp->value);}	
		if(!strcmp(vp->item, "no3_sun"))     {tmier3[4] |=1;}	
		if(!strcmp(vp->item, "no3_mon"))     {tmier3[4] |=2;}	
		if(!strcmp(vp->item, "no3_tue"))     {tmier3[4] |=4;}	
		if(!strcmp(vp->item, "no3_wed"))     {tmier3[4] |=8;}	
		if(!strcmp(vp->item, "no3_thu"))     {tmier3[4] |=0x10;}	
		if(!strcmp(vp->item, "no3_fri"))     {tmier3[4] |=0x20;}	
		if(!strcmp(vp->item, "no3_sat"))     {tmier3[4] |=0x40;}	
		if(!strcmp(vp->item, "no3_eve"))     {tmier3[4] =0x80;}	
		if(!strcmp(vp->item, "no3_src_sel")) {tmier3[8]= atoi(vp->value);}	
		if(!strcmp(vp->item, "no3_last"))    {eCMDStatus=SendTVSetTimerCommand(3,&tmier3[0]);}	

		
		// Timer4
		if(!strcmp(vp->item, "no4_1st"))     {tmier4[5]= 0; tmier4[6]=0; tmier4[7]=0;	}	
		if(!strcmp(vp->item, "no4_en"))      {tmier4[5]= 1;}	
		if(!strcmp(vp->item, "no4_on"))      {tmier4[6]= 1;}	
		if(!strcmp(vp->item, "no4_off"))     {tmier4[7]= 1;}	
		if(!strcmp(vp->item, "tm4_on_h"))    {tmier4[0]= atoi(vp->value);}	
		if(!strcmp(vp->item, "tm4_on_m"))    {tmier4[1]= atoi(vp->value);}	
		if(!strcmp(vp->item, "tm4_off_h"))   {tmier4[2]= atoi(vp->value);}	
		if(!strcmp(vp->item, "tm4_off_m"))   {tmier4[3]= atoi(vp->value);}	
		if(!strcmp(vp->item, "no4_sun"))     {tmier4[4] |=1;}	
		if(!strcmp(vp->item, "no4_mon"))     {tmier4[4] |=2;}	
		if(!strcmp(vp->item, "no4_tue"))     {tmier4[4] |=4;}	
		if(!strcmp(vp->item, "no4_wed"))     {tmier4[4] |=8;}	
		if(!strcmp(vp->item, "no4_thu"))     {tmier4[4] |=0x10;}	
		if(!strcmp(vp->item, "no4_fri"))     {tmier4[4] |=0x20;}	
		if(!strcmp(vp->item, "no4_sat"))     {tmier4[4] |=0x40;}	
		if(!strcmp(vp->item, "no4_eve"))     {tmier4[4] =0x80;}	
		if(!strcmp(vp->item, "no4_src_sel")) {tmier4[8]= atoi(vp->value);}	
		if(!strcmp(vp->item, "no4_last"))    {eCMDStatus=SendTVSetTimerCommand(4,&tmier4[0]);}	
 		
	}	
    }
	if(eCMDStatus==eScaler_Standy){		
		hs->errormsg = "Standby!";
		errmsgflag = ERRORMSG_WAIT5;
		httpd_init_file(fs_open("NoService4.htm",&hs->op_index));    
	}	
	else if(eCMDStatus==eScaler_Mode_Change){
		hs->errormsg = "Change the Setting";
		errmsgflag = ERRORMSG_WAIT3;		
		httpd_init_file(fs_open("NoService4.htm",&hs->op_index));
	}	
	else
        httpd_init_file(fs_open("smtp.htm",&hs->op_index));

    return OK;
}
#endif

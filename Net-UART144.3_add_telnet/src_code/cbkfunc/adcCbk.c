#include <module.h>
#ifdef MODULE_ADC
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

#include "..\HtmCfiles\adc_htm.h"
#include "..\smtp\smtp.h"
#include "..\HttpServer\application.h"
extern u8_t errmsgflag ;


extern struct httpd_info *hs;
#define CODING_DBG_MESSAGE_1 0
u8_t current_channel=1, current_vrs=4;//ADC0, vrs3

u8_t m_Mail_To[32];

extern SSMTPData SMTPData;


char *itoah(u8_t bv);
u16_t asc_to_hex(char asc[]);




u8_t m_smtp_day;
u8_t m_smtp_hour;
u8_t m_smtp_min;
u8_t m_smtp_check;



eSCALER_ST smtp_status_day_hour_min(u8_t cStatus, u8_t cDay, u8_t cHour,u8_t cMin);




void smtp_Get_MailTo_CCTo(int MailNo)
{
	u16_t i,j;
	u16_t fount_at_symble=0;
	j=0;
	m_Mail_To[0]=0;
  	for(i=0;i<MAX_MAIL_TO_ADDR_LEN;i++)
  	{
  		switch(SMTPData.pEEPROM_SMTP->MAIL_TO[i])
  		{
  			case ';'	: if(MailNo==fount_at_symble){
						  	m_Mail_To[i-j]=0;
						    //printf("mail list %d =%s\n",MailNo,&m_Mail_To[0]);
  						  }
						  j=i+1;
  						  fount_at_symble++;
						  break;
  			case '\0'	: if(MailNo==fount_at_symble){
						  	m_Mail_To[i-j]=0;
						    //printf("mail list %d =%s\n",MailNo,&m_Mail_To[0]);
  						  }
						  j=i+1;
						  fount_at_symble++;
						  break;
			default		: if(MailNo==fount_at_symble)
						  	m_Mail_To[i-j]=SMTPData.pEEPROM_SMTP->MAIL_TO[i];
						  break;
  		}
		if(fount_at_symble>MailNo)
		   break;	
  	}
}


void AdcCGIstrcpy_add_comma(char* str,char* cgistr)
{
	char buf[3]="00";
	u16_t i,j,len=strlen(cgistr);

	for(i=0,j=0;i<len;i++)
	{
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
	str[j]=';';
	str[j+1]='\0';
}


void AdcCGIstrcpy(char* str,char* cgistr)
{
	char buf[3]="00";
	u16_t i,j,len=strlen(cgistr);

	for(i=0,j=0;i<len;i++)
	{
//		WatchDogTimerReset();
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
	str[j+1]='\0';
}




u16_t adc_convert(void)
{
  u8_t byte_data, adc_h, adc_l;
  u16_t adcValue;
#if 0
  u8_t;
#endif

    //Turn on ADC
    byte_data = IP210RegRead(0x8370);
    byte_data = ((current_vrs-1)<<2) | ((current_channel-1)<<4) | 0x1;
    IP210RegWrite(0x8370, byte_data); 
    //Wait Calibration
    //bit7 calibrated status
    do{
        byte_data = IP210RegRead(0x8370);
    }while(!(byte_data&0x80));

    	//Start ADC 
        byte_data = IP210RegRead(0x8370);
        byte_data |= 0x2;
        IP210RegWrite(0x8370, byte_data); 

   	    
		do{
            byte_data = IP210RegRead(0x8370);
      	}while(byte_data&0x02);

     	adc_l= IP210RegRead(0x8371);
    	adc_h= IP210RegRead(0x8372);
        //printf("\nadc_h=[%x],adc_l=[%x]", (u16_t)adc_h, (u16_t)adc_l);
        adc_h=(adc_h&0x03);
        //printf("\nadc_h=[%x],adc_l=[%x]", (u16_t)adc_h, (u16_t)adc_l);
		if(adc_h&0x02)
		{
            //printf("\nnegative");
#if 0
		    if(!(adc_h&0x01))
			{
			        //printf("\nnegative-h01");
               // adc_h|=0x01;
			    byte_data=adc_l;
				for(i=0;i<8;i++)
				{
				    if(byte_data&0x80)
					{
					    adc_l|=0x80>>i;
					}
					else
					{
					    break;
					}
				    ;byte_data<<=1;
				}
			}
#endif
            adc_h|=0xfc;
			adcValue=(u16_t)adc_h<<8|adc_l;
    		adcValue=~adcValue+1;
			adcValue=512-adcValue;
		}
		else
		{
            //printf("\npositive");
            adcValue=((u16_t)adc_h&0x03)<<8|adc_l;
			adcValue+=512;
		}
        //printf("\nadcValue=[%x]", (u16_t)adcValue);

    //Turn off ADC
    byte_data = IP210RegRead(0x8370);
    byte_data &= ~0x1;
    IP210RegWrite(0x8370, byte_data); 
    
	adcValue>>=3;

	return adcValue;
}
u8_t ssi_adc(u8_t varid, data_value *vp)
{ 
		u16_t select_chk;
#if CODING_DBG_MESSAGE_1
               printf("\nssi_adc[%02x]", (u16_t)varid);
#endif
    switch (varid)
    {
    	
		case CGI_SMTP_PORT:	
			vp->type=digits_3_int;
			vp->value.digits_3_int=SMTPData.pEEPROM_SMTP->SMTPPort;
			break;	   
    	case CGI_SMTP_ENABLE: 
			 if(m_smtp_check==0)
			 	vp->value.string =""; 	
			 else
			 	vp->value.string ="checked"; 
			 vp->type = string;
			 break;    
			 
    	case CGI_SMTP_SBUJECT:
			vp->value.string =SMTPData.pEEPROM_SMTP->MAIL_SUBJ[0];
			vp->type = string;
			break;	  

    	case CGI_SMTP_FROM:
			vp->value.string =SMTPData.pEEPROM_SMTP->MAIL_FROM;
			vp->type = string;
			break;	  
     
    	case CGI_SMTP_LOGIN_ID:
			vp->value.string =SMTPData.pEEPROM_SMTP->AuthID;
			vp->type = string;
			break;	  

    	case CGI_SMTP_PAS:
			vp->value.string =SMTPData.pEEPROM_SMTP->AuthPW;
			vp->type = string;
			break;	  

			
    	case CGI_SMTP_SERVER:
			  vp->value.string =SMTPData.pEEPROM_SMTP->SMTPServer;
			  vp->type = string;
	  		  break;	
		case CGI_SMTP_MAILT1:
			   smtp_Get_MailTo_CCTo(0);
			   vp->type = string;
			   vp->value.string =&m_Mail_To[0]; 
			   break;
		
		case CGI_SMTP_MAILT2:
			   smtp_Get_MailTo_CCTo(1);
			   vp->type = string;
			   vp->value.string =&m_Mail_To[0]; 
			   break;
		case CGI_SMTP_SEND_H:	
			vp->type=digits_3_int;
			vp->value.digits_3_int=m_smtp_hour;
			break;	   

		case CGI_SMTP_SEND_M:	
			vp->type=digits_3_int;
			vp->value.digits_3_int=m_smtp_min;
			break;	   

		case CGI_SMTP_WK_1:
		case CGI_SMTP_WK_2:
		case CGI_SMTP_WK_3:
		case CGI_SMTP_WK_4:
		case CGI_SMTP_WK_5:
		case CGI_SMTP_WK_6:
		case CGI_SMTP_WK_7:
		case CGI_SMTP_WK_8:
 			select_chk= varid-CGI_SMTP_WK_1;
			if(select_chk==m_smtp_day){ 
				vp->value.string ="Selected"; 
			}
			else
				vp->value.string ="";
			vp->type = string;
			break;



        default:
                break;
    }
    return OK;
}
int write_ee=0;
void Send_Status_Report_Message(u8_t cType, u8_t * pData);

u8_t cgi_adc(u8_t cnt, void *vptr)
{
	req_data *vp = vptr;
	int i;	

	u16_t mail_list_len=0;
	eSCALER_ST eCMDStatus=GetTVState();
	
	vp = vptr;
	
    if(eCMDStatus==eScaler_PowerOn){
	 for (i=0; i < cnt; i++, vp++)
    {       
			if(!strcmp(vp->item, "mai_to_list"))
			{
				//printf("mail list 1 str=%s\n",vp->value);
				mail_list_len=strlen(vp->value);
				if(mail_list_len!=0){
				  AdcCGIstrcpy_add_comma(SMTPData.pEEPROM_SMTP->MAIL_TO,vp->value);	
				  mail_list_len=strlen(SMTPData.pEEPROM_SMTP->MAIL_TO);
				}  
				else
				  SMTPData.pEEPROM_SMTP->MAIL_TO[0]=0;	
			}
			
			if(!strcmp(vp->item, "cc_to_list"))
			{
				if(mail_list_len!=0)
					AdcCGIstrcpy(&SMTPData.pEEPROM_SMTP->MAIL_TO[mail_list_len],vp->value);	
				else	
					AdcCGIstrcpy(SMTPData.pEEPROM_SMTP->MAIL_TO,vp->value);	

				//printf("mail list 1 len=%d\n",mail_list_len);
				//printf("mail string=%s\n%s\n",SMTPData.pEEPROM_SMTP->MAIL_TO,vp->value);

				//for(yy=0;yy<35;yy++)
				//	printf("%d %c\n",yy,SMTPData.pEEPROM_SMTP->MAIL_TO[yy]);
				
			}
			
			if(!strcmp(vp->item, "smtp_srv"))
			{
				SMTPData.pEEPROM_SMTP->SMTPEnable=0;
				m_smtp_check=0;
				AdcCGIstrcpy(SMTPData.pEEPROM_SMTP->SMTPServer,vp->value);
				write_ee=1;
			}	
			
			if(!strcmp(vp->item, "smtp_login"))
			{
				AdcCGIstrcpy(SMTPData.pEEPROM_SMTP->AuthID,vp->value);
			}	
			
			if(!strcmp(vp->item, "smtp_pass"))
			{
				AdcCGIstrcpy(SMTPData.pEEPROM_SMTP->AuthPW,vp->value);
			}	
			
			if(!strcmp(vp->item, "smtp_subject"))
			{
				AdcCGIstrcpy(SMTPData.pEEPROM_SMTP->MAIL_SUBJ[0],vp->value);
			}	
			
			if(!strcmp(vp->item, "smtp_from"))
			{
				AdcCGIstrcpy(SMTPData.pEEPROM_SMTP->MAIL_FROM,vp->value);
			}	

			
			if(!strcmp(vp->item, "sta_rpt")){
				SMTPData.pEEPROM_SMTP->SMTPEnable=1;
				m_smtp_check=1;
			}	

			
			if(!strcmp(vp->item, "day_sel")){
				m_smtp_day= atoi(vp->value);
			}	

			
			if(!strcmp(vp->item, "send_hour")){
				m_smtp_hour= atoi(vp->value);
			}	


			if(!strcmp(vp->item, "smtp_port")){
				SMTPData.pEEPROM_SMTP->SMTPPort= atoi(vp->value);
			}	


			if(!strcmp(vp->item, "send_min")){
				m_smtp_min= atoi(vp->value);
				smtp_status_day_hour_min(m_smtp_check,m_smtp_day,m_smtp_hour,m_smtp_min);
			}	

			
			if(!strcmp(vp->item, "test_act")){
				ModuleInfo.SMTPInfo.MAIL_MSG[0][0]='d';
				ModuleInfo.SMTPInfo.MAIL_MSG[0][1]='e';
				ModuleInfo.SMTPInfo.MAIL_MSG[0][2]='g';
				ModuleInfo.SMTPInfo.MAIL_MSG[0][3]='r';
				ModuleInfo.SMTPInfo.MAIL_MSG[0][4]='e';
				ModuleInfo.SMTPInfo.MAIL_MSG[0][5]='e';
				ModuleInfo.SMTPInfo.MAIL_MSG[0][6]=':';
				ModuleInfo.SMTPInfo.MAIL_MSG[0][7]='t';
				ModuleInfo.SMTPInfo.MAIL_MSG[0][8]='e';
				ModuleInfo.SMTPInfo.MAIL_MSG[0][9]='s';
				ModuleInfo.SMTPInfo.MAIL_MSG[0][10]='t';
				ModuleInfo.SMTPInfo.MAIL_MSG[0][11]=0;
				ModuleInfo.SMTPInfo.MAIL_MSG[0][12]=0;
				Send_Status_Report_Message(2,&ModuleInfo.SMTPInfo.MAIL_MSG[0][0]);
				smtp_send_mail_test(0);
				printf("Test Mail\n"); //peter
				
			}	
	}
	printf("Sumbit\n");
	//if(write_ee==1)
	//  SaveModuleInfo2EEPROM(SMTPData.pEEPROM_SMTP,sizeof(SSMTPInfo));
    }
    
	if(eCMDStatus==eScaler_Standy){		
		hs->errormsg = "Standby!";
		errmsgflag = ERRORMSG_WAIT5;
		httpd_init_file(fs_open("NoService5.htm",&hs->op_index));    
	}	
	else if(eCMDStatus==eScaler_Mode_Change){
		hs->errormsg = "Change the Setting";
		errmsgflag = ERRORMSG_WAIT3;		
		httpd_init_file(fs_open("NoService5.htm",&hs->op_index));
	}	
	else
        httpd_init_file(fs_open("adc.htm",&hs->op_index));

    return OK;
}
#endif//MODULE_ADC

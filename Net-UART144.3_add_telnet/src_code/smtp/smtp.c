/******************************************************************************
*
*   Name:           smtp.c
*
*   Description:    SMTP client protocol
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#include "module.h"
#ifdef MODULE_SMTP
#include "smtp.h"
#include "base64.h"
#include <string.h>
#include "uip.h"
#include "../net_uart/dns.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef MODULE_NET_UART
#include "net_uart.h"
#endif
#ifdef SUPPORT_SMTP_AUTH
#include "cram_md5.h"
#endif //SUPPORT_SMTP_AUTH
SSMTPData SMTPData;
void hex_to_asc(u8_t hex, s8_t * buf);
void CatNetworkConfigMsg(char* Msg,char* endl);//Cat NetworkConfigMsg to Msg

u16_t gpio_cnt=0;
u8_t  last_status=0;
/******************************************************************************
*
*  Function:    smtp_chk_MailToTotal
*
*  Description: Check how may destination mail address
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void smtp_chk_MailToTotal()
{
	u16_t i;
	u8_t fount_at_symble=0;
	SMTPData.MailToTotal=0;
	SMTPData.MailToOffsetStart[0]=0;
  for(i=0;i<MAX_MAIL_TO_ADDR_LEN;i++)
  {
  	switch(SMTPData.pEEPROM_SMTP->MAIL_TO[i])
  	{
  	case '@':
 			fount_at_symble=1;
  		break;
  	case ';':
  		if(fount_at_symble==0)break;
  		SMTPData.MailToOffsetEnd[SMTPData.MailToTotal]=i-1;
  		if(SMTPData.MailToTotal<MAX_MAIL_TO)
  		{
  			SMTPData.MailToTotal++;
  			//set next start
  			SMTPData.MailToOffsetStart[SMTPData.MailToTotal]=i+1;
  		}
 			fount_at_symble=0;
			break;
  	case '\0':
  		if(fount_at_symble==0)return;
  		SMTPData.MailToOffsetEnd[SMTPData.MailToTotal]=i;
 			SMTPData.MailToTotal++;
 			//end of mail to string
 			return;
  	}
  }
}

void strcat_mail_to(char* dest, u8_t index)
{
	u8_t i,j=0;
	u8_t* dest2=dest+strlen(dest);
	for(i=SMTPData.MailToOffsetStart[index];i<=SMTPData.MailToOffsetEnd[index];i++)
	{
		dest2[j++]=SMTPData.pEEPROM_SMTP->MAIL_TO[i];
	}
	dest2[j]='\0';
}
/******************************************************************************
*
*  Function:    ModifyMail
*
*  Description: Modify the previous mail message
*               
*  Parameters:  sub:    subtitile
*               msg:    message body
*               MailNo: Mail number in array
*  Returns:     MailNo
*               
*******************************************************************************/
u8_t ModifyMail(char* sub,char* msg,u8_t MailNo)
{
    if(SMTPData.MailTotal>=MAX_SMTP_MAIL)return MAIL_LIST_FULL;//Mail List Full
	if(MailNo==MAIL_CREATE)MailNo=SMTPData.MailTotal++;
    SMTPData.pMAIL_SUBJ[MailNo]=sub;
    SMTPData.pMAIL_MSG[MailNo]=msg;
	return MailNo;
}

/******************************************************************************
*
*  Function:    smtp_init
*
*  Description: Init smtp
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void smtp_init()
{
	SMTPData.pEEPROM_SMTP=&ModuleInfo.SMTPInfo;
    AddMail(ModuleInfo.SMTPInfo.MAIL_SUBJ[0],ModuleInfo.SMTPInfo.MAIL_MSG[0]);
    AddMail(ModuleInfo.SMTPInfo.MAIL_SUBJ[1],ModuleInfo.SMTPInfo.MAIL_MSG[1]);
    AddMail(ModuleInfo.SMTPInfo.MAIL_SUBJ[2],ModuleInfo.SMTPInfo.MAIL_MSG[2]);
	SMTPData.MailNo=0;
	SMTPData.MailToCnt=0;
	SMTPData.Status=SMTP_IDLE;
	SMTPData.timer=100;
}

/******************************************************************************
*
*  Function:    smtp_connect
*
*  Description: connect to mail SMTP serve
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void smtp_connect()
{	
	uip_connect((u16_t*)&SMTPData.ServerIPAddr,SMTPData.pEEPROM_SMTP->SMTPPort);
}

/******************************************************************************
*
*  Function:    smtp_send_mail
*
*  Description: send mail out
*               
*  Parameters:  MailNo: Mail number
*               
*  Returns:     0:fail 1:success
*               
*******************************************************************************/
u8_t smtp_send_mail(u8_t MailNo)
{
	if(SMTPData.pEEPROM_SMTP->SMTPEnable==0)return 0;
    if(SMTPData.Status==SMTP_IDLE)
	{
		SMTPData.Status=SMTP_QUERY_DNS;
	}
	else return 0;//fail

	SMTPData.MailNo=MailNo;
	SMTPData.WaitConnRetry=0;
	SMTPData.WaitConnDelay=0;
	return 1;//success
}


u8_t smtp_send_mail_test(u8_t MailNo)
{
    if(SMTPData.Status==SMTP_IDLE)
	{
		SMTPData.Status=SMTP_QUERY_DNS;
	}
	else return 0;//fail

	SMTPData.MailNo=MailNo;
	SMTPData.WaitConnRetry=0;
	SMTPData.WaitConnDelay=0;
	return 1;//success
}


/******************************************************************************
*
*  Function:    smtp_timer
*
*  Description: callback function for timer
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void smtp_timer()
{
	switch(SMTPData.Status)
	{
		case SMTP_QUERY_DNS:
			 SMTPData.ServerIPAddr=DNS_Result(SMTPData.pEEPROM_SMTP->SMTPServer);
			 if(SMTPData.ServerIPAddr==DNS_NOT_MATCH_NAME)
			 {	
			 		if(SMTPData.WaitConnDelay++>WAIT_CONN_DELAY)
			 		{
							SMTPData.WaitConnDelay=0;
							if(SMTPData.WaitConnRetry++<WAIT_CONN_RETRY)
							{
								DNS_Query(SMTPData.pEEPROM_SMTP->SMTPServer,100);
							}
							else
							{
								SMTPData.Status=SMTP_IDLE;
							}
			 		}
			 }
			 else if(SMTPData.ServerIPAddr!=DNS_WAIT_ACK)
			 {
			 		SMTPData.Status=SMTP_CONNECTING;
			 }
			 break;
		case SMTP_CONNECTING:
			 SMTPData.Status=SMTP_WAITCONNECT;
			 smtp_connect();
			 break;
		case SMTP_WAITCONNECT:
			 if(SMTPData.WaitConnDelay++>WAIT_CONN_DELAY)
			 {
				SMTPData.WaitConnDelay=0;
				if(SMTPData.WaitConnRetry++<WAIT_CONN_RETRY)
				{
					smtp_connect();
				}
				else
				{
					SMTPData.Status=SMTP_IDLE;
				}
			 }
			 break;
	}
}

/******************************************************************************
*
*  Function:    smtp_send_cmd_helo
*
*  Description: send HELO command
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void smtp_send_cmd_helo()
{
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C1');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS1");rx_over=0;}
#endif
#endif
	if(SMTPData.Status==SMTP_HELO)
	{	strcpy(uip_appdata,"HELO ");	}
	else if(SMTPData.Status==SMTP_EHLO)
	{	strcpy(uip_appdata,"EHLO ");	}
	strcat(uip_appdata,EEConfigInfo.Comment);
	strcat(uip_appdata,CRLF);
	uip_len=strlen(uip_appdata);
	uip_slen=uip_len;
}

void smtp_send_starttls()
{
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C2');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS2");rx_over=0;}
#endif
#endif
	strcpy(uip_appdata,"STARTTLS");
	strcat(uip_appdata,CRLF);
	uip_len=strlen(uip_appdata);
	uip_slen=uip_len;
}

/******************************************************************************
*
*  Function:    smtp_send_cmd_mail
*
*  Description: Send command MAIL
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void smtp_send_cmd_mail()
{
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C3');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS3");rx_over=0;}
#endif
#endif
	strcpy(uip_appdata,"MAIL FROM: <");
	strcat(uip_appdata,SMTPData.pEEPROM_SMTP->MAIL_FROM);
	strcat(uip_appdata,">");
	strcat(uip_appdata,CRLF);
	uip_len=strlen(uip_appdata);
	uip_slen=uip_len;
	printf("mail from==%s",uip_appdata);//peter
}

/******************************************************************************
*
*  Function:    smtp_send_cmd_rcpt
*
*  Description: Send command RCPT
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void smtp_send_cmd_rcpt()
{
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C4');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS4");rx_over=0;}
#endif
#endif
	strcpy(uip_appdata,"RCPT TO: <");
	//strcat(uip_appdata,SMTPData.pEEPROM_SMTP->MAIL_TO[SMTPData.MailToCnt]);
	strcat_mail_to(uip_appdata, SMTPData.MailToCnt);
	strcat(uip_appdata,">");
	strcat(uip_appdata,CRLF);
	uip_len=strlen(uip_appdata);
	uip_slen=uip_len;	
	printf("mail to=%s",uip_appdata);//peter
}

/******************************************************************************
*
*  Function:    smtp_send_cmd_data
*
*  Description: Send command DATA
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void smtp_send_cmd_data()
{
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C5');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS5");rx_over=0;}
#endif
#endif
	strcpy(uip_appdata,"DATA");
	strcat(uip_appdata,CRLF);
	uip_len=strlen(uip_appdata);
	uip_slen=uip_len;
}

/******************************************************************************
*
*  Function:    smtp_send_cmd_quit
*
*  Description: Send command QUIT
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void smtp_send_cmd_quit()
{
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C6');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS6");rx_over=0;}
#endif
#endif
	strcpy(uip_appdata,"QUIT");
	strcat(uip_appdata,CRLF);
	uip_len=strlen(uip_appdata);
	uip_slen=uip_len;
}

/******************************************************************************
*
*  Function:    smtp_send_mailbody
*
*  Description: Send mailbody
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/

extern void CGIstrcpy(char* str,char* cgistr);


void smtp_send_mailbody()
{	
	u16_t i=0;
	char buf[10];
	u8_t message_body[80]={0};
	
	strcpy (uip_appdata, "Message-ID: <");
	for(i=0;i<8;i++)
	{
		hex_to_asc(rand()%255,buf);
		strcat (uip_appdata, buf);
	}
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C7');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS7");rx_over=0;}
#endif
#endif
	strcat (uip_appdata, SMTPData.pEEPROM_SMTP->MAIL_FROM);
	strcat (uip_appdata, ">");
	strcat (uip_appdata, CRLF);
	strcat (uip_appdata, "From: ");
	strcat (uip_appdata,SMTPData.pEEPROM_SMTP->MAIL_FROM);
	strcat (uip_appdata, CRLF);

	for(i=0;i<SMTPData.MailToTotal;i++)
	{
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C8');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS8");rx_over=0;}
#endif
#endif
		strcat (uip_appdata, "To: ");
		//strcat (uip_appdata, SMTPData.pEEPROM_SMTP->MAIL_TO[i]);
		strcat_mail_to(uip_appdata,i);
		strcat (uip_appdata, CRLF);
	}
	memset(message_body, 0, 80);
	b64encode(SMTPData.pMAIL_SUBJ[SMTPData.MailNo], message_body);
	strcat (uip_appdata, "SUBJECT: ");
	strcat (uip_appdata, "=?utf-8?B?");
	strcat (uip_appdata, message_body);
	strcat (uip_appdata, "?=");
	strcat (uip_appdata, CRLF);
	strcat (uip_appdata, "Content-Type: text/html; charset=utf-8");
	strcat (uip_appdata, CRLF);
	strcat (uip_appdata, "Content-Transfer-Encoding: base64");
	strcat (uip_appdata, CRLF);
	strcat (uip_appdata, CRLF);
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C8');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS22");rx_over=0;}
#endif
#endif
	memset(message_body, 0, 80);
    printf("Send SMTP Message=%s\n",SMTPData.pMAIL_MSG[SMTPData.MailNo]);//peter 
	b64encode(SMTPData.pMAIL_MSG[SMTPData.MailNo], message_body);  //peter disable the original code for test 
	strcat (uip_appdata, message_body);
	strcat (uip_appdata, CRLF);
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C9');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS9");rx_over=0;}
#endif
#endif
/*
	strcat (uip_appdata,EEConfigInfo.Comment);
    strcat (uip_appdata,"'s Information:");
	CatNetworkConfigMsg(uip_appdata,"\n");
	strcat (uip_appdata, CRLF);
*/
	uip_len=strlen(uip_appdata);
	uip_slen=uip_len;
}

/******************************************************************************
*
*  Function:    smtp_send_mailbody_end
*
*  Description: End of send mail body
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void smtp_send_mailbody_end()
{
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C10');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS10");rx_over=0;}
#endif
#endif
	//End with "CRLE.CRLE"
	strcpy (uip_appdata, CRLF);
	strcat (uip_appdata, ".");
	strcat (uip_appdata, CRLF);
	uip_len=strlen(uip_appdata);
	uip_slen=uip_len;
}
/******************************************************************************
*
*  Function:    smtp_send_auth
*
*  Description: send AUTH command
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
//u8_t auth_flag=0;
void smtp_send_cmd_auth()
{
//	u8_t encoded[100],*ori;

//	ori=SMTPData.pEEPROM_SMTP->AuthID;
//	cram_md5_encode(ori,encoded);
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C11');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS11");rx_over=0;}
#endif
#endif
	printf("AUTH LOGIN\n");
	strcpy (uip_appdata, "AUTH LOGIN ");
//	strcpy (uip_appdata+11, encoded);
	strcat (uip_appdata, CRLF);
	uip_len=strlen(uip_appdata);
	uip_slen=uip_len;
}


u8_t encoded[100];


void smtp_send_cmd_auth_plain()
{
	//u8_t message_body[10]={0};
	u16_t data_id_len,data_len;

	strcpy (uip_appdata, "AUTH PLAIN ");

	encoded[0]=0;
	data_id_len=strlen(SMTPData.pEEPROM_SMTP->AuthID);	
	memcpy(&encoded[1],SMTPData.pEEPROM_SMTP->AuthID,data_id_len);

	encoded[1+data_id_len]=0;
	data_len=strlen(SMTPData.pEEPROM_SMTP->AuthPW);	
	memcpy(&encoded[2+data_id_len],SMTPData.pEEPROM_SMTP->AuthPW,data_len);
	
	b64encode_datalen((2+data_id_len+data_len),encoded, &uip_appdata[11]);
	
	strcat (uip_appdata, CRLF);
	uip_len=strlen(uip_appdata);
	printf("Client Send %d %d %d =%s",data_id_len,data_len,uip_len,uip_appdata);
	uip_slen=uip_len;
}


/******************************************************************************
*
*  Function:    smtp_send_auth_id_pw
*
*  Description: send user ID or password
*               
*  Parameters:  is_id: 1:UserID, 0:password
*               
*  Returns:     
*               
*******************************************************************************/
#ifdef SUPPORT_SMTP_AUTH
void smtp_send_auth_id_pw(u8_t is_id)
{	
	u8_t *ori;
	if(is_id==1)
	{	ori=SMTPData.pEEPROM_SMTP->AuthID;	}
	else if(is_id==0)
	{	ori=SMTPData.pEEPROM_SMTP->AuthPW;	}
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C12');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS12");rx_over=0;}
#endif
#endif
	cram_md5_encode(ori,encoded);
	strcpy (uip_appdata, encoded);
	strcat (uip_appdata, CRLF);
	uip_len=strlen(uip_appdata);
	uip_slen=uip_len;
}
#endif
/******************************************************************************
*
*  Function:    smtp_send_packet
*
*  Description: Send packet out
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void smtp_send_packet()
{
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C13');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS13");rx_over=0;}
#endif
#endif
	  	switch(SMTPData.Status)
  		{
			case SMTP_HELO:
			case SMTP_EHLO:
				smtp_send_cmd_helo();
				break;			 
//			case SMTP_STARTTLS:		 smtp_send_starttls();break;
#ifdef SUPPORT_SMTP_AUTH
			case SMTP_AUTH_PLAIN:    smtp_send_cmd_auth_plain();break;
			case SMTP_AUTH:          smtp_send_cmd_auth();break;
			case SMTP_AUTH_ID:       smtp_send_auth_id_pw(1);break;
			case SMTP_AUTH_PW:       smtp_send_auth_id_pw(0);break;
//			case SMTP_AUTH_PLAIN:	 smtp_send_auth_id_pw(2);break;
#endif//SUPPORT_SMTP_AUTH
			case SMTP_MAIL:          smtp_send_cmd_mail();break;
			case SMTP_RCPT:
				                     SMTPData.MailToCnt--;
			                         smtp_send_cmd_rcpt();
					                 SMTPData.MailToCnt++;
				                     break;
			case SMTP_DATA:          smtp_send_cmd_data();break;
			case SMTP_SEND_MAIL:     smtp_send_mailbody();break;
			case SMTP_SEND_MAIL_END: smtp_send_mailbody_end();break;
			case SMTP_QUIT:          smtp_send_cmd_quit();break;
//			default:
//				smtp_send_cmd_helo();
//				break;
		}	
}

/******************************************************************************
*
*  Function:    smtp
*
*  Description: callback function for uip
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
u8_t ehlo_flag=0;
u8_t ehlo_count=1;
void smtp()
{
u16_t datalen,kk,jj;
	
	if(uip_connected()) {
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C14');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS14");rx_over=0;}
#endif
#endif
		SMTPData.Status=SMTP_220;
		printf("SMTP Connect\n");
	}
	if(uip_acked()) {
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C15');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS15");rx_over=0;}
#endif
#endif
	}
  
  if(uip_newdata()) {
		char buf[4];
		char Message[21];
		memcpy(buf,uip_appdata,3);
		buf[3]='\0';
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C16');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS16");rx_over=0;}
#endif
#endif

		//printf("State=%d Buf=%s\n",(u16_t)SMTPData.Status,uip_appdata);//peter
		printf("State=%d Buf=%s\n",(u16_t)SMTPData.Status,buf);//peter		
		uip_appdata[uip_len]=0; 					
		datalen=strlen(uip_appdata);		
		printf("Server %d %d =%s\n",datalen, uip_len, uip_appdata);//peter
		switch(atoi(buf))
		{
			case SMTP_SERVER_ACK220:
				  SMTPData.Status=SMTP_EHLO;
				  break;
			case SMTP_SERVER_ACK221:
				  SMTPData.Status=SMTP_IDLE;
				  break;
#ifdef SUPPORT_SMTP_AUTH
			case SMTP_SERVER_ACK235:
 	     	      SMTPData.Status=SMTP_MAIL;
				  break;
#endif//SUPPORT_SMTP_AUTH
			case SMTP_SERVER_ACK503:
				SMTPData.Status=SMTP_HELO;
				break;

			case SMTP_SERVER_ACK535:
				printf("ack 535\n");
				SMTPData.Status=SMTP_AUTH;
				break;

			case SMTP_SERVER_ACK250:
				  switch(SMTPData.Status)
				  {
					 case SMTP_EHLO2:
					 case SMTP_EHLO:

						
						for(kk=0;kk<datalen-1;kk++){
							if(uip_appdata[kk]==0x0a) {
								for(jj=kk+1;jj<datalen-1;jj++){
									if(uip_appdata[jj]==0x0d) {		
									   memcpy(Message,&uip_appdata[kk+1],20);	
									   Message[20]=='\0';
									   //printf("Server Message=%s\n",Message);
									   
									   if(!strcmp(Message, "250-AUTH LOGIN PLAIN")){
										   SMTPData.Status=SMTP_AUTH_PLAIN;
										   printf("250-AUTH LOGIN PLAIN\n");
										   break;
									   }						   
									   
									   if(!strcmp(Message, "250-AUTH=LOGIN PLAIN")){
										   SMTPData.Status=SMTP_AUTH;
										   printf("250-AUTH LOGIN\n");
										   break;
									   }						   
									}   

								}
							}
						}	
						if(SMTPData.Status==SMTP_AUTH_PLAIN)
							break;
						
						if(SMTPData.Status==SMTP_AUTH)
							break;
						else{
							SMTPData.Status=SMTP_AUTH;
							break;
						}	
							
						if(uip_appdata[uip_len-4]=='O')
						{
							if(uip_appdata[uip_len-3]=='K')
							{
								printf("OK");
								ehlo_flag=0;
								ehlo_count=1;
								if(SMTPData.pEEPROM_SMTP->AuthEnable)
					     	    	SMTPData.Status=SMTP_AUTH;
								else
									SMTPData.Status=SMTP_MAIL;
							}
						}
						else
						{
							ehlo_flag=1;
							SMTPData.Status=SMTP_EHLO2;
							memset(uip_appdata, 0, uip_len);	
						}
						break;
				     case SMTP_HELO:
#ifdef SUPPORT_SMTP_AUTH
							if(SMTPData.pEEPROM_SMTP->AuthEnable)
				     	    	SMTPData.Status=SMTP_AUTH;
							else
								SMTPData.Status=SMTP_MAIL;
#else//SUPPORT_SMTP_AUTH
				     	    SMTPData.Status=SMTP_MAIL;
#endif//SUPPORT_SMTP_AUTH
				     	    break;
				     case SMTP_MAIL:
    			     		SMTPData.Status=SMTP_RCPT;
 							SMTPData.MailToCnt=0;
							smtp_chk_MailToTotal();
 			     	      SMTPData.MailToCnt++;
				     	    break;
				     case SMTP_RCPT:
					      if(SMTPData.MailToCnt++>=SMTPData.MailToTotal)SMTPData.Status=SMTP_DATA;	
				     	    break;
				     case SMTP_SEND_MAIL_END:
				     	    SMTPData.Status=SMTP_QUIT;
				     	    break;				     	    				     	    				     	    
				  }
				  break;
#ifdef SUPPORT_SMTP_AUTH
			case SMTP_SERVER_ACK334:
				if(SMTPData.Status==SMTP_AUTH_ID)SMTPData.Status=SMTP_AUTH_PW;
		  		if(SMTPData.Status==SMTP_AUTH)SMTPData.Status=SMTP_AUTH_ID;
				  break;
#endif//SUPPORT_SMTP_AUTH
			case SMTP_SERVER_ACK354:
				  SMTPData.Status=SMTP_SEND_MAIL;
				  break;				  				  				  
		}
		printf("SMPT Status=%d\n",(u16_t)SMTPData.Status); //peter
  }  
  if(uip_rexmit()||uip_newdata()) {
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C17');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS17");rx_over=0;}
#endif
#endif
		smtp_send_packet();
  }else if(uip_acked()) {
		if(SMTPData.Status==SMTP_SEND_MAIL)
		{
			SMTPData.Status=SMTP_SEND_MAIL_END;
			smtp_send_packet();
		}
  }
  if(uip_poll()) {
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C18');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS18");rx_over=0;}
#endif
#endif
	if(SMTPData.IdleCnt++>120)
	//if(SMTPData.IdleCnt++>320)
	{
		SMTPData.Status=SMTP_IDLE;
		printf("SMTP Time-out\n");
	}
	if(ehlo_flag)
	{
		ehlo_count++;
		if(ehlo_count>=30)
		{
			ehlo_flag=0;
			ehlo_count=1;
			if(SMTPData.pEEPROM_SMTP->AuthEnable)
     	    	SMTPData.Status=SMTP_AUTH;
			else
				SMTPData.Status=SMTP_MAIL;
			smtp_send_packet();
		}	
	}
  }
  else
  {
	SMTPData.IdleCnt=0;
  }
   if(uip_closed()||uip_aborted())
   {
		SMTPData.Status=SMTP_IDLE;
   }
   
}

/******************************************************************************
*
*  Function:    SMTPOnTCPRx
*
*  Description: callback function for uip
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
u8_t SMTPOnTCPRx()
{
  if(uip_conn->rport==SMTPData.pEEPROM_SMTP->SMTPPort)
  {
  		smtp();
  		return 1;
  }
  return 0;
}

void smtp_check_mail(){
	u8_t j=0;
	/* Send mail after 2 minute */
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r16");rx_over=0;}
#endif
#endif
	j=SMTP_GPIO_PIN1;
	j*=2;
	j|=SMTP_GPIO_PIN0;
	if(gpio_cnt++>=GPIO_CHECK_RATE)
	{
		if(j!=0){
			smtp_send_mail(j-1);
		}
		gpio_cnt=0;
	}
    else
    {
/* Send mail when status change */
		if(j!=0)
		{
			if(last_status!=j)
			{
				smtp_send_mail(j-1);
				gpio_cnt=0;
			}
		}
	}
	last_status=j;
}
#endif //MODULE_SMTP

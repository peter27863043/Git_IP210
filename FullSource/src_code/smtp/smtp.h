/******************************************************************************
*
*   Name:           smtp.h
*
*   Description:    
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
#include "module.h"
#include "type_def.h"
#include "eepdef.h"
#ifdef MODULE_SMTP
#define SUPPORT_SMTP_AUTH
#ifndef SMTP_H
#define SMTP_H
#define SMTP_ENABLE_MASK 0x01
#define AUTH_ENABLE_MASK 0x01
#define CRLF "\r\n"

#define GPIO_CHECK_RATE 1200
#ifdef IC_PACKAGE_128_PIN
#define SMTP_GPIO_PIN0 P3_2
#define SMTP_GPIO_PIN1 P3_3
#else
#define SMTP_GPIO_PIN0 P3_5
#define SMTP_GPIO_PIN1 P1_2
#endif 

enum {
	SMTP_IDLE,
	SMTP_QUERY_DNS,
	SMTP_CONNECTING,
	SMTP_WAITCONNECT,
	SMTP_220,          //wait for 220
	SMTP_HELO,         //send HELLO command,     and wait for ACK
	SMTP_EHLO,
	SMTP_EHLO2,
	SMTP_AUTH,         //send AUTH command,      and wait for ACK
	SMTP_STARTTLS,     //send STARTTLS handshake	
	SMTP_AUTH_ID,      //send user ID,           and wait for ACK
	SMTP_AUTH_PW,      //send password,          and wait for ACK
	SMTP_MAIL,         //send MAIL command,      and wait for ACK
	SMTP_RCPT,         //send RCPT command,      and wait for ACK
	SMTP_DATA,         //send DATA command,      and wait for ACK
	SMTP_SEND_MAIL,    //send email data,        and wait for ACK
	SMTP_SEND_MAIL_END,//end of send email data, and wait for ACK
	SMTP_QUIT          //send QUIT command,      and wait for ACK
};
#define MAX_SMTP_MAIL 50
#define WAIT_CONN_DELAY 10//n*100ms
#define WAIT_CONN_RETRY 10
#define MAX_MAIL_TO 10
typedef struct _SSMTPData
{
   SSMTPInfo* pEEPROM_SMTP;
	u32_t ServerIPAddr;
	u8_t MailNo;
	u8_t MailTotal;
	u8_t MailToCnt;
	u8_t MailToTotal;
	u8_t MailToOffsetStart[MAX_MAIL_TO];
	u8_t MailToOffsetEnd[MAX_MAIL_TO];
	u8_t Status;
  	u8_t* pMAIL_SUBJ[MAX_SMTP_MAIL];
	u8_t* pMAIL_MSG[MAX_SMTP_MAIL];
	u8_t WaitConnRetry;
	u8_t WaitConnDelay;
	u8_t timer;
  	u8_t IdleCnt;  
}SSMTPData;

//ACKxyz:
//x=2:Positive Completion reply
//x=3:Positive Intermediate

//y=2:Connections
//y=5:Mail system

#define SMTP_SERVER_ACK220 220 //Service ready
#define SMTP_SERVER_ACK221 221 //Service closing transmission channel
#define SMTP_SERVER_ACK235 235 //Login success
#define SMTP_SERVER_ACK250 250 //Requested mail action okay, completed
#define SMTP_SERVER_ACK334 334 //AUTH ACK
#define SMTP_SERVER_ACK354 354 //Start mail input; end with <CRLF>.<CRLF>
#define SMTP_SERVER_ACK535 535 //Server request AUTH
#define SMTP_SERVER_ACK503 503 //Server request say ehlo first
//#define SMTP_SERVER_ACK504 504 //AUTH LOGIN/PLAIN TYPE ERROR

#define MAIL_LIST_FULL 255
#define MAIL_CREATE 255

void smtp_check_mail();
void smtp_timer();
void smtp_init();
u8_t smtp_send_mail(u8_t MailNo);
u8_t ModifyMail(char* sub,char* msg,u8_t MailNo);
#define AddMail(sub,msg) ModifyMail(sub,msg,MAIL_CREATE);//255: new mail
#define SMTPReset() {SMTPData.Status=SMTP_IDLE;}
u8_t SMTPOnTCPRx();

extern SSMTPData SMTPData;

extern u16_t gpio_cnt;
extern u8_t  last_status;

#endif //SMTP_H
#endif //MODULE_SMTP

/*******************************************************************************
* SMTP Description:
* MailNo 0 to 3 is load from eeprom
* MailNo 4 to 49 is modify in runtime
*
* 1. Please call "AddMail()" to create new mail, it will return MailNo
* 2. Please call "smtp_send_mail(MailNo)" to send this mail
* 3. If you want to modify mail infomation, use "ModifyMail(sub,msg,MailNo)".
Example:
  u8_t MailNo=AddMail("subject01","message01");
  smtp_send_mail(MailNo);
*******************************************************************************/
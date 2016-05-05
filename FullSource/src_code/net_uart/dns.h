/******************************************************************************
*
*   Name:           dns.h
*
*   Description:    
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#include "module.h"
#ifdef MODULE_DNS
#ifndef DNS_H
#define DNS_H
#include "type_def.h"
typedef struct _SDNSPacket
{
	u16_t ID;
	u16_t Flags;
	u16_t NumOfQuestions;
	u16_t NumOfAnswers;
	u16_t NumOfAuthoritativeAnswers;
	u16_t NumOfAdditionalAnswers;
	u8_t  Questions[1];
}SDNSPacket;

typedef struct _SAnswer
{
	u16_t DomainName;
	u16_t QueryType;
	u16_t QueryClass;
	u32_t TTL;
	u16_t DataLen;
	u32_t IPAddr;
}SAnswer;

void DNS_Query(char* name,u16_t TimeOut);
void DNS_UDP_RECEIVE();

u32_t DNS_Result(char* Name);//return value:DNS_WAIT_ACK, DNS_NOT_MATCH_NAME
#define DNS_WAIT_ACK 0x00000000
#define DNS_NOT_MATCH_NAME 0xAAAAAAAA

typedef struct _SDNSInfo
{
	char *Name;
	u32_t IPAddr;
	u16_t TimeOut;
}SDNSInfo;
#endif //DNS_H
#endif //MODULE_DNS
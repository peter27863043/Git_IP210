/******************************************************************************
*
*   Name:           dns.c
*
*   Description:    DNS client protocol
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#include "module.h"
#ifdef MODULE_DNS
#include "dns.h"
#include "udp.h"
#include "uip.h"
#include "stdio.h"
#ifdef MODULE_NET_UART
#include "net_uart.h"
#endif
#include <string.h>
#define DNS_HADER_ID         0x12,0x34
#define DNS_HADER_FLAG       0x01,0x00
#define DNS_HADER_NumOfQue   0x00,0x01
#define DNS_HADER_NumOfAns   0x00,0x00
#define DNS_HADER_NumOfAut   0x00,0x00
#define DNS_HADER_NumOfAdd   0x00,0x00
#define DNS_HADER_QueryType  0x00,0x01
#define DNS_HADER_QueryClass 0x00,0x01
SDNSInfo DNSInfo;
u16_t stoui(char* str_value);

/******************************************************************************
*
*  Function:    DNS_Query
*
*  Description: Query DNS server what is the IP address of this name
*               
*  Parameters:  name:    name for ask
*               TimeOut: If no response how much will case fail.
*  Returns:     
*               
*******************************************************************************/
void DNS_Query(char* name,u16_t TimeOut)
{
	u8_t* pData=UdpData.p_DataPkt->udpload;
	u8_t i,j,IsIPAddrStr=1,cnt;
	u8_t *pLastLen,Len;
	char buf[4];
	char DNS_QUERY_TEMP[]={
	DNS_HADER_ID,
	DNS_HADER_FLAG,
	DNS_HADER_NumOfQue,
	DNS_HADER_NumOfAns,
	DNS_HADER_NumOfAut,
	DNS_HADER_NumOfAdd
    };
	char DNS_QUERY_TEMP2[]={
	DNS_HADER_QueryType,
	DNS_HADER_QueryClass
	};

	for(i=0;i<strlen(name);i++)
	{
		if(name[i]<'0'||name[i]>'9')
		{
			if(name[i]!='.')IsIPAddrStr=0;
		}
	}	
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
	DNSInfo.Name=name;
	DNSInfo.IPAddr=0;
	DNSInfo.TimeOut=TimeOut;

	if(IsIPAddrStr)
	{
		printf("IP Address\n");
		j=-1;
		for(i=0;i<4;i++)
		{
			cnt=0;
			j++;
			for(;j<=strlen(name);j++)
			{
				if(name[j]=='.'||name[j]=='\0')
				{
					buf[cnt]='\0';
					break;
				}
				else
				{
					buf[cnt++]=name[j];
				}
			}
			((u8_t*)&DNSInfo.IPAddr)[i]=(u8_t)stoui(buf);
		}
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
	}
	else
	{
		printf("DNS Query\n");
		memcpy(pData,DNS_QUERY_TEMP,sizeof(DNS_QUERY_TEMP));
		pData+=sizeof(DNS_QUERY_TEMP);

		pLastLen=pData++;//reserve for 1st_len
		Len=0;
		for(i=0;i<strlen(name)+1;i++)
		{
			if(name[i]=='.'||name[i]=='\0')
			{
				*pLastLen=Len;
				pLastLen=pData;
				Len=0;	
			}
			else
			{
				Len++;
				*pData=name[i];
			}
			pData++;
		}
		*(pData-1)=0;
		memcpy(pData,DNS_QUERY_TEMP2,sizeof(DNS_QUERY_TEMP2));
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
		//UDP settings:
		UdpData.SrcIPAddr[0] =uip_hostaddr[0];
	  	UdpData.SrcIPAddr[1] =uip_hostaddr[1];
  		UdpData.DestIPAddr[0]=uip_dnsaddr[0];
		UdpData.DestIPAddr[1]=uip_dnsaddr[1];
		UdpData.SrcPort=53;
		UdpData.DestPort=1000;
		udp_send(sizeof(DNS_QUERY_TEMP)+strlen(name)+2+sizeof(DNS_QUERY_TEMP2));
	}
}

/******************************************************************************
*
*  Function:    DNS_UDP_RECEIVE
*
*  Description: callback function for UDP
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void DNS_UDP_RECEIVE()
{
	u16_t i;
	SDNSPacket* pDNSPacket=(SDNSPacket*)UdpData.p_DataPkt->udpload;	
	u8_t* pAnswers=pDNSPacket->Questions;
	SAnswer* pAnswerFormat;
	if(UdpData.p_DataPkt->srcport!=53)return;

	for(i=0;i<pDNSPacket->NumOfQuestions;i++)
	{
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
		pAnswers+=strlen(pAnswers)+1;//QuestionFormat.QueryName
		pAnswers+=4;//QuestionFormat.QueryType and QuestionFormat.QueryClass
	}
	for(i=0;i<pDNSPacket->NumOfAnswers;i++)
	{
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
		pAnswerFormat=(SAnswer*)pAnswers;
		if(pAnswerFormat->QueryType==0x01)
		{
			DNSInfo.IPAddr=pAnswerFormat->IPAddr;
		}
		pAnswers+=12+pAnswerFormat->DataLen;
	}
}

/******************************************************************************
*
*  Function:    DNS_Result
*
*  Description: Get result of Query
*               
*  Parameters:  Same name of DNS_Query
*               
*  Returns:     If equal DNS_NOT_MATCH_NAME, the name is not match or timeout
*               if not equal DNS_NOT_MATCH_NAME, that is IP address for DNS server
*******************************************************************************/
u32_t DNS_Result(char* Name)
{
	u32_t result=DNS_NOT_MATCH_NAME;
	if(DNSInfo.Name==Name)
	{
		result=DNSInfo.IPAddr;
		DNSInfo.IPAddr=DNS_NOT_MATCH_NAME;
	}
	if(DNSInfo.TimeOut--<=0)DNSInfo.IPAddr=DNS_NOT_MATCH_NAME;
	return result;
}
#endif //MODULE_DNS

/******************************************************************************
*
*   Name:           
*
*   Description:    
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#include "udp_tel.h"
#include "module.h"
#include "net_uart.h"
#include "udp.h"
#include "public.h"
#include "timer.h"
#include "telnet.h"
#include "MACdriver.h"
#include "os_cbk.h"
#include <string.h>
#include <stdio.h>
#ifndef MODULE_NET_UART
extern SNetUARTData NetUARTData;
#endif
extern void WatchDogTimerReset();
#ifdef MODULE_UDP_TELNET
udp_tel_data_t udp_tel_data;
u16_t port_learn[UDP_TEL_MAX_IP_RANGE]={0};
#ifdef MODULE_FLASH512
u8_t udp_nor_brcst=0;
u16_t udp_nor_brcst_port=0;
void udp_learn_ip();
#endif
#ifdef DB_UDP_TEL
u16_t db_rx[3],db_tx[3];//M,K,Byte
u16_t net_fifo_overflow=0;
void db_inc(u16_t value, u16_t* ptr)
{
	ptr[0]+=value;
	ptr[1]+=ptr[0]/1024;
	ptr[0]=ptr[0]%1024;
	ptr[2]+=ptr[1]/1024;
	ptr[1]=ptr[1]%1024;
}
#endif //DB_UDP_TEL
#ifdef MODULE_VLAN_TAG
extern u8_t eth_vlan;
extern u16_t uip_udpchksum(void);
extern u16_t uip_ipchksum(void);
u8_t udp_send_pkt=0;
#ifdef DB_FIFO
extern u8_t rx_over;
#endif
#endif
//----------ADD FOR DNS---------------------------------------------------------
extern u32_t DNS_Result(char* Name);
extern void DNS_Query(char* name,u16_t TimeOut);
#include "dns.h"
u32_t udp_check_ip[10]=0;
u8_t udp_dns=5;
u8_t udp_dns_count=0;
//------------------------------------------------------------------------------
u8_t get_bit(u32_t* value, u8_t offset)
{
	return ((value[0]>>offset) & 0x1);
}
//------------------------------------------------------------------------------
void set_bit(u32_t* value, u8_t offset, u8_t set_value)
{
	u32_t tmp=1<<offset;
	if(set_value)
	{
		value[0]|=tmp;
	}
	else
	{
		value[0]&=~tmp;
	}
}
//------------------------------------------------------------------------------
void udp_tel_updata_ip_aval()
{
	u8_t i, valid=0;
	u32_t tmp=1;
	udp_tel_data.valid_ip=0;
	for(i=0;i<UDP_TEL_MAX_IP_RANGE;i++)
  	{
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r33");rx_over=0;}
#endif
#endif
#ifdef MODULE_DNS
		if(udp_check_ip[i]!=0)
#else
	  	if(udp_tel_data.pEEPROM_UDPTel->remote[i].ip[0]!=0)
#endif
	  	{
	  		udp_tel_data.valid_ip|=tmp;
#ifdef MODULE_FLASH512
			if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR)
			{
#ifdef MODULE_DNS
				if(udp_check_ip[i]==0xffffffff)
#else
				if( (ModuleInfo.UDPTelInfo.remote[i].ip[0]==0xffff)&&(ModuleInfo.UDPTelInfo.remote[i].ip[1]==0xffff) )
#endif
				{
					udp_nor_brcst_port=ModuleInfo.UDPTelInfo.remote[i].port;
					udp_nor_brcst=1;
				}
			}
#endif
	  	}
	  	tmp=tmp<<1;
  	}
}
//------------------------------------------------------------------------------
void udp_tel_init()
{
#ifdef MODULE_FLASH512
	u8_t i;
#endif
  udp_tel_data.pEEPROM_UDPTel=&ModuleInfo.UDPTelInfo;
  NetUARTData.pkt_rx_data_offset=UdpData.p_DataPkt->udpload;
  NetUARTData.pkt_tx_data_offset=UdpData.p_DataPkt->udpload;
  //udp_tel_init_ip_diff(IP_DIFF_DISABLE);
  udp_tel_data.current_ip_num=0;  
  //Initial DNS flag
  udp_tel_data.dns_wait=0;
  udp_tel_data.dns_retry=0;
  udp_tel_data.dns_idle=0;

  udp_tel_updata_ip_aval();
  IP210_Update_Timer_Counter();
  udp_tel_data.nxt_run_start = timercounter;
  udp_tel_data.nxt_blk_start = timercounter;
  udp_tel_data.is_send=0;
  udp_tel_data.initial_flag=0;
  RxBuf=UDP_RxBuf;
  TxBuf=UDP_TxBuf;
  TelnetData.RexmitLen=0;
  max_uart_rx_buf_len=UDP_MAX_UART_RX_BUF_LEN;
  max_uart_tx_buf_len=UDP_MAX_UART_TX_BUF_LEN;
  RingInit(UartRxBuf,RxBuf,max_uart_rx_buf_len);
  RingInit(UartTxBuf,TxBuf,max_uart_tx_buf_len);  
#ifdef MODULE_FLASH512
	if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS)||(udp_nor_brcst==1) )
	{
		for(i=0;i<UDP_TEL_MAX_IP_RANGE;i++) //if listen mode clear ip address
	  	{
			memset(&ModuleInfo.dido_info.remote[i].dnsip, 0, 10);
			ModuleInfo.UDPTelInfo.remote[i].ip[0]=0;
			ModuleInfo.UDPTelInfo.remote[i].ip[1]=0;
			ModuleInfo.UDPTelInfo.remote[i].port=0;
	  	}
	}
	else //normal
	{
		for(i=0;i<UDP_TEL_MAX_IP_RANGE;i++) //if listen mode clear ip address
	  	{
			if(ModuleInfo.dido_info.remote[i].ip_demo==0)
			{
				udp_check_ip[i]=ModuleInfo.UDPTelInfo.remote[i].ip[0];
				udp_check_ip[i]=udp_check_ip[i]<<16;
				udp_check_ip[i]|=ModuleInfo.UDPTelInfo.remote[i].ip[1];
			}
		}
	}
#endif
#ifdef DB_UDP_TEL
  db_tx[0]=0;
  db_tx[1]=0;
  db_tx[2]=0;
  db_rx[0]=0;
  db_rx[1]=0;
  db_rx[2]=0;
#endif //DB_UDP_TEL
}

/******************************************************************************
*
*  Function:    udp_tel_chk_last_send
*
*  Description: Check if last packet had sent to all ip in the list or not.
*               
*  Parameters: get_ip: ip
*               
*  Returns: 0: all is done, 1: still some ip in it., 2: this ip not in ARP
*               
*******************************************************************************/
#define END_OF_TABLE 0
#define GOT_IP 1
extern struct arp_entry arp_table[UIP_ARPTAB_SIZE];
u8_t udp_tel_getip(u16_t* ip, u16_t* port)
{
	u8_t result=0;

	while(udp_tel_data.current_ip_num<UDP_TEL_MAX_IP_RANGE)
	{
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r111");rx_over=0;}
#endif
#endif
#ifdef MODULE_DNS
		ip[0]=udp_check_ip[udp_tel_data.current_ip_num]>>16;
		ip[1]=udp_check_ip[udp_tel_data.current_ip_num]&0xffff;
#else
	    ip[0]=udp_tel_data.pEEPROM_UDPTel->remote[udp_tel_data.current_ip_num].ip[0];
	    ip[1]=udp_tel_data.pEEPROM_UDPTel->remote[udp_tel_data.current_ip_num].ip[1];
#endif
		if(udp_tel_data.pEEPROM_UDPTel->remote[udp_tel_data.current_ip_num].port==0)		//learning port
		{
			if(port_learn[udp_tel_data.current_ip_num]==0)
			{ port[0]=udp_tel_data.pEEPROM_UDPTel->local_port; }
			else
			{ port[0]=port_learn[udp_tel_data.current_ip_num]; }
		}
		else
	   	{ port[0]=udp_tel_data.pEEPROM_UDPTel->remote[udp_tel_data.current_ip_num].port; }

	  	if(ip[0]==0)
	  	{
			udp_tel_data.current_ip_num++;
	  		continue;
	  	}
	  	if(get_bit(&udp_tel_data.is_send,udp_tel_data.current_ip_num))
	  	{
			udp_tel_data.current_ip_num++;
	  		continue;
	  	}
		result=1;
		break;
	}
	return result;
}
//------------------------------------------------------------------------------
struct arp_entry {
  u16_t ipaddr[2];
  struct uip_eth_addr ethaddr;
  u16_t time;
};

void udp_tel_send_one_pkt()
{
	u8_t j=0,i=0;
	u8_t* old_buf;
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r34");rx_over=0;}
#endif
#endif
#ifdef MODULE_FLASH512
	if(udp_nor_brcst)
	{
		//UDP settings:
		UdpData.SrcIPAddr[0]=uip_hostaddr[0];
		UdpData.SrcIPAddr[1]=uip_hostaddr[1];
		UdpData.DestIPAddr[0]=0xffff;
		UdpData.DestIPAddr[1]=0xffff;
		//UdpData.SrcPort=udp_tel_data.pEEPROM_UDPTel->local_port;
		if(udp_nor_brcst_port==0)
		{ udp_nor_brcst_port=udp_tel_data.pEEPROM_UDPTel->local_port; }
		UdpData.SrcPort=udp_nor_brcst_port;
		UdpData.DestPort=udp_tel_data.pEEPROM_UDPTel->local_port;
//		udp_send(TelnetData.RexmitLen);
		set_bit(&udp_tel_data.is_send,udp_tel_data.current_ip_num-1,1);
#ifdef MODULE_VLAN_TAG
		old_buf=udp_set_buf(&UDP_TxPktBuf[UIP_LLH_LEN+8]);
#else
		old_buf=udp_set_buf(&UDP_TxPktBuf[UIP_LLH_LEN]);
#endif
		udp_send(TelnetData.RexmitLen-TelnetData.Delimiter_drop_len);
#ifdef MODULE_VLAN_TAG
		chg_uip_buf(&UDP_TxPktBuf[8]);
#else
		chg_uip_buf(UDP_TxPktBuf);
#endif
		if(uip_len)
		{
			uip_arp_out();
#ifdef MODULE_VLAN_TAG
			if(ModuleInfo.VLANINFO.vlan)
			{
				UDP_TxPktBuf[4]=0x81; UDP_TxPktBuf[5]=0;
				memcpy(&UDP_TxPktBuf[6], ModuleInfo.VLANINFO.value_id, 2);
				EthernetSend(UDP_TxPktBuf,uip_len);
			}
			else
			{ EthernetSend(&UDP_TxPktBuf[4],uip_len); }	
#else
			EthernetSend(UDP_TxPktBuf,uip_len);
#endif
			uip_len=0;
		}
		restore_uip_buf();
		udp_set_buf(old_buf);
		if(TelnetData.RexmitLen)RingIncRi(UartRxBuf,TelnetData.RexmitLen);
		TelnetData.RexmitLen=0;
		return;
	}
#endif
	if(udp_tel_getip(UdpData.DestIPAddr,&UdpData.DestPort)==END_OF_TABLE)
	{ return; }
//------------------------For Different Lan------------------------------------
  	if((UdpData.DestIPAddr[0] & uip_arp_netmask[0])==(uip_hostaddr[0] & uip_arp_netmask[0])&&
       (UdpData.DestIPAddr[1] & uip_arp_netmask[1])==(uip_hostaddr[1] & uip_arp_netmask[1]))
	{
		for(j=0;j<UIP_ARPTAB_SIZE;j++)
		{
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r35");rx_over=0;}
#endif
#endif
			if((UdpData.DestIPAddr[0]==arp_table[j].ipaddr[0])&&(UdpData.DestIPAddr[1]==arp_table[j].ipaddr[1]))
			{ break; }
		}
	}
//---------------------------------------------------------------------------------*/
	//UDP settings:
	UdpData.SrcIPAddr[0] =uip_hostaddr[0];
	UdpData.SrcIPAddr[1] =uip_hostaddr[1];
	//UdpData.SrcPort=udp_tel_data.pEEPROM_UDPTel->local_port;
	UdpData.SrcPort=UdpData.DestPort;
	UdpData.DestPort=udp_tel_data.pEEPROM_UDPTel->local_port;
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r36");rx_over=0;}
#endif
#endif
//	if(j!=UIP_ARPTAB_SIZE)
	{
		//ReSendUARTRx2Eth(); //move to new_block_init()
#ifdef MODULE_VLAN_TAG
		old_buf=udp_set_buf(&UDP_TxPktBuf[UIP_LLH_LEN+8]);
#else
		old_buf=udp_set_buf(&UDP_TxPktBuf[UIP_LLH_LEN]);
#endif
		udp_send(TelnetData.RexmitLen-TelnetData.Delimiter_drop_len);
#ifdef MODULE_VLAN_TAG
		chg_uip_buf(&UDP_TxPktBuf[8]);
#else
		chg_uip_buf(UDP_TxPktBuf);
#endif
		if(uip_len)
		{
			uip_arp_out();
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r37");rx_over=0;}
#endif
#endif
#ifdef MODULE_VLAN_TAG
			if(ModuleInfo.VLANINFO.vlan)
			{
				UDP_TxPktBuf[4]=0x81; UDP_TxPktBuf[5]=0;
				memcpy(&UDP_TxPktBuf[6], ModuleInfo.VLANINFO.value_id, 2);
				EthernetSend(UDP_TxPktBuf,uip_len);
			}
			else
			{ EthernetSend(&UDP_TxPktBuf[4],uip_len); }			
#else
			EthernetSend(UDP_TxPktBuf,uip_len);
#endif
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r38");rx_over=0;}
#endif
#endif
			if(j!=UIP_ARPTAB_SIZE)
			{
				set_bit(&udp_tel_data.is_send,udp_tel_data.current_ip_num,1);
			}
			udp_tel_data.current_ip_num++;
			uip_len=0;
		}
		restore_uip_buf();
		udp_set_buf(old_buf);
#ifdef DB_UDP_TEL
		db_inc(TelnetData.RexmitLen,db_tx);
#endif //DB_UDP_TEL
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r39");rx_over=0;}
#endif
#endif
	}
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r40");rx_over=0;}
#endif
#endif
}
//------------------------------------------------------------------------------
void new_block_init()
{
	u8_t* old_buf;
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r41");rx_over=0;}
#endif
#endif
	SendUARTRx2Eth(1);

	if(TelnetData.RexmitLen==0)
		return;

	udp_tel_data.is_send=0;
	udp_tel_data.current_ip_num=0;
	udp_tel_data.initial_flag=1;
#ifdef MODULE_VLAN_TAG
	old_buf=udp_set_buf(&UDP_TxPktBuf[UIP_LLH_LEN+8]);
#else
	old_buf=udp_set_buf(&UDP_TxPktBuf[UIP_LLH_LEN]);
#endif
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r42");rx_over=0;}
#endif
#endif
	NetUARTData.pkt_tx_data_offset=UdpData.p_DataPkt->udpload;
	ReSendUARTRx2Eth();
	udp_set_buf(old_buf);
	NetUARTData.pkt_tx_data_offset=UdpData.p_DataPkt->udpload;	
	//ReSet ReTransmit Time
	IP210_Update_Timer_Counter();
	udp_tel_data.nxt_run_start=timercounter;
}
//------------------------------------------------------------------------------
void udp_tel_chk_send()
{
	uip_len=0;

	if(UartRxBuf.TotalLen!=0)
	{
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r43");rx_over=0;}
#endif
#endif
#ifdef MODULE_VLAN_TAG
		if(ModuleInfo.VLANINFO.vlan)
		{
			if( (ModuleInfo.VLANINFO.value_id[0]!=0)||(ModuleInfo.VLANINFO.value_id[1]!=0) )
			{
				uip_buf_real=&UDP_TxPktBuf[8];
				udp_send_pkt=1;
			}
		}
#endif
#ifdef MODULE_FLASH512
		if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS)  //if no find ip address hold data in ring buff
		{
#ifdef MODULE_DNS
			if( (udp_check_ip[0]==0)&&(udp_check_ip[0]==0) )
#else
			if( (ModuleInfo.UDPTelInfo.remote[0].ip[0]==0)&&(ModuleInfo.UDPTelInfo.remote[0].ip[1]==0) )
#endif
			{
#ifdef MODULE_VLAN_TAG
                udp_send_pkt=0;
#endif
				return;
            }
		}
#endif
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r112");rx_over=0;}
#endif
#endif
		if(udp_tel_data.initial_flag==0)
		{ new_block_init(); }

		udp_tel_updata_ip_aval();
#ifdef MODULE_FLASH512
		if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR)
		{
			if(udp_nor_brcst)
			{
				udp_tel_send_one_pkt();
#ifdef MODULE_VLAN_TAG
				eth_vlan=0;
				udp_send_pkt=0;
#endif
				return;
			}
		}
#endif
		if(udp_tel_data.is_send!=udp_tel_data.valid_ip)
		{
			if(udp_tel_data.current_ip_num>=UDP_TEL_MAX_IP_RANGE)
			{
				if(Delta_Time(udp_tel_data.nxt_run_start)>=SAME_BLK_NXT_RUN_TIMER)
				{
					if(TelnetData.RexmitLen)RingIncRi(UartRxBuf, TelnetData.RexmitLen);
					TelnetData.RexmitLen=0;
					udp_tel_data.initial_flag=0;
				}
				else
				{
					udp_tel_data.current_ip_num=0;
					udp_tel_send_one_pkt();
				}
			}
			else
			{
#ifdef MODULE_NET_UART
				NET_UART_LOOP('C');
#ifdef DB_FIFO
				if(rx_over){printf("\n\r44");rx_over=0;}
#endif
#endif
				udp_tel_send_one_pkt();
			}
		}//if(is_send!=valid_ip)
		else
		{
			if(TelnetData.RexmitLen)RingIncRi(UartRxBuf, TelnetData.RexmitLen);
			TelnetData.RexmitLen=0;
			udp_tel_data.initial_flag=0;
//			new_block_init();
		}
#ifdef MODULE_VLAN_TAG
		eth_vlan=0;
		udp_send_pkt=0;
#endif
  	}//if(UartRxBuf.TotalLen!=0)
}
//------------------------------------------------------------------------------
#define UMEM_OVERFLOW_MSG_ON
void udp_tel_rx()
{
	u8_t i, valid=0;
#ifdef MODULE_DNS
	u16_t ip[2]=0;
#endif
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r45");rx_over=0;}
#endif
#endif
	if(UdpData.p_DataPkt->destport!=udp_tel_data.pEEPROM_UDPTel->local_port)
	{	return;	}
	else
	{
#ifdef MODULE_FLASH512
		if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS) //like server
		{ udp_learn_ip(); }
		else if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR) //like client
		{
			if(udp_nor_brcst) //if broadcast ip accept all unicast ip
			{
				if( (UdpData.p_DataPkt->srcipaddr[0]==0xffff)&&(UdpData.p_DataPkt->srcipaddr[1]==0xffff) )
				{ return; }
				else
				{ udp_learn_ip(); }
#ifdef MODULE_NET_UART
				NET_UART_LOOP('C');
#ifdef DB_FIFO
				if(rx_over){printf("\n\r46");rx_over=0;}
#endif
#endif
			}
			else
			{
				for(i=0;i<UDP_TEL_MAX_IP_RANGE;i++)
			  	{
#ifdef MODULE_NET_UART
					NET_UART_LOOP('C');
#ifdef DB_FIFO
					if(rx_over){printf("\n\r47");rx_over=0;}
#endif
#endif
#ifdef MODULE_DNS
					if(ModuleInfo.dido_info.remote[i].ip_demo==1)
					{
						ip[0]=udp_check_ip[i]>>16;
						ip[1]=udp_check_ip[i]&0xffff;
					}
					else
					{
						ip[0]=udp_tel_data.pEEPROM_UDPTel->remote[i].ip[0];
						ip[1]=udp_tel_data.pEEPROM_UDPTel->remote[i].ip[1];
					}
				  	if(ip[0]==UdpData.p_DataPkt->srcipaddr[0])
				  	{
						if(ip[1]==UdpData.p_DataPkt->srcipaddr[1])
						{ valid=1; break; }
				  	}
#else
				  	if(udp_tel_data.pEEPROM_UDPTel->remote[i].ip[0]==UdpData.p_DataPkt->srcipaddr[0])
				  	{
						if(udp_tel_data.pEEPROM_UDPTel->remote[i].ip[1]==UdpData.p_DataPkt->srcipaddr[1])
						{ valid=1; break; }
				  	}
#endif
			  	}
				if(valid)
				{
					if(port_learn[i]!=UdpData.p_DataPkt->srcport)		//learning port
					{ port_learn[i]=UdpData.p_DataPkt->srcport; }
				}
				else
				{ return; }
			}
		}
#else
		for(i=0;i<UDP_TEL_MAX_IP_RANGE;i++)
	  	{
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r48");rx_over=0;}
#endif
#endif
#ifdef MODULE_DNS
			if(ModuleInfo.dido_info.remote.ip_demo[i]==1)
			{
				ip[0]=udp_check_ip[i]>>16;
				ip[1]=udp_check_ip[i]&0xffff;
			}
			else
			{
				ip[0]=udp_tel_data.pEEPROM_UDPTel->remote[i].ip[0];
				ip[1]=udp_tel_data.pEEPROM_UDPTel->remote[i].ip[1];
			}
		  	if(ip[0]==UdpData.p_DataPkt->srcipaddr[0])
		  	{
				if(ip[1]==UdpData.p_DataPkt->srcipaddr[1])
				{	valid=1;	break;	}
		  	}
#else
		  	if(udp_tel_data.pEEPROM_UDPTel->remote[i].ip[0]==UdpData.p_DataPkt->srcipaddr[0])
		  	{
				if(udp_tel_data.pEEPROM_UDPTel->remote[i].ip[1]==UdpData.p_DataPkt->srcipaddr[1])
				{	valid=1;	break;	}
		  	}
#endif
	  	}
		if(valid)
		{
			if(port_learn[i]!=UdpData.p_DataPkt->srcport)		//learning port
			{	port_learn[i]=UdpData.p_DataPkt->srcport;	}
		}
		else
		{	return;	}
#endif
	}
	uip_len=UdpData.RxLength;
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r49");rx_over=0;}
#endif
#endif

#ifdef DB_UDP_TEL
db_inc(uip_len,db_rx);
#endif //DB_UDP_TEL

	if(ReadEth2UARTTxBuf()==0)
	{
#ifdef UMEM_OVERFLOW_MSG_ON
	  	printf("UTM");//TX_Overflow
#endif
	  	while(ReadEth2UARTTxBuf()==0)
  	    {  	
  			WatchDogTimerReset(); 		
  	  		NET_UART_LOOP('G');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r50");rx_over=0;}
#endif
  		}
	}
	uip_len=0;		//joe debug for udp telnet will send wrong arp packet from Uart to Vcom. 20081231
}
#ifdef MODULE_FLASH512
void udp_learn_ip()
{
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r51");rx_over=0;}
#endif
#endif
#ifdef MODULE_DNS
	udp_check_ip[0]=UdpData.p_DataPkt->srcipaddr[0];
	udp_check_ip[0]=udp_check_ip[0]<<16;
	udp_check_ip[0]|=UdpData.p_DataPkt->srcipaddr[1];
#else
	ModuleInfo.UDPTelInfo.remote[0].ip[0]=UdpData.p_DataPkt->srcipaddr[0];	//learn IP
	ModuleInfo.UDPTelInfo.remote[0].ip[1]=UdpData.p_DataPkt->srcipaddr[1];
#endif
	if(udp_nor_brcst!=1)
	{
		if(port_learn[0]!=UdpData.p_DataPkt->srcport)		//learning port
		{ port_learn[0]=UdpData.p_DataPkt->srcport; }
	}
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r52");rx_over=0;}
#endif
#endif
}
#endif
void UDP_TEL_Timer(){
#ifdef MODULE_FLASH512
	if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR)||
        (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS) )
#else
	if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP)
#endif
	{
#ifdef MODULE_DNS
		if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS)
		{ udp_dns=1; }
		if(udp_dns!=1)
		{
			udp_get_dns();
			CheckPktSend();
		}
		else
#endif
		{
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r5");rx_over=0;}
#endif
#endif
			udp_tel_chk_send();
			WatchDogTimerReset();
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r6");rx_over=0;}
#endif
#endif
		}
	}
}
#endif //MODULE_UDP_TELNET//For udp start get demo name ip address
extern u32_t tcp_check_ip;
#ifdef MODULE_COMMAND_LINE
extern u8_t ATD_close;
#endif
#ifdef MODULE_DNS
u8_t udp_get_dns()
{
	u8_t result=ERROR;
	u32_t get_ip=0;

#ifdef MODULE_FLASH512
	if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR)
#else
	if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP)
#endif
	{
		while(udp_dns_count<10)
		{
			if(ModuleInfo.dido_info.remote[udp_dns_count].ip_demo==1)
			{
				if(strlen(ModuleInfo.dido_info.remote[udp_dns_count].dnsip)>1)
				{
					get_ip=DNS_Result(ModuleInfo.dido_info.remote[udp_dns_count].dnsip);
					if(get_ip==DNS_NOT_MATCH_NAME)
					{	
						if(udp_tel_data.dns_wait++>200)
						{
							udp_tel_data.dns_wait=0;
							if(udp_tel_data.dns_retry++<50)
							{
								DNS_Query(ModuleInfo.dido_info.remote[udp_dns_count].dnsip, 100); 
								break;
							}
							else
							{
								udp_tel_data.dns_retry=0;
								udp_dns_count++;
								continue;
							}
						}
					}
					else if(get_ip!=DNS_WAIT_ACK)
					{
						udp_check_ip[udp_dns_count]=get_ip;
						result=OK;
						//printf("\n\r%d ip=%lx",(u16_t)udp_dns_count,(u32_t)udp_check_ip[udp_dns_count]);
						udp_dns_count++;
						break;
					}
				}
				else
				{ udp_dns_count++; }
			}
			else
			{ udp_dns_count++; }
		}
		if(udp_dns_count==10)
		{ if(udp_dns!=1){ udp_dns--; udp_dns_count=0; };}
		udp_tel_updata_ip_aval();
	}
	else
	{
#ifdef MODULE_COMMAND_LINE
		if(ATD_close)
		{ get_ip=DNS_Result(TelnetData.pEEPROM_Telnet->CLI_RemoteIPAddr); }
		else
#endif
		{
			if(TelnetData.pEEPROM_Telnet->ip_demo)
			{ get_ip=DNS_Result(TelnetData.pEEPROM_Telnet->RemoteDNSIPAddr); }
			else
			{
				get_ip=TelnetData.pEEPROM_Telnet->RemoteIPAddr[0];
				get_ip=get_ip<<16;
				get_ip|=TelnetData.pEEPROM_Telnet->RemoteIPAddr[1];
			}
		}
		if(get_ip==DNS_NOT_MATCH_NAME)
		{	
#ifdef MODULE_COMMAND_LINE
			if(ATD_close)
			{ DNS_Query(TelnetData.pEEPROM_Telnet->CLI_RemoteIPAddr, 100); }
			else
#endif
			{ DNS_Query(TelnetData.pEEPROM_Telnet->RemoteDNSIPAddr, 100); }
		}
		else if(get_ip!=DNS_WAIT_ACK)
		{
			tcp_check_ip=get_ip;
			//printf("\n\r ip=%lx",(u32_t)tcp_check_ip);
			result=OK;
		}
	}
	return result;
}
#endif

//#define DEGUB_RX
//#define PKTDUMP
/******************************************************************************
*
*   Name:           application.c
*
*   Description:    web and TCP/IP process invoke
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
//include data
#include <module.h>
#include <stdio.h>
#include "uip.h"
#include "httpd.h"
#include "type_def.h"
#include "IP210reg.h"
#include "switch.h"
#include "timer.h"
#include "MACdriver.h"
#include "os_cbk.h"
#include "eeprom.h"
#include <string.h>
#include "option.h"
#include "wdg.h"
#include "dido.h"
#include "udp_tel.h"
#include "..\smtp\smtp.h"
#include "vtag.h"
#ifdef MODULE_SMALL_UDP
#include "..\net_uart\udp.h"
#endif
#include "CommandLine.h"
#include "net_uart.h"

#ifdef MODULE_DHCPC
#include "dhcpc.h"
extern u8_t DHCP_get_ip;
#endif

#ifdef MODULE_NET_CMD
#include "../netcmd/net_cmd.h"
#endif 

#ifdef MODULE_SMTP
#include "../smtp/smtp.h"
#endif
#ifdef MODULE_UDP_NET_CMD
	extern void udp_net_cmd(); //joe 20080702 NetCmdonUDP
#endif
#include "gpio.h"
#include "MDC_MDIO.h"

#ifdef MODULE_DNS
#include "../net_uart/dns.h"
#endif

#ifdef MODULE_SNMP_TRAP
#define MAX_SNMP_CNT 100
u16_t SNMP_cnt=MAX_SNMP_CNT;
#include "../snmp_trap/snmp_trap.h"
#endif

#ifdef MODULE_RS485
#include "gpio.h"
#endif

#ifdef MODULE_NET_UART_CTRL
#include "netuart_ctrl.h"
#endif
//------------------Server Connect----------
#ifdef MODULE_NET_UART
	u32_t conn_timer=0;
	extern u8_t conn_flag;
#else
	SRingBuffer UartRxBuf,UartTxBuf;
	u16_t max_uart_rx_buf_len;
	u16_t max_uart_tx_buf_len;
	u8_t volatile xdata* UartBuf=UARTBUF_START_ADDR;
	u8_t volatile xdata* RxBuf;
	u8_t volatile xdata* TxBuf;
#endif
#ifdef DB_FIFO
extern u8_t rx_over;
#endif
/******************************************************20081211
void db_test();
unsigned int getkey2(void);
extern void Delay(u16_t data delay);
u8_t db_flag=0;
//*************************************************************/
#ifdef MODULE_UART_AUTO_SEND
	void uart_auto_send();
	u8_t f_auto_send=0;
#endif
//public data
CONFIG_T    config_g;
static u32_t xdata start, current;

extern u8_t borcast_flag;
#ifdef MODULE_VLAN_TAG
extern u8_t udp_send_pkt;
#endif
void CheckPktSend()
{
	if(uip_len)
	{
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\rY");rx_over=0;}
#endif
#endif
#ifdef MODULE_NET_CMD
		if(	NetCmdData.CurrentPktType == TypeUCmd ) //Joe for UDP NetCmd from Different LAN
		{
			if(borcast_flag)
			{
				borcast_flag=0;
				memcpy(uip_buf_real, uip_buf_real+6, 6);
				memcpy(uip_buf_real+6, uip_ethaddr.addr, 6);
				uip_len += 14;
			}
			else
			{ uip_arp_out(); }
			NetCmdData.CurrentPktType=TypeTCmd;
		}
		else
#endif
		{ uip_arp_out(); }
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\rX");rx_over=0;}
#endif
#endif
#ifdef MODULE_VLAN_TAG
		EthernetSend(eth_vlan_buf, uip_len);
#else
		EthernetSend(&uip_buf_real,uip_len);
#endif
		uip_len=0;
	}
}

u8_t fiber_not_link_cnt=0;
void fiber_cover()
{
	if(Read_MDC(0x1f,0x1)&0x04)
	{
		fiber_not_link_cnt=0;
	}
	else
	{
		fiber_not_link_cnt++;
		if(fiber_not_link_cnt>=50)
		{
			Write_MDC(0x1f,0x0,0x8000);
			fiber_not_link_cnt=0;
		}
	}
}
void fiber_cover2()//disable phy power saving
{
	u16_t tmp10,tmp00,tmp00b;

	tmp00=Read_MDC(0x1f,0x00);
	tmp10=Read_MDC(0x1f,0x10);
	Write_MDC(0x1f,0x00,tmp00|0x1000);
	tmp00b=Read_MDC(0x1f,0x00);
	if((tmp00b&0x1000)!=0x1000)
	{//it is fiber
		Write_MDC(0x1f,0x10,tmp10&0xff7f);
	}
	Write_MDC(0x1f,0x00,tmp00);
}
//For UDP Protocol
void UDP_RECEIVE()
{
#ifdef MODULE_DNS
	DNS_UDP_RECEIVE();
#endif
#ifdef MODULE_DHCPC     
	DHCPC_UDP_RECEIVE();
#endif
#ifdef MODULE_UDP_TELNET
#ifdef MODULE_VLAN_TAG
	if(EnVlanCheck())
#endif
	{ udp_tel_rx(); }
#endif
#ifdef MODULE_NET_UART
	NET_UART_LOOP('B');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r32");rx_over=0;}
#endif
#endif
#ifdef MODULE_UDP_NET_CMD //joe 20080702 NetCmdonUDP
	udp_net_cmd();
#endif
}
//For TCP Protocol
u8_t on_tcp_rx()
{
#ifdef MODULE_NET_UART	
#ifdef MODULE_VLAN_TAG
	if(EnVlanCheck())
#endif
	{ if(NetUartOnTCPRx()){ return 1; } }
#endif

#ifdef MODULE_NET_CMD
	if(NetCmdOnTCPRx()){ return 1; }
#endif

#ifdef MODULE_SMTP
	if(SMTPOnTCPRx()){ return 1; }
#endif

#ifdef MODULE_NET_UART_CTRL
	if(nuc_on_tcp_rx()){ return 1; }
#endif

#ifdef MODULE_TCP_DIDO
	if(dido_tcp_rx_fun()){return 1;}
#endif
  return 0;
}
u32_t system_parameter=0;
void* get_system_parameter(u16_t ID)
{
	switch(ID)
	{
		case SYS_PARAM_EXPIRE_TIME: system_parameter=60000; break;
	}
	return &system_parameter;
}
/******************************************************************************
*
*  Function:    application()
*
*  Description: Network process
*               
*  Parameters:  None
*               
*  Returns:     None
*               
*******************************************************************************/
u8_t in_isr=0;
u8_t EEPROMLoadDefault(void);
void EEPROMInit(void);
void application(void)
{
	u8_t tmpvalue=0;
	u8_t i=0,l=0;
#ifdef MODULE_SMTP
	u16_t j=0;
#endif
#ifdef DEGUB_RX
    u8_t DebugRxCnt=0;
#endif
    uip_arp_init();

	restore_uip_buf();
	uip_init();	
#ifdef MODULE_MUIP
	muip_init();
#endif

#ifdef MODULE_NET_UART
#ifdef MODULE_RS485
       P4_0=0x0;
#endif

	NetUartInit();

#ifdef MODULE_DIDO
	dido_init();
#endif

#endif
#ifdef MODULE_SMALL_UDP
	udp_init(&uip_buf[UIP_LLH_LEN],&uip_len,uip_hostaddr);
#endif //MODULE_SMALL_UDP

#ifdef MODULE_CLI_CUSTOMIAZE
#else
    httpd_init();	
#endif
#ifdef MODULE_FULL_MSG
    printf("\n\rWebCodeVersion : %s",WEB_CODE_VERSION);
    printf("\n\rWebPageVersion : %s",WEB_PAGE_VERSION);
#endif    
    IP210_Update_Timer_Counter();
	current = timercounter;
	start = current;
	uip_len=0;

#ifdef MODULE_DHCPC
	DHCPCInit();
#endif

#ifdef MODULE_SMTP
    SMTP_GPIO_PIN0=0;//jc_db: need to add
    SMTP_GPIO_PIN1=0;//jc_db: need to add
	smtp_init();
#endif

#ifdef MODULE_NET_CMD
	NetCmdInit();
#endif

#ifdef MODULE_NET_UART_CTRL
	nuc_init();
#endif

#ifdef MODULE_COMMAND_LINE
	CLI_init();
#endif

	fiber_cover2();

	os_initial();

	TR0=1;

	while (1)
	{
		//db_test();
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r1");rx_over=0;}
#endif
#endif
		LoadDefault();

		os_loop();

        WatchDogTimerReset();

#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\rW");rx_over=0;}
#endif
#endif
		CheckPktSend();
#ifdef MODULE_UART_AUTO_SEND
		uart_auto_send();
#endif
	
#ifdef MODULE_VLAN_TAG
		uip_len = EthernetReceive(&eth_vlan_buf[4]);
#else		
		uip_len = EthernetReceive(&uip_buf_real);
#endif
		if (uip_len > 0)
		{
			int uip_len_tmp;

#ifdef MODULE_VLAN_TAG
			Check_Vlan_Tag();
#endif
#ifdef MODULE_NET_UART
			NET_UART_LOOP('A');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r8");rx_over=0;}
#endif
#endif
			uip_len_tmp=uip_len;

#ifdef MODULE_NET_CMD
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r9");rx_over=0;}
#endif
#endif
			LocalCmd();
			if(uip_len)
			{
#ifdef MODULE_VLAN_TAG
				EthernetSend(eth_vlan_buf, uip_len);
#else
				EthernetSend(&uip_buf_real,uip_len);
#endif
			}
#endif //MODULE_NET_CMD
			uip_len=uip_len_tmp;
			uip_arp_ipin();
			uip_len=uip_len_tmp;
			uip_arp_arpin();
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rE");rx_over=0;}
#endif
#endif
			if(uip_len)
			{
#ifdef MODULE_VLAN_TAG
				EthernetSend(eth_vlan_buf, uip_len);
#else
				EthernetSend(&uip_buf_real,uip_len);
#endif
			}
#ifdef MODULE_SMALL_UDP
			uip_len=uip_len_tmp;
			udp_process();
#ifdef MODULE_NET_UART
			NET_UART_LOOP('B');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r11");rx_over=0;}
#endif
#endif
			CheckPktSend();
#endif //MODULE_SMALL_UDP
#ifdef MODULE_MUIP
			uip_len=uip_len_tmp;
			muip_process();
#endif//MODULE_MUIP
			uip_len=uip_len_tmp;
			uip_process(UIP_DATA);
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r12");rx_over=0;}
#endif
#endif
			CheckPktSend();
#ifdef MODULE_MUIP
			muip_send();
#endif//MODULE_MUIP
#ifdef MODULE_UART_AUTO_SEND
			uart_auto_send();
#endif
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r13");rx_over=0;}
#endif
#endif
		}
		IP210_Update_Timer_Counter();
		current = timercounter;
        if(Delta_Time(start)>=IP210_TIMER_SCALE)  // 100 msec.
        {
			os_loop_100ms();

			Gateway_timer();
			if(uip_len)
			{
#ifdef MODULE_VLAN_TAG
				EthernetSend(eth_vlan_buf, uip_len);
#else
				EthernetSend(&uip_buf_real,uip_len);
#endif
				uip_len=0;
			}
#ifdef MODULE_NET_UART
			NET_UART_LOOP('A');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r15");rx_over=0;}
#endif
			NetUartTimer();
			CheckPktSend();
			uip_len=0;
#endif
#ifdef MODULE_UART_AUTO_SEND
			uart_auto_send();
#endif
#ifdef MODULE_NET_CMD
			NetCmdTimer();
#endif //MODULE_NET_CMD

#ifdef MODULE_SNMP_TRAP
			if(SNMP_cnt--<=0)
			{
				SNMP_cnt=MAX_SNMP_CNT;
				SNMP_TRAP();
				CheckPktSend();
				uip_len=0;
			}
#endif
#ifdef MODULE_SMTP
			smtp_check_mail();
			smtp_timer();
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r78");rx_over=0;}
#endif
#endif
			CheckPktSend();
			uip_len=0;
#endif //MODULE_SMTP
#ifdef MODULE_UART_AUTO_SEND
			uart_auto_send();
#endif
#ifdef MODULE_DHCPC
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r17");rx_over=0;}
#endif
#endif
			DHCPCTimer();
			if(uip_len)
			{
				uip_arp_out();
#ifdef MODULE_VLAN_TAG
				EthernetSend(eth_vlan_buf, uip_len);
#else
				EthernetSend(&uip_buf_real,uip_len);
#endif
				uip_len=0;
			}
#endif
			for (i=0; i < UIP_CONNS; ++i)
			{
#ifdef MODULE_NET_UART
				NET_UART_LOOP('D');
#ifdef DB_FIFO
				if(rx_over){printf("\n\r18");rx_over=0;}
#endif
#endif
				uip_conn = &uip_conns[i];
				uip_process(UIP_TIMER);
#ifdef MODULE_NET_UART
				NET_UART_LOOP('D');
#ifdef DB_FIFO
				if(rx_over){printf("\n\rH");rx_over=0;}
#endif
#endif
				CheckPktSend();			
#ifdef MODULE_MUIP
				muip_send();
#endif//MODULE_MUIP
#ifdef MODULE_UART_AUTO_SEND
				uart_auto_send();
#endif
#ifdef MODULE_NET_UART
				NET_UART_LOOP('E');
#ifdef DB_FIFO
				if(rx_over){printf("\n\r19");rx_over=0;}
#endif
#endif
			}
			start = timercounter;
			uip_arp_timer();
			if(uip_len)
			{
				//uip_arp_out();
#ifdef MODULE_VLAN_TAG
				EthernetSend(eth_vlan_buf, uip_len);
#else
				EthernetSend(&uip_buf_real,uip_len);
#endif
				uip_len=0;
			}
		}
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r20");rx_over=0;}
#endif
#endif
    } // end while(1)
} // end main()
/*
void db_test()
{
	//char choice;
	char cmd[2];
	int i=0;
	u16_t status=0;
	u8_t tmpvalue=0;

	for(i=0;i<1;i++)
	{
		if(RI)
		{	break;	}
	}
	//choice=SBUF;
	//if((choice==0x30)||((choice==0x31)||(choice==0x32)||(choice==0x33)||(choice==0x34)||(choice==0x35))&&(strlen(cmd)==0))
	if(strlen(cmd)==0)
	{
		cmd[0]=getkey2();
		cmd[1]=0x0;

		if(!strncmp(cmd, "0", 1)){
			memset(cmd, 0, 2);
		}
		else if(!strncmp(cmd, "1", 1))	//star
		{
			if(db_flag==0)
			{
				printf("\n\rstar");
				db_flag=1;	
				memset(cmd, 0, 2);	
			}
			else if(db_flag==1)
			{
				db_flag=0;
				memset(cmd, 0, 2);
				printf("\n\rstop");
			}
			memset(cmd, 0, 2);		
		}
		else if(!strncmp(cmd, "2", 1))	//stop
		{
			memset(cmd, 0, 2);
		}
		else if(!strncmp(cmd, "3", 1)){
			memset(cmd, 0, 2);
		}
		else if(!strncmp(cmd, "4", 1)){
			memset(cmd, 0, 2);
		}else if(!strncmp(cmd, "5", 1)){
			memset(cmd, 0, 2);
		}else if(!strncmp(cmd, "6", 1)){
			memset(cmd, 0, 2);
		}
	}
	else
	{	RI=0;	}
}

unsigned int getkey2(void)
{	
	int a;
	u8_t jo=0x0;

	if(!RI){return jo;}
	a = SBUF;
	RI = 0;
	if(a==27)
	{	
		while(!RI)
		{
			Delay(5000);
			RI=1;
		}
		a=SBUF;
		RI=0;
		if(a==27)
		;
		else
		{
			if(a==91)
			{
				while (!RI);
				a = SBUF;
				RI = 0;
			}
		}
		return a;
	}
}
*/

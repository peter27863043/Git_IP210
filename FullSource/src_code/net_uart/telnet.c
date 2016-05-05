/******************************************************************************
*
*   Name:           telnet.c
*
*   Description:    RS232 to telnet
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/

#include "module.h"
#include "muip.h"
#ifdef MODULE_NET_UART
//#define UMEM_OVERFLOW_MSG_ON
//#define UFIFO_OVERFLOW_MSG_ON
//#define UFIFO_TX_EMPTY_MSG_ON
//#define SW_UART_LOOBACK //software uart loopback
//#define DEBUG_TEMP1
//#define TCP_TX_FLOWCTRL
//----------------------------------------------------------------------------------------------------------------------
#include "public.h"
#include "telnet.h"
extern struct uip_conn *uip_conn;
//#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "timer.h"
#include "udp.h"
#include "IP210reg.h"
#include "../sys/uart.h"
#include "net_uart.h"
#include "os_cbk.h"
#ifdef MODULE_RS485
#include "gpio.h"
#endif
CTelnetData TelnetData;
//u8_t RxBuf[MAX_UART_RX_BUF_LEN];
//u8_t TxBuf[MAX_UART_TX_BUF_LEN];
u8_t volatile xdata* UartBuf=UARTBUF_START_ADDR;
u8_t volatile xdata* RxBuf;
u8_t volatile xdata* TxBuf;
u16_t max_uart_rx_buf_len;
u16_t max_uart_tx_buf_len;
u32_t char_delimiter_offset;//bit15: find or not, bit14~0:offset
u32_t tcp_check_ip=0;
#ifdef MODULE_FLASH512 
u8_t lb_flag=0;	//HW Loop Back
#endif
extern u8_t udp_get_dns();
extern u8_t muip_first_ack;
#define MAX_SILENT_DELIMITER 10
u16_t silent_delimiter[MAX_SILENT_DELIMITER];
u8_t silent_ri;
u8_t silent_wi;
u8_t silent_total;
//----------Server connect
u8_t conn_flag=0;
//u8_t heart_flag=0;
#ifdef DB_FIFO
u8_t rx_over=0;
#endif
extern u32_t conn_timer;
//------------------------------
#ifdef MODULE_VLAN_TAG
extern u8_t eth_vlan;
//extern u8_t vlan_value[4];
#endif
#ifdef MODULE_UART_AUTO_SEND
	extern void uart_auto_send();
#endif
#define SILENT_AT_FULL 0x8000
SRingBuffer UartRxBuf,UartTxBuf;
#ifdef SW_UART_LOOBACK
	int SWUartInputLen=0;
#endif
#ifdef MODULE_TELNET_RFC
	extern u8_t dtr_rts_flag;
	extern void telnet_rfc();
	extern void check_rfc2217_signal();
#endif
#endif //MODULE_NET_UART
//---------------------------------------
//extern u8_t db_flag;
//-------------joe for CLI---------------
#ifdef MODULE_COMMAND_LINE
extern u8_t ATH_flag;
extern u8_t ATD_close;
extern u8_t ATD_flag;
extern u8_t cmd_flag;
extern u8_t listen_back;
extern u8_t code_flag;
extern u8_t result;
extern void re_rorn();
extern void Delay(u16_t data delay);
u8_t *con="CONNECT\n\r";
u8_t listen_wait=0;
u8_t CLI_star_flag=1;
u8_t CLI_flag=0;
u8_t CLI_con=0;
u8_t CLI_tmp[3]={0,0,0};
u16_t CLI_local=0;
u8_t CLI_tmp_local=0;
u8_t CLI_tmp_back=0;
u8_t CLI_read_flag=1;
u32_t s_con_timer=0;
u8_t end[4]="\n\r";
void ResendUARTRx2UART();
extern u8_t tmp[];
extern u8_t *CMD;
extern void command_type();
extern void up2lwr(u8_t *buf, u8_t len);
extern u8_t force_reset_countdown;
extern u8_t echo_flag;
#endif
//---------------------------------------
extern void WatchDogTimerReset();
#ifdef MODULE_NET_UART
void telnet_connected_init();
#ifdef MODULE_REVERSE_TELNET
#define REVERSE_TELNET_CMD_TOTAL 2
#define REVERSE_TELNET_MAX_LEN 15
u8_t reverse_telnet_current;
u8_t reverse_telnet_len[REVERSE_TELNET_CMD_TOTAL]={
   15,
   14
};
u8_t reverse_telnet_data[REVERSE_TELNET_CMD_TOTAL][REVERSE_TELNET_MAX_LEN]={
   0xff,0xfb,0x01,0xff,0xfb,0x03,0xff,0xfb,0x00,0xff,0xfd,0x01,0xff,0xfd,0x00,
   0xff,0xfa,0x2c,0x05,0x09,0xff,0xf0,0xff,0xfa,0x2c,0x05,0x0c,0xff,0xf0,0x00
};
u8_t reverse_telnet_on_tcp_rx()
{
	if(uip_connected())
	{
		telnet_connected_init();
		reverse_telnet_current=0;
 	    memcpy(uip_appdata,reverse_telnet_data[reverse_telnet_current],REVERSE_TELNET_MAX_LEN);
    	uip_len=reverse_telnet_len[reverse_telnet_current];
    	uip_slen=uip_len;
	}
	
	if(reverse_telnet_current>=REVERSE_TELNET_CMD_TOTAL)
	{	return 0;	}
	
	if(uip_acked())
	{
		reverse_telnet_current++;
  		uip_len=0;
	  	uip_slen=uip_len;
	  	if(reverse_telnet_current>=REVERSE_TELNET_CMD_TOTAL)return 1;
  }
   
  //if(uip_rexmit()||uip_newdata()|uip_poll())
  if(uip_poll())
  {
  	memcpy(uip_appdata,reverse_telnet_data[reverse_telnet_current],REVERSE_TELNET_MAX_LEN);
  	uip_len=reverse_telnet_len[reverse_telnet_current];;
  	uip_slen=uip_len;
  }
  return 1;
}
#endif //MODULE_REVERSE_TELNET
#endif //MODULE_NET_UART
#ifdef MODULE_NET_UART
//-------------------------------------------Telnet Server---------------------------------------------------------
	
void ip210_isr_ctrl(u8_t status)
{
	if(status){}
#ifdef MODULE_NET_UART_INT
	u8_t value=TelnetData.UART_INT_ENABLE;
	u8_t int8004=IP210RegRead(REG_INTERRUPT_ENABLE)&0xEF;
	if(in_isr==IN_ISR_UART)return;	
	if(status)
	{
        int8004|=0x10;
	}
	if(RingAvallibleSpace(UartRxBuf)>=255)
	{
		value=TelnetData.UART_INT_ENABLE|UART_INT_RX;
	}
	else
	{
		value=TelnetData.UART_INT_ENABLE&(~UART_INT_RX);
	}
	IP210RegWrite(REG_UART_INTERRUPT_ENABLE,value);
	IP210RegWrite(REG_INTERRUPT_ENABLE,int8004);
#endif //MODULE_NET_UART_INT
}
//#else //MODULE_NET_UART_INT
//#define ip210_isr_ctrl(status)



void ip210_uart_isr_process()
{
#ifdef MODULE_NET_UART_INT
	u8_t int_status0,int_status1,int_status2;
	ip210_isr_ctrl(0);
	in_isr|=IN_ISR_UART;
  int_status0=IP210RegRead(REG_STATUS);
  int_status1=IP210RegRead(REG_UART_INTERRUPT_IDENTIFICATION);
  int_status2=int_status1&0x0f;		   
  switch(int_status2)
  {
     case 4:
             CLEAR_UART_RX_BUF('0');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r56");rx_over=0;}
#endif
 	           NetUartLoop();
		         break;     	
     case 2:
     	       break;
	}
  in_isr&=~IN_ISR_UART;
  ip210_isr_ctrl(1);
#endif //MODULE_NET_UART_INT  
}
//-------------------------------------------Telnet Server---------------------------------------------------------
/******************************************************************************
*
*  Function:    telnet_init_com
*
*  Description: Init telnet server/client common part
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void telnet_init_com()
{
  TelnetData.pEEPROM_Telnet=&ModuleInfo.TelnetInfo;
  TelnetData.pEEPROM_UART=&ModuleInfo.UARTInfo;

  TelnetData.conn_client=0;

  TelnetData.ConnectStatus=tcDisconnected;
  //TelnetData.WaitAck=0;
  TelnetData.UART_MEM_OF_B=0;
  TelnetData.UART_MEM_OF_K=0;
  TelnetData.UART_MEM_OF_M=0;
  TelnetData.UART_FIFO_Overflow=0;
  //TelnetData.idle=0;
  TelnetData.ReTxTimeout=0;
  TelnetData.LastUartRxTimer=0;
  NetUARTData.pTelnetData->uip_conn_last=0;
#ifdef MODULE_VLAN_TAG
  if(ModuleInfo.VLANINFO.vlan==1)
	  NetUARTData.pTelnetData->t_vlan_flag=1;
  else
          NetUARTData.pTelnetData->t_vlan_flag=0;
#endif
  RxBuf=TCP_RxBuf;
  TxBuf=TCP_TxBuf;
  max_uart_rx_buf_len=TCP_MAX_UART_RX_BUF_LEN;
  max_uart_tx_buf_len=TCP_MAX_UART_TX_BUF_LEN;
  RingInit(UartRxBuf,RxBuf,max_uart_rx_buf_len);
  RingInit(UartTxBuf,TxBuf,max_uart_tx_buf_len);
  TelnetData.RexmitLen=0;
  char_delimiter_offset=0;
  silent_ri=0;
  silent_wi=0;
  silent_total=0;
  UART_Settings.UART_Enable =1;//turn on UART
  UART_Settings.Rx_FIFO_threshold = 0;//issue interrupt when an amount of data received in RXFIFO 
  UART_Settings.divisor =BaudrateTable[TelnetData.pEEPROM_UART->Baudrate].Divisor;//xelect baudrate by filling in a predefined parameter
  UART_Settings.UART_IER =0;
  UART_Settings.Character_Bits =TelnetData.pEEPROM_UART->Character_Bits;
  UART_Settings.Parity_Type =TelnetData.pEEPROM_UART->Parity_Type;
  UART_Settings.Stop_Bit =TelnetData.pEEPROM_UART->Stop_Bit;
  UART_Settings.HW_Flow_Control =TelnetData.pEEPROM_UART->HW_Flow_Control;
#ifdef MODULE_RS485
	switch(TelnetData.pEEPROM_UART->OP_Mode)
	{
		case UART_INFO_OP_M_RS232:
			P4_0=1;
			P4_5=1;
			break;
		case UART_INFO_OP_M_RS422:
			P4_0=0;
			P4_5=0;
			break;
		case UART_INFO_OP_M_RS485:
			P4_0=0;
			P4_5=0;
			break;
	}
#else//only for RS232
	P4_0=1;
	P4_5=1;
#endif//#ifdef MODULE_RS485  
  UART_Set_Property();  
  NetUARTData.pkt_rx_data_offset=uip_appdata;
  NetUARTData.pkt_tx_data_offset=uip_appdata;
#ifdef SW_UART_LOOBACK
	SWUartInputLen=0;
#endif

#ifdef MODULE_NET_UART_INT
  TelnetData.UART_INT_ENABLE=UART_INT_RX;
  IP210RegWrite(REG_UART_FIFO_CONTROL,UART_RX_FIFO_LEVEL);
  ip210_uart_isr_process();//clear int status to 0xc1
  ip210_isr_ctrl(1);
  EX0=1;
#endif //MODULE_NET_UART_INT  
}

/******************************************************************************
*
*  Function:    telnetd_close
*
*  Description: close telent server
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void telnetd_close()
{
	uip_unlisten(TelnetData.pEEPROM_Telnet->TelentPort);
	if(TelnetData.ConnectStatus==tcConnected)
		TelnetData.ConnectStatus=tcWaitDisconnected;
	else 
		TelnetData.ConnectStatus=tcDisconnected;
}

/******************************************************************************
*
*  Function:    telnetd_init
*
*  Description: telnet server init
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void telnetd_init()
{  
  telnetd_listen();	
}

/******************************************************************************
*
*  Function:    telnetd_listen
*
*  Description: telnet server listen
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void telnetd_listen()
{
	uip_listen(TelnetData.pEEPROM_Telnet->TelentPort); 	
	NetUARTData.pTelnetData->ConnectStatus=tcListen;
}
void telnet_connected_init()
{
	u8_t *con1="0\n\r";
	if(conn_flag)		//Server connect
	{	uip_close();	return;	}
	else
	{
		TelnetData.uip_conn_last=uip_conn;
		TelnetData.ConnectStatus=tcConnected;
		TelnetData.WaitAck=0;
		TelnetData.idle=0;
		conn_flag=1;
#ifdef MODULE_COMMAND_LINE
		if(ATD_close)
		{
			if(code_flag){	UART_Transmit_Multi(con, strlen(con));	}
			else{	UART_Transmit_Multi(con1, strlen(con1));	}
		}
#endif
	}
}
/******************************************************************************
*
*  Function:    telnetd
*
*  Description: telnet server for uip callback function
*               
*  Parameters:  
*               
*  Returns:     
*               
******************************************************************************/
void telnetd()
{
#ifdef MODULE_COMMAND_LINE
	u8_t cont[8]="RING\n\r";
#endif
	CLEAR_UART_RX_BUF('0');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r57");rx_over=0;}
#endif
#ifdef MODULE_REVERSE_TELNET
	if(NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet == 1)
	{
		if(reverse_telnet_on_tcp_rx())return;
	}
#endif //MODULE_REVERSE_TELNET
	if(uip_connected())			//start and initialize
	{
#ifdef MODULE_COMMAND_LINE
		if(CLI_flag)
		{
			UART_Transmit_Multi(cont, 8);
			if(cmd_flag)
			{	UART_Transmit_Multi(CMD, strlen(CMD));	}
			listen_wait=1;
			IP210_Update_Timer_Counter();
			s_con_timer = timercounter;
			IP210_Update_Timer_Counter();
		}
		telnet_connected_init();
#else
		telnet_connected_init();
#endif
	}
	if(NetUARTData.pTelnetData->ConnectStatus==tcConnected)		//always use new connect
	{
		if(NetUARTData.pTelnetData->uip_conn_last!=uip_conn)
		{
			uip_close();
			return;
		}
	}
	if(uip_acked())
	{
		if(muip_first_ack==0)
		{
			if(TelnetData.WaitAck)
			{
				ip210_isr_ctrl(0);//disable
				RingIncRi(UartRxBuf,TelnetData.RexmitLen);
				ip210_isr_ctrl(1);//enable
				TelnetData.RexmitLen=0;
			}
		    TelnetData.WaitAck=0;
		}
		CLEAR_UART_RX_BUF('0');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r58");rx_over=0;}
#endif
	}
	if(uip_newdata())
	{
		CLEAR_UART_RX_BUF('0');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r59");rx_over=0;}
#endif
#ifdef MODULE_TELNET_RFC
		if(NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet == 2)
		{
			telnet_rfc();
			if(dtr_rts_flag==1)
			{	
				if(TelnetData.WaitAck==0)
				{	check_rfc2217_signal();	}
			}
		}
#endif
//-----------------------------for when in the new data clear count
		IP210_Update_Timer_Counter();
		conn_timer = timercounter;
		IP210_Update_Timer_Counter();
		conn_flag = 1;
//------------------------------------------------------------------
#ifdef DEBUG_TEMP1
		int i;
		u8_t temp=NetUARTData.pkt_rx_data_offset[0];
		for(i=1;i<uip_len;i++)
		{
			if(temp+1!=NetUARTData.pkt_rx_data_offset[i])
			{
				printf("\n\r[jc_db:%d,%d]",(u16_t)(temp+1),(u16_t)NetUARTData.pkt_rx_data_offset[i]);
				printf("[@%x]",&NetUARTData.pkt_rx_data_offset[i]);
			}
			temp++;
		}
#endif //DEBUG_TEMP1
		if(ReadEth2UARTTxBuf()==0)
		{
#ifdef UMEM_OVERFLOW_MSG_ON
			printf("UTM");//TX_Overflow
#endif
	  		while(ReadEth2UARTTxBuf()==0)
  	    	{  	
  				WatchDogTimerReset();  		
  	  			NET_UART_LOOP('F');
#ifdef DB_FIFO
				if(rx_over){printf("\n\r29");rx_over=0;}
#endif
	  	  	}
#ifdef TCP_TX_FLOWCTRL
			uip_stop();
#endif//TCP_TX_FLOWCTRL	
			if(HUART_CTS==1){	//Joe Fix when Hardware Flowcontrol on will send uip stop
				uip_stop();
			}	  
		} 
	}
	if(uip_rexmit())
	{
		NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r60");rx_over=0;}
#endif
		if(TelnetData.ReTxTimeout++>MODULE_TELNET_RE_TX_TIMEOUT)
		{
			TelnetData.ReTxTimeout=0;
			if(TelnetData.pEEPROM_Telnet->op_mode==NU_OP_MODE_CLIENT)
			{
				conn_flag = 0;
				uip_close();
			}
		}
		ReSendUARTRx2Eth();
	}
  //printf(",%x(%x)",(u16_t)TelnetData.ReTxTimeout,(u16_t)uip_flags);//jc_db:n2r
	if(uip_newdata() || uip_acked())
	{
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r61");rx_over=0;}
#endif
		WriteEthRx2UART();
//-----------------------------for when in the new data clear count
		IP210_Update_Timer_Counter();
		conn_timer = timercounter;
		IP210_Update_Timer_Counter();
		conn_flag = 1;
//------------------------------------------------------------------
#ifdef MODULE_TELNET_RFC
		if(NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet == 2)
		{
			telnet_rfc();
			if(dtr_rts_flag==1)
			{
				if(TelnetData.WaitAck==0)
				{	check_rfc2217_signal();	}
			}
		}
#endif
  		TelnetData.ReTxTimeout=0;
		if(TelnetData.WaitAck==0&&UartRxBuf.TotalLen!=0)
		{
			SendUARTRx2Eth(0);
#ifdef MODULE_COMMAND_LINE
			if(CLI_flag==0)		
#endif
			{
				if(TelnetData.RexmitLen-TelnetData.Delimiter_drop_len!=0)
				{
					ReSendUARTRx2Eth();
					TelnetData.WaitAck=1;
				}
			}
     	} 
     	TelnetData.idle=0;
	} 
	else if(uip_poll())
	{
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r62");rx_over=0;}
#endif
		WriteEthRx2UART();
#ifdef MODULE_COMMAND_LINE
		if(listen_back)
		{
			if(listen_wait==0)
			{
				listen_back=0;
				if(CLI_flag)
				{
					TelnetData.ConnectStatus=tcDisconnected;
					TelnetData.WaitAck=0;	uip_close();	conn_flag = 0;
					return;
				}					
			}
		}
		if(ATH_flag)
		{	TelnetData.WaitAck=0;	uip_close();	conn_flag = 0;	ATH_flag=0;		return;	}
#endif
#ifdef MODULE_TELNET_RFC
		if(NetUARTData.pTelnetData->pEEPROM_Telnet->enable_reverse_telnet == 2)
		{
			if(dtr_rts_flag==1)
			{
				if(TelnetData.WaitAck==0)
				{	check_rfc2217_signal();	}
			}
		}
#endif
		if(TelnetData.WaitAck==0&&UartRxBuf.TotalLen!=0)
		{
			SendUARTRx2Eth(0);
#ifdef MODULE_COMMAND_LINE
			if(CLI_flag==0)		
#endif
			{
				if(TelnetData.RexmitLen-TelnetData.Delimiter_drop_len!=0)
				{
					ReSendUARTRx2Eth();
					TelnetData.WaitAck=1;
				}
			}
		}
		if(TelnetData.pEEPROM_Telnet->ClientTimeout!=0)
		{
			if(TelnetData.idle++>=TelnetData.pEEPROM_Telnet->ClientTimeout*600)
			{
				TelnetData.idle=0;
				if(TelnetData.pEEPROM_Telnet->op_mode==NU_OP_MODE_CLIENT)
				{
					conn_flag = 0;
					uip_close();
				}
			}
		}
	}
	if(uip_timedout())
	{
		conn_flag = 0;
		RingInit(UartRxBuf,RxBuf,max_uart_rx_buf_len);
		RingInit(UartTxBuf,TxBuf,max_uart_tx_buf_len);
		uip_close();
	}
	if(uip_closed()||uip_aborted())
	{
		if(NetUARTData.pTelnetData->uip_conn_last==uip_conn)
		{
			conn_flag = 0;
			TelnetData.ConnectStatus=tcDisconnected;
			TelnetData.WaitAck=0;
			TelnetData.conn_client=0;
			ip210_isr_ctrl(0);
//			RingInit(UartRxBuf,RxBuf,max_uart_rx_buf_len);
//			RingInit(UartTxBuf,TxBuf,max_uart_tx_buf_len);
			ip210_isr_ctrl(1);
		}else if(uip_conn==TelnetData.uip_conn_client){
			TelnetData.conn_client=0;
		}
	}
	if(TelnetData.ConnectStatus==tcWaitDisconnected)
	{
		conn_flag = 0;
		uip_close();
	}
}
//-------------------------------------------Telnet Client---------------------------------------------------------

/******************************************************************************
*
*  Function:    telnet_init
*
*  Description: telnet client init
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void telnet_init()
{
	TelnetData.pEEPROM_Telnet->op_mode=NU_OP_MODE_CLIENT;
}

/******************************************************************************
*
*  Function:    telnet_close
*
*  Description: telnet client close
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void telnet_close()
{
	if(TelnetData.ConnectStatus==tcConnected)
		TelnetData.ConnectStatus=tcWaitDisconnected;
	else 
		TelnetData.ConnectStatus=tcDisconnected;
}

/******************************************************************************
*
*  Function:    telnet_connect
*
*  Description: telnet client connect to remote telnet server
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
extern u8_t udp_send_pkt;
u8_t telnet_connect()
{
	u8_t result=ERROR;
#ifdef MODULE_DNS
	u16_t ip[2]=0;

	if(tcp_check_ip==0){
		result=udp_get_dns();
	}else
		result=OK;

	if(result==ERROR)
		return result;

	ip[0]=tcp_check_ip>>16;
	ip[1]=tcp_check_ip&0xFFFF;

#ifdef MODULE_COMMAND_LINE
	if(tcp_check_ip!=0)
	{
#ifdef MODULE_VLAN_TAG
		if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_CLIENT)
		{
			if(ModuleInfo.VLANINFO.vlan)
			{
				eth_vlan_buf[0]=0x81;
				eth_vlan_buf[1]=0;
				eth_vlan_buf[2]=ModuleInfo.VLANINFO.value_id[0];
				eth_vlan_buf[3]=ModuleInfo.VLANINFO.value_id[1];
				udp_send_pkt=1;
			}
		}
#endif
		if(ATD_close)
		{	TelnetData.uip_conn_client=uip_connect(ip, TelnetData.pEEPROM_Telnet->CLI_RemotePort);	}
		else
		{	TelnetData.uip_conn_client=uip_connect(ip, TelnetData.pEEPROM_Telnet->TelentPort);	}
	}
#else
	if(tcp_check_ip!=0)
	{
#ifdef MODULE_VLAN_TAG
		if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_CLIENT)
		{
			if(ModuleInfo.VLANINFO.vlan)
			{
				eth_vlan_buf[0]=0x81;
				eth_vlan_buf[1]=0;
				eth_vlan_buf[2]=ModuleInfo.VLANINFO.value_id[0];
				eth_vlan_buf[3]=ModuleInfo.VLANINFO.value_id[1];
				udp_send_pkt=1;
			}
		}
#endif
		TelnetData.uip_conn_client=uip_connect(ip, TelnetData.pEEPROM_Telnet->TelentPort);
	}
#endif
#else
#ifdef MODULE_VLAN_TAG
	if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_CLIENT)
	{
		if(ModuleInfo.VLANINFO.vlan)
		{
			eth_vlan_buf[0]=0x81;
			eth_vlan_buf[1]=0;
			eth_vlan_buf[2]=ModuleInfo.VLANINFO.value_id[0];
			eth_vlan_buf[3]=ModuleInfo.VLANINFO.value_id[1];
			udp_send_pkt=1;
		}
	}
#endif
	TelnetData.uip_conn_client=uip_connect(TelnetData.pEEPROM_Telnet->RemoteIPAddr,TelnetData.pEEPROM_Telnet->TelentPort);
#endif
	return OK;
}

/******************************************************************************
*
*  Function:    telnet
*
*  Description: telent client for uip callback function
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void telnet()
{
	telnetd();
}

//----------------------------------------UART---------------------------------------------------------------------

/******************************************************************************
*
*  Function:    ClearUARTRxBuf
*
*  Description: Read from UART Rx FIFO to memory ring buffer
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
//#define DB_UART_FIFO_OVERFLOW
#ifdef DB_UART_FIFO_OVERFLOW
char db_ufo[2];
#endif //DB_UART_FIFO_OVERFLOW
void ClearUARTRxBuf()
{
#ifdef MODULE_COMMAND_LINE
	u8_t i;
	u8_t *con1="1\n\r";
#endif
	u16_t UART_newdata_len;
	ip210_isr_ctrl(0);//disable
	IP210_Update_Timer_Counter();

	if(UartRxBuf.TotalLen)
	{
		if(TelnetData.pEEPROM_UART->delimiter_en&UART_INFO_DILIMITER_EN_SILENT)
		{
			if(Delta_Time(TelnetData.LastUartRxTimer)>TelnetData.pEEPROM_UART->delimiter[2]*20)
			{
				if(TelnetData.WaitAck==0)
				{
					if(silent_total+1<MAX_SILENT_DELIMITER)
					{
						u16_t last_wi;
						last_wi=silent_wi?silent_wi-1:MAX_SILENT_DELIMITER-1;
						TelnetData.LastUartRxTimer=timercounter;
						//if((silent_delimiter[last_wi]!=UartRxBuf.wi)||(silent_total==0))
						if(silent_delimiter[last_wi]!=UartRxBuf.wi)
						{
							if(RingAvallibleSpace(UartRxBuf)!=0)
							{
								silent_delimiter[silent_wi]=UartRxBuf.wi;							
							}
							else
							{
								silent_delimiter[silent_wi]=UartRxBuf.wi|SILENT_AT_FULL;
							}						
							silent_wi++;
							silent_total++;					
							if(silent_wi>=MAX_SILENT_DELIMITER)silent_wi=0;
						}
					}
				}
			}
		}
	}//if(UartRxBuf.TotalLen+UART_newdata_len)	
	
	if(UART_Settings.Parity_Type)
	{
		ClearUARTRxBuf_ParityError();
		ip210_isr_ctrl(1);//enable
		return;
	}
#ifdef MODULE_FLASH512 
	if(lb_flag==0)
#endif
	{ UART_newdata_len=UART_Rx_FIFO_Status(); } //check Rx FIFO data size
#ifdef DB_UART_FIFO_OVERFLOW
	printf("%c",db_ufo[0]);
	if(UART_newdata_len==255)printf("#");
#endif //DB_UART_FIFO_OVERFLOW
//#ifdef UFIFO_OVERFLOW_MSG_ON
#ifdef DB_FIFO
	if(UART_newdata_len==255){/*printf(" URF");*/ rx_over=1;}
#endif
//#endif //UFIFO_OVERFLOW_MSG_ON

#ifdef SW_UART_LOOBACK
	UART_newdata_len=SWUartInputLen;
#endif
	//Hardware flow control
	if(UART_Settings.HW_Flow_Control)
	{
//printf("the UartRxBuf index is %u\n\r", (int)RingAvallibleSpace(UartRxBuf)); //debug
		if(RingAvallibleSpace(UartRxBuf)<=1024)
		{
			//UART RX flow control= stop to receive
			UART_Set_RTS();
		}
		else
		{
			//UART RX flow control= start to receive
			UART_Clear_RTS();
		}
	}
//*****************************************************************************************
#ifdef MODULE_COMMAND_LINE
	#ifndef MODULE_CLI_CUSTOMIAZE
	if(NetUARTData.pTelnetData->pEEPROM_Telnet->cli_en)
	#endif
	{
		if(CLI_star_flag==1)
		{
#ifdef MODULE_FLASH512
			if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS)||
				(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR) )
#else
			if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP)
#endif
			{
				CLI_star_flag=0;		
				UART_Transmit_Multi(end, 4);
				if(code_flag){	UART_Transmit_Multi(con, strlen(con));	}
				else{	UART_Transmit_Multi(con1, strlen(con1));	}
				CLI_flag=0; CLI_con=0;			
			}
			else
			{
				TelnetData.WaitAck=0;
				CLI_flag=1;  CLI_con=0;
				CLI_star_flag=0;
				RingInit(UartRxBuf,RxBuf,max_uart_rx_buf_len);
				memset(UartRxBuf.buf, 0, max_uart_rx_buf_len);
				if(cmd_flag)
				{
					UART_Transmit_Multi(end, 4);
					UART_Transmit_Multi(CMD, strlen(CMD));	
				}
				UART_newdata_len=0;	
				RingIncWi(UartRxBuf,UART_newdata_len);						
			}
		}
	}
#endif
//*****************************************************************************************	
//	if((UART_newdata_len>0) && (RingAvallibleSpace(UartRxBuf) > 1024)) // modified by james	
	if(UART_newdata_len>0) // modified by jesse
	{
		if(RingAvallibleSpace(UartRxBuf)>=UART_newdata_len)
		{
			if(UART_newdata_len==255)TelnetData.UART_FIFO_Overflow++;
			TelnetData.LastUartRxTimer=timercounter;
			if(RingWBDistance(UartRxBuf)>=UART_newdata_len)
			{
				UART_Receive_Multi(&UartRxBuf.buf[UartRxBuf.wi],UART_newdata_len);
#ifdef SW_UART_LOOBACK
				{
					int i=0;
					for(i=0;i<UART_newdata_len;i++)
					{
						UartRxBuf.buf[UartRxBuf.wi+i]=NetUARTData.pkt_rx_data_offset[i];
					}
				}
#endif //SW_UART_LOOBACK
//----------------------------------joe for CLI-----------------------------------
#ifdef MODULE_COMMAND_LINE
#ifndef MODULE_CLI_CUSTOMIAZE
				if( (CLI_flag==0)&&(NetUARTData.pTelnetData->pEEPROM_Telnet->cli_en==1) )
#else
				if(CLI_flag==0)
#endif
				{		
					for(i=0; i<UART_newdata_len; i++)
					{
						if(CLI_con==0)
						{
							if(UartRxBuf.buf[UartRxBuf.wi+i] == 0x2b)
							{
								if( ((UART_newdata_len-i)==1)||((UART_newdata_len-i)==2) )
								{
									memcpy(CLI_tmp, &UartRxBuf.buf[UartRxBuf.wi+i], (UART_newdata_len-i));
									CLI_con += UART_newdata_len-i;
									break;
								}
								else
								{
									memcpy(CLI_tmp, &UartRxBuf.buf[UartRxBuf.wi+i], 3);
									CLI_con=3;
								}
							}
						}
						else
						{
							if( (UART_newdata_len-i)==1 )
							{
								memcpy(CLI_tmp+CLI_con, &UartRxBuf.buf[UartRxBuf.wi], (UART_newdata_len-i));
								CLI_con += UART_newdata_len-i;
							}
							else
							{
								memcpy(CLI_tmp+CLI_con, &UartRxBuf.buf[UartRxBuf.wi], (3-CLI_con));
								CLI_con=3;
							}
						}
						if(CLI_con==3)
						{
							if(!strncmp(CLI_tmp, "+++", 3))
							{
								memset(CLI_tmp, 0, 3);
								TelnetData.WaitAck=0;
								CLI_flag=1;  CLI_con=0;
								RingInit(UartRxBuf,RxBuf,max_uart_rx_buf_len);
								RingInit(UartTxBuf,TxBuf,max_uart_tx_buf_len);
								result=0;
								if(cmd_flag)
								{
									re_rorn();										
									UART_Transmit_Multi(CMD, strlen(CMD));
								}
								else
								{	re_rorn();	}
								UART_newdata_len=0;
								break;
							}
							else
							{	CLI_con=0;	}
						}
					}
				}
				RingIncWi(UartRxBuf,UART_newdata_len);
				if(CLI_flag)
				{	ResendUARTRx2UART();	CLI_local=UartRxBuf.wi;}
#else
//--------------------------------------------------------------------------------
				RingIncWi(UartRxBuf,UART_newdata_len);	//change RingWi local
#endif
			}
			else
			{
				int WB=RingWBDistance(UartRxBuf);
				UART_Receive_Multi(&UartRxBuf.buf[UartRxBuf.wi],WB);
//----------------------------------joe for CLI-----------------------------------
#ifdef MODULE_COMMAND_LINE

#endif
//--------------------------------------------------------------------------------
#ifdef SW_UART_LOOBACK
				{
					int i=0;
					for(i=0;i<WB;i++)
					{
						UartRxBuf.buf[UartRxBuf.wi+i]=NetUARTData.pkt_rx_data_offset[i];
					}
				}
#endif //SW_UART_LOOBACK
				UartRxBuf.wi=0;
				UART_Receive_Multi(&UartRxBuf.buf[UartRxBuf.wi],UART_newdata_len-WB);
#ifdef SW_UART_LOOBACK
				{
					int i=0;
					for(i=0;i<UART_newdata_len-WB;i++)
					{
						UartRxBuf.buf[UartRxBuf.wi+i]=NetUARTData.pkt_rx_data_offset[WB+i];
					}
				}
#endif //SW_UART_LOOBACK
				UartRxBuf.wi+=UART_newdata_len-WB;
				UartRxBuf.TotalLen+=UART_newdata_len;
			}
		}
		else
		{
			if(!(UART_Settings.HW_Flow_Control)){ // added by james
				TelnetData.UART_MEM_OF_B+=UART_newdata_len;
				if(TelnetData.UART_MEM_OF_B>=1024)
				{
					TelnetData.UART_MEM_OF_B-=1024;
					TelnetData.UART_MEM_OF_K++;
				}
				if(TelnetData.UART_MEM_OF_K>=1024)
				{
					TelnetData.UART_MEM_OF_K-=1024;
					TelnetData.UART_MEM_OF_M++;
				}
#ifdef UMEM_OVERFLOW_MSG_ON
				printf("URM");//RX_OverFlow
#endif
			}
		}
	}
	ip210_isr_ctrl(1);//enable
}

/******************************************************************************
*
*  Function:    SendUARTRx2Eth
*
*  Description: Send UART Rx memory buffer to ethernet
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void SendUARTRx2Eth(u8_t protocol)
{	
	u16_t mss=uip_mss();
	u16_t i,current_pos,current_pos2,len;
	s32_t wrap=0;	
	TelnetData.Delimiter_drop_len=0;
#ifdef MODULE_MUIP
  	mss=muip_mss();
#endif//MODULE_MUIP
#ifdef MODULE_COMMAND_LINE
	if(CLI_flag)return;
#endif
	if(protocol==1)mss=udp_mss();
	if(TelnetData.pEEPROM_UART->delimiter_en)
	{

		for(i=0;i<UartRxBuf.TotalLen;i++)
		{
			if(i%100==0)
			{
					NET_UART_LOOP('C');
#ifdef DB_FIFO
					if(rx_over){printf("\n\r63");rx_over=0;}
#endif
			}
			current_pos=UartRxBuf.ri+i+wrap;
			if(current_pos>=UartRxBuf.MaxLen)
			{
				wrap=0-UartRxBuf.ri-i;
				current_pos=0;
			}
			if(TelnetData.pEEPROM_UART->delimiter_en&UART_INFO_DILIMITER_EN_SILENT)
			{
				if(silent_delimiter[silent_ri]&SILENT_AT_FULL)
				{
					if(RingAvallibleSpace(UartRxBuf)==0)
					{
						silent_delimiter[silent_ri]=silent_delimiter[silent_ri]&0x7fff;
						i=mss-1;
						break;
					}
				}
				current_pos2=current_pos+1;
				if(current_pos2>=UartRxBuf.MaxLen)
				{
					current_pos2=0;
				}
				if(silent_total&&(current_pos2==silent_delimiter[silent_ri]))
				{
					char_delimiter_offset=(current_pos2)|FOUND_SILENT_DELIMITER;
					if(i<=mss)
					{
						silent_ri++;
						silent_total--;
						if(silent_ri>=MAX_SILENT_DELIMITER)silent_ri=0;					
					}
					break;
				}
			}
			if(TelnetData.pEEPROM_UART->delimiter_en&UART_INFO_DILIMITER_EN_CHAR1)
			{
				if(UartRxBuf.buf[current_pos]==TelnetData.pEEPROM_UART->delimiter[0])
				{
					char_delimiter_offset=current_pos|FOUND_CHAR_DELIMITER;
					break;
				}
			}
			if(TelnetData.pEEPROM_UART->delimiter_en&UART_INFO_DILIMITER_EN_CHAR2)			
			{
				if(UartRxBuf.buf[current_pos]==TelnetData.pEEPROM_UART->delimiter[1])
				{
					char_delimiter_offset=current_pos|FOUND_CHAR_DELIMITER;
					break;
				}				
			}
		}//for(i=0;i<UartRxBuf.TotalLen;i++)
		if(i==UartRxBuf.TotalLen)
		{
			char_delimiter_offset=current_pos|FOUND_NO_DELIMITER;
			TelnetData.RexmitLen=0;
		}//if(i==UartRxBuf.TotalLen)
		else
		{
			len=i+1;
			TelnetData.RexmitLen=(len<=mss)?len:mss;
		}//if(i==UartRxBuf.TotalLen)
		
	}//if(TelnetData.pEEPROM_UART->delimiter_en)
	else
	{
 		TelnetData.RexmitLen=(UartRxBuf.TotalLen<=mss)?UartRxBuf.TotalLen:mss;
 	}//if(TelnetData.pEEPROM_UART->delimiter_en).else
 	
 	if((char_delimiter_offset&FOUND_CHAR_DELIMITER)&&TelnetData.pEEPROM_UART->delimiter_drop&&TelnetData.RexmitLen)
	{
		TelnetData.Delimiter_drop_len=1;
	}
	if(TelnetData.Delimiter_drop_len==1)
	{
		if(TelnetData.RexmitLen-TelnetData.Delimiter_drop_len==0)	//joe debug when drop len=1 and enable character in first word 20090107
		{	RingIncRi(UartRxBuf, 1);	}
	}
}

/******************************************************************************
*
*  Function:    ReSendUARTRx2Eth
*
*  Description: Retransmit UART Rx memory buffer to ethernet
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void ReSendUARTRx2Eth()
{
	u16_t mss=uip_mss();
	u16_t len=0;

#ifdef MODULE_COMMAND_LINE
	if(CLI_flag){return;}
#endif
#ifdef MODULE_FLASH512
	if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR)||
		(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS) )
	{ mss=udp_mss(); }

	if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP_NOR)&&
		(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP_LIS) )
#else
	if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP)mss=udp_mss();

	if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP)
#endif
	{ if(TelnetData.RexmitLen>1){ mss=(TelnetData.RexmitLen/2)+(TelnetData.RexmitLen%2); } }

	len=(TelnetData.RexmitLen<=mss)?TelnetData.RexmitLen:mss;
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r30");rx_over=0;}
#endif
#endif
	if(RingRBDistance(UartRxBuf)>len)
	{
		memcpy(NetUARTData.pkt_tx_data_offset,&UartRxBuf.buf[UartRxBuf.ri],len);
	}
	else
	{
		memcpy(NetUARTData.pkt_tx_data_offset,&UartRxBuf.buf[UartRxBuf.ri],RingRBDistance(UartRxBuf));
		memcpy(NetUARTData.pkt_tx_data_offset+RingRBDistance(UartRxBuf),&UartRxBuf.buf[0],len-RingRBDistance(UartRxBuf));
	}
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r31");rx_over=0;}
#endif
#endif
	if(TelnetData.RexmitLen<=mss)
	{
		uip_len=TelnetData.RexmitLen-TelnetData.Delimiter_drop_len;
	}
	else
	{
		uip_len=len;
	}
	uip_slen=uip_len;
#ifdef MODULE_FLASH512
	if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_LIS)||
		(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP_NOR) )
#else	
	if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_UDP)
#endif
	{	
		return; //muip not support for UDP
	}
#ifdef MODULE_MUIP	
	muip_need_send=0;
	CLEAR_UART_RX_BUF('0');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r64");rx_over=0;}
#endif
	ip210_isr_ctrl(0);//disable	
	if(TelnetData.RexmitLen>mss)//muip mode
	{
		u8_t i;
		u16_t remain_len=TelnetData.RexmitLen-len;
		u16_t old_ri=UartRxBuf.ri;		
	
		muip_need_send=1;

		muip_buf_cnt=remain_len/mss;

		if(remain_len%mss)muip_buf_cnt+=1;

  		muip_conn=uip_conn;

		for(i=0;i<muip_buf_cnt;i++)
		{

			CLEAR_UART_RX_BUF('0');
#ifdef DB_FIFO
			if(rx_over){printf("\n\r65");rx_over=0;}
#endif
			UartRxBuf.ri+=len;
			RingCheckRi(UartRxBuf);
			  
			len=(remain_len<=mss)?remain_len:mss;
			  
		  	if(i+1==muip_buf_cnt)
			{ muip_len[i]=len-TelnetData.Delimiter_drop_len; }
		  	else
			{ muip_len[i]=len; }

			remain_len-=len;
		
			if(RingRBDistance(UartRxBuf)>len)
			{
#ifdef MODULE_VLAN_TAG
				memcpy(muip_buf[i]+MUIP_APPDATA+4,&UartRxBuf.buf[UartRxBuf.ri],len);
#else
				memcpy(muip_buf[i]+MUIP_APPDATA,&UartRxBuf.buf[UartRxBuf.ri],len);
#endif
			}
			else
			{
#ifdef MODULE_VLAN_TAG
				memcpy(muip_buf[i]+MUIP_APPDATA+4,&UartRxBuf.buf[UartRxBuf.ri],RingRBDistance(UartRxBuf));
				memcpy(muip_buf[i]+MUIP_APPDATA+4+RingRBDistance(UartRxBuf),&UartRxBuf.buf[0],len-RingRBDistance(UartRxBuf));
#else
				memcpy(muip_buf[i]+MUIP_APPDATA,&UartRxBuf.buf[UartRxBuf.ri],RingRBDistance(UartRxBuf));
				memcpy(muip_buf[i]+MUIP_APPDATA+RingRBDistance(UartRxBuf),&UartRxBuf.buf[0],len-RingRBDistance(UartRxBuf));
#endif
			}
		}
		UartRxBuf.ri=old_ri;//restore ri
		if( (len-TelnetData.Delimiter_drop_len)==0 )	//Joe Debug for Delimit Drop Len 20090722
		{
			muip_need_send=0;
			muip_buf_cnt=0;
		}
//		UartRxBuf.TotalLen=old_total_len;//restore total len
	}
	ip210_isr_ctrl(1);//enable
  	CLEAR_UART_RX_BUF('0');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r66");rx_over=0;}
#endif
#endif //MODULE_MUIP
}

/******************************************************************************
*
*  Function:    WriteEthRx2UART
*
*  Description: Write data form UART Tx memory ring buffer to UART.
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void WriteEthRx2UART()
{
	int len,tx_fifo_space;
#ifdef MODULE_COMMAND_LINE
	if(CLI_flag){return;}
#endif
//	ip210_isr_ctrl(0);//disable
	if(RingAvallibleSpace(UartTxBuf)>=UartTxBufSafeSpace)
	{
		if(uip_stopped(uip_conn))
	  	{
			uip_restart();
		}
	}
	if(UartTxBuf.TotalLen>0)
	{
#ifdef MODULE_RS485
		if(TelnetData.pEEPROM_UART->OP_Mode==UART_INFO_OP_M_RS485)
		{
			P4_0=0x1;
		}
#endif
		tx_fifo_space=UART_Tx_FIFO_Status();
#ifdef UFIFO_TX_EMPTY_MSG_ON
		if((tx_fifo_space>=250)&&(UartTxBuf.TotalLen>=1024))printf("UTE");
#endif //UFIFO_TX_EMPTY_MSG_ON
		if(tx_fifo_space>0)
		{
			if(UartTxBuf.TotalLen>=tx_fifo_space)
			{
				len=tx_fifo_space;
			}
			else
			{
				len=UartTxBuf.TotalLen;
			}
		
			if(RingRBDistance(UartTxBuf)>=len)
			{
				UART_Transmit_Multi(&UartTxBuf.buf[UartTxBuf.ri],len);
				RingIncRi(UartTxBuf,len);
			}
			else
			{	
				int RB=RingRBDistance(UartTxBuf);
				UART_Transmit_Multi(&UartTxBuf.buf[UartTxBuf.ri],RB);
				RingIncRi(UartTxBuf,RB);
				UART_Transmit_Multi(&UartTxBuf.buf[UartTxBuf.ri],len-RB);
				RingIncRi(UartTxBuf,len-RB);
			}		
		}
   }
   TelnetData.UART_INT_ENABLE=UART_INT_RX;
   if(UartTxBuf.TotalLen>0)
   {
   		TelnetData.UART_INT_ENABLE|=UART_INT_TX;
   }
//	ip210_isr_ctrl(1);//enable
}


/******************************************************************************
*
*  Function:    ResendUARTRx2UART
*
*  Description: Write data form UART Rx memory ring buffer to UART for CLI
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
#ifdef MODULE_COMMAND_LINE
void ResendUARTRx2UART()
{
	u8_t i;
	u8_t *back="\b \b";

	if(UartRxBuf.wi>CLI_local)
	{
		for(i=0;i<UartRxBuf.wi-CLI_local;i++)
		{
			if(UartRxBuf.buf[CLI_local+i]==0xd)
			{
				tmp[CLI_tmp_local]='\0';
				command_type();	
				memset(tmp, 0, 50);
				CLI_tmp_back=0;
				CLI_tmp_local=0;
				if(CLI_flag==0){	CLI_local=0;	}
				else if(ATD_flag==1)
				{
					ip210_isr_ctrl(0);
					RingInit(UartRxBuf,RxBuf,max_uart_rx_buf_len);
					RingInit(UartTxBuf,TxBuf,max_uart_tx_buf_len);
					ip210_isr_ctrl(1);
					CLI_flag=0;
					CLI_local=0;	
					break;
				}
				else
				{
					if(cmd_flag)
					{	UART_Transmit_Multi(CMD, strlen(CMD));	}
				}
			}
			else if(UartRxBuf.buf[CLI_local+i]==0xa)
			{ continue;	}
			else if(UartRxBuf.buf[CLI_local+i]==0x8)
			{
				if(CLI_tmp_back>0)
				{	UART_Transmit_Multi(back, strlen(back));	CLI_tmp_back--;	CLI_tmp_local--;	}	
			}
			else
			{
				CLI_tmp_back++;
				memcpy(tmp+CLI_tmp_local, &UartRxBuf.buf[CLI_local+i],1);
//				*(tmp+CLI_tmp_local)=up2lwr(*(tmp+CLI_tmp_local));
				if(CLI_tmp_local<50)
				{	CLI_tmp_local++;	}
				if(echo_flag)
				{	UART_Transmit_Multi(&UartRxBuf.buf[CLI_local+i],1);		}
			}
		}
		if(CLI_flag==0)return;
		RingIncWi(UartRxBuf,(UartRxBuf.wi-CLI_local));
	}
}
#endif
/******************************************************************************
*
*  Function:    ReadEth2UARTTxBuf
*
*  Description: Read data form ethernet to UART Tx memory ring buffer
*               
*  Parameters:  
*               
*  Returns:     1: successfully read to buffer  0: buffer overflow
*               
*******************************************************************************/
extern unsigned char IP210_DMA(unsigned short src_addr, unsigned short des_addr, unsigned short length, unsigned char cmd);
int ReadEth2UARTTxBuf()
{
#ifdef MODULE_NET_CMD_EEP_CFG	
	if(TelnetData.pEEPROM_Telnet->op_mode==NU_OP_MODE_DISABLE)return 1;
#endif//MODULE_NET_CMD_EEP_CFG	
#ifdef MODULE_COMMAND_LINE
	if(CLI_flag)return 1;
#endif
#ifdef SW_UART_LOOBACK
	SWUartInputLen=uip_len;
	if(RingAvallibleSpace(UartRxBuf)<=SWUartInputLen)return 0;
	CLEAR_UART_RX_BUF('1');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r68");rx_over=0;}
#endif
	SWUartInputLen=0;
	return 1;
#endif
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r69");rx_over=0;}
#endif
#ifdef MODULE_FLASH512 
	if(lb_flag)
	{
		if(RingAvallibleSpace(UartRxBuf)>=uip_len)
		{
			if(RingWBDistance(UartRxBuf)>=uip_len)
			{
				IP210_DMA((u16_t)NetUARTData.pkt_rx_data_offset, (u16_t)(&UartRxBuf.buf[UartRxBuf.wi]), uip_len, BIT_CMD_MODE_M2M);
				RingIncWi(UartRxBuf,uip_len);
			}
			else
			{
				int WB=RingWBDistance(UartRxBuf);
				IP210_DMA((u16_t)NetUARTData.pkt_rx_data_offset, (u16_t)&(UartRxBuf.buf[UartRxBuf.wi]), WB, BIT_CMD_MODE_M2M);
				UartRxBuf.wi=0;
				IP210_DMA((u16_t)(NetUARTData.pkt_rx_data_offset+WB), (u16_t)&(UartRxBuf.buf[UartRxBuf.wi]), uip_len-WB, BIT_CMD_MODE_M2M);
				UartRxBuf.wi+=uip_len-WB;
				UartRxBuf.TotalLen+=uip_len;
			}
		}
		else
		{ return 0; }
	}
	else
#endif
	{
		if(RingAvallibleSpace(UartTxBuf)>=uip_len)
		{
			if(RingWBDistance(UartTxBuf)>=uip_len)
			{
				IP210_DMA((u16_t)NetUARTData.pkt_rx_data_offset, (u16_t)(&UartTxBuf.buf[UartTxBuf.wi]), uip_len, BIT_CMD_MODE_M2M);
				RingIncWi(UartTxBuf,uip_len);
			}
			else
			{
				int WB=RingWBDistance(UartTxBuf);
				IP210_DMA((u16_t)NetUARTData.pkt_rx_data_offset, (u16_t)&(UartTxBuf.buf[UartTxBuf.wi]), WB, BIT_CMD_MODE_M2M);
				UartTxBuf.wi=0;
				IP210_DMA((u16_t)(NetUARTData.pkt_rx_data_offset+WB), (u16_t)&(UartTxBuf.buf[UartTxBuf.wi]), uip_len-WB, BIT_CMD_MODE_M2M);
				UartTxBuf.wi+=uip_len-WB;
				UartTxBuf.TotalLen+=uip_len;
			}
		}
		else	
		{
			return 0;//UART_TX_OVERFLOW
		}
	}
	return 1;//Success
}

void ClearUARTRxBuf_ParityError(void)
{
  u8_t i, rx_status, reg_data, Rx_data;
  u16_t ring_space, write_boundary;

//    ET0_count++;
    //check whether there's any char in UART Rx FIFO
	rx_status = IP210RegRead(REG_UART_RX_FIFO_STATUS);

    if(rx_status > 0)       
	{
//printf("\n rx_status = [%d] ", (u16_t)rx_status);
        ring_space = RingAvallibleSpace(UartRxBuf);
//printf("\n ring_space = [%d] ", (u16_t)ring_space);
        if(ring_space >= rx_status)//if available space greater than or equal to incoming chars
        {
       	TelnetData.LastUartRxTimer=timercounter;
				write_boundary = RingWBDistance(UartRxBuf);
//printf("\n write_boundary = [%d] ", (u16_t)write_boundary);
				if(write_boundary > rx_status)
				{
	                reg_data = IP210RegRead(REG_UART_LINE_STATUS);
                    Rx_data =  IP210RegRead(REG_UART_RECEIVE_BUFFER);
                    while( !(reg_data & 0x04) )//check if Parity Error in Line Status Register
					{
//printf("\n REG_UART_LINE_STATUS=[%02x],  Rx_data = [%02x] ", (u16_t)reg_data, (u16_t)Rx_data);
                        //ok
                        UartRxBuf.buf[UartRxBuf.wi] = Rx_data;
//printf("\n UartRxBuf.buf[%d] = [%02x] ", (u16_t)UartRxBuf.wi, (u16_t)UartRxBuf.buf[UartRxBuf.wi]);
                        UartRxBuf.wi++;
                        UartRxBuf.TotalLen++;
                        write_boundary--;
						rx_status--;
//printf("\n OK, rx_status = [%d] ", (u16_t)rx_status);
//printf("\n write_boundary = [%d] ", (u16_t)write_boundary);

						if(rx_status > 0)
						{
      	                    reg_data = IP210RegRead(REG_UART_LINE_STATUS);
                            Rx_data =  IP210RegRead(REG_UART_RECEIVE_BUFFER);
						}
						else
						{
                         	//rx_status = IP210RegRead(REG_UART_RX_FIFO_STATUS);//jc_db:n2a
							if(rx_status == 0)
							{
							    break;//get out of the while
							}
							else
							{
      	                        reg_data = IP210RegRead(REG_UART_LINE_STATUS);
                                Rx_data =  IP210RegRead(REG_UART_RECEIVE_BUFFER);
							}
						}
    				}
//printf("\n REG_UART_LINE_STATUS=[%02x],  Rx_data = [%02x] ", (u16_t)reg_data, (u16_t)Rx_data);
					if(rx_status > 0)
					{
					    rx_status--;
					    //parity error//drop them
						for(i=0;i<rx_status-1;i++)
                            Rx_data =  IP210RegRead(REG_UART_RECEIVE_BUFFER);

					    //clear the rest chars in Rx FIFO
                        rx_status = IP210RegRead(REG_UART_RX_FIFO_STATUS);
                        while(rx_status > 0)
					    {
					        IP210RegRead(REG_UART_RECEIVE_BUFFER);
                            rx_status = IP210RegRead(REG_UART_RX_FIFO_STATUS);
					    }
					}
				}//if(write_boundary > rx_status)
				else
				{
	                reg_data = IP210RegRead(REG_UART_LINE_STATUS);
                    Rx_data =  IP210RegRead(REG_UART_RECEIVE_BUFFER);
                    while( !(reg_data & 0x04) )//check if Parity Error in Line Status Register
                    {
                        //ok char, put into rx_buf
                        //check rx boundary
                        if(write_boundary == 0)
                        {
                            UartRxBuf.buf[0] = Rx_data;
                            UartRxBuf.wi = 1;//redirect write ptr to the begining of Rx buf
                            UartRxBuf.TotalLen++;
            				write_boundary = RingWBDistance(UartRxBuf);
                        }//if(write_boundary == 0)
                        else
                        {
                            UartRxBuf.buf[UartRxBuf.wi] = Rx_data;
                            UartRxBuf.wi++;
                            UartRxBuf.TotalLen++;
                            write_boundary--;
                        }//if(write_boundary == 0)
						rx_status--;
						if(rx_status > 0)
						{
      	                    reg_data = IP210RegRead(REG_UART_LINE_STATUS);
                            Rx_data =  IP210RegRead(REG_UART_RECEIVE_BUFFER);
						}
						else
						{
                         	//rx_status = IP210RegRead(REG_UART_RX_FIFO_STATUS);//jc_db:n2a
							if(rx_status == 0)
							{
							    break;//get out of the while
							}
							else
							{
      	                        reg_data = IP210RegRead(REG_UART_LINE_STATUS);
                                Rx_data =  IP210RegRead(REG_UART_RECEIVE_BUFFER);
							}
						}
                    }//while( !(reg_data & 0x04) )//check if Parity Error in Line Status Register

					if(rx_status > 0)
					{
					    rx_status--;
					    //parity error//drop them
						for(i=0;i<rx_status-1;i++)
                            Rx_data =  IP210RegRead(REG_UART_RECEIVE_BUFFER);

					    //clear the rest chars in Rx FIFO
                        rx_status = IP210RegRead(REG_UART_RX_FIFO_STATUS);
                        while(rx_status > 0)
					    {
					        IP210RegRead(REG_UART_RECEIVE_BUFFER);
                            rx_status = IP210RegRead(REG_UART_RX_FIFO_STATUS);
					    }
					}

				}//if(write_boundary > rx_status)
	    }//if(ring_space >= rx_status)
		else
		{
			if(!(UART_Settings.HW_Flow_Control)){ // added by james
//				u16_t i;
//				for(i=0;i<UART_newdata_len;i++)IP210RegRead(REG_UART_RECEIVE_BUFFER);//drop UART FIFO content
				TelnetData.UART_MEM_OF_B+=rx_status;//rx_status means the incoming chars
				if(TelnetData.UART_MEM_OF_B>=1024)
				{	
					TelnetData.UART_MEM_OF_B-=1024;
					TelnetData.UART_MEM_OF_K++;
				}
				if(TelnetData.UART_MEM_OF_K>=1024)
				{	
					TelnetData.UART_MEM_OF_K-=1024;
					TelnetData.UART_MEM_OF_M++;
				}
#ifdef UMEM_OVERFLOW_MSG_ON
				printf("URM");//RX_OverFlow
#endif
			}
		}//if(ring_space >= reg_data)

    if(UART_Settings.HW_Flow_Control)
    {
				//flow control
				if(ring_space <= 1024)
				{
					//UART RX flow control= stop to receive
					UART_Set_RTS();
				}
				else
				{
					//UART RX flow control= start to receive
					UART_Clear_RTS();
				}
		}//if(UART_Settings.HW_Flow_Control)
    }//if(reg_data > 0)  
//IP210RegRead(REG_UART_RECEIVE_BUFFER)
}
#ifdef MODULE_RS485
void UART_RS485_Update_TxControl()
{
	u8_t reg_data=0;

	reg_data = IP210RegRead(REG_UART_LINE_STATUS);
	if(reg_data&0x40)
	{
		P4_0=0;
	}
}
#endif//#ifdef MODULE_RS485
#ifdef MODULE_UART_AUTO_SEND
extern u8_t f_auto_send;
void uart_auto_send()
{
	u16_t timer0_tick=0;

	if(NetUARTData.pTelnetData->pEEPROM_Telnet->cli_en==0)
	{
		CLEAR_UART_RX_BUF('4');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r70");rx_over=0;}
#endif
#ifdef MODULE_FLASH512
		if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP_NOR)||
            (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP_LIS) )
#else
		if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode!=NU_OP_MODE_UDP) )
#endif
		{
			TR0=0;
			timer0_tick=TH0*256+TL0;
	//		if(timer0_tick>=46900)	//at about 10ms
			if(timer0_tick>=4690)
			{
				if(TelnetData.WaitAck==0&&UartRxBuf.TotalLen!=0)
				{ f_auto_send=1; }
				TH0=0;
				TL0=0;		
			}
			TR0=1;
		}
	}
	if(f_auto_send)
	{
		uip_conn=TelnetData.uip_conn_last;
		uip_process(UIP_TIMER);
		CheckPktSend();			
#ifdef MODULE_MUIP
		muip_send();
#endif//MODULE_MUIP
		f_auto_send=0;
	}
}
#endif
void check_RS485(){	
	if(TelnetData.pEEPROM_UART->OP_Mode!=UART_INFO_OP_M_RS232)
	{ LOAD_DEFAULT_PIN=0; }
	else
	{ LOAD_DEFAULT_PIN=1; }
}

void telnet_server_timeout(){
	if(conn_flag)	//joe modify juset use get newdate to reflash timer do not use heart_beat 20081202
	{
		if(NetUARTData.pTelnetData->pEEPROM_Telnet->tcptimeout==0)
		{	conn_flag=0;	}
		else if(NetUARTData.pTelnetData->pEEPROM_Telnet->tcptimeout!=99)
		{
			if(Delta_Time(conn_timer) >= (6000*NetUARTData.pTelnetData->pEEPROM_Telnet->tcptimeout))
			{	conn_flag=0;	}
		}
	}
}
#endif //MODULE_NET_UART


/******************************************************************************
*
*   Name:           net_uart.c
*
*   Description:    RS232 to Ethernet
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#include <module.h>
#include <stdio.h>
#include "net_uart.h"
#include "string.h"
#include "telnet.h"
#include "udp_tel.h"
#include "timer.h"
#include "public.h"
#include "os_cbk.h"
#ifdef MODULE_NET_UART
SNetUARTData NetUARTData;
//--------------------------
#endif
#ifdef MODULE_TELNET_RFC
STelnetRFC2217 TelnetRFC2217;
extern u8_t code pr_v[5];
void telnet_rfc(); //joe for telnet RFC2217
void com_port_option();
void rfc_loop();
void check_rfc2217_signal();
u8_t dtr_rts_flag=0;
SRFCPkt* RFCPkt;
#endif
#ifdef MODULE_COMMAND_LINE
extern u8_t ATD_flag; 
extern u8_t ATD_close;
extern u8_t CLI_flag;
extern u8_t listen_wait;
#endif
//--------------------------
SBaudrateTable BaudrateTable[BAUDRATE_NUM]={
UART_RATE_110,      "110",// 0
UART_RATE_300,      "300",// 1
UART_RATE_1200,    "1200",// 2
UART_RATE_2400,    "2400",// 3
UART_RATE_4800,    "4800",// 4
UART_RATE_9600,    "9600",// 5
UART_RATE_19200,  "19200",// 6
UART_RATE_38400,  "38400",// 7
UART_RATE_57600,  "57600",// 8
UART_RATE_115200,"115200",// 9
UART_RATE_230400,"230400",//10
UART_RATE_460800,"460800",//11
UART_RATE_921600,"921600" //12
};

void CHECK_210()
{
	u8_t value=0;
	
	value=IP210RegRead(REG_CHIP_CONFIGURE_1);
	if(value&0x40)
	{ while(1){} }
}
#ifdef MODULE_NET_UART
/******************************************************************************
*
*  Function:    NetUartInit
*
*  Description: Init NetUart module
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void NetUartInit()
{
	NetUARTData.timer=0;
    NetUARTData.pTelnetData=&TelnetData;
	telnet_init_com();
	switch(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode)
	{
		case NU_OP_MODE_CLIENT:
			telnet_init();
			break;
		case NU_OP_MODE_SERVER:
			telnetd_init();
			break;
#ifdef MODULE_UDP_TELNET
#ifdef MODULE_FLASH512
		case NU_OP_MODE_UDP_LIS:
		case NU_OP_MODE_UDP_NOR:
#else
		case NU_OP_MODE_UDP:
#endif
			udp_tel_init();
			break;						
#endif //MODULE_UDP_TELNET
	}
}

/******************************************************************************
*
*  Function:    NetUartTelnetClose
*
*  Description: Close NetUart
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void NetUartTelnetClose()
{
	switch(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode)
	{
		case NU_OP_MODE_CLIENT:
			telnet_close();
			break;
		case NU_OP_MODE_SERVER:
			telnetd_close();
			break;
	}
}

/******************************************************************************
*
*  Function:    NetUartOnTCPRx
*
*  Description: callback function for uip
*               
*  Parameters:  
*
*  Returns:     1: This is NetUart Packet, and NetUart had process this packet
*               0: Not NetUart Packet, need process by others
*               
*******************************************************************************/
u8_t NetUartOnTCPRx()
{
	switch(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode)
	{
		case NU_OP_MODE_CLIENT:
		case NU_OP_MODE_SERVER:
#ifdef MODULE_COMMAND_LINE
			if(ATD_close==0)
			{
				if(uip_conn->lport==NetUARTData.pTelnetData->pEEPROM_Telnet->TelentPort)
				{
					telnetd();
					return 1;
				}
				if(uip_conn->rport==NetUARTData.pTelnetData->pEEPROM_Telnet->TelentPort)
				{
					telnet();
					return 1;
				}
			}
			else
			{
				if(uip_conn->rport==NetUARTData.pTelnetData->pEEPROM_Telnet->CLI_RemotePort)
				{
					telnet();
					return 1;
				}
			}
#else
			if(uip_conn->lport==NetUARTData.pTelnetData->pEEPROM_Telnet->TelentPort)
			{
				telnetd();
				return 1;
			}
			if(uip_conn->rport==NetUARTData.pTelnetData->pEEPROM_Telnet->TelentPort)
			{
				telnet();
				return 1;
			}
#endif
			return 0;
		default:
			return 0;
	}
}

/******************************************************************************
*
*  Function:    NetUartLoop
*
*  Description: callback function for OS loop
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
void NetUartLoop()
{
#ifdef DB_UDP_TEL
u16_t tmp;
extern u16_t net_fifo_overflow;
extern u16_t EthernetReceiveFreeSpace();
tmp=EthernetReceiveFreeSpace();
if(tmp<=1514)
{
		if(tmp)
		{
//			printf("[%d]",tmp);
			net_fifo_overflow++;
		}
}
#endif
#ifdef DB_UART_FIFO_OVERFLOW
	printf("%c",db_ufo[1]);
#endif //DB_UART_FIFO_OVERFLOW

#ifdef MODULE_RS485
	if(TelnetData.pEEPROM_UART->OP_Mode==UART_INFO_OP_M_RS485)
	{
		UART_RS485_Update_TxControl();
	}
#endif

	CLEAR_UART_RX_BUF('4');
  	WriteEthRx2UART();
	CLEAR_UART_RX_BUF('5');
}

/******************************************************************************
*
*  Function:    NetUartTimer
*
*  Description: callback function for OS timer
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
#ifdef MODULE_VLAN_TAG
extern u8_t udp_send_pkt;
#endif
void NetUartTimer()
{
#ifdef DB_UDP_TEL
	extern u16_t db_rx[3];
	extern u16_t db_tx[3];
	extern u16_t net_fifo_overflow;
	if(NetUARTData.timer++>MAX_TIMER_TCP_RECONNECT)
	{
		NetUARTData.timer=0;
		printf("\nt=%d,%d,%d,r=%d,%d,%d",db_tx[2],db_tx[1],db_tx[0],db_rx[2],db_rx[1],db_rx[0]);
		printf(",f=%d",net_fifo_overflow);
	}
#endif //DB_UDP_TEL
	switch(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode)
	{
		case NU_OP_MODE_CLIENT:
			if((NetUARTData.pTelnetData->ConnectStatus==tcDisconnected)&&(TelnetData.conn_client==0))//for debug
			{
				NetUARTData.timer++;
				if(NetUARTData.timer<MAX_TIMER_TCP_RECONNECT){
					return;
				}
				if(telnet_connect()==OK){
					NetUARTData.timer=0;
					TelnetData.conn_client=1;
				}
			}
			break;
		case NU_OP_MODE_SERVER:
			if(NetUARTData.pTelnetData->ConnectStatus==tcDisconnected){//for debug
				#ifdef MODULE_COMMAND_LINE
				if(ATD_close==1){
					if(TelnetData.conn_client==0){
						if(telnet_connect()==OK)
							TelnetData.conn_client=1;
					}
				}
				else
				#endif
				{	
					telnetd_listen();
				}
			}
			break;
	}
}
#endif //MODULE_NET_UART


/******************************************************************************
*
*  Function:    Telnet RFC2217
*
*  Description: Data for Commend
*               
*  Parameters:  
*               
*  Returns:     
*               
*******************************************************************************/
#ifdef MODULE_TELNET_RFC
u16_t local_iac=0, local_len=0, local_end_len=0;
void telnet_rfc()
{
	u8_t flag=1, i=1, CMPflag=0;
//------------------------find IAC SB--------------joe for dtr signal 20081006
	for(local_iac=0; local_iac<uip_len; local_iac++) //search for rfc star
	{
		if(uip_appdata[local_iac] == IAC)
		{
			if( (uip_appdata[local_iac+1] == SB) || (uip_appdata[local_iac+1] == DO) || (uip_appdata[local_iac+1] == WILL) ||
				(uip_appdata[local_iac+1] == DONT) || (uip_appdata[local_iac+1] == WONT) )
			{	dtr_rts_flag=1;	break;	}
		}
	}
	if(local_iac == uip_len)
	{	return;	}
	for(local_end_len=local_iac; local_end_len<uip_len; local_end_len++) //search for rfc end
	{
		if(uip_appdata[local_end_len] == IAC)
		{
			if( (uip_appdata[local_end_len+1] == DO) || (uip_appdata[local_end_len+1] == WILL) ||
				(uip_appdata[local_end_len+1] == DONT) || (uip_appdata[local_end_len+1] == WONT) )
			{
				if(uip_appdata[local_end_len+3] == IAC)
				{	continue;	}
				else
				{	local_end_len += 3;		break;	}
			}
			else
			{
				if(uip_appdata[local_end_len+1] == SB)
				{	continue;	}
				if(uip_appdata[local_end_len+1] == SE)
				{
					if(uip_appdata[local_end_len+2] != IAC)
					{	local_end_len += 2;		break;	}
				}
			}
		}
	}
	memcpy(uip_appdata, uip_appdata+local_iac, (local_end_len - local_iac));
//-------------------------------------------------
	RFCPkt=(SRFCPkt*)uip_appdata;
	while(flag)
	{
		switch(RFCPkt->NAME)
		{
			case WILL:
				if(RFCPkt->OPTION == 0x2c)
					RFCPkt->NAME = DO;
				else
					RFCPkt->NAME = DONT;
			break;
			case DO:
				if(RFCPkt->OPTION == 0x2c || RFCPkt->OPTION == 0x27)
					RFCPkt->NAME = WILL;
				else
					RFCPkt->NAME = WONT;
			break;
			case SB:
				CMPflag=1;
				flag=0;
			break;
		}
		if(flag)
		{
			RFCPkt=(SRFCPkt*)(uip_appdata+sizeof(SRFCPkt)*i);
			if(RFCPkt->pIAC != IAC)
				flag = 0;
			else
				i++;
		}
	}
	if(CMPflag)
	{	com_port_option();	}
	else
	{	uip_slen = uip_len;	}
	return;
}
#endif
#ifdef MODULE_TELNET_RFC
void com_port_option()
{
	u8_t *cp_data = uip_appdata;
	u8_t *NetUART = "NetUART";
	u8_t i=0, coun=0,j=0;
	u16_t k=0;
	u8_t tmp;
	u8_t parity[5]={0, 8, 24, 40, 56};
	u8_t cp_buff[100];
	u32_t baudrate=0, baudrate_table=0;

	memset(cp_buff, 0, 100);
	memcpy(cp_buff, uip_appdata+local_iac, (local_end_len - local_iac));
	
	for(k=0;k<uip_len;k++)
	{
		switch(cp_data[k])
		{
			case SIGNATURE:
				if(cp_data[k-1] != COM_PORT_OPTION){ break; }
				coun=7;
				cp_buff[k] = ACK_SIGNATURE;
				memcpy(cp_buff+(k+1), NetUART, 7);
				memcpy(cp_buff+(k+1+coun), cp_data+(k+1), uip_len-(k+1));
				break;
			case BAUDRATE: //Baudrate
				if(cp_data[k-1] != COM_PORT_OPTION){ break; }
				cp_buff[k+coun] = ACK_BAUDRATE;
				for(i=0;i<4;i++)
				{
					baudrate |= (u32_t)*(cp_data+(k+1)+i)<<(8*(3-i));
				}
				if(baudrate == 0)
				{
					baudrate = TelnetData.pEEPROM_UART->Baudrate;
					sscanf(BaudrateTable[baudrate].Name,"%lu", &baudrate_table);
					for(i=0;i<4;i++)
					{
						*(cp_buff+(k+coun+4)-i) |= baudrate_table>>(8*i) & 0x0000ff;
					}
				}
				for(j=0;j<BAUDRATE_NUM;j++)
				{
					sscanf(BaudrateTable[j].Name,"%lu", &baudrate_table);
				  	if(baudrate_table == baudrate)
				  	{
						NetUARTData.pTelnetData->pEEPROM_UART->Baudrate=j;
					  	UART_Settings.divisor = BaudrateTable[j].Divisor;
					}
	  			}
				break;
			case DATASIZE:
				if(cp_data[k-1] != COM_PORT_OPTION){ break; }
				cp_buff[k+coun] = ACK_DATASIZE;
				if(cp_data[k+1] == 0)
				{
					cp_buff[k+coun+1] = TelnetData.pEEPROM_UART->Character_Bits+5;
				}
				else
				{
					NetUARTData.pTelnetData->pEEPROM_UART->Character_Bits=cp_data[k+1]-5;
					UART_Settings.Character_Bits=cp_data[k+1]-5;				
				}
				break;
			case PARITY:
				if(cp_data[k-1] != COM_PORT_OPTION){ break; }
				cp_buff[k+coun] = ACK_PARITY;				
				if(cp_data[k+1] == 0)
				{
					for(i=0;i<5;i++)
					{
						if(TelnetData.pEEPROM_UART->Parity_Type == parity[i])
						{	break;	}
					}
					cp_buff[k+coun+1] = i+1;
				}
				else
				{
					for(i=0;i<5;i++)
					{
						if(parity[cp_data[k+1]-1] == pr_v[i])
						{	break;	}
					}
					NetUARTData.pTelnetData->pEEPROM_UART->Parity_Type=parity[cp_data[k+1]-1];
					UART_Settings.Parity_Type=parity[cp_data[k+1]-1];				
				}
				break;
			case STOPSIZE:
				if(cp_data[k-1] != COM_PORT_OPTION){ break; }
				cp_buff[k+coun] = ACK_STOPSIZE;
				if(cp_data[k+1] == 0)
				{
					cp_buff[k+coun+1] = (TelnetData.pEEPROM_UART->Stop_Bit)+1;
				}
				else
				{
					tmp = cp_data[k+1]-1;
					if(tmp == 2){	tmp = 1; }
					NetUARTData.pTelnetData->pEEPROM_UART->Stop_Bit=tmp; //Stop size 0: 1  1: 1.5 or 2
					UART_Settings.Stop_Bit=tmp;
				}
				break;
			case CONTROL:
				if(cp_data[k-1] != COM_PORT_OPTION){ break; }
				cp_buff[k+coun] = ACK_CONTROL;
				switch(cp_data[k+1])
				{
					case FC_SETTING:
						tmp = NetUARTData.pTelnetData->pEEPROM_UART->HW_Flow_Control;
						cp_buff[k+coun+1] = tmp;
						break;
					case DTR_REQ:
						tmp=IP210RegRead(REG_UART_MODEM_CONTROL)&0x1;
						cp_buff[k+coun+1] = ~tmp;
						break;
					case DTR_ON:
						tmp=IP210RegRead(REG_UART_MODEM_CONTROL)&~0x1;
						IP210RegWrite(REG_UART_MODEM_CONTROL,tmp);
						break;
					case DTR_OFF: 	
						tmp=IP210RegRead(REG_UART_MODEM_CONTROL)|0x1;
						IP210RegWrite(REG_UART_MODEM_CONTROL,tmp);
						break;
					case RTS_REQ:
						tmp=IP210RegRead(REG_UART_MODEM_CONTROL)&0x2;
						cp_buff[k+coun+1] = ~tmp;
						break;
					case RTS_ON:
						tmp=IP210RegRead(REG_UART_MODEM_CONTROL)&~0x2;
						IP210RegWrite(REG_UART_MODEM_CONTROL,tmp);
						break;
					case RTS_OFF:
						tmp=IP210RegRead(REG_UART_MODEM_CONTROL)|0x2;
						IP210RegWrite(REG_UART_MODEM_CONTROL,tmp);
						break;
					case NONE_O:
					case NONE_I:
						NetUARTData.pTelnetData->pEEPROM_UART->HW_Flow_Control=0x0;
						UART_Settings.HW_Flow_Control=0x0;
						break;
					case XON_OFF_O:
					case XON_OFF_I:
						NetUARTData.pTelnetData->pEEPROM_UART->HW_Flow_Control=0x0;
						UART_Settings.HW_Flow_Control=0x0;
						break;
					case HW_O:
					case HW_I:
						NetUARTData.pTelnetData->pEEPROM_UART->HW_Flow_Control=0x1;
						UART_Settings.HW_Flow_Control=0x1;
						break;
				}	
				break;
			case SUSPEND:
				if(cp_data[k-1] != COM_PORT_OPTION){ break; }
				cp_buff[k+coun] = ACK_SUSPEND;
//				cp_buff[k+coun+1] = 0x00;
				break;
			case RESUME:
				if(cp_data[k-1] != COM_PORT_OPTION){ break; }
				cp_buff[k+coun] = ACK_RESUME;
//				cp_buff[k+coun+1] = 0x00;
				break;
			case LINESTATE_MASK:
				if(cp_data[k-1] != COM_PORT_OPTION){ break; }
				cp_buff[k+coun] = ACK_LINESTATE_MASK;
//				cp_buff[k+coun+1] = 0x00;			
				break;
			case MODEMSTATE_MASK:
				if(cp_data[k-1] != COM_PORT_OPTION){ break; }
				cp_buff[k+coun] = ACK_MODEMSTATE_MASK;
//				cp_buff[k+coun+1] = 0x00;
				break;
			case PURGE_DATA:
				if(cp_data[k-1] != COM_PORT_OPTION){ break; }
				cp_buff[k+coun] = ACK_PURGE_DATA;
//				cp_buff[k+coun+1] = 0x00;
				break;
		}
	}
	UART_Set_Property();
	uip_len = (local_end_len - local_iac) + coun;
	memcpy(cp_data, cp_buff, uip_len);
	uip_slen = uip_len;
	return;
}

void rfc_loop()
{
	u8_t value=IP210RegRead(REG_UART_MODEM_STATUS);
	u8_t old=TelnetRFC2217.last_signal;

	TelnetRFC2217.last_signal = (value>>4) & 0xf;
	value = ((value>>4) & 0xf);
	if(old != TelnetRFC2217.last_signal)
	{
		if((old&0x1) != (TelnetRFC2217.last_signal&0x1))
		{
			TelnetRFC2217.cts_signal = 1;
			if((value&0x1)==1)
				TelnetRFC2217.cts_value = 0;
			else
				TelnetRFC2217.cts_value = 1;
		}
		if((old&0x2) != (TelnetRFC2217.last_signal&0x2))
		{
			TelnetRFC2217.dsr_signal = 1;
			if((value&0x2)==0x2)
				TelnetRFC2217.dsr_value = 0;
			else
				TelnetRFC2217.dsr_value = 1;
		}
		if((old&0x4) != (TelnetRFC2217.last_signal&0x4))			//RI
		{
			TelnetRFC2217.ri_signal = 1;
			if((value&0x4)==0x4)
				TelnetRFC2217.ri_value = 0;
			else
				TelnetRFC2217.ri_value = 1;
		}
		if((old&0x8) != (TelnetRFC2217.last_signal&0x8))			//DCD
		{
			TelnetRFC2217.dcd_signal = 1;
			if((value&0x8)==0x8)
				TelnetRFC2217.dcd_value = 0;
			else
				TelnetRFC2217.dcd_value = 1;
		}
	}
}

void check_rfc2217_signal()
{
	u8_t pkt_signal[7]={0xff, 0xfa, 0x2c, 0x6b, 0x00, 0xff, 0xf0};

	if( (TelnetRFC2217.cts_signal == 1)||(TelnetRFC2217.dsr_signal == 1)||(TelnetRFC2217.ri_signal == 1)||(TelnetRFC2217.dcd_signal == 1) )
	{
		TelnetData.WaitAck = 1;
		if(TelnetRFC2217.cts_signal == 1)
		{
			TelnetRFC2217.cts_signal = 0;
			pkt_signal[4] = (TelnetRFC2217.cts_value<<4);
			pkt_signal[4] |= 0x1;
		}
		if(TelnetRFC2217.dsr_signal == 1)
		{
			TelnetRFC2217.dsr_signal = 0;
			pkt_signal[4] = pkt_signal[4]|(TelnetRFC2217.dsr_value<<5);
			pkt_signal[4] |= 0x2;
		}
		if(TelnetRFC2217.ri_signal == 1)
		{
			TelnetRFC2217.ri_signal = 0;
			pkt_signal[4] = pkt_signal[4]|(TelnetRFC2217.ri_value<<2);
			pkt_signal[4] |= 0x40;
		}
		if(TelnetRFC2217.dcd_signal == 1)
		{
			TelnetRFC2217.dcd_signal = 0;
			pkt_signal[4] = pkt_signal[4]|(TelnetRFC2217.dcd_value<<7);
			pkt_signal[4] |= 0x8;
		}
		memcpy(uip_appdata, pkt_signal, 7);
		uip_len = 7;
		uip_slen=uip_len;
	}
	return;
}

#endif

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
#ifndef NET_UART_H
#define NET_UART_H
//#ifdef MODULE_NET_UART		//20081215 joe for module net uart
#include "type_def.h"
#include "uart.h"
#include "telnet.h"
#include "udp_tel.h"
#include "module.h"
#include "public.h"
#include "gpio.h"
#define NU_OP_MODE_DISABLE 0
#define NU_OP_MODE_SERVER  1
#define NU_OP_MODE_CLIENT  2
#ifdef MODULE_FLASH512
#define NU_OP_MODE_UDP_LIS 4
#define NU_OP_MODE_UDP_NOR 3
#else
#define NU_OP_MODE_UDP     3
#endif

#ifdef MODULE_TELNET_RFC
typedef struct _RFCPKT
{
	u8_t  pIAC;
	u8_t  NAME;
	u8_t  OPTION;
}SRFCPkt;
#endif

typedef struct _STelnetRFC2217
{
	u8_t cts_signal;
	u8_t cts_value;
	u8_t dsr_signal;
	u8_t dsr_value;
	u8_t ri_signal;
	u8_t ri_value;
	u8_t dcd_signal;
	u8_t dcd_value;
	u8_t last_signal;
}STelnetRFC2217;


typedef struct _SNetUARTData
{
   u8_t timer;
   CTelnetData* pTelnetData;
   u8_t* pkt_rx_data_offset;
   u8_t* pkt_tx_data_offset;
}SNetUARTData;
//#define MAX_TIMER_TCP_RECONNECT 50
#define MAX_TIMER_TCP_RECONNECT 25
extern SNetUARTData NetUARTData;
typedef struct _SBaudrateTable
{
	u16_t Divisor;
	char Name[7];
}SBaudrateTable;
extern SBaudrateTable BaudrateTable[];
#define BAUDRATE_NUM 13

void NetUartInit();
void NetUartTelnetClose();
u8_t NetUartOnTCPRx();//return 0:not NetUart packet, 1:NetUart packet
void NetUartLoop();
void NetUartTimer();

#define HUART_RXD P2_0
#define HUART_TXD P2_1
#define HUART_RTS P2_2
#define HUART_DTR P2_3
#define HUART_CTS P2_4
#define HUART_DSR P2_5
#define HUART_DCD P2_6
#define HUART_RI  P2_7
//RFC2217
#define COM_PORT_OPTION 	44
#define IAC					255
#define	WILL				251
#define	WONT				252
#define	DO					253
#define	DONT				254
#define SB					250
#define SE					240
#define SIGNATURE			0
#define BAUDRATE			1
#define DATASIZE			2
#define PARITY				3
#define STOPSIZE			4
#define CONTROL				5
#define LINESTATE			6
#define MODEMSTATE			7
#define SUSPEND				8
#define RESUME				9
#define LINESTATE_MASK		10
#define MODEMSTATE_MASK		11
#define PURGE_DATA			12
#define ACK_SIGNATURE		100
#define ACK_BAUDRATE		101
#define ACK_DATASIZE		102
#define ACK_PARITY			103
#define ACK_STOPSIZE		104
#define ACK_CONTROL			105
#define ACK_LINESTATE		106
#define ACK_MODEMSTATE		107
#define ACK_SUSPEND			108
#define ACK_RESUME			109
#define ACK_LINESTATE_MASK	110
#define ACK_MODEMSTATE_MASK	111
#define ACK_PURGE_DATA		112
//set_control
#define FC_SETTING			0
#define NONE_O				1
#define XON_OFF_O			2
#define HW_O				3
#define DTR_REQ				7
#define DTR_ON				8
#define DTR_OFF				9
#define RTS_REQ				10
#define RTS_ON				11
#define RTS_OFF				12
#define NONE_I				14
#define XON_OFF_I			15
#define HW_I				16

#ifdef DB_UART_FIFO_OVERFLOW
	extern char db_ufo[2];
	#define NET_UART_LOOP(a) {db_ufo[1]=a;NetUartLoop();}
#else
	#define NET_UART_LOOP(a) NetUartLoop()
#endif

//#endif//MODULE_NET_UART
#endif
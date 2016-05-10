/******************************************************************************
*
*   Name:           telnet.h
*
*   Description:    
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#ifndef TELNET_H
#define TELNET_H
#include "uip.h"
#include "muip.h"
#include "module.h"
#include "public.h"
#include "eepdef.h"
//#ifdef MODULE_NET_UART
#define UART_INT_RX 0x1
#define UART_INT_TX 0x2
#define UART_RX_FIFO_LEVEL 0x80
//-------------------------------------------Telnet Server---------------------------------------------------------
enum {tcDisconnected,tcListen,tcConnected,tcWaitDisconnected};

typedef struct _CTelnetData
{
	struct uip_conn *uip_conn_last;
	struct uip_conn *uip_conn_client;
	u8_t conn_client;
	u32_t idle;
	u8_t WaitAck;
	u8_t ReTxTimeout;
	u16_t  RexmitLen;
	u8_t Delimiter_drop_len;
	u32_t LastUartRxTimer;
	u8_t ConnectStatus;	
	STelnetInfo* pEEPROM_Telnet;
	SUARTInfo* pEEPROM_UART;
  	u16_t PollCnt;
  	u16_t UART_MEM_OF_B;//byte
  	u16_t UART_MEM_OF_K;//kilo
  	u32_t UART_MEM_OF_M;//Mega
  	u32_t UART_FIFO_Overflow;
  	u8_t  UART_INT_ENABLE;
	u8_t t_vlan_flag;	
	u8_t t_vlan_tag[4];
}CTelnetData;

extern CTelnetData TelnetData;
void telnetd_init();
void telnetd();
void telnetd_close();
void telnetd_listen();
void check_RS485();
void telnet_server_timeout();
//-------------------------------------------Telnet Client---------------------------------------------------------
u8_t telnet_connect();
void telnet_close();
void telnet_init();
void telnet();
void telnet_init_com();
//------------------------------------------------UART-------------------------------------------------------------
#define UART_IN_ISR 0x1
void ClearUARTRxBuf_ParityError(void);//sorbica071224
void ClearUARTRxBuf();
#ifdef DB_UART_FIFO_OVERFLOW
extern char db_ufo[2];
#define CLEAR_UART_RX_BUF(a) {db_ufo[0]=a;ClearUARTRxBuf();}
#else
#define CLEAR_UART_RX_BUF(a) ClearUARTRxBuf()
#endif

void WriteEthRx2UART();
//------------------------------------------RingBuffer-------------------------------------------------------------------
#include "ring.h"
//------------------------------------------UART Declare-----------------------------------------------------------
void SendUARTRx2Eth(u8_t protocol);//0:tcp, 1:udp
void ReSendUARTRx2Eth();
void WriteEthRx2UART();
int ReadEth2UARTTxBuf();//1:Success 0:UART_TX_OVERFLOW
#define UART_TX_BUFFER 16
#define UART_RX_BUFFER 255

//Common buffer parameter:
//#define UARTBUF_START_ADDR 0x2C00	//need modify project option too.
#define UARTBUF_START_ADDR 0x3B00 //Move 2k buff to httpd server //peter
#define MAX_NET_UART_BUF_LEN (0x7000-UARTBUF_START_ADDR)

//TCP Telnet buffer parameter:
//#define TCP_MAX_UART_TX_BUF_LEN 0xDFC
#define TCP_MAX_UART_TX_BUF_LEN 0xBB8
#define TCP_MAX_UART_RX_BUF_LEN (MAX_NET_UART_BUF_LEN-TCP_MAX_UART_TX_BUF_LEN)
//-----------------------
#define TCP_RxBuf UartBuf
#define TCP_TxBuf (UartBuf+TCP_MAX_UART_RX_BUF_LEN)
//-----------------------
/*TCP buffer:
-------------0x0000
|    SYS    |
-------------0x3950 = UARTBUF_START_ADDR
|    RX     | (2AF8)
-------------0x6448 = UARTBUF_START_ADDR+TCP_MAX_UART_RX_BUF_LEN
|    TX     | (BB8)
-------------0x7000 = UARTBUF_START_ADDR+MAX_NET_UART_BUF_LEN
|    MAC    |
|    BUF    |
-------------
*/

//UDP Telnet buffer parameter:
#ifdef MODULE_MUIP
//	#define RELEASE_FOR_MAC_RXFIFO (0x1E48+0x5ee)
	#define RELEASE_FOR_MAC_RXFIFO 0x19B0
	#define UDP_MAX_TX_PACKET_BUF 0
#else
	#define RELEASE_FOR_MAC_RXFIFO 0x19B0
	#define UDP_MAX_TX_PACKET_BUF 0x5ee
#endif//MODULE_MUIP

#define UDP_MAX_NET_UART_BUF_LEN (MAX_NET_UART_BUF_LEN-RELEASE_FOR_MAC_RXFIFO-UDP_MAX_TX_PACKET_BUF)
#ifdef MODULE_MUIP
	#define UDP_MAX_UART_RX_BUF_LEN 0xE00
#else
	#define UDP_MAX_UART_RX_BUF_LEN 0xC00
#endif
#define UDP_MAX_UART_TX_BUF_LEN (UDP_MAX_NET_UART_BUF_LEN-UDP_MAX_UART_RX_BUF_LEN)
//-----------------------
#define UDP_RxBuf UartBuf //Stare Address
#define UDP_TxBuf    (UartBuf+UDP_MAX_UART_RX_BUF_LEN)
//for udp transmit, it will not modify uip_buf
#ifdef MODULE_MUIP
	#define UDP_TxPktBuf muip_buf[0]
#else
	#define UDP_TxPktBuf (UDP_TxBuf+UDP_MAX_UART_TX_BUF_LEN)
#endif//MODULE_MUIP
/*UDP buffer:
-------------0x0000
|    SYS    |
-------------0x3B00 = UARTBUF_START_ADDR
|    RX     | (C50)
-------------0x4750 = UARTBUF_START_ADDR+UDP_MAX_UART_RX_BUF_LEN
|    TX     | (F00)
-------------0x5650 = UARTBUF_START_ADDR+UDP_MAX_NET_UART_BUF_LEN
| TxPktBuf  | (19B0)
-------------0x7000 = UARTBUF_START_ADDR+MAX_NET_UART_BUF_LEN
|    MAC    |
|    BUF    |
-------------
*/

extern u8_t volatile xdata* UartBuf;
extern u8_t volatile xdata* RxBuf;
extern u8_t volatile xdata* TxBuf;
extern u16_t max_uart_rx_buf_len;
extern u16_t max_uart_tx_buf_len;

extern u16_t silent_delimiter[];
extern u8_t silent_ri;
extern u8_t silent_wi;
extern u8_t silent_total;
#define FOUND_NO_DELIMITER   0x00000000
#define FOUND_CHAR_DELIMITER   0x00010000
#define FOUND_SILENT_DELIMITER 0x00020000
extern u32_t char_delimiter_offset;//bit31~16: 0:not found, 1:char, 2:silent, bit15~0:offset


#define UartTxBufSafeSpace 1496//MAX telnet data, 1496=1536-40
extern SRingBuffer UartRxBuf,UartTxBuf;
#define TELNET_POLL_MAX 300
#ifdef MODULE_RS485
	void UART_RS485_Update_TxControl();
#endif//#ifdef MODULE_RS485
//#endif//MODULE_NET_UART
#endif
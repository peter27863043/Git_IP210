/****************************************************************************
*
*	Name:			IP210 MACdriver.h
*
*	Description:	MAC Driver use functions
*
*	Copyright:		(c) 2005-2050    IC Plus Crop.  
*	                All rights reserved.	by DeNa
*
****************************************************************************/
void MAC_init();
void Resize_RX_Buf(u8_t rx_start);//add from v1.15
s16_t	EthernetReceive(u8_t	*rx_buf);
void	reset_tx();
u16_t	uip_ipchecksum(void);
#define uip_tcpchecksum()		uip_ipchecksum()
void	uip_add32(u8_t* op32,u16_t op16);
void	IP210_CheckSum(u8_t *addr,u16_t len);
extern u8_t	EthernetSend(u8_t	*tx_buf,	s16_t plen);


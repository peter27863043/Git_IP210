/****************************************************************************
*
*	Name:			IP210 MACdriver.C
*
*	Description:	MAC Driver
*
*	Copyright:		(c) 2005-2050    IC Plus Crop.  
*	                All rights reserved.	by DeNa
*
*	Version:		v1.15_NetUART
****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "wdg.h"
//#include "Reg51.h"
#include <module.h>
#include "type_def.h"
#include "ip210reg.h"
#include "eeprom.h"
#include "eeprominit.h"
#include "fm_web_com.h"
#include "vtag.h"
#ifndef FIRMWARE_UPDATE
#include "net_uart.h"
#endif
#ifdef MODULE_VLAN_TAG
	#include "telnet.h"
#endif
#define	UIP_BUFFER	uip_buf
//#define	IP210_MAC_CTRL_0 0x8100
extern u8_t* UIP_BUFFER;


//global value for this file
u16_t	Read_Point;//=RX_BUF_START; //movidy from v1.15
u8_t	Tx_Current=0;
u8_t	TxTime[4];
u8_t	turn_around=0;
u8_t	Tx_bak=0;
//extern u8_t	Tx_Current;
//extern u8_t	TxTime[4];

//extern function
extern unsigned char IP210_DMA(unsigned short src_addr, unsigned short des_addr, unsigned short length, unsigned char cmd);
//unsigned char IP210_DMA(unsigned short src_addr, unsigned short des_addr
//						, unsigned short length, unsigned char cmd)
//{	cmd=cmd;
//	memcpy((u16_t *)des_addr,(u16_t *)src_addr,length);
//	return 1;
//}
#ifndef FIRMWARE_UPDATE
#ifdef MODULE_VLAN_TAG
extern u8_t *vlan_tag_pkt(u8_t *input_buf, u16_t *len_ptr);
extern u8_t udp_send_pkt;
extern u8_t vlan_value[4];
#endif
#endif//#ifndef FIRMWARE_UPDATE
/******************************************************************************
*
*  Function:    MAC_Address_fill
*
*  Description: fill MAC address from EEPROM struct to IP210 register
*               
*  Parameters:  No (need external EEConfigInfo structure)
*               
*  Returns:     No
*               
*******************************************************************************/
void MAC_Address_fill(void)
{	u8_t	i;
	for(i=0;i<6;i++)
	  { IP210RegWrite( REG_MAC_ADDR+i,	EEConfigInfo.MACID[i] );	}
}


/******************************************************************************
*
*  Function:    IP_Address_fill
*
*  Description: fill IP address from EEPROM struct to IP210 register
*               
*  Parameters:  No (need external EEConfigInfo structure)
*               
*  Returns:     No
*               
*******************************************************************************/
void IP_Address_fill(void)
{	u8_t	i;
	for(i=0;i<4;i++)
	  { IP210RegWrite( REG_IP_ADDR+i,	EEConfigInfo.IPAddress[i] );	}
}


/******************************************************************************
*
*  Function:    Resize_RX_Buf
*
*  Description: Change MAC Rx Buffer Start Pointer
*               
*  Parameters:  rx_start:	High byte of Rx_Buffer start point .
*               
*  Returns:     No
*               
*******************************************************************************/
void Resize_RX_Buf(u8_t rx_start)//add from v1.15
{	u8_t MAC_Setting=IP210RegRead(REG_MAC_CONTROL_0 );
	
	IP210RegWrite( REG_MAC_CONTROL_0,MAC_Setting&~MC0_RX_ENABLE );
	IP210RegWrite( REG_RX_START,rx_start);
	Read_Point=((u16_t)IP210RegRead(REG_RX_START)<<8);
	IP210RegWrite( REG_MAC_CONTROL_0,MAC_Setting);
}

/******************************************************************************
*
*  Function:    MAC_init
*
*  Description: MAC initial function
*               
*  Parameters:  no
*               
*  Returns:     No
*               
*******************************************************************************/
void MAC_init(void)
{
	u8_t reg_value;
	IP210RegWrite( REG_MAC_CONTROL_0 , 
				MC0_SPEED100 | MC0_DUPLEX_F | MC0_TX_ENABLE | MC0_RX_ENABLE | MC0_FLOW_CTRL_EN |MC0_MAX_FRAME_1536 	);

	IP210RegWrite( REG_RX_FILTER_0 , RF0_MY_MAC_EN | RF0_MSCT_EN | RF0_BSCT_EN | RF0_PAUSE_EN);
//	IP210RegWrite( REG_RX_FILTER_0 , RF0_ALL_EN);
	IP210RegWrite( REG_RX_FILTER_1 , 0x0 );

    reg_value=IP210RegRead(REG_CHIP_CONFIGURE_0);
    reg_value=(reg_value&0x20)|CHIP_CFG_VALUE;
	IP210RegWrite( REG_CHIP_CONFIGURE_0,reg_value);
	Read_Point=((u16_t)IP210RegRead(REG_RX_START)<<8); //add from v1.15
////	IP210RegWrite( 0x8102 , 0x0 );//DENA_STOP
	MAC_Address_fill();
	IP_Address_fill();
}



/******************************************************************************
*
*  Function:    TFD_index
*
*  Description: TFD index management function.
*				called after every Tx command.
*               
*  Parameters:  No
*               
*  Returns:     No
*               
*******************************************************************************/
void	TFD_index(void)
{
    Tx_bak=Tx_Current;
	Tx_Current++;
	Tx_Current%=4;
}


/******************************************************************************
*
*  Function:    check_packet_type
*
*  Description: packet type check function.
*               
*  Parameters:  *tx_buf:	pointer of Tx buffer starting address
*               
*  Returns:     packet type
*				0x00	pkt_802_3		
*				0x01	pkt_TCP_IPv4	
*				0x02	pkt_UDP_IPv4	
*				0x03	pkt_ICMP_IPv4	
*				0x05	pkt_TCP_IPv6	
*				0x06	pkt_UDP_IPv6	
*				0x07	pkt_ICMP_IPv6	
*				0x08	pkt_ARP		
*				0x09	pkt_BPDU	
*				0x0a	pkt_RARP	
*******************************************************************************/
#ifndef FIRMWARE_UPDATE
#ifdef DB_FIFO
extern u8_t rx_over;
#endif
#ifdef MODULE_VLAN_TAG
extern u8_t eth_vlan;
#endif
#endif//#ifndef FIRMWARE_UPDATE

#ifndef FIRMWARE_UPDATE
u8_t check_packet_type(u8_t	*tx_buf)
{
	u8_t ether_type_offset=0, ip_type_offset=0;
#ifdef MODULE_VLAN_TAG
	u8_t VLAN_TAG=4, value=0;
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\rB");rx_over=0;}
#endif
#endif
	if( (udp_send_pkt==1)||(eth_vlan==1)||( (NetUARTData.pTelnetData->t_vlan_flag==1)&&(NetUARTData.pTelnetData->uip_conn_last==uip_conn) ) )
	{
		ether_type_offset=ETHER_TYPE_OFFSET+4;
		ip_type_offset=IP_TYPE_OFFSET+4;
	}
	else
#endif
	{
		ether_type_offset=ETHER_TYPE_OFFSET;
		ip_type_offset=IP_TYPE_OFFSET;
	}
#ifdef MODULE_VLAN_TAG
	if( (udp_send_pkt==1)||(eth_vlan==1)||( (NetUARTData.pTelnetData->t_vlan_flag==1)&&(NetUARTData.pTelnetData->uip_conn_last==uip_conn) ) )
	{
		IP210RegWrite(REG_ETH_TYPE_OFFSET, VLAN_TAG);
		value = IP210RegRead(REG_ETH_TYPE_OFFSET);
		return 0;
	}
	else
#endif
	{
		if(IP210RegRead(REG_ETH_TYPE_OFFSET))
			{ return pkt_802_3; }
		
		if( *(tx_buf+ETHER_TYPE_OFFSET)==0x08 && *(tx_buf+ETHER_TYPE_OFFSET+1)==0x00)
		  {
			if( *(tx_buf+IP_TYPE_OFFSET)==0x06 )
			  {return pkt_TCP_IPv4;}
			if( *(tx_buf+IP_TYPE_OFFSET)==0x11 )
			  {return pkt_UDP_IPv4;}
			if( *(tx_buf+IP_TYPE_OFFSET)==0x01 )
			  {return pkt_ICMP_IPv4;}
		  }
		 else if( *(tx_buf+ETHER_TYPE_OFFSET)==0x86 && *(tx_buf+ETHER_TYPE_OFFSET+1)==0xdd)
		  {return pkt_ICMP_IPv6;}	//add from v1.15
		return pkt_802_3;
	}
}
#else//#ifndef FIRMWARE_UPDATE
u8_t check_packet_type(u8_t	*tx_buf)
{
	if(IP210RegRead(REG_ETH_TYPE_OFFSET))
	  return pkt_802_3;
	
	if( *(tx_buf+ETHER_TYPE_OFFSET)==0x08 && *(tx_buf+ETHER_TYPE_OFFSET+1)==0x00)
	  {
		if( *(tx_buf+IP_TYPE_OFFSET)==0x06 )
		  {return pkt_TCP_IPv4;}
		if( *(tx_buf+IP_TYPE_OFFSET)==0x11 )
		  {return pkt_UDP_IPv4;}
		if( *(tx_buf+IP_TYPE_OFFSET)==0x01 )
		  {return pkt_ICMP_IPv4;}
	  }
	 else if( *(tx_buf+ETHER_TYPE_OFFSET)==0x86 && *(tx_buf+ETHER_TYPE_OFFSET+1)==0xdd)
	  {return pkt_ICMP_IPv6;}	//add from v1.15
	return pkt_802_3;
}
#endif


/******************************************************************************
*
*  Function:    EthernetSend
*
*  Description: Starting Packets send function
*               Notice:current version will not wait Tx complete
*  Parameters:  *tx_buf:	pointer of Tx buffer starting address
*				plen:		packet length
*               
*  Returns:     Tx result
*				0x01:	TRUE
*				0x00:	FALSE
*               
*******************************************************************************/
#ifndef FIRMWARE_UPDATE
#ifdef DB_FIFO
extern u8_t rx_over;
#endif
u8_t EthernetSend(u8_t *tx_buf, s16_t plen)
{
	u8_t ptype=0, chr=0;
	u8_t *send_buf=tx_buf;
#ifdef MODULE_VLAN_TAG
	u8_t tag_buf[4]={0x81, 0, 0, 0};
	u8_t f_tag=0, value=0;
	if( (tx_buf[4]==0x81)&&(tx_buf[5]==0x0) )
	{
		send_buf=&tx_buf[4];
		plen+=4;
		uip_len+=4;
		f_tag=1;
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r53");rx_over=0;}
#endif
#endif
			memcpy(tag_buf, send_buf, 4);
			memcpy(send_buf, &send_buf[4], 12);
			memcpy(&send_buf[12], tag_buf, 4);
	}
	else
	{
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\rF");rx_over=0;}
#endif
#endif
		if((NetUARTData.pTelnetData->uip_conn_last==uip_conn)&&(TelnetData.ConnectStatus==tcConnected))
		{
			if(NetUARTData.pTelnetData->t_vlan_flag)
			{
				plen+=4;
				uip_len+=4;
				f_tag=1;
				memcpy(&tag_buf[2], ModuleInfo.VLANINFO.value_id, 2);
				memcpy(send_buf, &send_buf[4], 12);
				memcpy(&send_buf[12], tag_buf, 4);
			}
			else
			{ send_buf=&send_buf[4]; }
		}
		else if((NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_CLIENT)&&(udp_send_pkt==1)||
			    (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode>NU_OP_MODE_CLIENT)&&(udp_send_pkt==1)){
			plen+=4;
			uip_len+=4;
			f_tag=1;
			memcpy(tag_buf, send_buf, 4);
			memcpy(send_buf, &send_buf[4], 12);
			memcpy(&send_buf[12], tag_buf, 4);
			udp_send_pkt=0;
		}
		else
		{ send_buf=&send_buf[4]; }
	}
#endif
	if(  IP210RegRead(Tx_Current*4 + REG_TXD_START+3)& TFD_START )
	  { return	FALSE;}
	if(plen <8 )
	  { return	FALSE;}
	ptype=check_packet_type(send_buf);

#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\rC");rx_over=0;}
#endif
#endif
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+0, (u8_t)((u16_t)send_buf &0xff)  );	//PrtL
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+1, (u8_t)((u16_t)send_buf >>8	)  );	//PtrH
//	IP210RegWrite16(Tx_Current*4+ REG_TXD_START+0, (u16_t)tx_buf  );		//
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+2, (u8_t)(plen&0xff	)  );	//LenL
	chr=(plen>>8);
	chr&= TFD_LEN_H;
	chr|= (ptype<<3);
	chr|=TFD_START;
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+3, chr );		//LenH & Control_Flag


	//Timer[Tx_Current]=Timer;	//get current timer 
	TFD_index();				//define use which TFDs

#ifdef MODULE_VLAN_TAG	//Write back to Zero
	value = IP210RegRead(REG_ETH_TYPE_OFFSET);
	value = 0;
	IP210RegWrite(REG_ETH_TYPE_OFFSET, value);
	if(f_tag)
	{ vlan_tag_pkt(send_buf, &uip_len); }
	if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode>NU_OP_MODE_CLIENT){
		eth_vlan=0;
		memset(vlan_value, 0, 4);
	}
#endif
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\rD");rx_over=0;}
#endif
#endif
	return	TRUE;
}
#else//#ifndef FIRMWARE_UPDATE
u8_t	EthernetSend(u8_t	*tx_buf,	s16_t plen)
{	u8_t	ptype	,	chr;
	if(  IP210RegRead(Tx_Current*4 + REG_TXD_START+3)& TFD_START )
	  { return	FALSE;}
	if(plen <8 )
	  { return	FALSE;}
	ptype=check_packet_type(tx_buf);
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+0, (u8_t)((u16_t)tx_buf &0xff)  );	//PrtL
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+1, (u8_t)((u16_t)tx_buf >>8	)  );	//PtrH
//	IP210RegWrite16(Tx_Current*4+ REG_TXD_START+0, (u16_t)tx_buf  );		//
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+2, (u8_t)(plen&0xff	)  );	//LenL
	chr=(plen>>8);
	chr&= TFD_LEN_H;
	chr|= (ptype<<3);
	chr|=TFD_START;
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+3, chr );		//LenH & Control_Flag


	//Timer[Tx_Current]=Timer;	//get current timer 
	TFD_index();				//define use which TFDs
	return	TRUE;
}

#endif//#ifndef FIRMWARE_UPDATE

u8_t	EthernetSend2(u8_t	*tx_buf,	s16_t plen)
{	u8_t	ptype=pkt_802_3	,	chr;
	if(  IP210RegRead(Tx_Current*4 + REG_TXD_START+3)& TFD_START )
	  {return	FALSE;}
	if(plen <8 )
	  {return	FALSE;}
//	ptype=pkt_802_3;
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+0, (u8_t)((u16_t)tx_buf &0xff)  );	//PrtL
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+1, (u8_t)((u16_t)tx_buf >>8	)  );	//PtrH
//	IP210RegWrite16(Tx_Current*4+ REG_TXD_START+0, (u16_t)tx_buf  );		//
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+2, (u8_t)(plen&0xff	)  );	//LenL
	chr=(plen>>8);
	chr&= TFD_LEN_H;
	chr|= (ptype<<3);
	chr|=TFD_START;
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+3, chr );		//LenH & Control_Flag


	//Timer[Tx_Current]=Timer;	//get current timer 
	TFD_index();				//define use which TFDs
	return	TRUE;
}

/******************************************************************************
*
*  Function:    reset_tx
*
*  Description: reset TX MAC function
*
*  Parameters:  No
*               
*  Returns:     No
*               
*******************************************************************************/
/*void	reset_tx()
{
	IP210RegWrite(REG_MAC_CONTROL_1, IP210RegRead(REG_MAC_CONTROL_1)| MC1_RESET_TX_MAC  );
	while( IP210RegRead(REG_MAC_CONTROL_1)| MC1_RESET_TX_MAC ){}	//wait TxMAC reset OK
	Tx_Current=0;
	//TxTimer[0]=TxTimer[1]=TxTimer[2]=TxTimer[3]=0;
	IP210RegWrite(REG_MAC_CONTROL_0, IP210RegRead(REG_MAC_CONTROL_0)| MC0_TX_ENABLE );
}
*/
/******************************************************************************
*
*  Function:    monitor_Tx_MAC
*
*  Description: Function to check if TX MAC need reset
*
*  Parameters:  No
*               
*  Returns:     No
*               
*******************************************************************************/
/*void	monitor_Tx_MAC()
{	u8_t	n=0,reset_mac=FALSE;

	while(!reset_mac && n<4)
	  {
	  	if(IP210RegRead(n*4 + REG_TXD_START+3)&TFD_START)
		  {
//		  	if( (Timer-Timer[n]) > Tx_Time_out )
			  {	reset_mac=TRUE;	}
		  }
	  	n++;
	  }
	if(reset_mac)
	  {	reset_tx();	}
}*/

/******************************************************************************
*  Function:    EthernetReceiveLookAhead
*  Description: Function to get Rx Data Buffer pointer
*  Parameters:  u16_t* len	packet length pointer
*  Returns:     u8_t*
*				xxxx: Current Rx Packet point
*				0000: No Rx packet
*******************************************************************************/
u8_t*	EthernetReceiveLookAhead(u16_t* len)
{	if(!(IP210RegRead( Read_Point)& RX_VALID) )
		{	return 0; 					}
	  else
	  	{	u16_t	pkt_addr=Read_Point;
	  		u16_t	packet_len=IP210RegRead( pkt_addr ) ;
			packet_len&= RX_LEN_H;
			packet_len<<=8;
			if(pkt_addr!=RX_BUF_END)
			  {	packet_len|=IP210RegRead( pkt_addr+1);	}
			 else
			  {	packet_len|=IP210RegRead( RX_BUF_START);}
	  		*len=packet_len;
	  		return (u8_t*)(Read_Point+HEADER_LEN);	
	  	}
}

u16_t RWmismatch=0;
void check_MAC_status(void)
{
	if( IP210RegRead16(REG_RX_READ_PTR_L)!=IP210RegRead16(REG_RX_WRITE_PTR_L) )
	{	//printf("\r\n[%d]R/W point MisMatch...R:%04x r:%04x W:%04x ",(u16_t)RWmismatch,IP210RegRead16(REG_RX_READ_PTR_L),Read_Point,IP210RegRead16(REG_RX_WRITE_PTR_L));
		RWmismatch++;
	}
	if(RWmismatch>6000)
	{	//printf("\r\n[%d]R/W point MisMatch...R:%04x r:%04x W:%04x ",(u16_t)RWmismatch,IP210RegRead16(REG_RX_READ_PTR_L),Read_Point,IP210RegRead16(REG_RX_WRITE_PTR_L));
	  	printf("\n\rMac error or too much collision");
		WDT_DIS=0;	WDTEN=1;
		while(1){}
	}
}

/******************************************************************************
*
*  Function:    EthernetReceive
*
*  Description: Packets receive function
*               
*  Parameters:  *rx_buf:	pointer of Rx buffer starting address
*               
*  Returns:     plen:		receive packet length
*				0:			No packet received
*				!0 value:	length of received packet
*               
*******************************************************************************/
//#ifndef MODULE_VLAN_TAG
//sfr P4   = 0xE8;
//#endif
//sbit p45= P4^5;
s16_t	EthernetReceive(u8_t	*rx_buf)
{	u16_t	packet_len,pkt_addr;
	u8_t	p_type;
	if(!(IP210RegRead( Read_Point)& RX_VALID) ){
		check_MAC_status();
		return FALSE; 
	}
	RWmismatch=0;
	//pkt_addr=IP210RegRead16(REG_RX_READ_PTR_L);
	pkt_addr=Read_Point;
	packet_len=IP210RegRead( pkt_addr ) ;
	p_type= (packet_len & RX_PKT_TYPE)>>3;//get packet type
	packet_len&= RX_LEN_H;
	packet_len<<=8;
	if(pkt_addr!=RX_BUF_END)
	  {	packet_len|=IP210RegRead( pkt_addr+1);	}
	 else
	  {	packet_len|=IP210RegRead( RX_BUF_START);}
//------------------------------------------------------------------
	pkt_addr+= HEADER_LEN;
	//DMA action 
	if( (pkt_addr+packet_len) > RX_BUF_END)
	  {	s16_t fst_len=RX_BUF_END+1-pkt_addr   ;s16_t sec_len; 	
	  	u8_t offset=0;
		if(fst_len<0){fst_len=0;}
	  	sec_len=packet_len-fst_len;
		if(fst_len)
		  {IP210_DMA( pkt_addr, (u16_t)rx_buf ,  fst_len  ,BIT_CMD_MODE_M2M);}
		 else
		  { offset=pkt_addr-RX_BUF_END-1; }
		IP210_DMA( RX_BUF_START+offset, (u16_t)(rx_buf+fst_len),sec_len,BIT_CMD_MODE_M2M); 
	  }
	 else
	  { IP210_DMA( pkt_addr, (u16_t)rx_buf ,  packet_len  ,BIT_CMD_MODE_M2M); }
/// mark dma moving for debug 20060430

	pkt_addr+=(packet_len);
  	if( pkt_addr >RX_BUF_END)
	  {
	   pkt_addr-=(RX_BUF_END-RX_BUF_START+1);//v1.15
	   turn_around++; turn_around%=2;
	  }

	Read_Point=pkt_addr;
	if(turn_around)
	  { pkt_addr|= (RX_BUF_END+1);  }//modify from v1.15
	IP210RegWrite16(REG_RX_READ_PTR_L, pkt_addr );	
//if( (packet_len>0x5ea)||(packet_len<60) ) { p45=0; }
	return	packet_len;
}

/******************************************************************************
*  Function:    EthernetReceiveSkip
*  Description: Packets receive function
*  Parameters:  no
*  Returns:     plen:		skip packet length
*				0:			No skip packet
*				!0 value:	length of skip packet
*******************************************************************************/
s16_t	EthernetReceiveSkip()
{	u16_t	packet_len,pkt_addr;
	if(!(IP210RegRead( Read_Point)& RX_VALID) )
		{   return FALSE; 		}
	//pkt_addr=IP210RegRead16(REG_RX_READ_PTR_L);
	pkt_addr=Read_Point;
	packet_len=IP210RegRead( pkt_addr ) ;
	packet_len&= RX_LEN_H;
	packet_len<<=8;
	if(pkt_addr!=RX_BUF_END)
	  {	packet_len|=IP210RegRead( pkt_addr+1);	}
	 else
	  {	packet_len|=IP210RegRead( RX_BUF_START);}

	pkt_addr+=(packet_len+ HEADER_LEN);
  	if( pkt_addr >RX_BUF_END)
	  {
	   pkt_addr-=(RX_BUF_END-RX_BUF_START+1);//v1.15
	   turn_around++; turn_around%=2;
	  }

	Read_Point=pkt_addr;
	if(turn_around)
	  { pkt_addr|= (RX_BUF_END+1);  }//modify from v1.15
	IP210RegWrite16(REG_RX_READ_PTR_L, pkt_addr );	
	return	packet_len;
}

/******************************************************************************
*  Function:    EthernetReceiveNoDMA
*  Description: Packets receive function
*  Parameters:  *rx_buf:	pointer of Rx buffer starting address
*  Returns:     plen:		receive packet length
*				0:			No packet received
*				!0 value:	length of received packet
*******************************************************************************/
#ifdef USE_NO_DMA
s16_t	EthernetReceiveNoDMA(u8_t	*rx_buf)
{	u16_t	packet_len,pkt_addr;
	u8_t	p_type;
	if(!(IP210RegRead( Read_Point)& RX_VALID) )
		{ //check_MAC_status();
		  return FALSE; 
		}
	RWmismatch=0;
	//pkt_addr=IP210RegRead16(REG_RX_READ_PTR_L);
	pkt_addr=Read_Point;
	packet_len=IP210RegRead( pkt_addr ) ;
	p_type= (packet_len & RX_PKT_TYPE)>>3;//get packet type
	packet_len&= RX_LEN_H;
	packet_len<<=8;
	if(pkt_addr!=RX_BUF_END)
	  {	packet_len|=IP210RegRead( pkt_addr+1);	}
	 else
	  {	packet_len|=IP210RegRead( RX_BUF_START);}
//------------------------------------------------------------------
	pkt_addr+= HEADER_LEN;
	//DMA action 
	if( (pkt_addr+packet_len) > RX_BUF_END)
	  {	s16_t fst_len=RX_BUF_END+1-pkt_addr   ;s16_t sec_len; 	
	  	u8_t offset=0;
		if(fst_len<0){fst_len=0;}
	  	sec_len=packet_len-fst_len;
		if(fst_len)
		  {memcpy( (u8_t*)pkt_addr, rx_buf ,  fst_len  );}
		 else
		  { offset=pkt_addr-RX_BUF_END-1; }
		memcpy( (u8_t*)RX_BUF_START+offset, rx_buf+fst_len,sec_len); 
	  }
	 else
	  { memcpy( (u8_t*)(pkt_addr), rx_buf ,  packet_len  ); }
/// mark dma moving for debug 20060430

	pkt_addr+=(packet_len);
  	if( pkt_addr >RX_BUF_END)
	  {
	   pkt_addr-=(RX_BUF_END-RX_BUF_START+1);//v1.15
	   turn_around++; turn_around%=2;
	  }

	Read_Point=pkt_addr;
	if(turn_around)
	  { pkt_addr|= (RX_BUF_END+1);  }//modify from v1.15
	IP210RegWrite16(REG_RX_READ_PTR_L, pkt_addr );	
//if( (packet_len>0x5ea)||(packet_len<60) ) { p45=0; }
	return	packet_len;
}
#endif



/******************************************************************************
*  Function:    EthernetReceiveFreeSpace
*  Description: Function to get Rx free space
*  Parameters:  No
*  Returns:     s16_t
*				xxxx: Current free space
*******************************************************************************/
u16_t EthernetReceiveFreeSpace()
{	u16_t WritePtr=IP210RegRead16(REG_RX_WRITE_PTR_L);
	if(WritePtr==Read_Point)
	  {	return 0;	}
	 else if(Read_Point>WritePtr)
	  {	return Read_Point-WritePtr;	}
	 else
	  {	return ((RX_BUF_END-RX_BUF_START)-(WritePtr-Read_Point));	}
}

/*
s16_t	EthernetReceiveHeader(u8_t	*rx_buf,u16_t *header)
{	u16_t	packet_len,pkt_addr;
	u8_t	p_type;
	if(!(IP210RegRead( Read_Point)& RX_VALID) )
		{ return FALSE; }
	//pkt_addr=IP210RegRead16(REG_RX_READ_PTR_L);
	pkt_addr=Read_Point;
	packet_len=IP210RegRead( pkt_addr ) ;
	p_type= (packet_len & RX_PKT_TYPE)>>3;//get packet type
*header=packet_len;		*header<<=8;
	packet_len&= RX_LEN_H;
	packet_len<<=8;
	if(pkt_addr!=RX_BUF_END)
	  {	packet_len|=IP210RegRead( pkt_addr+1);	}
	 else
	  {	packet_len|=IP210RegRead( RX_BUF_START);}
*header|=(packet_len&0xff);
//------------------------------------------------------------------

	//DMA action 
	IP210_DMA( pkt_addr+HEADER_LEN, (u16_t)rx_buf ,  packet_len  ,BIT_CMD_MODE_M2M);
	IP210_DMA( (pkt_addr+HEADER_LEN>RX_BUF_END?pkt_addr+HEADER_LEN-RX_BUF_START:pkt_addr+HEADER_LEN)
			, (u16_t)rx_buf ,  packet_len  ,BIT_CMD_MODE_M2M);
/// mark dma moving for debug 20060430

	pkt_addr+=(packet_len+ HEADER_LEN);
  	if( pkt_addr >RX_BUF_END)
	  {
	   pkt_addr-=(RX_BUF_END-RX_BUF_START+1);//v1.15
	   turn_around++; turn_around%=2;
	  }

	Read_Point=pkt_addr;
	if(turn_around)
	  { pkt_addr|= (RX_BUF_END+1);  }//modify from v1.15
	IP210RegWrite16(REG_RX_READ_PTR_L, pkt_addr );	
//if( (packet_len>0x5ea)||(packet_len<60) ) { p45=0; }
	return	packet_len;
}
*/

/******************************************************************************
*
*  Function:    IP210_CheckSum
*
*  Description: Using MAC DMA to do packet checksum
*               
*  Parameters:  *addr:	pointer of packet starting address
*               plen:	packet length
*
*  Returns:     No
*               
*******************************************************************************/
/*void	IP210_CheckSum(u8_t *addr,u16_t len)
{	if(len<60){return;}
	IP210RegWrite16(REG_DMA_SOURCE_ADDRESS_L, ((u16_t)addr ));
	IP210RegWrite16(REG_DMA_DESTINATION_ADDRESS_L, ((u16_t)addr ));
	IP210RegWrite16(REG_DMA_LENGTH_L,len);
		
	IP210RegWrite(REG_DMA_COMMAND, (pkt_TCP_IPv4<<4)|BIT_CMD_MODE_CHECKSUM);

}*/


/******************************************************************************
*
*  Function:    uip_ipchecksum
*
*  Description: Packet checksum function, utilitys for uip.
*				
*               
*  Parameters:  plen:	packet length
*				(*addr:	Global uip packet starting address)
*
*  Returns:     checksum result
*				0xffff	:normal packet
*				other	:if not normal packet
*						 the ckecksum value will fill in packet	
*               
*******************************************************************************/
/*u16_t	uip_ipchecksum(u16_t len)
{
	if(!IP210RegRead(REG_ETH_TYPE_OFFSET))
	  return 0xffff;
//	IP210_DMA((u16_t)UIP_BUFFER ,(u16_t)UIP_BUFFER, 1,(pkt_TCP_IPv4<<4)|BIT_CMD_MODE_CHECKSUM);
	IP210_CheckSum(UIP_BUFFER,len);
}

#define uip_tcpchecksum(x)		uip_ipchecksum(x)

*/
/******************************************************************************
*
*  Function:    uip_add32
*
*  Description: 32-bit addition fumction
*               
*  Parameters:  *op32:	pointer of original value
*				op16	operation value
*
*  Returns:     No
*				(save back to *op32)
*               
*******************************************************************************/
void uip_add32(u8_t *op32, u16_t op16)
{
  
  uip_acc32[3] = op32[3] + (op16 & 0xff);
  uip_acc32[2] = op32[2] + (op16 >> 8);
  uip_acc32[1] = op32[1];
  uip_acc32[0] = op32[0];
  
  if(uip_acc32[2] < (op16 >> 8)) {
    ++uip_acc32[1];    
    if(uip_acc32[1] == 0) {
      ++uip_acc32[0];
    }
  }
  
  
  if(uip_acc32[3] < (op16 & 0xff)) {
    ++uip_acc32[2];  
    if(uip_acc32[2] == 0) {
      ++uip_acc32[1];    
      if(uip_acc32[1] == 0) {
	++uip_acc32[0];
      }
    }
  }
}
/*void	uip_add32(u8_t* op32,u16_t op16)
{
	(u32_t)*uip_acc32 = *op32 + op16;
}*/

void wait_packet_sent()
{
   do
   {
   	WatchDogTimerReset();
   }while(IP210RegRead(Tx_bak*4 + REG_TXD_START+3)&TFD_START);
}


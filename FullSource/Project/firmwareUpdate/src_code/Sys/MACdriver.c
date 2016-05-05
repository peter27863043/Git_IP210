/****************************************************************************
*
*	Name:			IP210 MACdriver.C
*
*	Description:	MAC Driver
*
*	Copyright:		(c) 2005-2050    IC Plus Crop.
*	                All rights reserved.	by DeNa
*
*	Version:		v1.16
****************************************************************************/
#include <stdio.h>
#include <string.h>

#include "type_def.h"
#include "fm_web_com.h"
#include "kernelsetting.h"
#define	UIP_BUFFER	uip_buf
#define uip_tcpchecksum(x)		uip_ipchecksum(x)
u16_t	Read_Point;//=RX_BUF_START; //movidy from v1.15
u8_t	Tx_Current=0;
u8_t	TxTime[4];
u8_t	turn_around=0;
u8_t RWmismatch=0;	// add from v1.16 , use for count R/W point mismatch times
extern u8_t IP210_DMA(u16_t src_addr, u16_t des_addr, u16_t length, u8_t cmd);
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
{	
	u8_t i;

	for(i=0;i<6;i++){
		IP210RegWrite( REG_MAC_ADDR+i,	KSetting_Info.netif->MACID[i] );
	}
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
{	
	u8_t i;
	for(i=0;i<4;i++){
		IP210RegWrite( REG_IP_ADDR+i,	KSetting_Info.netif->IPAddress[i] );
	}
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
{	
	u8_t MAC_Setting=IP210RegRead(REG_MAC_CONTROL_0 );
	
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
void MAC_init()
{
	u8_t reg_value;
	
	Resize_RX_Buf(0x50);
  	//chance 20100510 add 
	IP210RegWrite16(REG_RX_READ_PTR_L,Read_Point);//20090828
	IP210RegWrite16(REG_RX_WRITE_PTR_L,Read_Point);//20090828
  	//-------------------------------------------------------------  
	IP210RegWrite( REG_MAC_CONTROL_0 ,
	MC0_SPEED100 | MC0_DUPLEX_F | MC0_TX_ENABLE | MC0_RX_ENABLE | MC0_FLOW_CTRL_EN | MC0_MAX_FRAME_1536 	);

	//IP210RegWrite( REG_RX_FILTER_0 , RF0_ALL_EN);
	IP210RegWrite( REG_RX_FILTER_0 , RF0_MY_MAC_EN | RF0_MSCT_EN | RF0_BSCT_EN | RF0_PAUSE_EN);
	IP210RegWrite( REG_RX_FILTER_1 , 0x0 );

    if(KSetting_Info.PHY_Setting==KS_USE_INTERNAL_PHY)
    {   
        reg_value=IP210RegRead(REG_CHIP_CONFIGURE_0);
//        reg_value=(reg_value&0xef)|0x80;
	    reg_value=(reg_value&0x20)|0x87;
        IP210RegWrite( REG_CHIP_CONFIGURE_0, reg_value);//use internal PHT and enable LED Mode(bit7)
    }
	Read_Point=((u16_t)IP210RegRead(REG_RX_START)<<8); //add from v1.15
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
void TFD_index(void)
{
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
u8_t check_packet_type(u8_t	*tx_buf)
{
	if(IP210RegRead(REG_ETH_TYPE_OFFSET))
		return pkt_802_3;
	if( *(tx_buf+ETHER_TYPE_OFFSET)==0x08 && *(tx_buf+ETHER_TYPE_OFFSET+1)==0x00){
		if( *(tx_buf+IP_TYPE_OFFSET)==0x06 ){
			return pkt_TCP_IPv4;
		}
		if( *(tx_buf+IP_TYPE_OFFSET)==0x11 ){
			return pkt_UDP_IPv4;
		}
		if( *(tx_buf+IP_TYPE_OFFSET)==0x01 ){
			return pkt_ICMP_IPv4;
		}
	}
	else if( *(tx_buf+ETHER_TYPE_OFFSET)==0x86 && *(tx_buf+ETHER_TYPE_OFFSET+1)==0xdd){
		return pkt_ICMP_IPv6;
	}	//add from v1.15
	return pkt_802_3;
}

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
u8_t EthernetSend(u8_t *tx_buf, s16_t plen)
{	
	u8_t ptype, chr;
	if(  IP210RegRead(Tx_Current*4 + REG_TXD_START+3)& TFD_START ){
		return	FALSE;
	}
	if(plen <8 ){
		return	FALSE;
	}
	ptype=check_packet_type(tx_buf);
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+0, (u8_t)((u16_t)tx_buf &0xff)  );	//PrtL
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+1, (u8_t)((u16_t)tx_buf >>8	)  );	//PtrH
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+2, (u8_t)(plen&0xff	)  );	//LenL
	chr=(plen>>8);
	chr&= TFD_LEN_H;
	chr|= (ptype<<3);
	chr|=TFD_START;
	IP210RegWrite( Tx_Current*4 + REG_TXD_START+3, chr );		//LenH & Control_Flag
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
void reset_tx()
{
	IP210RegWrite(REG_MAC_CONTROL_1, IP210RegRead(REG_MAC_CONTROL_1)| MC1_RESET_TX_MAC  );
	while( IP210RegRead(REG_MAC_CONTROL_1)| MC1_RESET_TX_MAC ){}	//wait TxMAC reset OK
	Tx_Current=0;
	IP210RegWrite(REG_MAC_CONTROL_0, IP210RegRead(REG_MAC_CONTROL_0)| MC0_TX_ENABLE );
}

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
void monitor_Tx_MAC()
{	
	u8_t n=0,reset_mac=FALSE;

	while(!reset_mac && n<4)
	{
	  	if(IP210RegRead(n*4 + REG_TXD_START+3)&TFD_START)
		{
			reset_mac=TRUE;
		}
	  	n++;
	}
	if(reset_mac){
		reset_tx();
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
s16_t	EthernetReceive(u8_t	*rx_buf)
{	
	u16_t	packet_len,pkt_addr;
	u8_t	p_type;

	if(!(IP210RegRead( Read_Point)& RX_VALID) ){
		return FALSE; 
	}
	RWmismatch=0;
	pkt_addr=Read_Point;
	packet_len=IP210RegRead( pkt_addr ) ;
	p_type= (packet_len & RX_PKT_TYPE)>>3;//get packet type
	packet_len&= RX_LEN_H;
	packet_len<<=8;
	if(pkt_addr!=RX_BUF_END){
		packet_len|=IP210RegRead( pkt_addr+1);
	}
	else{
		packet_len|=IP210RegRead( RX_BUF_START);
	}

	pkt_addr+= HEADER_LEN;

	if( (pkt_addr+packet_len) > RX_BUF_END){
		s16_t fst_len=RX_BUF_END+1-pkt_addr;
		s16_t sec_len; 	
	  	u8_t offset=0;

		if(fst_len<0){fst_len=0;}
	  	sec_len=packet_len-fst_len;
		if(fst_len){
			IP210_DMA( pkt_addr, (u16_t)rx_buf ,  fst_len  ,BIT_CMD_MODE_M2M);
		}
		else{
			offset=pkt_addr-RX_BUF_END-1;
		}
		IP210_DMA( RX_BUF_START+offset, (u16_t)(rx_buf+fst_len),sec_len,BIT_CMD_MODE_M2M); 
	}
	else
	{ IP210_DMA( pkt_addr, (u16_t)rx_buf ,  packet_len  ,BIT_CMD_MODE_M2M); }

	pkt_addr+=(packet_len);
  	if( pkt_addr >RX_BUF_END){
	   pkt_addr-=(RX_BUF_END-RX_BUF_START+1);//v1.15
	   turn_around++; turn_around%=2;
	}
	Read_Point=pkt_addr;
	if(turn_around){
		pkt_addr|= (RX_BUF_END+1);  
	}//modify from v1.15
	IP210RegWrite16(REG_RX_READ_PTR_L, pkt_addr );
	return	packet_len;
}
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
void IP210_CheckSum(u8_t *addr,u16_t len)
{	
	IP210RegWrite16(REG_DMA_SOURCE_ADDRESS_L, ((u16_t)addr ));
	IP210RegWrite16(REG_DMA_DESTINATION_ADDRESS_L, ((u16_t)addr ));
	IP210RegWrite16(REG_DMA_LENGTH_L,len);
	IP210RegWrite(REG_DMA_COMMAND, (pkt_TCP_IPv4<<4)|BIT_CMD_MODE_CHECKSUM);
}
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
u16_t	uip_ipchecksum(u16_t len)
{
	if(!IP210RegRead(REG_ETH_TYPE_OFFSET)){
		return 0xffff;
	}
	IP210_CheckSum(UIP_BUFFER,len);
}
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



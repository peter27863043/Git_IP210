#include <stdio.h>
#include <string.h>
#include "type_def.h"
#include "image_mode.h"
#include "net_uart.h"
#include "vtag.h"
#include "udp.h"

u8_t eth_vlan=0;
u8_t vlan_value[4]={0};

void Check_Vlan_Tag(){
	if(ModuleInfo.VLANINFO.vlan==1)	//Enable VLAN Tag
	{
#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
#ifdef DB_FIFO
		if(rx_over){printf("\n\r7");rx_over=0;}
#endif
#endif
		if( (eth_vlan_buf[16]==0x81)&&(eth_vlan_buf[17]==0x00) )
		{
			vlan_tag_pkt(&eth_vlan_buf[4], &uip_len);
			uip_buf_real=&eth_vlan_buf[8];
			uip_buf=&eth_vlan_buf[8];
			eth_vlan=1;
		}
		else
		{
			uip_buf_real=&eth_vlan_buf[4];
			uip_buf=&eth_vlan_buf[4];
		}
		re_init();
	}
}

u8_t *vlan_tag_pkt(u8_t *input_buf, u16_t *len_ptr)
{
	u8_t tag_buf[16]=0;
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r21");rx_over=0;}
#endif
#endif
	memcpy(tag_buf, &input_buf[12], 4);
	memcpy(vlan_value, tag_buf, 4); //remember vlan tag value
	memcpy(&tag_buf[4], input_buf, 12);
	memcpy(input_buf, tag_buf, 16);
	*len_ptr-=4;
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r22");rx_over=0;}
#endif
#endif
	return &input_buf[4];
}

u8_t EnVlanCheck()
{
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r23");rx_over=0;}
#endif
#endif
	if(ModuleInfo.VLANINFO.vlan==1)
	{
			if( (vlan_value[2]!=ModuleInfo.VLANINFO.value_id[0])||(vlan_value[3]!=ModuleInfo.VLANINFO.value_id[1]) )
			{ return 0; }
	}
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r24");rx_over=0;}
#endif
#endif
	return 1;
}

void re_init()
{
	restore_uip_buf();
	WriteEthRx2UART();
	udp_init(&uip_buf[UIP_LLH_LEN],&uip_len,uip_hostaddr);
	if( (NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_SERVER)||
	(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_CLIENT) )
	{
		NetUARTData.pkt_rx_data_offset=uip_appdata;
		NetUARTData.pkt_tx_data_offset=uip_appdata;
	}
	else
	{
		NetUARTData.pkt_rx_data_offset=UdpData.p_DataPkt->udpload;
		NetUARTData.pkt_tx_data_offset=UdpData.p_DataPkt->udpload;
	}
}

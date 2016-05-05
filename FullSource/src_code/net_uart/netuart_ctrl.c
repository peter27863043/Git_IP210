#include "module.h"
#include "public.h"
#include "netuart_ctrl.h"
#include "uip.h"
#include "string.h"
#include "eepdef.h"
#include "net_uart.h"
#include "timer.h"
#ifdef MODULE_NET_UART_CTRL
#ifndef MODULE_NET_UART
extern SNetUARTData NetUARTData;
#endif
//---------------Server Connect-------
//#define DB_NUC
extern u8_t conn_flag;
extern u32_t conn_timer;
//------------------------------------
netuart_ctrl_t netuart_ctrl;
NUC_signal Send_signal;
//------------------------------------------------------------------------------
void nuc_init()
{
	if(NetUARTData.pTelnetData->pEEPROM_Telnet->ctr_en==1){
		if(NetUARTData.pTelnetData->pEEPROM_Telnet->op_mode==NU_OP_MODE_CLIENT)
			netuart_ctrl.mode=NUC_CLIENT;
		else
			netuart_ctrl.mode=NUC_SERVER;
	}else
		netuart_ctrl.mode=NUC_DISABLE;

	netuart_ctrl.status=NUC_IDLE;
	netuart_ctrl.ack=0;
	netuart_ctrl.on_change=0;

	Send_signal.ri=0;
	Send_signal.dcd=0;
	Send_signal.cts=0;
	Send_signal.dsr=0;
}

void nuc_send()
{
	pNUC_signal NUC_signal=(pNUC_signal)uip_appdata;

	netuart_ctrl.on_change=0;
	netuart_ctrl.ack=1;

	NUC_signal->ri=Send_signal.ri;
	NUC_signal->dcd=Send_signal.dcd;
	NUC_signal->cts=Send_signal.cts;
	NUC_signal->dsr=Send_signal.dsr;

	uip_len=sizeof(Send_signal)-1;
	uip_slen=uip_len;

	#ifdef DB_NUC
	printf("\n\rSend Signal Change:");
	printf("\n\rCTS=%d",(u16_t)Send_signal.cts);
	printf("\n\rDSR=%d",(u16_t)Send_signal.dsr);
	if(TelnetData.pEEPROM_UART->rd_io==1){
		printf("\n\rRI=%d",(u16_t)Send_signal.ri);
		printf("\n\rDCD=%d",(u16_t)Send_signal.dcd);
	}
	#endif
}

void nuc_process(){
	u8_t tmpvalue=IP210RegRead(REG_UART_MODEM_CONTROL);
	pNUC_signal Rcv_signal=(pNUC_signal)uip_appdata;

	tmpvalue&=~0x03;
	if(Rcv_signal->cts==0)
		tmpvalue|=0x02;
	if(Rcv_signal->dsr==0)
		tmpvalue|=0x01;
	IP210RegWrite(REG_UART_MODEM_CONTROL, tmpvalue);

	#ifdef DB_NUC
	printf("\n\rSet Signal:");
	printf("\n\rRTS=");
	printf((tmpvalue&0x02)?"1":"0");
	printf("\n\rDTR=");
	printf((tmpvalue&0x01)?"1":"0");
	#endif

	if(TelnetData.pEEPROM_UART->rd_io==0){
		if(Rcv_signal->ri==1)
			RI_NUC_O=1;
		else
			RI_NUC_O=0;

		if(Rcv_signal->dcd==1)
			DCD_NUC_O=1;
		else
			DCD_NUC_O=0;
		#ifdef DB_NUC
		printf("\n\rRI=%d",(u16_t)RI_NUC_O);
		printf("\n\rDCD=%d",(u16_t)DCD_NUC_O);
		#endif
	}
}
//------------------------------------------------------------------------------
void check_signal_change()
{
	u8_t tmpvalue=IP210RegRead(REG_UART_MODEM_STATUS);

	if(netuart_ctrl.ack==1)
		return;
	tmpvalue=0xFF-tmpvalue;
	if(Send_signal.old!=tmpvalue){
		Send_signal.old=tmpvalue;
		netuart_ctrl.on_change=1;

		if(tmpvalue&0x10)	//CTS
			Send_signal.cts=1;
		else
			Send_signal.cts=0;

		if(tmpvalue&0x20)	//DSR
			Send_signal.dsr=1;
		else
			Send_signal.dsr=0;
	}
	if(TelnetData.pEEPROM_UART->rd_io==1){	//RI,DCD Input
		RI_NUC_I=1;
		if(RI_NUC_I!=Send_signal.ri){		//RI
			Send_signal.ri=RI_NUC_I;
			netuart_ctrl.on_change=1;
		}

		DCD_NUC_I=1;
		if(DCD_NUC_I!=Send_signal.dcd){		//DCD
			Send_signal.dcd=DCD_NUC_I;
			netuart_ctrl.on_change=1;
		}
	}
}
//------------------------------------------------------------------------------
u8_t nuc_on_tcp_rx()
{

	if(netuart_ctrl.mode==NUC_DISABLE)
		return 0;

	if(netuart_ctrl.mode==NUC_SERVER){
		if(uip_conn->lport!=TelnetData.pEEPROM_Telnet->ctrl_port){
			return 0;
		}
	}else{
		if(uip_conn->rport!=TelnetData.pEEPROM_Telnet->ctrl_port){
			return 0;
		}
	}

	if(uip_connected())
	{
		#ifdef DB_NUC
		printf("\n\rNUC Connect");
		#endif
		netuart_ctrl.status=NUC_CONNECT;
		netuart_ctrl.ack=0;
		netuart_ctrl.on_change=0;
	}
	if(uip_acked())
	{
		if(netuart_ctrl.ack==1)
			netuart_ctrl.ack=0;
	}
	if(uip_newdata())
	{
		nuc_process();
	}
	if(uip_rexmit())
	{
		nuc_send();
	}
	if(uip_poll()) 
	{
		check_signal_change();
		if(netuart_ctrl.on_change==1)
			nuc_send();
	}
	if(uip_timedout())
	{
		uip_close();
	}
	if(uip_closed()||uip_aborted()){
		#ifdef DB_NUC
		printf("\n\rNUC Close");
		#endif
		netuart_ctrl.status=NUC_IDLE;
		netuart_ctrl.ack=0;
		netuart_ctrl.on_change=0;
	}
	return 1;
}
//------------------------------------------------------------------------------
void nuc_on_loop()
{
	u16_t ip[2]={0};

	if( (netuart_ctrl.mode!=NUC_DISABLE) && (netuart_ctrl.status!=NUC_CONNECT) ){
		switch(netuart_ctrl.mode){
			case NUC_SERVER:
				if(netuart_ctrl.status!=NUC_TOLISTEN){
					uip_listen(TelnetData.pEEPROM_Telnet->ctrl_port);
					netuart_ctrl.status=NUC_TOLISTEN;
					#ifdef DB_NUC
					printf("\n\rNUC Listen");
					#endif
				}
				break;
			case NUC_CLIENT:
				if(netuart_ctrl.status!=NUC_TOCONN){
					if(tcp_check_ip!=0){
						ip[0]=tcp_check_ip>>16;
						ip[1]=tcp_check_ip&0xFFFF;
						uip_connect(ip, TelnetData.pEEPROM_Telnet->ctrl_port);
						#ifdef DB_NUC
						printf("\n\rNUC Conn:%04x %04x %d",(u16_t)ip[0],(u16_t)ip[1],(u16_t)TelnetData.pEEPROM_Telnet->ctrl_port);
						#endif
						netuart_ctrl.status=NUC_TOCONN;
					}
				}
				break;
			default:
				break;
		}
	}
}
//------------------------------------------------------------------------------

#endif //MODULE_NET_UART_CTRL
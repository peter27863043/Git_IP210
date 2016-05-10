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
extern u8_t conn_flag;
extern u32_t conn_timer;
//extern u8_t heart_flag;
//------------------------------------
netuart_ctrl_t netuart_ctrl;
//------------------------------------------------------------------------------
void nuc_init()
{
	uip_listen(TelnetData.pEEPROM_Telnet->ctrl_port);
}
//------------------------------------------------------------------------------
u8_t nuc_chk_pw(u8_t* pw)
{
	if(strcmp(pw,EEConfigInfo.LoginPassword)==0)return 1;
	return 0;
}
//------------------------------------------------------------------------------
void nuc_send(u8_t op)
{
	u8_t tmp;
	const u8_t parity_value[5]={0,8,24,40,56};
	unc_uart_config_t* conf  =(unc_uart_config_t*)uip_appdata;
	unc_uart_signal_t* signal=(unc_uart_signal_t*)uip_appdata;		
	unc_uart_result_t* result=(unc_uart_result_t*)uip_appdata;
	result->op=op;
	uip_len=0;
	switch(op)
	{
		case 0x02:
			result->result=netuart_ctrl.result;
			uip_len=sizeof(unc_uart_result_t);
			break;
		case 0x04:
		  	conf->baudrate = NetUARTData.pTelnetData->pEEPROM_UART->Baudrate; 	
      		conf->data_size = NetUARTData.pTelnetData->pEEPROM_UART->Character_Bits + 5;	      
      		for(tmp=0;tmp<5;tmp++)
      		{
      			if(parity_value[tmp] == NetUARTData.pTelnetData->pEEPROM_UART->Parity_Type)
      			{
      				conf->parity = tmp;
      			}
      		}
      		conf->stop_size = (NetUARTData.pTelnetData->pEEPROM_UART->Stop_Bit == 0)?0:1;
      		conf->flow_ctrl = (NetUARTData.pTelnetData->pEEPROM_UART->HW_Flow_Control == 1)?2:0;			
     		uip_len=sizeof(unc_uart_config_t);     	
			break;
		case 0x11:
			signal->signal=netuart_ctrl.last_uart_signal;
			uip_len=sizeof(unc_uart_signal_t);
			break;
		case 0x14:
			signal->signal=netuart_ctrl.last_uart_signal;
			uip_len=sizeof(unc_uart_signal_t);			
			break;
		case 0x16:
			result->result=netuart_ctrl.result;
			uip_len=sizeof(unc_uart_result_t);			
			break;
		case 0x22:
			uip_len=1;
			break;
	}
	uip_slen=uip_len;

#ifdef DB_NET_UART_CTRL
{
	u16_t i;
	printf("\n\r ctrl.send:");
	for(i=0;i<uip_len;i++)
	{
		printf("%x,",(u16_t)uip_appdata[i]);
	}
}
#endif //DB_NET_UART_CTRL	
}
//------------------------------------------------------------------------------
u8_t nuc_process_cmd()
{
	u8_t is_my_cmd=1;
	unc_uart_config_t* conf  =(unc_uart_config_t*)uip_appdata;
	unc_uart_signal_t* signal=(unc_uart_signal_t*)uip_appdata;
	unc_uart_result_t* result=(unc_uart_result_t*)uip_appdata;
	                     //none, odd, even, mark, space
	const u8_t parity_value[5]={0,8,24,40,56};
	u8_t tmp;
#ifdef DB_NET_UART_CTRL
{
	u16_t i;
	printf("\n\r ctrl.receive:");
	for(i=0;i<uip_len;i++)
	{
		printf("%x,",(u16_t)uip_appdata[i]);
	}
}
#endif //DB_NET_UART_CTRL
	switch(conf->op)
	{
		case 0x01:
			//UART configure Setting
			netuart_ctrl.result=1;//password error;
//			if(nuc_chk_pw(conf->password))		//joe for new control portocol without password 20081105
//			{
				NetUARTData.pTelnetData->pEEPROM_UART->Baudrate = conf->baudrate;
				UART_Settings.divisor = BaudrateTable[conf->baudrate].Divisor;
				
				NetUARTData.pTelnetData->pEEPROM_UART->Character_Bits = conf->data_size-5;
				UART_Settings.Character_Bits = conf->data_size-5;
				
				NetUARTData.pTelnetData->pEEPROM_UART->Parity_Type = parity_value[conf->parity];
				UART_Settings.Parity_Type = parity_value[conf->parity];
				
				tmp = (conf->stop_size==0)?0:1;
				NetUARTData.pTelnetData->pEEPROM_UART->Stop_Bit = tmp;
				UART_Settings.Stop_Bit = tmp;
				
				tmp = (conf->flow_ctrl==2)?1:0;
				NetUARTData.pTelnetData->pEEPROM_UART->HW_Flow_Control = tmp;
				UART_Settings.HW_Flow_Control = tmp;
				
				UART_Set_Property();
				netuart_ctrl.result=0;
//			}
		  	SaveModuleInfo2EEPROM(TelnetData.pEEPROM_UART,sizeof(SUARTInfo));
			nuc_send(0x02);
			break;
		case 0x03:
			//UART config query
			nuc_send(0x04);
			break;
		case 0x13:
			//UART signal query
			nuc_send(0x14);
			break;
		case 0x15:
			//UART signal set
			netuart_ctrl.result=1;//password error;
//			if(nuc_chk_pw(signal->password))
//			{
				tmp=IP210RegRead(REG_UART_MODEM_CONTROL)&0x0c;
				tmp|=((signal->signal&0x08)?0x0:0x1) | ((signal->signal&0x04)?0x0:0x2);//reverse
				//tmp|=((signal->signal&0x08)?0x1:0x0) | ((signal->signal&0x04)?0x2:0x0);//normal
                //printf("\n\r app set signal=%x,set mcr=%x",(u16_t)signal->signal,(u16_t)tmp);
				IP210RegWrite(REG_UART_MODEM_CONTROL,tmp);
				netuart_ctrl.result=0;
//			}
			nuc_send(0x16);
			break;
		case 0x21:		//Keep Alive
/*
			heart_flag = 1;
			conn_flag = 1;
			IP210_Update_Timer_Counter();
			conn_timer = timercounter;
			IP210_Update_Timer_Counter();
*/
			nuc_send(0x22);
			break;
		default:
			is_my_cmd=0;
			break;
	}
	return is_my_cmd;
}
//------------------------------------------------------------------------------
void check_signal_change_op()
{
	if(netuart_ctrl.event_queue_wait_ack[0]==0)
	{
		netuart_ctrl.event_queue_wait_ack[0]=netuart_ctrl.event_queue[0];
		netuart_ctrl.event_queue[0]=0;
	}
	if(netuart_ctrl.event_queue_wait_ack[0]&0x1)
	{
		nuc_send(0x11);
	}
}
//------------------------------------------------------------------------------
u8_t nuc_on_tcp_rx()
{
	u8_t is_my_cmd;
	if(NetUARTData.pTelnetData->pEEPROM_Telnet->ctr_en==1)
	{
		if(uip_conn->lport!=TelnetData.pEEPROM_Telnet->ctrl_port)
		{
			return 0;
		}
	}
	else
	{	return 0;	}
	if(uip_connected())
	{
//		if(TelnetData.ConnectStatus!=tcConnected)	//Modify by joe if telnet don't connect can't connect control
//		{	uip_close();	}
	}
	if(uip_acked())
	{
		if(netuart_ctrl.event_queue_wait_ack[0])
		{
			netuart_ctrl.event_queue_wait_ack[0]=0;
		}
	}
	if(uip_newdata())
	{
		is_my_cmd=nuc_process_cmd();
	}
	if(uip_rexmit()||uip_newdata())
	{
		if(is_my_cmd==0)
		{
			check_signal_change_op();
		}
	}
	else if(uip_acked())
	{	}
	if(uip_poll()) 
	{
		check_signal_change_op();
	}
	else
	{	}
	if(uip_timedout())
	{
		uip_close();
	}
	if(uip_closed()||uip_aborted())
	{	}
	return 1;
}
//------------------------------------------------------------------------------
void nuc_on_loop()
{
	u8_t msr=IP210RegRead(REG_UART_MODEM_STATUS);
	u8_t old=netuart_ctrl.last_uart_signal;
	msr=0xff-msr;//reverse msr
	//printf("\n\r%x",(u16_t)msr);//jc_db:n2r
	netuart_ctrl.last_uart_signal=( ((u8_t)msr&0x10)|((u8_t)msr&0x20)|(((u8_t)msr&0x40)<<1)|(((u8_t)msr&0x80)>>1) );
    if(old!=netuart_ctrl.last_uart_signal)
	{
#ifdef DB_NET_UART_CTRL
        printf("\n\r HUART Signal changed, old=%x, new=%x",(u16_t)old,(u16_t)netuart_ctrl.last_uart_signal);
#endif //DB_NET_UART_CTRL
		netuart_ctrl.event_queue[0]=0x1;//send op 0x11
	}
}
//------------------------------------------------------------------------------

#endif //MODULE_NET_UART_CTRL
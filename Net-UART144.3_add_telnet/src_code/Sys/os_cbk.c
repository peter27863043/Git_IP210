#include <stdio.h>
#include <string.h>
#include "type_def.h"
#include "os_cbk.h"
#include "timer.h"
#include "eepdef.h"
#include "gpio.h"
#include "telnet.h"
#include "net_uart.h"
#include "CommandLine.h"
#include "udp_tel.h"
#include "image_mode.h"
#include "netuart_ctrl.h"
#include "switch.h"
#include "MDC_MDIO.h"
#include "Application.h"

#ifdef MODULE_FIRMWARE
void CPU_GOTO_FFF7H(void);
u8_t firm_flag=0;
u8_t boot_flag=0;
#endif
u8_t force_reset_countdown=0;
u8_t load_default_pin_pressed=1;
u16_t gateway_count=600;
u16_t firm_count=0;
u16_t reboot_check _at_ 0x7ffe;
void LoadDefault(){

	if (reboot_check==0x1289){
		printf("\n\rLoad Default Setting...need:%d, pin%d reboot=%04x",(u16_t)need_load_default,(u16_t)load_default_pin_pressed,reboot_check);
		EEPROMLoadDefault();
		EEPROMInit();
		need_load_default=0;
		load_default_pin_pressed=0;
		printf("OK");
		force_reset_countdown=3;
		reboot_check=0x1234;
	}

	
	if(need_load_default || load_default_pin_pressed)
	{
		if (reboot_check!=0x1234){
			if(Delta_Time(0L)>500)//5 sec
			{
				printf("\n\rLoad Default Setting...need:%d, pin%d reboot=%04x",(u16_t)need_load_default,(u16_t)load_default_pin_pressed,reboot_check);
				EEPROMLoadDefault();
				EEPROMInit();
				need_load_default=0;
				load_default_pin_pressed=0;
				printf("OK");
				force_reset_countdown=5;
				reboot_check=0x1234;
			}
		}
		if (reboot_check==0x1234)
			load_default_pin_pressed=0;
			
	}
	if((LOAD_DEFAULT_PIN==1)&&(load_default_pin_pressed==1))//not pressed
	{
		load_default_pin_pressed=0;
	}
}

void os_initial(){
#ifndef MODULE_FLASH512
	u16_t tmp=0;
#endif
//RS485
	IP210RegWrite(REG_PAD_CONTROL, 0x3);
	LOAD_DEFAULT_PIN=1;		//(HW > FW) For RS422/485 we pull this pin high
//Check Link
	#ifndef MODULE_FLASH512
	tmp=Read_MDC(PHY_1F, MII_STATUS);
	if( (tmp&0x0004)==0 )
		P3_4=1;
	#endif
	Check_Speed();
}

extern eSCALER_ST m_eScaler_State;
extern eSCALER_ST SendTVSetLANLinkCommand(u8_t GetCmd,u8_t cData);
u8_t link_count;
u8_t Link_Status_keep;

void Check_Link_1Sec(){
	u16_t tmp=0;

	if(m_eScaler_State<=eScaler_RS232_No_Service){
		Link_Status_keep=0xaa;
		return;
	}
	
	if(link_count>10) {
		tmp=Read_MDC(PHY_1F, MII_STATUS);
		if( (tmp&0x0004)!=0 ) {//0:Unlink 1:Link
			if(Link_Status_keep!=1){
				Link_Status_keep=1;
				printf("Link Status 0K\n");
				SendTVSetLANLinkCommand(TV_CMD_LAN_LINK,'1');
				P4_0=1;
			}
		}
		else{
			if(Link_Status_keep!=0){
				Link_Status_keep=0;
				printf("Link Status NG\n");	
				SendTVSetLANLinkCommand(TV_CMD_LAN_LINK,'0');
				P4_0=0;
			}
		}
		link_count=0;
	}
	else
		link_count++;	
}
#ifndef MODULE_FLASH512
void Check_Link(){
	u8_t link_count=0;
	u16_t tmp=0;
	tmp=Read_MDC(PHY_1F, MII_STATUS);
	if( (tmp&0x0004)!=0 ) //0:Unlink 1:Link
	{
		link_count++;			
		if(link_count==10)
		{
			link_count=0;
			tmp=Read_MDC(PHY_1F, MII_CONTROL);	//0:10 Based 1:100 Based
			if( (tmp&0x2000)==0 )
				P3_4=1;
			else
				P3_4=0;
		}
	}
	else
	{
		P3_4=1;
	}
}
#endif

#ifdef MODULE_FIRMWARE
void Check_FirmwareUpdate(){	
	if( (firm_flag==1)||(boot_flag==1) )
	{
		firm_count++;
		if(firm_count>20)
		{
			if(firm_flag==1)
			{	CPU_GOTO_FFF7H();	}
			if(boot_flag==1)
			{	FUU_Run();	}
		}
	}
}
#endif

void Gateway_timer(){
	gateway_count++;
	if(gateway_count>=GATEWAY_RESET_TIME)
	{
			gateway_mac_req();
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rZZ");rx_over=0;}
#endif
#endif
	}
}

void os_loop(){
	#ifdef MODULE_RS485
		check_RS485();
	#endif
	#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
	#ifdef DB_FIFO
		if(rx_over){printf("\n\rG");rx_over=0;}
	#endif
	#endif
	#ifdef MODULE_COMMAND_LINE
		CLI_Timer();
	#endif
	#ifdef MODULE_UDP_TELNET
		UDP_TEL_Timer();
	#endif
	#ifdef MODULE_NET_UART
		telnet_server_timeout();
	#endif
}
u8_t Link_Status_Change=1;
void Check_Speed(){
	u16_t status=0;
	u8_t tmpvalue=0;

	status=Read_MDC(PHY_1F, MII_STATUS);
	if( (status&0x0004)==0 ) //0:Unlink 1:Link
		Link_Status_Change=1;

	if( (Link_Status_Change==1)&&((status&0x0004)!=0) ){
		Link_Status_Change=0;
		//printf("\n\rChange Link Speed...");
		status=Read_MDC(PHY_1F, 0x0);

		tmpvalue=IP210RegRead(REG_MAC_CONTROL_0);
		tmpvalue&=~0x03;

		if( (status&0x2000)!=0 ){
			tmpvalue|=0x01;
			//printf("100Mbps,");
		}else{
			//printf("10Mbps,");
		}
		if( (status&0x0100)!=0){
			tmpvalue|=0x02;
			//printf("Full");
		}else{
			//printf("Half");
		}
		IP210RegWrite(REG_MAC_CONTROL_0, tmpvalue);
	}
}

void os_loop_100ms(){
	
	Check_Speed();
	Check_Link_1Sec();
	#ifndef MODULE_FLASH512
		Check_Link();
	#endif
	#ifdef MODULE_FIRMWARE
		Check_FirmwareUpdate();
	#endif
	#ifdef MODULE_NET_UART_CTRL        	
		nuc_on_loop();        	
	#endif
	#ifdef MODULE_NET_UART
		NET_UART_LOOP('C');
	#ifdef DB_FIFO
		if(rx_over){printf("\n\r14");rx_over=0;}
	#endif
	#endif
	#ifdef MODULE_TELNET_RFC
		rfc_loop();
	#endif
	fiber_cover();
	if(force_reset_countdown){
		if(force_reset_countdown--==1)
			CPU_REBOOT();
	}
}

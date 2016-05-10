/******************************************************************************
*
*   Name:           main.c
*
*   Description:    The Start Program Of C Code
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
//include data
#include <stdio.h>
#include <intrins.h>
#include "uip.h"	
#include "uip_arp.h"
#include "switch.h"
#include "timer.h"
#include "utility.h"
#include "eeprom.h"
#include "MACdriver.h"
#include <string.h>
#include "public.h"
#include "eeprominit.h"
#include "option.h"
#include "gpio.h"
#include "module.h"
#include "eepdef.h"
#include "net_uart.h"
#ifdef FLASH_512
#include "wdg.h"
#endif
//------------------------------------------------------------------------------
// Functions definition
//------------------------------------------------------------------------------
//*************************WDT TEST FUNC
u8_t WDT_TEST_FUNC(u16_t sddr_limit);
//*************************
void application();
extern void set_switch_status();
#ifdef FLASH_512
void FU_Check(void);
#endif
//public data
u8_t MODE_TYPE=0xff;    //Switch Mode=> bit 0:Fibet bit1:MII/RvMII
#define UserModuleInit()				
#ifdef MODULE_UART_AUTO_SEND
	void Timercounter0_Init(void);			
#endif
/******************************************************************************
*
*  Function:    NetworkConfig()
*
*  Description: Setting network infomation from eeprom to system
*               
*  Parameters:  None
*               
*  Returns:     None
*               
*******************************************************************************/
void CatNetworkConfigMsg(char* Msg,char* endl)//Cat NetworkConfigMsg to Msg
{
#ifdef MODULE_FULL_MSG	
	u8_t i;
    sprintf(&Msg[strlen(Msg)],"%s%s",endl,TOP_BANNER);    
    sprintf(&Msg[strlen(Msg)],"%sMAC Address    : ",endl);
	for(i =0 ;i<6;i++)
	{
  		sprintf(&Msg[strlen(Msg)],"%2.2x ",(u16_t)uip_ethaddr.addr[i]);
	}
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C9');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS21");rx_over=0;}
#endif
#endif
#else
	if(endl){}
	if(Msg){}
//    sprintf(&Msg[strlen(Msg)],"%s%sMAC Address    : ",endl,endl);
#endif //MODULE_FULL_MSG  
/*
	for(i =0 ;i<6;i++)
	{
  		sprintf(&Msg[strlen(Msg)],"%2.2x ",(u16_t)uip_ethaddr.addr[i]);
	}
*/
#ifdef MODULE_FULL_MSG	    
        sprintf(&Msg[strlen(Msg)],"%sIP Address      : %d.%d.%d.%d",endl,(u16_t)((u8_t*)uip_hostaddr)[0],(u16_t)((u8_t*)uip_hostaddr)[1],(u16_t)((u8_t*)uip_hostaddr)[2],(u16_t)((u8_t*)uip_hostaddr)[3]);
	    sprintf(&Msg[strlen(Msg)],"%sSubnet Mask     : %d.%d.%d.%d",endl,(u16_t)((u8_t*)uip_arp_netmask)[0],(u16_t)((u8_t*)uip_arp_netmask)[1],(u16_t)((u8_t*)uip_arp_netmask)[2],(u16_t)((u8_t*)uip_arp_netmask)[3]);
	    sprintf(&Msg[strlen(Msg)],"%sGateway Address : %d.%d.%d.%d",endl,(u16_t)((u8_t*)uip_arp_draddr)[0],(u16_t)((u8_t*)uip_arp_draddr)[1],(u16_t)((u8_t*)uip_arp_draddr)[2],(u16_t)((u8_t*)uip_arp_draddr)[3]);	
#ifdef MODULE_NET_UART
			NET_UART_LOOP('C9');
#ifdef DB_FIFO
			if(rx_over){printf("\n\rS23");rx_over=0;}
#endif
#endif
		sprintf(&Msg[strlen(Msg)],"%sDNS Address     : %d.%d.%d.%d",endl,(u16_t)((u8_t*)uip_dnsaddr)[0],(u16_t)((u8_t*)uip_dnsaddr)[1],(u16_t)((u8_t*)uip_dnsaddr)[2],(u16_t)((u8_t*)uip_dnsaddr)[3]);	
        sprintf(&Msg[strlen(Msg)],"%s%s",endl,BTN_BANNER);
#endif        
}

void NetworkConfig()
{
    #define HTONS htons
	int i;
	for(i =0 ;i<6;i++)
	{
  		uip_ethaddr.addr[i]=EEConfigInfo.MACID[i];
	}
	uip_hostaddr[0]    =HTONS(((u16_t)EEConfigInfo.IPAddress[0] << 8) | EEConfigInfo.IPAddress[1]);
	uip_hostaddr[1]    =HTONS(((u16_t)EEConfigInfo.IPAddress[2] << 8) | EEConfigInfo.IPAddress[3]);
	uip_arp_netmask[0] =HTONS(((u16_t)EEConfigInfo.SubnetMask[0] << 8) | EEConfigInfo.SubnetMask[1]);
	uip_arp_netmask[1] =HTONS(((u16_t)EEConfigInfo.SubnetMask[2] << 8) | EEConfigInfo.SubnetMask[3]);
	uip_arp_draddr[0]  =HTONS(((u16_t)EEConfigInfo.Gateway[0] << 8) | EEConfigInfo.Gateway[1]);
	uip_arp_draddr[1]  =HTONS(((u16_t)EEConfigInfo.Gateway[2] << 8) | EEConfigInfo.Gateway[3]);	
	uip_dnsaddr[0]  =HTONS(((u16_t)EEConfigInfo.DNSAddress[0] << 8) | EEConfigInfo.DNSAddress[1]);
	uip_dnsaddr[1]  =HTONS(((u16_t)EEConfigInfo.DNSAddress[2] << 8) | EEConfigInfo.DNSAddress[3]);

	strcpy(uip_buf_real,"");
	CatNetworkConfigMsg(uip_buf_real,"\n\r");
	printf(uip_buf_real);
}
void CPU_GOTO_FFF7H(void);
#ifdef FLASH_512
void Delay0(unsigned int H)
{
    while(H-->0)
    {
        if(RI==1)
            break;
        _nop_(); 
    }
}
char getch2(void)
{   char a;
    while (!RI);
    a = SBUF;
    RI = 0;
    return a;
}
u8_t EEPROMLoadDefault(void);
void Kbhit_EE_Load_Default()
{
	Delay0(60000);
	if(RI)
	{
		switch(getch2())
		{
			case 'd':
				printf("\n\r=> Load Default EEPROM ...");
				EEPROMLoadDefault();
				ForceMACAddress();
				EEPROMInit();
				break;
			case 'u':
				{					
					char keyword[]="update";
					char cur_type[sizeof(keyword)]="u";
					u8_t i=1;
					printf("\n\rcurrent type: %s",cur_type);
					while(1)
					{
						if(getch2()==keyword[i])
						{
							cur_type[i]=keyword[i];
							i++;
						}
						cur_type[i]='\0';
						printf("\r current type: %s",cur_type);
						if(keyword[i]=='\0')break;
					}
				}
				printf("\n\r Are you sure to do firmware update [y/n]");
				if(getch2()=='y')CPU_GOTO_FFF7H();
				break;
		}
	}
}
#endif //FLASH_512
//#define LED_SW P1_2
#ifdef MODEULE_FLASH_485
#define LED_SW P3_2
#endif
#define ON_RANGE 255
#define OFF_RANGE (ON_RANGE*2)
u16_t led_status=0;
void flashLed()
{
#ifdef MODEULE_FLASH_485
		led_status++;
		if((led_status>=0)&&(led_status<=ON_RANGE))
		{
			LED_SW=1;
		}
		else if((led_status>=ON_RANGE)&&(led_status<=OFF_RANGE))
		{
			LED_SW=0;
		}
		else
		{
			led_status=1;
		}
#endif
}
/*void eep_hw_load_def()
{
	printf("\n\r=> Load Default EEPROM ...");
	LOAD_DEFAULT_PIN=1;
	if(LOAD_DEFAULT_PIN==0)
	{
		EEPROMLoadDefault();
		EEPROMInit();
	}
}*/
/******************************************************************************
*
*  Function:    main()
*
*  Description: Start Function Of C Code
*               
*  Parameters:  None
*               
*  Returns:     None
*               
*******************************************************************************/

void main (void)
{	
	u8_t RXFIFO_START=0x70;
	u16_t kkk;
	
	//P4 Output Enable : bit1 / bit6
	TI=1;
	OUTPUTEN=0x42;
	IP210_SWReset();//utility.h
	CPUInit();
//**********************************************
	WDT_DIS=0x0;	//enable HW WDT
	WDTEN=1;		//enable SW watch dog timer
//**********************************************
#ifdef ET0_INTERRUPT //sorbica071219
#ifdef MODULE_FLASH_485
//init Timer0
TMOD &= 0xF;
TMOD |= 0x0;
TL0=0x00;
TH0=0xc4;
//ET0=0x1;
EA=0x1;
TR0=0x1;
#endif
#endif//#ifdef ET0_INTERRUPT //sorbica071219	
	Serial_Init();//serial.h
	Timercounter_Init();//timer.h
#ifdef MODULE_UART_AUTO_SEND
	Timercounter0_Init();
#endif
#ifdef FLASH_512
    //reset WDT in case WDT would cause a CPU Reset
    WatchDogTimerReset();
    FU_Check();
  	printf("\n\r\n\r\n\rType <d> to load default eeprom:");
#endif
	EEPROMInit();//eeprominit.h
#ifdef FLASH_512	
	Kbhit_EE_Load_Default();
#endif
	//eep_hw_load_def();
    TRACE_TEST(("\n->MAC_init()\n~"));
#ifdef MODULE_NET_UART
	#ifdef MODULE_UDP_TELNET
#ifdef MODULE_FLASH512
	if( (ModuleInfo.TelnetInfo.op_mode==NU_OP_MODE_UDP_LIS)||(ModuleInfo.TelnetInfo.op_mode==NU_OP_MODE_UDP_NOR) )
#else
	if(ModuleInfo.TelnetInfo.op_mode==NU_OP_MODE_UDP)
#endif
	{
		RXFIFO_START-=RELEASE_FOR_MAC_RXFIFO>>8;
	}
	#endif //MODULE_UDP_TELNET
#endif
    Resize_RX_Buf(RXFIFO_START);
	//printf("\nRXFIFO_START=0x%x",(u16_t)RXFIFO_START);
	MAC_init();//MACdriver.h

    //TRACE_TEST(("\n->UserModuleInit()~"));
	//UserModuleInit();
	
    TRACE_TEST(("\n->IsUseConsoleMode()~"));
//	IsUseConsoleMode();

    TRACE_TEST(("\n->NetworkConfig()~"));
	NetworkConfig();

	//GpioDeviceInit();
    TRACE_TEST(("\n->WebServer()~"));
	application();
}
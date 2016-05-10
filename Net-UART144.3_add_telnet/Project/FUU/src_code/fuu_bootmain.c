#pragma regparms
/******************************************************************************
*
*   Name:           BOOTMAIN.C
*
*   Description:    Firmware Update main function 
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
//20060105
//#include <reg52.h>
#include <stdio.h>
#include "net.h"
#include "\udp\udp.h"
#include "\icmp\icmp.h"
#include "public.h"
#include "version.h"
#include "timer.h"
#include "net_cmd.h"//20080304
//******Network information***********
//ip, mac, dns, gateway information
NET_INFO icp_netinfo;
static u32_t xdata start, current;
// host ipaddrss
//u16_t uip_hostaddr[2];
//******Tx, Rx Buffer******************
//tx, rx buffer
//u8_t xdata uip_fw_buf[UIP_BUFSIZE+2];
// tx rx len
//volatile u16_t uip_len;
//*************************************
//***Others****************************
//u16_t ipid; // for Internet Protocol
u16_t blok_num;  // for TFTP
u16_t Rx_data_len; //input or output data length
u8_t  tftp_last_flag; //for last packet flag
u8_t  tftp_start_flag; //for last packet flag
u32_t tftp_counter;

u8_t Global_flag_restart=0;
u8_t Global_flag_firmwareupdate_inprogress=0;//0:not in progress, 1:in progress by web, 2: in progress by tftp
u8_t  Global_suspend_count;
extern u32_t inspect_img512k;

void CheckPktSend()
{
	if(uip_len>0)
	{
		uip_arp_out();
		EthernetSend(uip_buf_real,uip_len);
		uip_len=0;
	}
}
//*************************************
extern u16_t Phase_Flash_Erase;//sorbica071018
/******************************************************************************
*
*  Function:    boot_init
*
*  Description:  Initial MAC address, IP address, and ipid
*               
*  Parameters:  NONE
*               
*  Returns:     NONE
*               
*******************************************************************************/
void boot_init(void)
{ 
	u8_t i;
	printf("\n\rMAC Address             : ");
    for(i=0;i<5;i++)
    {
        icp_netinfo.MY_MAC[i]=IP210RegRead(REG_MAC_ADDR+i);
        uip_ethaddr.addr[i]=icp_netinfo.MY_MAC[i];
		printf("%02x.",(u16_t)icp_netinfo.MY_MAC[i]);
    }
    icp_netinfo.MY_MAC[i]=IP210RegRead(REG_MAC_ADDR+i);
    uip_ethaddr.addr[i]=icp_netinfo.MY_MAC[i];    
	printf("%02x",(u16_t)icp_netinfo.MY_MAC[i]);   

	printf("\n\rIP  Address             : ");
    for(i=0;i<3;i++) 
    {
		icp_netinfo.MY_IP[i] = IP210RegRead(REG_IP_ADDR+i);
		printf("%d.",(u16_t)icp_netinfo.MY_IP[i]);
    }    
	icp_netinfo.MY_IP[i] = IP210RegRead(REG_IP_ADDR+i);
	printf("%d",(u16_t)icp_netinfo.MY_IP[i]);
	printf("\n\r==================================================");
//**************chance*******************
	uip_hostaddr[0]    =htons(((u16_t)icp_netinfo.MY_IP[0] << 8) | icp_netinfo.MY_IP[1]);
	uip_hostaddr[1]    =htons(((u16_t)icp_netinfo.MY_IP[2] << 8) | icp_netinfo.MY_IP[3]);
//***************************************
	uip_arp_netmask[0] =htons(((u16_t)icp_netinfo.MY_MASK[0] << 8) | icp_netinfo.MY_MASK[1]); 
	uip_arp_netmask[1] =htons(((u16_t)icp_netinfo.MY_MASK[2] << 8) | icp_netinfo.MY_MASK[3]); 
	uip_arp_draddr[0]  =htons(((u16_t)icp_netinfo.MY_GW[0] << 8) | icp_netinfo.MY_GW[1]); 
	uip_arp_draddr[1]  =htons(((u16_t)icp_netinfo.MY_GW[2] << 8) | icp_netinfo.MY_GW[3]); 
	//IP Identifiacton initial
	ipid=0;
    //tftp last packet flag
	tftp_last_flag=0;
	//tftp first packet flag
	tftp_start_flag=0;
    //tftp block number
	blok_num=0;
	//tftp counter
	tftp_counter=0;
}

/******************************************************************************
*
*  Function:    bootmain
*
*  Description:  Firmware Update main function handle ARP, UDP, ICMP
*
*  Parameters:  none
*
*  Returns:     none
*
*******************************************************************************/

void bootmain(void)
{    
	u8_t i;
	u16_t uip_len_tmp;
    //----initialization MAC and IP----
    boot_init();
    uip_arp_init();
    uip_init();
    httpd_init();
	printf("\n\rTFTP Start......\n\r"); 
    while(1)
    {
        if(Global_flag_restart>=20)
     	    CPU_GOTO_FFFDH();
        FirmwareUpdateEraseFlash_While();//sorbica071018

        uip_len = EthernetReceive(uip_fw_buf);
        uip_len_tmp=uip_len;
        if (uip_len > (UIP_LLH_LEN+IP_HEADER_LENGTH))
		{
			uip_arp_ipin();
			uip_len=uip_len_tmp;
	        if (rx_eth->type == HTONS(UIP_ETHTYPE_ARP))//0806
	        {
				LocalCmd();
				if(uip_len>0)
	        	{
					EthernetSend(uip_fw_buf,uip_len);
					uip_len=0;
	                continue;
				}
	            uip_len=uip_len_tmp;
				uip_arp_arpin();  
				if(uip_len>0)
				{
					EthernetSend(uip_fw_buf,uip_len);
					uip_len=0;
                    
				}
	            continue;
	        }
	        else if (rx_eth->type == HTONS(UIP_ETHTYPE_IP))//IP Packet
	        {
				if(iphdr->proto==UIP_PROTO_UDP)//UDP Packet
				{
					UDP_APPCALL();
					if(uip_len>0)
					{
						if(EthernetSend(uip_fw_buf,uip_len))
						{
							uip_len=0;							
							if(tftp_last_flag)					
							{
								delay_ms(1000);//wait 1 second
								tftp_last_flag=0;
								tftp_start_flag=0;
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
								Stop_Mirror_Mode();
#endif//sorbica071008
								CPU_GOTO_FFFDH();
							}
						}						
					}
					continue;
				}
				else
				{
	    		    uip_process(UIP_DATA);
			        CheckPktSend();
				}
	        }//ip packet end
		}//uip_len end
		IP210_Update_Timer_Counter();
        if (Delta_Time(start) >= IP210_TIMER_SCALE)  // 100 msec.
        {
            if(Global_flag_firmwareupdate_inprogress == 1)
            { 
                if(img512k == inspect_img512k)//img512k not increasing
                {
                    Global_suspend_count++;
                }
                else if(img512k > inspect_img512k)
                {
			        inspect_img512k = img512k;
                    Global_suspend_count = 0;
                }
                else
                {
                    printf("\nWeird ");    
                }
                if(Global_suspend_count >= 100)//having some problem
		    	{
                    Global_flag_restart=1;    
					Global_flag_firmwareupdate_inprogress=0;
                    printf("\n Firmware Update by Web failed, Restarting program, Global_suspend_count[%d]~~", (u16_t)Global_suspend_count);    
    			}
            }//if(Global_flag_firmwareupdate_inprogress == 1)

            if(Global_flag_restart!=0)
                Global_flag_restart++;
			for (i=0; i < UIP_CONNS; ++i)
            {
                uip_conn = &uip_conns[i];
                uip_process(UIP_TIMER);
				CheckPktSend();
            }
	        start = timercounter;
        }		
		if(tftp_start_flag)
		{
			tftp_counter++;
			if(tftp_counter>TFTP_Timeout)
			{
				//jump to startup
#ifdef FU_ALL_RUN_IN_MIRROR//sorbica071008
				Stop_Mirror_Mode();
#endif//sorbica071008
				printf("\n\rTFTP Timeout! Restart!");
				CPU_GOTO_FFFDH();
			}
		}
    }//while end
}


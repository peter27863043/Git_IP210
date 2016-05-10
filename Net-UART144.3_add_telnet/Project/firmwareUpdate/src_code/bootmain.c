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
#include "timer.h"
#include "kernelsetting.h"
//******Network information***********
//ip, mac, dns, gateway information
NET_INFO icp_netinfo;
static u32_t xdata start, current;
extern u8_t Flash_check_error_flag;// james 20090506
// host ipaddrss
//u16_t uip_hostaddr[2];
//******Tx, Rx Buffer******************
//tx, rx buffer
//u8_t xdata uip_fw_buf[UIP_BUFSIZE+2];
// tx rx len
//volatile u16_t uip_len;
//*************************************
//***Others****************************
u16_t ipid; // for Internet Protocol
u16_t blok_num;  // for TFTP
u16_t Rx_data_len; //input or output data length
u8_t  tftp_last_flag; //for last packet flag
u8_t  tftp_start_flag; //for last packet flag
u32_t tftp_counter;

u8_t Global_flag_restart=0;
u8_t Global_flag_firmwareupdate_inprogress=0;//0:not in progress, 1:in progress by web, 2: in progress by tftp
u8_t  Global_suspend_count;
extern u32_t inspect_img512k;
u8_t *uip_fw_buf=uip_buf_real_mem;//sorbica071122
u8_t last_tagged_ethertype_offset=0;//sorbica071123
void os_cbk_fu_switch_init(void);
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

void WatchDogTimerReset(void);//for WDT
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
	printf("\n\rMAC Address: ");
    for(i=0;i<5;i++)
    {
       icp_netinfo.MY_MAC[i]=IP210RegRead(REG_MAC_ADDR+i);
//       uip_ethaddr.addr[i]=icp_netinfo.MY_MAC[i];
//		printf("%02x.",(u16_t)icp_netinfo.MY_MAC[i]);
        uip_ethaddr.addr[i]=KSetting_Info.netif->MACID[i];
		printf("%02x.",(u16_t)KSetting_Info.netif->MACID[i]);

    }
    icp_netinfo.MY_MAC[i]=IP210RegRead(REG_MAC_ADDR+i);
//    uip_ethaddr.addr[i]=icp_netinfo.MY_MAC[i];    
//	printf("%02x",(u16_t)icp_netinfo.MY_MAC[i]);   
        uip_ethaddr.addr[i]=KSetting_Info.netif->MACID[i];
		printf("%02x",(u16_t)KSetting_Info.netif->MACID[i]);

	printf("\n\rIP  Address: ");
    for(i=0;i<3;i++) 
    {
		icp_netinfo.MY_IP[i] = IP210RegRead(REG_IP_ADDR+i);
//		printf("%d.",(u16_t)icp_netinfo.MY_IP[i]);
		printf("%d.",(u16_t)KSetting_Info.netif->IPAddress[i]);
    }    
	icp_netinfo.MY_IP[i] = IP210RegRead(REG_IP_ADDR+i);
//	printf("%d",(u16_t)icp_netinfo.MY_IP[i]);
	printf("%d",(u16_t)KSetting_Info.netif->IPAddress[i]);
	printf("\n\r==================================================");
//**************chance*******************
	uip_hostaddr[0]    =htons(((u16_t)KSetting_Info.netif->IPAddress[0] << 8) | KSetting_Info.netif->IPAddress[1]);
	uip_hostaddr[1]    =htons(((u16_t)KSetting_Info.netif->IPAddress[2] << 8) | KSetting_Info.netif->IPAddress[3]);
//***************************************
	uip_arp_netmask[0] =htons(((u16_t)KSetting_Info.netif->SubnetMask[0] << 8) | KSetting_Info.netif->SubnetMask[1]); 
	uip_arp_netmask[1] =htons(((u16_t)KSetting_Info.netif->SubnetMask[2] << 8) | KSetting_Info.netif->SubnetMask[3]); 
	uip_arp_draddr[0]  =htons(((u16_t)KSetting_Info.netif->Gateway[0] << 8) | KSetting_Info.netif->Gateway[1]); 
	uip_arp_draddr[1]  =htons(((u16_t)KSetting_Info.netif->Gateway[2] << 8) | KSetting_Info.netif->Gateway[3]); 
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

//sorbica071123
void RxFilter(void)//sorbica071122
{
  u8_t i, j;

//printf("\n Rx-");    
  if(uip_buf_real_mem[0xc]==0x08 && uip_buf_real_mem[0xd]==0x00 && uip_buf_real_mem[0xe]==0x45)
	{
//printf("1-");    
    uip_fw_buf = uip_buf_real_mem;	    
		uip_buf_real = uip_buf_real_mem;	   
	}
	else 
	{
//printf("2-");    
	    //if(uip_buf_real_mem[0xc]==0x08 && uip_buf_real_mem[0xd]==0x06 &&  uip_buf_real_mem[0x10]==0x08
		     //&&uip_buf_real_mem[0x11]==0x00 &&  uip_buf_real_mem[0x12]==0x06 &&  uip_buf_real_mem[0x13]==0x04)
	    if((uip_buf_real_mem[0xc]==0x08) && 
	       (uip_buf_real_mem[0xd]==0x06) &&  
	       (
          (uip_buf_real_mem[0x10]==0x08&&uip_buf_real_mem[0x11]==0x00)||
          (uip_buf_real_mem[0x10]==0xF0&&uip_buf_real_mem[0x11]==0x00)
	       )
	      ) 
    	{
//printf("3-");    
            uip_fw_buf = uip_buf_real_mem;
    	}
	    else//etherytpe not accepted, find them or drop
    	{
//printf("(old)");    
           //check last tagged packet's ether type offset first
           if(uip_buf_real_mem[0xc+last_tagged_ethertype_offset]==0x08 && uip_buf_real_mem[0xd+last_tagged_ethertype_offset]==0x00 && uip_buf_real_mem[0xe+last_tagged_ethertype_offset]==0x45)
           {
//printf("(1)");
               for(j=12;j>=1;j--)
		           uip_buf_real_mem[last_tagged_ethertype_offset+(j-1)] = uip_buf_real_mem[j-1];
               uip_fw_buf = uip_buf_real_mem+last_tagged_ethertype_offset;	    
		           uip_buf_real = uip_buf_real_mem+last_tagged_ethertype_offset;	    
		           uip_len-=last_tagged_ethertype_offset;
			         return;
           }
           else 
           if(uip_buf_real_mem[0xc+last_tagged_ethertype_offset]==0x08 && 
              uip_buf_real_mem[0xd+last_tagged_ethertype_offset]==0x06 &&  
              (
	              ( 
	                uip_buf_real_mem[0x10+last_tagged_ethertype_offset]==0x08 && 
	                uip_buf_real_mem[0x11+last_tagged_ethertype_offset]==0x00
	              )||
	              ( 
	                uip_buf_real_mem[0x10+last_tagged_ethertype_offset]==0xF0 && 
	                uip_buf_real_mem[0x11+last_tagged_ethertype_offset]==0x00
	              )	              
              )
             )
           /*if(uip_buf_real_mem[0xc+last_tagged_ethertype_offset]==0x08 && 
              uip_buf_real_mem[0xd+last_tagged_ethertype_offset]==0x06 &&  
              uip_buf_real_mem[0x10+last_tagged_ethertype_offset]==0x08 && 
              uip_buf_real_mem[0x11+last_tagged_ethertype_offset]==0x00 &&  
              uip_buf_real_mem[0x12+last_tagged_ethertype_offset]==0x06 &&  
              uip_buf_real_mem[0x13+last_tagged_ethertype_offset]==0x04)*/
           {
//printf("(2)");
                for(j=12;j>=1;j--)
				        uip_buf_real_mem[last_tagged_ethertype_offset+(j-1)] = uip_buf_real_mem[j-1];
                uip_fw_buf = uip_buf_real_mem+last_tagged_ethertype_offset;
			          uip_buf_real = uip_buf_real_mem+last_tagged_ethertype_offset;	   
    			      uip_len-=last_tagged_ethertype_offset;
				        return;
           }

//printf("4-");    
	      for(i=1;i<10;i++)
		    {
//printf("5-");    
              if(uip_buf_real_mem[0xc+i]==0x08 && uip_buf_real_mem[0xd+i]==0x00 && uip_buf_real_mem[0xe+i]==0x45)
            	{
//printf("6(%d)-", (u16_t)i);    
                    for(j=12;j>=1;j--)
					          uip_buf_real_mem[i+(j-1)] = uip_buf_real_mem[j-1];
                    uip_fw_buf = uip_buf_real_mem+i;	    
					          uip_buf_real = uip_buf_real_mem+i;	    
		    		        uip_len-=i;
                    last_tagged_ethertype_offset=i;
                    break;
            	}
        	    else 
        	    if(uip_buf_real_mem[0xc+i]==0x08 && 
        	       uip_buf_real_mem[0xd+i]==0x06 &&
        	       (  
        	          (
        	           uip_buf_real_mem[0x10+i]==0x08&&
		                 uip_buf_real_mem[0x11+i]==0x00
		                )||
        	          (
        	           uip_buf_real_mem[0x10+i]==0xF0&&
		                 uip_buf_real_mem[0x11+i]==0x00
		                )           
		             )
		            )        	    
        	    /*if(uip_buf_real_mem[0xc+i]==0x08 && 
        	       uip_buf_real_mem[0xd+i]==0x06 &&
        	       (  
        	          uip_buf_real_mem[0x10+i]==0x08&&
		                uip_buf_real_mem[0x11+i]==0x00 &&  
		                uip_buf_real_mem[0x12+i]==0x06 &&  
		                uip_buf_real_mem[0x13+i]==0x04
		             )
		            )*/
         	    {
//printf("7(%d)-", (u16_t)i);    
                    for(j=12;j>=1;j--)
                    uip_buf_real_mem[i+(j-1)] = uip_buf_real_mem[j-1];
                    uip_fw_buf = uip_buf_real_mem+i;
                    uip_buf_real = uip_buf_real_mem+i;	   
                    uip_len-=i;
                    last_tagged_ethertype_offset=i;
                    break;
             	}
    		}
			if (i==10)
			    uip_len=0;
	    }
	}
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
//	printf("\n\r==================================================");
//	printf("\n\rFirmware Update Version : %s",FW_CODE_VERSION); 
    //printf("\n\r%s",FW_CODE_VERSION);
    //----initialization MAC and IP----
    os_cbk_fu_switch_init();
    boot_init();
    uip_arp_init();
    uip_init();
    httpd_init();
	printf("\n\rTFTP Start......\n\r"); 
    while(1)
    {
        WatchDogTimerReset();
        if(Global_flag_restart>=20)
     	{    
            CPU_GOTO_FFFDH();
        }
		if(Flash_check_error_flag==0)
		{
			FirmwareUpdateEraseFlash_While();//sorbica071018
		}
//        uip_len = EthernetReceive(uip_fw_buf);
        uip_len = EthernetReceive(uip_buf_real_mem);
/*
        if(!uip_len)
        {
           if(Flash_check_error_flag==0)
           {// james 20090506
               FirmwareUpdateEraseFlash_While();//sorbica071018
		   }
        }
*/
		if(uip_len>0)
		{
		//printf("\n b uip_len=[%d])\n", (u16_t)uip_len);
			RxFilter();
		//printf("\n a uip_len=[%d])\n", (u16_t)uip_len);
		//for(i=0;i<20;i++)
		//    printf("%02x ", (u16_t)uip_buf_real_mem[i]);
		}
        uip_len_tmp=uip_len;
        if (uip_len > (UIP_LLH_LEN+IP_HEADER_LENGTH))
		{
			uip_arp_ipin();
			uip_len=uip_len_tmp;
            // ARP packet 
	        if (rx_eth->type == HTONS(UIP_ETHTYPE_ARP))//0806
	        {
//                printf("arp");
	            //*********search device 20080304 add by chance
	            /*
				LocalCmd();
				if(uip_len>0)
				{
					EthernetSend(uip_fw_buf,uip_len);
					uip_len=0;
	                continue;
				}
				
	            uip_len=uip_len_tmp;
	            */
	            //***************************************  
		   		//printf("\n\r ARP Packet Request");                                    
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
					//printf("\n\r UDP Packet Reply"); 
					UDP_APPCALL();
					if(uip_len>0)
					{
//						uip_arp_out();
						//printf("\n\r send UDP packet out!");
						if(EthernetSend(uip_fw_buf,uip_len))
						{
							uip_len=0;							
							if(tftp_last_flag)					
							{
								//printf("\n\r after ehernetsend and last tftp packet!");
								delay_ms(1000);//wait 1 second
								tftp_last_flag=0;
								tftp_start_flag=0;
//								printf("\n\r before tftp_last_flag Stop mirror mode!");

//								printf("\n\r after tftp_last_flag Stop mirror mode!");
								//printf("\n\r jump to check flash address!");																
								CPU_GOTO_FFFDH();
								//DATA SEND AFTER jump to web control
								//*****Sorbica will support	
							}
						}						
					}
					continue;
				}
				/*
				if(iphdr->proto==UIP_PROTO_ICMP)//ICMP Packet
				{
					//printf("\n\r ICMP Packet");
					ICMP_Process();
					if(uip_len>0)
					{
						//uip_arp_out();
						EthernetSend(uip_fw_buf,uip_len);   
						uip_len=0; 
					}
					  continue;
				}// icmp packet end
				*/
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
        	//chance 20080820  for arp issue
        	uip_arp_timer() ;
//printf("\n UIP_POLL ");    
            if(Global_flag_firmwareupdate_inprogress == 1)
            { 
                if(img512k == inspect_img512k)//img512k not increasing
                {
                    Global_suspend_count++;
//printf("\n UIP_POLL img512k=[%08lx], inspect_img512k[%08lx] Global_suspend_count[%d] ", (u32_t)img512k, (u32_t)inspect_img512k, (u16_t)Global_suspend_count);    
                }
                else if(img512k > inspect_img512k)
                {
//printf("\n UIP_POLL OK img512k=[%08lx], inspect_img512k[%08lx] ", (u32_t)img512k, (u32_t)inspect_img512k);    
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
                    printf("\n Firmware Update by Web failed, Restarting");// , Global_suspend_count[%d]~~", (u16_t)Global_suspend_count);    
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
				printf("\n\rTFTP Timeout! Restart!");
				CPU_GOTO_FFFDH();
			}
		}
    }//while end

}


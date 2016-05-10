/******************************************************************************
*
*   Name:           tftpd.C
*
*   Description:    TFTP Function
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
//20051021
#include <stdio.h>
#include <string.h>
#include "tftpd.h"
#include "\udp\udp.h"
extern u8_t Global_flag_firmwareupdate_inprogress;
extern u8_t *uip_fw_buf;//sorbica071122
//sorbica for big flashes
extern u8_t Flash_Bankno;
//sorbica for big flashes
/******************************************************************************
*
*  Function:    TFTP_Process
*
*  Description:  Handle TFTP packet.
*               
*  Parameters:  NONE
*               
*  Returns:     NONE
*               
*******************************************************************************/
void TFTP_Process()
{
	if(uip_len < (UIP_LLH_LEN+IP_HEADER_LENGTH+UDP_HEADER_LENGTH+TFTP_Header_Len)) 
	{
		uip_len = 0;
		return;
	}
    Rx_data_len=0;
	if(udph->destport==TFTP_Port)//69
	{		
		switch(TFTPD_PKT->opcode)
		{			
			case TFTP_RRQ://1
				//printf("\n\rNot Support!");
				uip_len = 0;
				break;
			case TFTP_WRQ://2				
				//Client send a write request, contains file name and opcode = 2
				if(tftp_start_flag)
				{
                    Stop_Mirror_Mode();
					printf("\n\rTFTP Fail! Restart!");					
					CPU_GOTO_FFFDH();
				}
				TFTPD_PKT->opcode=TFTP_ACK;
				blok_num=0;
				TFTPD_PKT->str.block_num=blok_num;

                if(Global_flag_firmwareupdate_inprogress==1)
                {
    				printf("\nUpdate by Web ");
                    break;
                }
                Global_flag_firmwareupdate_inprogress=2;

				tftp_start_flag=1;//system got first tftp packet
				udp_send(TFTP_WRQ_ACK_LEN);	
				printf("\n\rTFTP Block number:");				
				break;
			case TFTP_DATA://3
				//printf("tftp_start_flag=%x TFTPD_PKT->str.block_num=%x",(u16_t)tftp_start_flag,(u16_t)TFTPD_PKT->str.block_num);
				if(tftp_start_flag)//if got first tftp packet
				{					
					tftp_counter=0;
					blok_num++;
					//if receive packet block number not the same with our block number
	                // Drop the packet
					if(blok_num==TFTPD_PKT->str.block_num)
					{				
						//caculate the data length	
						Rx_data_len=iphdr->len-(IP_HEADER_LENGTH+ICMP_Header_Len+TFTP_Header_Len);				
						//Set the input data terminal symbol
						*(&(TFTPD_PKT->indata[0])+Rx_data_len)=0x0;
						//send buffer address and the data length to the flash function						
						if(FirmwareUpdateWriteData(&(TFTPD_PKT->indata[0]),Rx_data_len))
						{		
								//if flash write ok, send the ack back to client
								TFTPD_PKT->opcode=TFTP_ACK;
								TFTPD_PKT->str.block_num=blok_num;
								udp_send(TFTP_WRQ_ACK_LEN);	
								//If the data length smaller than 512, it's the last packet
	                            //set the tftp_flag=1
								printf("\r\t\t\t%d",(u16_t)blok_num);
								if(Rx_data_len<TFTP_DATA_LEN)
								{
									printf("...TFTP Finish! ");
									//printf("\n\r Last TFTP Packet!");
									tftp_last_flag=1;
								}
						}
						else
						{
							uip_len = 0;
							printf("\n\r Flash Write Error!");
                            Stop_Mirror_Mode();
							printf("\n\rTFTP Fail! Restart!");					
							CPU_GOTO_FFFDH();
						}
					}
					else
					{
						if(blok_num>TFTPD_PKT->str.block_num)
						{	
							TFTPD_PKT->opcode=TFTP_ACK;
							//TFTPD_PKT->str.block_num=blok_num;
							udp_send(TFTP_WRQ_ACK_LEN);
						}
						else
						{
							uip_len = 0;	
						}
						blok_num--;
					}
				}//end tftp_start_flag
				else
				{
					uip_len = 0;		
				}
				break;
			case TFTP_ACK://4
				uip_len = 0;
				//printf("\n\r Not Support!");
				break;
			case TFTP_ERROR://5
				uip_len = 0;
				//printf("\n\r Not Support!");
				break;
			default:
				break;
		}//end switch
	}//TFTP_Port
}
/*u8_t tftp_app(u8_t *indata,u16_t indata_len)
{
	//printf("\n\r data=%s datalen=%x",indata,(u16_t)indata_len);
	return 1;
}*/

/******************************************************************************
*
*   Name:           updateCbk.c
*
*   Description:    Handle firmware update function
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
//*******************Include files area**************************
#include <string.h>
#include <stdio.h>
#include "type_def.h"
#include "httpd.h"
#include "httpd_app.h"
#include "eeprom.h"
#include "eepdef.h"
#include "cgi.h"
#include "uip.h"
#include "flash.h"
#include "EraseFlash_htm.h"
#include "UpdateComplete_htm.h"
#include "firmwareUpdate_htm.h"
#include "TFTPInProgress_htm.h"

#define MAX_BUFFER_SIZE 4000
u8_t NETTMPBUFFER[MAX_BUFFER_SIZE];//chance 20100401
u8_t CBK_Flash_Size_Check();
//*******************Extern data area**************************
extern struct httpd_info *hs;
extern u32_t Global_flash_count;
extern u16_t Global_flash_addr;
extern u8_t Global_bank_index;
extern u16_t Phase_Flash_Erase;
extern u8_t Global_flag_restart;
extern u8_t Global_flag_firmwareupdate_inprogress;
extern uFW_EE_Info* ptr_EE_Info;
extern u8_t flashcheck_error_flag;
extern u16_t Flash_Bankno;
extern u16_t Flash_Erase_Bankno;
u8_t size_flag=0;//chance20100423 add for change bank number
SFILENAME *  fs_open03(char *name);  
//*******************Function prototype area*******************
//extern void CPU_GOTO_FFF7H(void);
/******************************************************************************
*
*  Function:    cgi_firmwareUpdate
*
*  Description: Handle firmware update request from client
*               
*  Parameters:  cnt: number of the cgi data, vp: CGI name and value.
*               
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*               
*******************************************************************************/
u8_t cgi_firmwareUpdate(u8_t cnt, void *vptr)
{
    req_data *vp = vptr;
    u8_t i=0,fflag=0;
    u16_t tmplen=0,offset=0,offset2=0;  
    u32_t update;
    u8_t *str_ptr=NULL,*str2=NULL;
	u32_t flashsize=0;
	//u8_t Flash_Bankno=8;

	flashsize = Flash_Bankno * 65536;

	if(cnt||vp){}
	if(hs->state!=HTTP_MULTIPACKET)
	{
		memset(NETTMPBUFFER,0,MAX_BUFFER_SIZE);
		hs->multipacket_datasize=0;
	}
	hs->state=HTTP_MULTIPACKET;

	if(Global_flag_firmwareupdate_inprogress==2)//Firmware Update by TFTP in progress, report notice webpage
	{
    	httpd_init_file(fs_open03("TFTPInProgress.htm"));
		return OK;
	}
	if(hs->multipacket_count<2)
	{		
		offset=uip_len-(hs->INpacket.http_Indata-uip_appdata);
		memcpy(&NETTMPBUFFER[hs->multipacket_datasize],hs->INpacket.http_Indata,offset);								     	   
		str_ptr=strfind(&NETTMPBUFFER[0],"Content-Type: application");
		if(str_ptr!=0)    
		{
			str2=strfind(str_ptr,"octet-stream\r\n\r\n");
			if(str2!=0)
			{
				if(strfind(str2+0x50,"13f0f013")!=0) //fin image key word
				{
					img512k=0;
					inspect_img512k=0;	
					Global_flag_firmwareupdate_inprogress=1; 
					hs->multipacket_count=2;  
					offset2=(str2-&NETTMPBUFFER[0]);
					offset2-=hs->multipacket_datasize;//minus first packet len
					vp[0].value=hs->INpacket.http_Indata+offset2;             	
					tmplen=offset-offset2;
					memset(NETTMPBUFFER,0,MAX_BUFFER_SIZE);
				}
				else
				{
					printf("\n\rError!! image key word");
					fflag=2;
				}
			}
			else
			{
				fflag=1;
			}
		}
		else   
		{	  
			fflag=1;
		}
		if(fflag>0)
		{
			if(memcmp(&NETTMPBUFFER[hs->multipacket_datasize+offset]-4,"--\r\n",4)!=0)
			{
				if(hs->multipacket_count==0)
				{
					hs->multipacket_datasize+=offset; 
					hs->multipacket_count++;
				}
				else
				{
					memset(NETTMPBUFFER,0,MAX_BUFFER_SIZE);
					hs->multipacket_datasize=0;
					memcpy(&NETTMPBUFFER[hs->multipacket_datasize],hs->INpacket.http_Indata+offset-100,100);
					hs->multipacket_datasize+=100;
				}
				return OK;
			}
			else
			{   
				WatchDogTimerReset();    
				httpd_init_file(fs_open03("TF.htm"));    
				return OK;
			}
		}
	}
	else
	{
		if((img512k+uip_len)>flashsize)
		{
			tmplen=flashsize-img512k;
		}
		else
		{
			tmplen=uip_len;
		}
	}
	if(tmplen!=0)
	{
		if(FirmwareUpdateWriteData(vp[0].value,tmplen))
		{		
			img512k+=tmplen;
			update=((img512k*100)/flashsize);
			if((img512k>0x20000)&&(!size_flag)) //chance add 20100423
			{
				size_flag=1;
				//CBK_Flash_Size_Check();
			} 
			printf("\rUpdating...%d%%",(u16_t)update);
			if(img512k==flashsize)
			{
				for(i=0;i<6;i++)
					EEPROM_Write_Byte(loc_EEMACID+i,EEConfigInfo.MACID[i]);
				httpd_init_file(fs_open03("UpdateComplete.htm"));
				Global_flag_restart=1;
				Global_flag_firmwareupdate_inprogress=0;
			}	            
		}
		else
		{
			printf("\r Write Fail! Restart!");					
			CPU_GOTO_FFFDH();
		}
	}
  	return OK;
}
u8_t ssi_firmwareUpdate(u8_t varid, data_value *vp)
{
	switch(varid)
	{
		case CGI_FUF_IP1:
		case CGI_FUF_IP2:
		case CGI_FUF_IP3:
		case CGI_FUF_IP4:
            	vp->value.digits_3_int=ptr_EE_Info->netif.IPAddress[varid];
			break;
		default:		   //
			vp->value.digits_3_int=0xff;
			break;
	}
	vp->type=digits_3_int;
	return OK;
}
/******************************************************************************
*
*  Function:    ssi_errormsg
*
*  Description: Httpd will send error message to client, while error occurs.
*
*  Parameters:  varid: SSI Tag ID, vp: SSI value and type.
*
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*
*******************************************************************************/
u8_t ssi_errormsg(u8_t varid, data_value *vp)
{
return OK;
}

u8_t ssi_eraseflash(u8_t varid, data_value *vp)
{
	switch(varid)
	{
        case CGI_FU_FSECTOR:
	        vp->value.digits_3_int=Flash_Erase_Bankno*16;
			break;
		case CGI_FU_IP1:
		case CGI_FU_IP2:
		case CGI_FU_IP3:
		case CGI_FU_IP4:
            	vp->value.digits_3_int=ptr_EE_Info->netif.IPAddress[varid-CGI_FU_IP1];
			break;
		case CGI_FU_ERASE_PROGRESS:
            	vp->value.digits_3_int=Phase_Flash_Erase;
			break;
		default:		   //
			vp->value.digits_3_int=0xff;
			break;
	}
	vp->type=digits_3_int;
	return OK;

}
/*
u8_t ssi_updatecomplete(u8_t varid, data_value *vp)
{
*/
	/*switch(varid)
	{
		case CGI_FUC_IP1:
            	vp->value.digits_3_int=ptr_EE_Info->netif.IPAddress[0];
			break;
		case CGI_FUC_IP2:
            	vp->value.digits_3_int=ptr_EE_Info->netif.IPAddress[1];
			break;
		case CGI_FUC_IP3:
            	vp->value.digits_3_int=ptr_EE_Info->netif.IPAddress[2];
			break;
		case CGI_FUC_IP4:
            	vp->value.digits_3_int=ptr_EE_Info->netif.IPAddress[3];
			break;
		default:		   //
			vp->value.digits_3_int=0xff;
			break;
	}*/
/*
    vp->value.digits_3_int=ptr_EE_Info->netif.IPAddress[varid];
	vp->type=digits_3_int;
	return OK;
}
*/
u8_t ssi_tftpinprogress(u8_t varid, data_value *vp)
{
    vp->value.digits_3_int=ptr_EE_Info->netif.IPAddress[varid];
    /*
	switch(varid)
	{

		case CGI_FUT_IP1:
            	vp->value.digits_3_int=ptr_EE_Info->netif.IPAddress[0];
			break;
		case CGI_FUT_IP2:
            	vp->value.digits_3_int=ptr_EE_Info->netif.IPAddress[1];
			break;
		case CGI_FUT_IP3:
            	vp->value.digits_3_int=ptr_EE_Info->netif.IPAddress[2];
			break;
		case CGI_FUT_IP4:
            	vp->value.digits_3_int=ptr_EE_Info->netif.IPAddress[3];
			break;
		default:		   //
			vp->value.digits_3_int=0xff;
			break;
	}
    */
	vp->type=digits_3_int;
	return OK;
}


u8_t cgi_callerase(u8_t cnt, void *vptr)
{
	req_data *vp = vptr;
	flashcheck_error_flag=0;
	httpd_init_file(fs_open03("EraseFlash.htm"));
	return OK;
}

u8_t CBK_Flash_Size_Check()
{
  FirmwareUpdatMirrorMode(0x8000,0x0,0x2800);
  Flash_CHECK_SIZE();
  Stop_Mirror_Mode();
  return 1;
}

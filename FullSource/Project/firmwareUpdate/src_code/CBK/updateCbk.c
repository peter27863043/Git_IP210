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
#include "Erase_htm.h"
#include "OK_htm.h"
#include "fu_htm.h"
#include "TF_htm.h"
#include "kernelsetting.h"
//#include "TFTPGoing_htm.h"
#include "main_htm.h"//chance add 20100322 for top frame
#define MAX_BUFFER_SIZE 4000
u8_t NETTMPBUFFER[MAX_BUFFER_SIZE];//chance 20100401
//*******************Extern data area**************************
extern u8_t page_flag;//chance add 20100322 for top frame
extern struct httpd_info *hs;
extern u32_t Global_flash_count;
extern u16_t Global_flash_addr;
extern u8_t Global_bank_index;
extern u16_t Phase_Flash_Erase;
extern u8_t Global_flag_restart;
extern u8_t Global_flag_firmwareupdate_inprogress;
extern u8_t Flash_check_error_flag;// james 20090506
//sorbica for big flashes
extern u16_t Flash_Bankno;
//sorbica for big flashes
u8_t size_flag=0;//chance20100423 add for change bank number
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
    u8_t i,fflag=0;
    u16_t tmplen=0,offset=0,offset2=0;
    u32_t update;
    u8_t *str_ptr=NULL,*str2=NULL;
    u32_t flashsize;
	WatchDogTimerReset();
    flashsize = Flash_Bankno * 65536;
	if(cnt||vp){}
	//chance modify
  if(hs->state!=HTTP_MULTIPACKET)
  {
    memset(NETTMPBUFFER,0,MAX_BUFFER_SIZE);
    hs->multipacket_datasize=0;
  }	
  //------------
	hs->state=HTTP_MULTIPACKET;

	/*if(Global_flag_firmwareupdate_inprogress==2)//Firmware Update by TFTP in progress, report notice webpage
	{
    httpd_init_file(fs_open03("TFTPGoing.htm"));
		return OK;
	}*/
	if(hs->multipacket_count<2)
	{		
      offset=uip_len-(hs->INpacket.http_Indata-uip_appdata);
//      printf("\n\r o=%d",(u16_t)offset);
      memcpy(&NETTMPBUFFER[hs->multipacket_datasize],hs->INpacket.http_Indata,offset);								     	   
	  str_ptr=strfind(&NETTMPBUFFER[0],"Content-Type: application");
      if(str_ptr!=0)    
	    {
		    str2=strfind(str_ptr,"octet-stream\r\n\r\n");
	      if(str2!=0)
	      {
//               printf("\n\r888=%s",str2+0x50);
	           if(strfind(str2+0x50,"13f0f013")!=0) //fin image key word
               {
//      	        printf("\n\r find start address");
		      	img512k=0;
		        inspect_img512k=0;	
		      	Global_flag_firmwareupdate_inprogress=1; 
		      	hs->multipacket_count=2; 
		      	//offset=vp[0].value-uip_appdata;   
		        offset2=(str2-&NETTMPBUFFER[0]);
	          //printf("\n\r off1=%d",(u16_t)offset2);
	            offset2-=hs->multipacket_datasize;//minus first packet len
	          //printf("\n\r off2=%d",(u16_t)offset2);
	            vp[0].value=hs->INpacket.http_Indata+offset2;             	
		      	tmplen=offset-offset2;
			       //printf("\n\r len=%d",(u16_t)tmplen);
	            memset(NETTMPBUFFER,0,MAX_BUFFER_SIZE);
               }
               else
               {
				  printf("\n\rError!! image key word");   
				  fflag=2;
                  //wring image
                   /*
                  if(memcmp(hs->INpacket.http_Indata+uip_len-4,"--\r\n",4)==0)
                  {
	                  WatchDogTimerReset();
	                  Global_suspend_count=0;
	                  httpd_init_file(fs_open03("TF.htm"));    
                  }            
        		  return OK;
                  */
               }
        }
        else
        {
  			  fflag=1;
         /* if(memcmp(hs->INpacket.http_Indata+uip_len-4,"--\r\n",4)!=0)
          {
            if(hs->multipacket_count==0)
            {
	            hs->multipacket_datasize+=offset; 
	            hs->multipacket_count++;
            }
            return OK;
          }
          else
          {   
             WatchDogTimerReset();
              Global_suspend_count=0;
              httpd_init_file(fs_open03("TF.htm"));    
          	  return OK;
          }*/
        }
     }
     else   
     {	  
			fflag=1;
          /*if(memcmp(hs->INpacket.http_Indata+uip_len-4,"--\r\n",4)!=0)
          {
            if(hs->multipacket_count==0)
            {
	            hs->multipacket_datasize+=offset; 
	            hs->multipacket_count++;
            }
            return OK;
          }
          else
          {   
             WatchDogTimerReset();    
              httpd_init_file(fs_open03("TF.htm"));    
          	  return OK;
          }   */
     }
     if(fflag>0)
     {
          if(memcmp(&NETTMPBUFFER[hs->multipacket_datasize+offset]-4,"--\r\n",4)!=0)
          {
//            printf("--1");
            if(hs->multipacket_count==0)
            {
//            printf("--2");
	            hs->multipacket_datasize+=offset; 
	            hs->multipacket_count++;
            }
            else
            {
//            printf("--3");
               //if(hs->multipacket_count==1)  //already know wrong data
               {
			           memset(NETTMPBUFFER,0,MAX_BUFFER_SIZE);
                 //hs->multipacket_count=2;
                 hs->multipacket_datasize=0;
                 memcpy(&NETTMPBUFFER[hs->multipacket_datasize],hs->INpacket.http_Indata+offset-100,100);
                 hs->multipacket_datasize+=100;
               } 

            }
            return OK;
          }
          else
          {   
//            printf("--4");
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
            {   size_flag=1;
                //check flash size again
                CBK_Flash_Size_Check();
            } 
	            printf("\rUpdating...%d%%",(u16_t)update);
			      if(img512k==flashsize)
			      {
			          httpd_init_file(fs_open03("OK.htm"));
								Global_flag_restart=1;
								Global_flag_firmwareupdate_inprogress=0;
			      }	            
//	     		return OK;
			}
			else
			{
				printf("\r Write Fail! Restart!");					
				CPU_GOTO_FFFDH();
				//#endif
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
            	vp->value.digits_3_int=KSetting_Info.netif->IPAddress[varid];
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
/*u8_t ssi_errormsg(u8_t varid, data_value *vp)
{
return OK;
}*/
extern u16_t Flash_Erase_Bankno;//chance20100423 for erase bank number
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
            	vp->value.digits_3_int=KSetting_Info.netif->IPAddress[varid];
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
u8_t ssi_updatecomplete(u8_t varid, data_value *vp)
{
    vp->value.digits_3_int=KSetting_Info.netif->IPAddress[varid];
	vp->type=digits_3_int;
	return OK;
}

u8_t cgi_callerase(u8_t cnt, void *vptr)
{
	Flash_check_error_flag=0;
	httpd_init_file(fs_open03("Erase.htm"));
	return OK;
}

u8_t ssi_warn(u8_t varid, data_value *vp){
return OK;
}
//chance add 20100322 for top frame
u8_t ssi_main(u8_t varid, data_value *vp)
{
    u8_t tmpBuf[15];
    memset(tmpBuf,0,15);
    switch(page_flag)    
    {
      case 0://Erase.htm
            sprintf(tmpBuf,"Erase.htm");
            break;
      case 1://Warn.htm
            sprintf(tmpBuf,"Warn.htm");
            break;
      case 2://TFTPGoing.htm
            sprintf(tmpBuf,"TFTPGoing.htm");
            break;
      case 3://TFTPGoing.htm
            sprintf(tmpBuf,"fu.htm");
            break;
    }
	vp->value.string=tmpBuf;
    vp->type=string;
	return OK;
}

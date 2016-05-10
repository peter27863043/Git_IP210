/******************************************************************************
*
*   Name:           httpd.c
*
*   Description:    HTTP Server
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
//include data
#include <stdlib.h>
#include "uip.h"
#include "httpd.h"
#include "fs.h"
#include "cgi.h"
#include "httpd_app.h"
#include <string.h>
#include <stdio.h>
//extern data
extern unsigned char errmsgflag ;
extern u8_t Flash_check_error_flag;// james 20090506
//function prototype

//public data
req_data reqdata[10];
struct httpd_info *hs;
u32_t img512k, inspect_img512k;
extern u16_t Flash_Bankno;
extern u16_t Flash_Erase_Bankno;
//chance add 20100322 for top frame
u8_t page_flag=0;//0: Erase.htm 1: Warn.htm 2: TFTPGoing.htm 3: fu.htm

//define data
#define rx_eth  ((ETH_HDR *)&uip_buf[0])
#define ISO_slash    0x2f   //"/"
#define ISO_space    0x20   //" "
#define ISO_nl       0x0a   //Line feed \n
#define ISO_cr       0x0d   // Carriage return \r
#define ISO_question 0x3f   //"?"
#define ISO_EOF      0x0   //'\0'

extern u16_t Phase_Flash_Erase;
/******************************************************************************
*
*  Function:    httpd_init
*
*  Description: Initialize the web server.
*               Starts to listen for incoming connection requests on TCP port 80. 
*               Clear cookie data, and challenge data
*               
*  Parameters:  None
*               
*  Returns:     None
*               
*******************************************************************************/
void
httpd_init(void)
{
  // Listen to port 80.
  uip_listen(HTTP_PORT);
//  printf("\n\r Chance:httpd_init");  
  //Clear Cookie and Challenge data and age 20070801
//  memset(&cookie_data, 0, sizeof(COOKIE_Type)) ;  
  //memset(&challenge_data, 0, sizeof(COOKIE_Type)) ; 

}
/******************************************************************************
*
*  Function:    httpd
*
*  Description: Handle http request from client
*
*  Parameters:  None
*               
*  Returns:     None
*               
*******************************************************************************/
void
httpd(void)
{
  SFILENAME *hfile_ptr;  
  u8_t *p_reqfile,*CR_ptr,*str_ptr;//filenum
  u8_t requestname_ptr=0, count=0;
  u16_t i_httpd;


    /* Pick out the application state from the uip_conn structure. */
    hs = (struct httpd_info *)(uip_conn->appstate);
//    printf("\n\r,ptr=%4.4x,cnt=%4.4x,ppos=%4.4x,cpos=%4.4x,tcpos=%4.4x,cntu=%4.4x ",hs->dataptr,hs->count,hs->html_pre_pos,hs->html_current_pos,hs->tag_current_pos,hs->count_unack);//jc_db
    /* We use the uip_ test functions to deduce why we were
       called. If uip_connected() is non-zero, we were called
       because a remote host has connected to us. If
       uip_newdata() is non-zero, we were called because the
       remote host has sent us new data, and if uip_acked() is
       non-zero, the remote host has acknowledged the data we
       previously sent to it. */
    if (uip_flags & UIP_CONNECTED) 
    {
//  printf("\n\r Chance:UIP_CONNECTED"); 
      /* Since we have just been connected with the remote host, we
         reset the state for this connection. The ->count variable
         contains the amount of data that is yet to be sent to the
         remote host, and the ->state is set to HTTP_NOGET to signal
         that we haven't received any HTTP GET request for this
         connection yet. */
      hs->state = HTTP_NOGET;
      hs->count = 0;
	  hs->poll_count=0;
      hs->multipacket_count=0;
      return;

    } 
    if (uip_flags & UIP_POLL)
    {
      /* If we are polled ten times, we abort the connection. This is
         because we don't want connections lingering indefinately in
         the system. */
      if (!(uip_conn->tcpstateflags & UIP_OUTSTANDING))
	  {
		  if (hs->poll_count++ >= 10) 
		  {    
		         uip_flags = UIP_ABORT;
	      }
	  }
      return;
    } 
    if (uip_flags & (UIP_CLOSE | UIP_ABORT))
    {
//  printf("\n\r Chance:UIP_CLOSE|UIP_ABORT");
        return;
    }
  if (uip_flags & UIP_NEWDATA)// && (hs->state == HTTP_NOGET))  
  {
//  printf("\n\r Chance:UIP_NEWDATA");
    if(hs->state == HTTP_NOGET) 
    {
		hs->poll_count=0;
		/* This is the first data we receive, and it should contain a
		GET or POST. */
		
		/* Check for GET or POST. */
		if (strncmp(uip_appdata, "GET ", 4) != 0)
		{

			if(strncmp(uip_appdata, "POST ", 5) != 0)
			{
				/* If it isn't a GET or POST, we abort the connection. */
				uip_flags = UIP_ABORT;
				return;
			}
			else
			{
//  printf("\n\r Chance:POST");
				requestname_ptr=5;
				hs->state=HTTP_POST;
			}
		}
		else
		{
//  printf("\n\r Chance:GET");
			requestname_ptr=4;
			hs->state=HTTP_GET;
		}	   

		uip_appdata[uip_len] = ISO_EOF;//end the indata	
		/* Find the file we are looking for. */
		for(i_httpd = requestname_ptr; i_httpd < strlen(&uip_appdata[requestname_ptr]); i_httpd++) 
		{
			if(uip_appdata[i_httpd] == ISO_space ||
			   uip_appdata[i_httpd] == ISO_cr ||
			   uip_appdata[i_httpd] == ISO_nl)			   
			  {
				   uip_appdata[i_httpd] = ISO_EOF;
				   break;
			  }

		}		
		hs->INpacket.http_cgiInfo=NULL;
		//for Method Get***************
		if(hs->state==HTTP_GET)
		{
			hs->INpacket.http_Indata=&(uip_appdata[requestname_ptr]);
			hs->INpacket.http_cgiInfo=strchr(hs->INpacket.http_Indata,ISO_question);//&uip_appdata[requestname_ptr], ISO_question);		
	        if (hs->INpacket.http_cgiInfo)
	 		{
				*hs->INpacket.http_cgiInfo=ISO_EOF;
				hs->INpacket.http_cgiInfo++;							
                //get cgi input item and value and total item
				count=querystr_func();
	        }

		}
		//***********************************
		//for Method Post***************
		if(hs->state==HTTP_POST)
		{
			hs->INpacket.http_Indata=&uip_appdata[i_httpd+1];//chance 20080624 for firefox3
			/*hs->INpacket.http_cgiInfo=strfind(hs->INpacket.http_Indata,"\r\n\r\n");
			//*************last value remove 0d0a 050815**************
			CR_ptr=strchr(hs->INpacket.http_cgiInfo,ISO_cr);
			if(CR_ptr)
			{
				*CR_ptr=ISO_EOF;
			}
			//*************************************************
			if(hs->INpacket.http_cgiInfo)
			{
				count=querystr_func();
			}*/
		}
//		*hs->INpacket.http_Indata=ISO_EOF;//end the cookie string
		//p_reqfile=NULL;
		p_reqfile=&uip_appdata[requestname_ptr];//get request file name
		//***************find the htm file in fs_files table*************	
        hfile_ptr=NULL;
		hfile_ptr=fs_open03(p_reqfile);//,&filenum);
		if(hfile_ptr!=NULL)
		{			
			hs->op_index=0;//filenum;
		}
		if(!hfile_ptr)
		{
//  printf("\n\r Chance:request file name=%s len=%d",p_reqfile,(u16_t)strlen(p_reqfile));
            //request file name not null, and length=1 and data is '/'
            //then open the login.htm
			//if((p_reqfile == ISO_slash )&& (strlen(p_reqfile) == 1)) 
            if(strncmp(p_reqfile, "/", 1) == 0)
			{
//                printf("\n\r Chance:open index.htm");

				if(Flash_check_error_flag ==0)
				{ 
//                    if(Phase_Flash_Erase < (Flash_Bankno*16))
                    if(Phase_Flash_Erase < (Flash_Erase_Bankno*16))
					{
                       page_flag=0;//Erase.htm
                       //hfile_ptr =fs_open03("main.htm");//,&hs->op_index); //index.htm
                    }
                    else
					{
                       page_flag=3;//fu.htm
				       //hfile_ptr =fs_open03("main.htm");//,&hs->op_index); //index.htm
                    } 						
                }
				else
				{
					page_flag=1;//Warn.htm					
				}
                hfile_ptr =fs_open03("main.htm");
			} 						
		}
		if(hfile_ptr)
		{
            str_ptr=strfind(p_reqfile,".");
  			httpd_init_file(hfile_ptr);
            if(!strncmp(str_ptr,"cgi",3))
            {
				#ifdef	HTTPD_DEBUG
					printf("\n\r Find the function");
				#endif	
                (*(HS->cgi_func.func))(count, reqdata);
				
            }

		}
    }
	else
    {
    	
	    uip_appdata[uip_len] = ISO_EOF;//end the indata
		count=0;
		//hs->multipacket_count++;     
		reqdata[0].item="MPKT";
//		reqdata[0].value=uip_appdata;        
		/*if(hs->multipacket_count==1)
		{
            str_ptr=NULL;
	        str_ptr=strfind(uip_appdata,"octet-stream");
	        str_ptr=strfind(str_ptr,"\r\n\r\n");
            reqdata[0].value=str_ptr;
            //printf("\n\r data=%x",(u16_t)*(reqdata[0].value));
		}
        else
        {
		    reqdata[0].value=uip_appdata; 
        }*/
        	   hs->INpacket.http_Indata=uip_appdata; 
		         reqdata[0].value=uip_appdata;         
//        printf("\n\r uip_len=%d",(u16_t)uip_len);
        (*(HS->cgi_func.func))(count, reqdata);
    }
  }    
	//*************ACK ********************
	if (uip_flags & UIP_ACKDATA) 
	{
		hs->poll_count=0;
		hs->retx_count=0;// if acked in this connection, we reset the retransmit counter
		if(HS->cgi_func_id==NO_EX_FUNC)
        {
			u16_t acklen;
			acklen=hs->last_len;
			if(hs->count >= acklen) 
			{	
	  
				hs->header_flag = 1;
	            if (hs->state == HTTP_HEADER)
			  	{	
	  		  		hs->count -= (acklen-hs->header_length);
			 	    hs->dataptr += (acklen-hs->header_length);				
					//hs->first_ack_flag=1;
				}
				else
				{
	  		  		hs->count -= acklen;
			 	    hs->dataptr += acklen;										
				}
			} 
			else 
			{
			  hs->count = 0;		  
			}
		}
		else //have cgi tag
		{
			hs->header_flag=1;
			hs->count=hs->count_unack;
			hs->tag_index_unack=hs->tag_index;//chance modified 050812				
			hs->row_num_unack=hs->row_num;
			hs->col_num_unack=hs->col_num;
		}
		hs->html_pre_pos=hs->html_current_pos;//modified by chance 050811
        if (hs->state == HTTP_HEADER)
        {
            hs->state = HTTP_FILE;
        }		
		if(hs->count == 0) 
		{
		    uip_flags=UIP_CLOSE ;
			return;
		}
	}    
	uip_len=0;    
	//***********************************POLLing*********************************
    if(!(uip_flags & UIP_POLL)) 
    {
		hs->poll_count=0;
        if (uip_flags & UIP_REXMIT)
        {	
  		  hs->retx_count++;
	      if(hs->retx_count >= 10) 
	      {
		         uip_flags = UIP_ABORT;
	      }
		  else
	      {
			//********for cgi tag use***************
			//if tag_index_unack=0 then tag_index=0;
			hs->tag_index=hs->tag_index_unack;//?(hs->tag_index_unack+1):(hs->tag_index_unack);//chance modified 050812
			hs->tag_flag=(hs->tag_index)?(hs->tag_flag):0;
			hs->row_num=hs->row_num_unack;
			hs->col_num=hs->col_num_unack;
			//************************************
			hs->dataptr=hs->hsfile->file_ptr;//restore default value
			hs->dataptr+=hs->html_pre_pos;// add have done postion
			hs->html_current_pos=hs->html_pre_pos;// HTML current posistion return to pre position
	      }
        }
        //********************  
		if(hs->state!=HTTP_MULTIPACKET)//for post mulitipacket chance20070823
		{              
	        if (hs->hsfile)
	        {
		        if (hs->header_flag==0)
				{
	                //if httpd header not send, then initial httpd header
	            	httpd_header_init();
				}
  //printf("\n\r Chance:Httpd_Data_Processing");  
	            //if file exist and need to send then processing.
				Httpd_Data_Processing();	
				uip_slen=uip_len;
			    hs->last_len=uip_len;
	        }
        }
    }
    return;

}


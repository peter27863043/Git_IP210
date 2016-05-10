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
#include <module.h>
#include <stdlib.h>
#include "uip.h"
#include "httpd.h"
#include "fs.h"
#include "IP210reg.h"
#include "cgi.h"
#include "module.h"
#include "wdg.h"
#include <string.h>
#include <stdio.h>
//extern data
#include "fs.h"
extern unsigned char errmsgflag ;
//function prototype
#ifdef MODULE_NET_UART
#include "net_uart.h"
#endif
#ifdef MODULE_SMTP
#include "../smtp/smtp.h"
#endif
#ifdef MODULE_NET_CMD
#include "../netcmd/net_cmd.h"
#endif 
#ifdef MODULE_NET_UART_CTRL
#include "netuart_ctrl.h"
#endif

#include "application.h"


extern void CHECK_210();
//public data
req_data reqdata[75];
struct httpd_info *hs;
//define data
#define rx_eth  ((ETH_HDR *)&uip_buf[0])
#define ISO_slash    0x2f   //"/"
#define ISO_space    0x20   //" "
#define ISO_nl       0x0a   //Line feed \n
#define ISO_cr       0x0d   // Carriage return \r
#define ISO_question 0x3f   //"?"
#define ISO_EOF      0x0   //'\0'
#include "httpd_app.h"

u8_t on_tcp_rx();

#ifdef MODULE_CLI_CUSTOMIAZE
void http_server(void)
{
	if(on_tcp_rx())return;
}
#else
u8_t EE_TMPBUFFER[MAX_BUFFER_SIZE];
u16_t getlen=0;
u8_t httpd_copydata();
u8_t httpd_check_cookie(u8_t *reqfile);
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
  uip_listen(80);
  
  //Clear Cookie and Challenge data and age 20070801
  memset(&cookie_data, 0, sizeof(COOKIE_Type)) ;  
  memset(&challenge_data, 0, sizeof(COOKIE_Type)) ; 

#ifdef MODULE_NET_UART
   NetUartInit();
#endif
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
#ifdef DB_FIFO
extern u8_t rx_over;
#endif
void
http_server(void)
{
  SFILENAME *hfile_ptr;  
  u8_t *p_reqfile, filenum,*CR_ptr,*str_ptr;
  u8_t requestname_ptr=0, count=0;
  u16_t i_httpd;
  u8_t tmp_EA=EA;

  EA=0;

	CHECK_210();
#ifdef MODULE_NET_UART
	NET_UART_LOOP('C');
#ifdef DB_FIFO
	if(rx_over){printf("\n\r55");rx_over=0;}
#endif
#endif
  if(on_tcp_rx()){ return; }
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
      /* Since we have just been connected with the remote host, we
         reset the state for this connection. The ->count variable
         contains the amount of data that is yet to be sent to the
         remote host, and the ->state is set to HTTP_NOGET to signal
         that we haven't received any HTTP GET request for this
         connection yet. */
      	memset(uip_conn->appstate,0,sizeof(struct httpd_info));//chance 20080609
      	hs->state = HTTP_NOGET;
		memset(EE_TMPBUFFER, 0xFF, MAX_BUFFER_SIZE);
		getlen=0;
      	hs->count = 0;
	  	hs->poll_count=0;
	  	EA=tmp_EA;
      	return;

    } 
    if (uip_flags & UIP_POLL)
    {
      /* If we are polled ten times, we abort the connection. This is
         because we don't want connections lingering indefinately in
         the system. */
		#ifdef	UIP_POLL_DEBUG	
	    printf("\n\r UIP_POLL! hs->poll_count=%d",(u16_t)hs->poll_count);
	    printf("\n\r uip_conn->tcpstateflags=%d",(u16_t)uip_conn->tcpstateflags);
		#endif 
	    if (!(uip_conn->tcpstateflags & UIP_OUTSTANDING))
		{
			if (hs->poll_count++ >= 10) 
			{    
				uip_flags = UIP_ABORT;
			}
		}
		EA=tmp_EA;
	    return;
    } 
    if (uip_flags & (UIP_CLOSE | UIP_ABORT))
    {
    	EA=tmp_EA;
        return;
    }
  	if (uip_flags & UIP_NEWDATA)// && (hs->state == HTTP_NOGET))  
  	{
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
					EA=tmp_EA;
					return;
				}
				else
				{
					requestname_ptr=5;
					hs->state=HTTP_POST;
				}
			}
			else
			{
				requestname_ptr=4;
				hs->state=HTTP_GET;
				//20090817
				if(!httpd_copydata())
				{
					return;
				}
				if(!httpd_check_cookie(&uip_appdata[requestname_ptr]))
				{
					if(hs->state==HTTP_GET_MULTIPACKET)
					{
						return;
					}
				}
				else
				{
//					hfile_ptr=(SSI_FILE_KERNEL*)fs_open("timeout.htm",&hs->op_index); 
				}
			}	   
			uip_appdata[uip_len] = ISO_EOF;//end the indata
			/* Find the file we are looking for. */
			for(i_httpd = requestname_ptr; i_httpd < strlen(&uip_appdata[requestname_ptr]); i_httpd++) 
			{
				WatchDogTimerReset();
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
					#ifdef	HTTPD_DEBUG							
					printf("\n\r http_Indata=%s",hs->INpacket.http_Indata);	
					#endif	
	                //get cgi input item and value and total item
					count=querystr_func();
		        }
	
			}
			//***********Cookie*******************
	       	//find cookie or Cookie
			hs->INpacket.http_Indata=&(uip_appdata[i_httpd+1]);
			#ifdef	HTTPD_DEBUG							
				printf("\n\r cookie http_Indata=%s",hs->INpacket.http_Indata);	
			#endif	
			findcookie(hs->INpacket.http_Indata);  
			
			//***********************************
			//for Method Post***************
			if(hs->state==HTTP_POST)
			{
				//for firefox3 20080625 chance
				#ifdef MULTI_PACKET
				hs->INpacket.http_cgiInfo+=2;
				str_ptr=NULL;
				str_ptr=strfind(hs->INpacket.http_Indata,"multipart/form-data");
				if(str_ptr!=NULL)
				{		
					count=0;
					reqdata[0].item="MPKT";
					reqdata[0].value=str_ptr;			  	
				}//***********************************
				else
				#endif
				{
					hs->INpacket.http_cgiInfo=strfind(hs->INpacket.http_Indata,"\r\n\r\n");
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
					}
			  	}
			}
			*hs->INpacket.http_Indata=ISO_EOF;//end the cookie string
			p_reqfile=NULL;
			p_reqfile=&uip_appdata[requestname_ptr];//get request file name
			//***************find the htm file in fs_files table*************
			hfile_ptr=NULL;		

			

			if((GetTVState())==eScaler_RS232_No_Service){
				hs->errormsg = "RS232 No Service";
				errmsgflag = ERRORMSG_WAIT5;
				if(!(strcmp(p_reqfile, "/telnet.htm"))){					//Picture&Sound	change xdata to code (telnet_htm.c, htmlfiles10.c)
					hfile_ptr=fs_open("NoService.htm",&filenum);
				}	
				else if(!(strcmp(p_reqfile, "/udp_tel.htm"))){				//Screen			change xdata to code (udp_tel_htm.c, htmlfiles11.c)
						hfile_ptr=fs_open("NoService2.htm",&filenum);
				}	
				else if(!(strcmp(p_reqfile, "/uart.htm"))){				//Setting			change xdata to code (uart_htm.c, htmlfiles12.c)
						hfile_ptr=fs_open("NoService3.htm",&filenum);
				}	
				else if(!(strcmp(p_reqfile, "/smtp.htm"))){				//Schedule		change xdata to code (smtp_htm.c, htmlfiles13.c) 
						hfile_ptr=fs_open("NoService4.htm",&filenum);
				}	
				else if(!(strcmp(p_reqfile, "/adc.htm"))){					//Mail Report
						hfile_ptr=fs_open("NoService5.htm",&filenum);
				}	
				else if(!(strcmp(p_reqfile, "/dido.htm"))){				//Network
						hfile_ptr=fs_open("NoService6.htm",&filenum);
				}					
				else
					hfile_ptr=fs_open(p_reqfile,&filenum);
			}
			else if((GetTVState())!=eScaler_PowerOn){
					hs->errormsg = "Standby!";
					errmsgflag = ERRORMSG_WAIT5;
					
					if(!(strcmp(p_reqfile, "/udp_tel.htm"))){				//Screen
							hfile_ptr=fs_open("NoService2.htm",&filenum);
					}	
					else if(!(strcmp(p_reqfile, "/uart.htm"))){ 			 	//Setting	
							hfile_ptr=fs_open("NoService3.htm",&filenum);
					}	
					else if(!(strcmp(p_reqfile, "/smtp.htm"))){ 				//Schedule
							hfile_ptr=fs_open("NoService4.htm",&filenum);
					}	
					else if(!(strcmp(p_reqfile, "/adc.htm"))){					//Mail Report
							hfile_ptr=fs_open("NoService5.htm",&filenum);
					}	
					else 
						hfile_ptr=fs_open(p_reqfile,&filenum);
					
			}		
			else 
				hfile_ptr=fs_open(p_reqfile,&filenum);
			
			
			if(hfile_ptr!=NULL)
			{
				printf("GET File Name=%s len=%d\n",p_reqfile,hfile_ptr->file_len); //peter test
/*
				if(!httpd_check_cookie(p_reqfile))
				{
printf("\n\rMultipacket %d",(u16_t)hs->state);
					if(hs->state==HTTP_GET_MULTIPACKET)
					{
printf("\n\rReturn");
						return;
					}
				}
				else
				{
//					hfile_ptr=(SSI_FILE_KERNEL*)fs_open("timeout.htm",&hs->op_index); 
				}
*/
				hs->op_index=filenum;
			}
			if(!hfile_ptr)
			{
	            //request file name not null, and length=1 and data is '/'
	            //then open the DEF_PAGE
				if((p_reqfile == ISO_slash )&& ((strlen(p_reqfile)) == 1)) 
				{
					hfile_ptr =fs_open(DEF_PAGE,&hs->op_index); //index.htm
				} 						
			}
			//***************check cookie****************		
#ifndef DIS_COOKIE		
	        if( strcmp(str_ptr, "/") != 0 )//DEF_PAGE can process
			{
				//besides login.cgi and md5.js, other files must check cookie

				if(strcmp(p_reqfile, "/cgi/login.cgi"))
		        {		        
					#ifdef	HTTPD_DEBUG
						printf("\n\r hs->INpacket.Cookie_item=%s",hs->INpacket.Cookie_item);
						printf("\n\r hs->INpacket.Cookie_data=%s",hs->INpacket.Cookie_data);
						printf("\n\r Cookie_data=%s",&(cookie_data.value[0]));
					#endif	    
					if(!(p_reqfile[1]=='l'&&p_reqfile[2]=='o'&&p_reqfile[3]=='g'&&p_reqfile[4]=='i'&&p_reqfile[5]=='n'))
		            if(!hs->INpacket.Cookie_data || httpd_parse_cookie()!= OK)
		            {		
		                //hfile_ptr =fs_open(DEF_PAGE,&hs->op_index);
		                httpd_login_fail();
		            }
		        }
			}
#endif// DIS_COOKIE
			if(!hfile_ptr)
			{
				hfile_ptr =fs_open(DEF_PAGE,&hs->op_index);
			}
			//*****************************
			//chance's Cookie
			hs->INpacket.Cookie_data = NULL;
			//**********************
			if(hfile_ptr)
			{
				//printf("http_server CGI Fuc %d=%s \n",count,reqdata);//peter
	            str_ptr=strfind(p_reqfile,".");
	  			httpd_init_file(hfile_ptr);
				
	            if(!strncmp(str_ptr,"cgi",3))
	            {
					#ifdef	HTTPD_DEBUG
						printf("\n\r Find the function");
					#endif	
	                (*(HS->cgi_func.func))(count, reqdata);
					
	            }
	            /*else
	            {
					httpd_init_file(hfile_ptr);
	            }*/
			}
			else
			{
	            //file not found, return 404 information
				httpd_file_not_found();	
			}
 	   	}
#ifdef MULTI_PACKET
		else
	    {
			if(!httpd_copydata())
			{
				hs->errormsg="Please modify your html file, Mothod Get can not receive more than 2000 bytes!";
				errmsgflag=ERRORMSG_LOGIN;
				httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
//				return;
			}
			else
			{
//				hs->INpacket.http_Indata=uip_appdata;
				findcookie(&EE_TMPBUFFER[getlen-30]);
				if(!hs->INpacket.Cookie_data)
				{ return; }
				else
				{
					if(httpd_parse_cookie()!=OK)
					{ httpd_login_fail(); }
					else
					{
						EE_TMPBUFFER[getlen]=ISO_EOF;
						for(i_httpd = 4; i_httpd < getlen; i_httpd++) 
						{
							WatchDogTimerReset();
							if(EE_TMPBUFFER[i_httpd] == ISO_space ||
							   EE_TMPBUFFER[i_httpd] == ISO_cr ||
							   EE_TMPBUFFER[i_httpd] == ISO_nl)			   
							{
								EE_TMPBUFFER[i_httpd] = ISO_EOF;
								break;
							}
						}
		
						hs->INpacket.http_Indata=&(EE_TMPBUFFER[4]);
						hs->INpacket.http_cgiInfo=strchr(hs->INpacket.http_Indata,ISO_question);//&uip_appdata[requestname_ptr], ISO_question);		
				        if (hs->INpacket.http_cgiInfo)
				 		{
							*hs->INpacket.http_cgiInfo=ISO_EOF;
							hs->INpacket.http_cgiInfo++;	
							#ifdef	HTTPD_DEBUG							
							printf("\n\r http_Indata=%s",hs->INpacket.http_Indata);	
							#endif	
			                //get cgi input item and value and total item
							count=querystr_func();
				        }
		//				*hs->INpacket.http_Indata=ISO_EOF;//end the cookie string
						p_reqfile=NULL;
						p_reqfile=&EE_TMPBUFFER[4];//get request file name
						//***************find the htm file in fs_files table*************
						hfile_ptr=NULL;		
				        hfile_ptr=NULL;
						hfile_ptr=fs_open(p_reqfile,&filenum);
		
						if(hfile_ptr!=NULL)
						{ hs->op_index=filenum; }
						//***************check cookie****************		
		
						if(!hfile_ptr)
						{
							hfile_ptr =fs_open(DEF_PAGE,&hs->op_index);
						}
						//*****************************
						//chance's Cookie
						hs->INpacket.Cookie_data = NULL;
						//**********************
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
				            /*else
				            {
								httpd_init_file(hfile_ptr);
				            }*/
						}
						else
						{
				            //file not found, return 404 information
							httpd_file_not_found();	
						}
					}
				}
			}
/*
		    uip_appdata[uip_len] = ISO_EOF;//end the indata
			count=0;
			//hs->multipacket_count++; //20080625
			reqdata[0].item="MPKT";
			reqdata[0].value=uip_appdata;
	        (*(HS->cgi_func.func))(count, reqdata);
*/
	    }
#endif
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
		#ifdef	UIP_REXMIT_DEBUG
	        printf("\n\r UIP_ACKDATA hs->tag_index_unack=%x",hs->tag_index_unack);
			printf("\n\r UIP_ACKDATA hs->row_num_unack=%x",hs->row_num_unack);
			printf("\n\r UIP_ACKDATA hs->col_num_unack=%x",hs->col_num_unack);
		#endif
		}
		hs->html_pre_pos=hs->html_current_pos;//modified by chance 050811
        if (hs->state == HTTP_HEADER)
        {
            hs->state = HTTP_FILE;
        }		
		if(hs->count == 0) 
		{
		    uip_flags=UIP_CLOSE ;
		  	EA=tmp_EA;
			return;
		}
	}    
	uip_len=0;    
	//***********************************POLLing*********************************
    if(!(uip_flags & UIP_POLL)) 
    {
		hs->poll_count=0;
	    //Send a piece of data, but not more than the MSS of the connection.      
	#ifdef	UIP_POLL_DEBUG	
		printf("\n\r Not Pollinghs->poll_count=%x",hs->poll_count);
	#endif  
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
       		//*************************************
            //memcpy(rx_eth->src_mac,uip_conn->rmacaddr,  6);
            //memcpy4(uip_conn->ack_nxt, uip_conn->ack_seq);
		#ifdef	UIP_REXMIT_DEBUG
			printf("\n\r Retransmit filename =%s",hs->file_name);
			printf("\n\r Retransmit hs->retx_count=%x",hs->retx_count);
			printf("\n\r Retransmit hs->tag_index=%x",hs->tag_index);
			printf("\n\r Retransmit hs->tag_index_unack=%x",hs->tag_index_unack);
			printf("\n\r Retransmit hs->row_num=%x",hs->row_num);
			printf("\n\r Retransmit hs->row_num_unack=%x",hs->row_num_unack);
			printf("\n\r Retransmit hs->col_num=%x",hs->col_num);
			printf("\n\r Retransmit hs->col_num_unack=%x",hs->col_num_unack);
		#endif
	      }
        }
        //********************  
#ifdef MULTI_PACKET         
		if(hs->state!=HTTP_MULTIPACKET)//for post mulitipacket chance20070823
#endif			
		{               		 
	        if (hs->hsfile)
	        {
		        if (hs->header_flag==0)
				{
	                //if httpd header not send, then initial httpd header
	            	httpd_header_init();
				}
	            //if file exist and need to send then processing.
				Httpd_Data_Processing();	
				uip_slen=uip_len;
			   	hs->last_len=uip_len;
				
				
	        }
    	}
  	}
    EA=tmp_EA;
    return;

}

u8_t httpd_copydata()
{
	if(uip_len+getlen>MAX_BUFFER_SIZE)
	{
		return 0;
	}
	else
	{
		memcpy(EE_TMPBUFFER+getlen, uip_appdata, uip_len);
		getlen+=uip_len;
		return 1;
	}
}

u8_t httpd_check_cookie(u8_t *reqfile)
{
	if( (strcmp(reqfile, "/cgi/login.cgi")) &&
		(strcmp(reqfile, "/login2.htm")) &&
		(strcmp(reqfile, "/login.htm")) )
	{
		if( (hs->state==HTTP_GET)&&
			(memcmp(&EE_TMPBUFFER[getlen-4], "\r\n\r\n", 4)!=0) )
		{
			hs->state=HTTP_GET_MULTIPACKET;
			return 0;
		}
//		if(!hs->INpacket.Cookie_data)
//		{ return 0; }
//		else
//		{ }
	}
	return 1;
}
#endif

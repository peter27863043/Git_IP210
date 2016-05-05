/******************************************************************************
*
*   Name:           cgi.c
*
*   Description:    Handle CGI and SSI information
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
//*******************Include files area**************************
#include <stdio.h>
#include <string.h>
#include "uip.h"
#include "cgi.h"
#include "httpd.h"
#include "fs.h"
#include "errormsg_htm.h"
//******************Extern function prototype********************
extern char *itostr(u8_t int_value);
extern u8_t no_tag_page();
extern u8_t with_tag_page();
extern u8_t last_tag_page();
extern tag_address_bigger_mss();
//******************structure define*****************************
//****************Global variable****************************

/******************************************************************************
*
*  Function:    Httpd_Data_Processing
*
*  Description: Process SSI and CGI information
*               
*  Parameters:  None
*               
*  Returns:     True or False
*               
*******************************************************************************/
#ifdef MODULE_CLI_CUSTOMIAZE
#else
bit_t Httpd_Data_Processing()
{

	if (hs->hsfile)
	{
		indata.source_ptr=hs->dataptr;
		indata.dest_ptr=&uip_appdata[uip_len];
		if(HS->cgi_func_id==NO_EX_FUNC)
		{//The file did not get the SSI tag information
			#ifdef	CGI_DEBUG
			printf("\n\r no_tag_page()");
			#endif
			no_tag_page();
			return 1;
		}
		else
		{
//			if((fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).pos==0)&&(fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).len==0)&&(fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).varid==0))
			if((fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).pos==0)&&(fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).varid==0))
			{
				#ifdef	CGI_DEBUG
				printf("\n\r last tag page");
				#endif
				last_tag_page();
				return 1;
			}
			else
			{
				#ifdef	CGI_DEBUG
//				dprintf("\n\r Httpd_Data_Processing hs->tag_index=%x",hs->tag_index);
				printf("\n\r Httpd_Data_Processing hs->html_current_pos=%x",(u16_t)(hs->html_current_pos));
				printf("\n\r Httpd_Data_Processing uip_len=%x",(u16_t)uip_len);
				#endif

				if((fsop[HS->cur_fsop_idx].get_tag(hs->tag_index).pos+uip_len-hs->html_current_pos)>uip_conn->mss)
				{	
				#ifdef	CGI_DEBUG
				printf("\n\r Httpd_Data_Processing tag_address_bigger_mss()");
				#endif
					tag_address_bigger_mss();
					return 1;
				}
				else
				{	
				#ifdef	CGI_DEBUG
				printf("\n\r Httpd_Data_Processing with_tag_page()");
				#endif
					with_tag_page();
					return 1;
				}
			}				
		}
	}
	return 0;
}
/*-----------------------------------------------------------------------------------*/
#endif

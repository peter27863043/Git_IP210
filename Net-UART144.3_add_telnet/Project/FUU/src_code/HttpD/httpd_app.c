/******************************************************************************
*
*   Name:           httpd_app.c
*
*   Description:    HTTP Server
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
#include "httpd_app.h"
/******************************************************************************
*
*  Function:    querystr_func
*
*  Description: find request name and vlue in the request string
*               
*  Parameters:  reqdata
*               
*  Returns:     number of request name
*               
*******************************************************************************/
u8_t querystr_func()
{

	u16_t count=0,index;
    while (*hs->INpacket.http_cgiInfo != ISO_EOF)
    {
        reqdata[count].item = hs->INpacket.http_cgiInfo;
        hs->INpacket.http_cgiInfo = strchr(hs->INpacket.http_cgiInfo, '=');
        if (!hs->INpacket.http_cgiInfo)
            break;
        *hs->INpacket.http_cgiInfo = ISO_EOF;//set '=' to 0
		    hs->INpacket.http_cgiInfo++;
        reqdata[count].value = hs->INpacket.http_cgiInfo;
        index = strcspn (hs->INpacket.http_cgiInfo, "&");
        hs->INpacket.http_cgiInfo += index;
        if (*hs->INpacket.http_cgiInfo == '&')
        {
            *hs->INpacket.http_cgiInfo = ISO_EOF;    // end the string
			hs->INpacket.http_cgiInfo++;
        }
		count++;

    }
	#ifdef	querystr_func_DEBUG
	for(index=0;index<count;index++)
	{
		printf("\n\r item=%s \n\r value=%s len=%x",reqdata[index].item,reqdata[index].value,(u8_t)(strlen(reqdata[index].value)));
	}
	#endif
	return count;
}
/******************************************************************************
*
*  Function:    strfind
*
*  Description: find string in the source string
*               
*  Parameters:  source string, the string user want to find
*               
*  Returns:     address of the first character of string user want to find 
                in the source string
*               
*******************************************************************************/
u8_t *strfind(u8_t *srcstr,u8_t *findstr)
{
	u8_t *ptr, *srcstr_end;
	u16_t len,i,check_count;
	srcstr_end=srcstr+strlen(srcstr);

	len=strlen(findstr);
	while(srcstr<srcstr_end)
	{
		ptr=strchr(srcstr,*findstr);
		if(ptr)
		{
			check_count=1;
			for(i=1;i<len;i++)
			{
				if(*(ptr+i)==*(findstr+i))
				{
					check_count++;
				}
			}
			if(check_count==len)
			{
				return (ptr+len);
			}
		}
		else
		{
			return NULL;		
		}
		srcstr=ptr+1;	
	}
	return NULL;		
}
/******************************************************************************
*
*  Function:    findcookie
*
*  Description: find cookie in the request string
*               
*  Parameters:  hs->INpacket.http_Indata
*               
*  Returns:     hs->cookie_name, hs->cookie_value
*               
*******************************************************************************/
/*void findcookie()//u8_t *srcstr)
{
	u8_t *ptr=NULL;
		#ifdef	findcookie_DEBUG
        printf("\n\r findcookie (%X)",hs->INpacket.http_Indata);
		#endif
	ptr = strfind(hs->INpacket.http_Indata, "Cookie:"); 
    if(!ptr)//not find Cookie: and find cookie:
    {
       ptr = strfind(hs->INpacket.http_Indata, "cookie:"); 
    }  
    if (ptr)//if find cookie
    {
		//remove space
		while(*ptr==ISO_space)
		{
			ptr++;
		}
		//ptr
        hs->INpacket.Cookie_item = ptr;
		ptr=NULL;
        ptr = strchr(hs->INpacket.Cookie_item, '=');
		if(ptr)//if find "="
		{
        	*ptr=ISO_EOF;
			ptr++;
			hs->INpacket.Cookie_data=ptr;
		}
		ptr=NULL;
        ptr = strchr(hs->INpacket.Cookie_data, ISO_cr);	
		if(ptr)
		{
			hs->INpacket.http_Indata=ptr;
		}
		#ifdef	findcookie_DEBUG
        printf("\n\r findcookie %s=%s",hs->INpacket.Cookie_item,hs->INpacket.http_Indata);
		#endif
    }
	else
	{
		#ifdef	findcookie_DEBUG
        printf("\n\r findcookie %x",ptr);
		#endif
	}
}
*/
/******************************************************************************
*
*  Function:    httpd_login_pass
*
*  Description: If username and password are correct, web server generate 
*
*               cookie data to client.
*
*  Parameters:  None
*               
*  Returns:     None
*               
*******************************************************************************/
/*void httpd_login_pass()
{
    hs->INpacket.Cookie_data = httpd_set_cookie();
    httpd_init_file(fs_open("index.htm",&hs->op_index));

}*/
/******************************************************************************
*
*  Function:    httpd_login_fail
*
*  Description: Invalid User name or Password, web server generate 
*
*               send error message to client.
*
*  Parameters:  None
*               
*  Returns:     None
*               
*******************************************************************************/
/*void httpd_login_fail()
{
    hs->errormsg = "Invalid User name or Password";
    errmsgflag = ERRORMSG_LOGIN ;
    httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
}*/
/******************************************************************************
*
*  Function:    httpd_ChallengeError
*
*  Description: Invalid Challenge Value or Challenge's Time Expire
*
*
*  Parameters:  None
*               
*  Returns:     None
*               
*******************************************************************************/
/*void httpd_ChallengeError()
{
    hs->errormsg = "Invalid Challenge Value or Challenge's Time Expire";
    errmsgflag = ERRORMSG_CHALLENGE ;
	hs->op_index=0;
    httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
}*/
/******************************************************************************
*
*  Function:    httpd_copy_data
*
*  Description: write data to uip buffer, and add the uip length
*
*  Parameters:  None
*               
*  Returns:     None
*               
*******************************************************************************/
void httpd_copy_data(char *indata, u16_t length)
{
    if (length > 0)
    {
        strcpy(&uip_appdata[uip_len], indata);
        uip_len += length;
    }
}
/******************************************************************************
*
*  Function:    httpd_header_init
*
*  Description: write http header information to uip buffer
*
*  Parameters:  None
*               
*  Returns:     None
*               
*******************************************************************************/
void httpd_header_init()
{
    uip_len = 0;
    httpd_copy_data("HTTP/1.0 " , 9);
    httpd_copy_data(hs->response , (u16_t)(strlen(hs->response)));
	httpd_copy_data("\r\n", 2);
	httpd_copy_data("Connection: close\r\n", 19);
    if (hs->contentType)
    {
        httpd_copy_data("Content-Type: ", 14);
        httpd_copy_data(hs->contentType , (u16_t)(strlen(hs->contentType)));
        httpd_copy_data("\r\n", 2);
    }

    if (!HS->cgi_func.tags)
	{
	    sprintf(&uip_appdata[uip_len], "Content-Length: %u\r\n", hs->count);
        uip_len += (u16_t)(strlen(&uip_appdata[uip_len]));
	}
    /*if (hs->INpacket.Cookie_data)
    {
        sprintf(&uip_appdata[uip_len],"Set-Cookie: ICPlusCookie=%s; path=/\r\n" , hs->INpacket.Cookie_data);
		uip_len += (u16_t)(strlen(&uip_appdata[uip_len]));
    }*/
	httpd_copy_data("\r\n", 2);
    hs->header_length = uip_len;

}
/******************************************************************************
*
*  Function:    httpd_init_file
*
*  Description: set file system for send function
*
*  Parameters:  None
*               
*  Returns:     None
*               
*******************************************************************************/
bit_t httpd_init_file(SFILENAME *infile)
{
	hs->header_flag = 0;
	hs->retx_count	= 0;			
    hs->count = infile->file_len;
//    printf(",file_len=%d, ptr=%x",infile->file_len,infile->file_ptr);//jc_db
	hs->hsfile =infile;
	hs->dataptr=infile->file_ptr;
	hs->state = HTTP_HEADER;
	//cgi tag information
	hs->tag_index=0;
	hs->tag_index_unack=0;
	hs->html_current_pos=0;
	hs->tag_current_pos=0;
	hs->html_pre_pos=0;
	hs->tag_flag=0;
	hs->row_num=0;
	hs->row_num_unack=0;
	hs->table_flag=0;
	hs->col_num=0;
	hs->col_num_unack=0;
	hs->table2_flag=0;
	hs->count_unack=0;
	hs->contentType=NULL;
	hs->response = "200 OK";
//	memcpy(uip_conn->rmacaddr, rx_eth->src_mac, 6);
    return OK;
}
/******************************************************************************
*
*  Function:    httpd_file_not_found
*
*  Description: If the request page is not exist, send the 404 page to client
*
*  Parameters:  None
*               
*  Returns:     None
*               
*******************************************************************************/
/*
void httpd_file_not_found()
{
    uip_len=0;
	hs->count=0;
    hs->hsfile = NULL;	
	hs->dataptr=NULL;
    hs->state = HTTP_HEADER;
    hs->response = "404 File Not Found";    
    httpd_header_init();
}
*/

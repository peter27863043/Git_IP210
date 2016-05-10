/******************************************************************************
*
*   Name:           httpd_app.h
*
*   Description:    HTTPD_APP Header
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
//include data
#include <module.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "type_def.h"
#include "uip.h"
#include "httpd.h"
#include "cgi.h"
//public data
extern unsigned char errmsgflag ;
extern req_data reqdata[10];
extern struct httpd_info *hs;
//define data
#define ISO_space    0x20   //" "
#define ISO_cr       0x0d   // Carriage return \r
#define ISO_EOF      0x0   //'\0'
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
u8_t querystr_func();
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
u8_t *strfind(u8_t *srcstr,u8_t *findstr);
/******************************************************************************
*
*  Function:    findcookie
*
*  Description: find cookie in the request string
*               
*  Parameters:  hs->query_string
*               
*  Returns:     hs->cookie_name, hs->cookie_value
*               
*******************************************************************************/
void findcookie();//u8_t *srcstr)
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
void httpd_login_pass();
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
void httpd_login_fail();
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
void httpd_ChallengeError();
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
void httpd_copy_data(char *indata, u16_t length);
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
void httpd_header_init();
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
bit_t httpd_init_file(SFILENAME *fsfile);
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
void httpd_file_not_found();

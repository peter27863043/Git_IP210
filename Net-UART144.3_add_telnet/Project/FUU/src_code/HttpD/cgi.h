/******************************************************************************
*
*   Name:           cgi.h
*
*   Description:    cgi header
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/

#ifndef __CGI_H__
#define __CGI_H__
#include "fs.h"
/* The HTTP server states: */
#define HTTP_NOGET        0
#define HTTP_FILE         1
#define HTTP_TEXT         2
#define HTTP_FUNC         3
#define HTTP_END          4
#define HTTP_HEADER       5
#define HTTP_POST         6
#define HTTP_GET          7
#define HTTP_MULTIPACKET  8
//*********Function Prototype*******
bit_t Httpd_Data_Processing();
//**********************************
#endif /* __CGI_H__ */

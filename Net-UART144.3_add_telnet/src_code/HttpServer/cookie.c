/******************************************************************************
*
*   Name:           cookie.c
*
*   Description:    Handle cookie check and generate
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
//*******************Include files area**************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uip.h"
#include "httpd.h"
#include "type_def.h"
#include "timer.h"
#ifdef MODULE_CLI_CUSTOMIAZE
#else
//*******************Global value**************************
COOKIE_Type cookie_data;
/******************************************************************************
*
*  Function:    httpd_parse_cookie
*
*  Description: Check the cookie time and cookie value
*               
*  Parameters:  None
*               
*  Returns:     If cookie is valid, return True. Otherwise, return False
*               
*******************************************************************************/
bit_t httpd_parse_cookie()
{
	u32_t currentage,sum_age;
	IP210_Update_Timer_Counter();
	currentage=timercounter;
	sum_age=currentage-cookie_data.age;
//	printf("\n\r currentage=%lu",currentage);
//	printf("\n\r cookie age=%lu",cookie_data.age);
//	printf("\n\r parsecookie=%s",hs->cookie_name);
	if(sum_age<0)
	{
		sum_age=(~sum_age)+1;
	}
	if(sum_age>*((u32_t*)get_system_parameter(SYS_PARAM_EXPIRE_TIME)))
	{
	    memset(cookie_data.value, 0, sizeof(cookie_data.value)) ;  
		//cookie_data.age=currentage;
	}
	else
	{	
		if(!(strcmp(hs->INpacket.Cookie_item,"ICPlusCookie")))//cookie name check 
		{
		//printf("\n\r parsecookievalue%s compare %s",hs->INpacket.Cookie_data, &(cookie_data.value[0]));
			if(!(strcmp(hs->INpacket.Cookie_data,&(cookie_data.value[0]))))//cookie value check
			{
				cookie_data.age=currentage;
				challenge_data.age=currentage;
				return OK;//ERROR
			}				
			//printf("\n\r Cookie data error");
		}
		//printf("\n\r Cookie item error");
	}
	return ERROR;
}
/******************************************************************************
*
*  Function:    httpd_set_cookie
*
*  Description: generate the cookie value
*               
*  Parameters:  None
*               
*  Returns:     Address of the cookie value
*               
*******************************************************************************/
u8_t *httpd_set_cookie()
{
	u8_t i_cookie;
	u8_t buff;
    srand((u16_t)timercounter);
	for (i_cookie = 0; i_cookie < (MAX_COOKIE_LEN-1); i_cookie++) 
	{
		//buff=(char)rand();
		do
		{
			buff=rand()%0xff;
		}while((buff<0x41)||(buff>0x7F));

		cookie_data.value[i_cookie]=buff;
	}
	cookie_data.value[4]=0x0;
	IP210_Update_Timer_Counter();
	cookie_data.age=timercounter;
	return &(cookie_data.value[0]);
}
#endif

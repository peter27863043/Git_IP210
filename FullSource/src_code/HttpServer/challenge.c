/******************************************************************************
*
*   Name:           challenge.c
*
*   Description:    Handle challenhe check and generate
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
//*******************Global value**************************
#ifdef MODULE_CLI_CUSTOMIAZE
#else
COOKIE_Type challenge_data;
#include "module.h"
#ifdef DIS_CHALLENGE
#else
/******************************************************************************
*
*  Function:    httpd_parse_challenge
*
*  Description: Check the challenge time and cookie value
*               
*  Parameters:  Address of the challenge will be checked
*               
*  Returns:     If challenge is valid, return True. Otherwise, return False
*               
*******************************************************************************/
bit_t httpd_parse_challenge(void *check_value)
{
	u32_t ch_currentage,ch_sum_age;
	IP210_Update_Timer_Counter();	
	ch_currentage=timercounter;
	ch_sum_age=ch_currentage-challenge_data.age;
//	printf("\n\r currentage=%lu",ch_currentage);
//	printf("\n\r challenge_data age=%lu",challenge_data.age);
//	printf("\n\r parsecookie=%s",hs->cookie_name);
	if(ch_sum_age<0)
	{
		ch_sum_age=(~ch_sum_age)+1;
	}
	if(ch_sum_age>*((u32_t*)get_system_parameter(SYS_PARAM_EXPIRE_TIME)))
	{
	    memset(challenge_data.value, 0, sizeof(challenge_data.value)) ;  
		//cookie_data.age=currentage;
	}
	else
	{	
//			printf("\n\r parse challenge value%s compare %s",check_value, &(challenge_data.value[0]));
			if(!(strcmp(check_value,&(challenge_data.value[0]))))//cookie value check
			{
				challenge_data.age=ch_currentage;
				return OK;//ERROR
			}				
//			printf("\n\r challenge value error");
	}
	return ERROR;
}
/******************************************************************************
*
*  Function:    httpd_set_challenge
*
*  Description: generate the challenge value
*               
*  Parameters:  None
*               
*  Returns:     Address of the challenge value
*               
*******************************************************************************/
u8_t *httpd_set_challenge()
{
	u8_t i_challenge;
	u8_t buff_challenge;
	IP210_Update_Timer_Counter();
    srand((u16_t)timercounter);
	for (i_challenge = 0; i_challenge < (MAX_COOKIE_LEN-1); i_challenge++) 
	{
		do
		{
			buff_challenge=rand()%0xff;
		}while(!(((buff_challenge>=0x30)&&(buff_challenge<=0x39))||((buff_challenge>=0x61)&&(buff_challenge<=0x7A))||((buff_challenge>=0x41)&&(buff_challenge<=0x5A))));

		challenge_data.value[i_challenge]=buff_challenge;
	}
	challenge_data.value[MAX_COOKIE_LEN-1]=0x0;
	IP210_Update_Timer_Counter();
	challenge_data.age=timercounter;
	return &(challenge_data.value[0]);
}
#endif //DIS_CHALLENGE
#endif

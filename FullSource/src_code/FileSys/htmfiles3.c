/******************************************************************************
*
*   Name:           
*
*   Description:    
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/



/******************************************************************************
Need to modify following string to match bank# :
* (for example bank03)
* files_table03 (x2) in array and in fs_open03()
* cgi_table03   (x2) in array and in fs_open03()
* fs_open03     (x1) in fs_open03()
* datacpy03     (x1) in datacpy03()
* get_tag03     (x1) in get_tag03()
******************************************************************************/







/******************************************************************************
*
*   Name:           htmfiles.c
*
*   Description:    htmfiles in bank0
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "type_def.h"
#include "fs.h"
#include "uip.h"
#include "httpd.h"
#include "module.h"
#include <string.h>
//*****include html files*********
#include "index_htm.c"
#include "left_htm.c"
#include "login_htm.c"
#include "login2_htm.c"

#ifdef MODULE_CLI_CUSTOMIAZE
#else

extern u8_t cgi_login(u8_t cnt, void *vptr);
extern u8_t ssi_login(u8_t varid, data_value *vp);

#include "errormsg_htm.c"
extern u8_t ssi_errormsg(u8_t varid, data_value *vp);

#ifdef MODULE_FIRMWARE
	#include "fw_up_wait_htm.c"
#endif

#ifdef MODULE_64K_HTM	//for CLI 64K
#else
	#ifdef MODULE_NET_UART
	#include "uart_htm.c"
	extern u8_t cgi_uart(u8_t cnt, void *vptr);
	extern u8_t ssi_uart(u8_t varid, data_value *vp);

	#include "telnet_htm.c"
	extern u8_t cgi_telnet(u8_t cnt, void *vptr);
	extern u8_t ssi_telnet(u8_t varid, data_value *vp);
	#endif
#endif//MODULE_64K_HTM

#ifdef MODULE_FLASH512
	#include "setDef_htm.c"
	extern u8_t cgi_setDef(u8_t cnt, req_data *vp);
	
	#include "setIP_htm.c"
	extern u8_t ssi_setip(u8_t varid, data_value *vp);
	extern u8_t cgi_setip(u8_t cnt, req_data *vp);
	
	#include "setPASS_htm.c"
	extern u8_t ssi_setpass(u8_t varid, data_value *vp);
	extern u8_t cgi_setpass(u8_t cnt, req_data *vp);
#endif

#include "Status_htm.c"
extern u8_t ssi_status(u8_t varid, data_value *vp);
extern u8_t cgi_status(u8_t cnt, req_data *vp);

#ifdef MODULE_SMTP
#include "smtp_htm.c"
extern u8_t ssi_smtp(u8_t varid, data_value *vp);
extern u8_t cgi_smtp(u8_t cnt, req_data *vp);
#endif

#ifdef MODULE_ADC
#include "adc_htm.c"
extern u8_t ssi_adc(u8_t varid, data_value *vp);
u8_t cgi_adc(u8_t cnt, req_data *vp);
#endif

#ifdef MODULE_UDP_TELNET
#include "udp_tel_htm.c"
extern u8_t ssi_udp_tel(u8_t varid, data_value *vp);
u8_t cgi_udp_tel(u8_t cnt, req_data *vp);
#endif

#ifdef MODULE_DIDO
#include "dido_htm.c"
extern u8_t ssi_dido(u8_t varid, data_value *vp);
extern u8_t cgi_dido(u8_t cnt, void *vptr);
#endif //MODULE_DIDO

u8_t cgi_errormsg(u8_t cnt, req_data *vp);
u8_t cgi_reset(u8_t cnt, req_data *vp);

#endif
//u8_t cgi_reboot(u8_t cnt, req_data *vp);
//*************File System*********
#ifdef MODULE_64K_HTM
	#define NUM_NET_UART 0
#else
	#ifdef MODULE_NET_UART
	#define NUM_NET_UART 2
	#else
	#define NUM_NET_UART 0
	#endif
#endif

#ifdef MODULE_SMTP
#define NUM_SMTP 1
#else
#define NUM_SMTP 0
#endif

#ifdef MODULE_ADC
#define NUM_ADC 1
#else
#define NUM_ADC 0
#endif

#ifdef MODULE_UDP_TELNET
#define NUM_UDP_TELNET 1
#else
#define NUM_UDP_TELNET 0
#endif

#ifdef MODULE_DIDO
#define NUM_DIDO 1
#else
#define NUM_DIDO 0
#endif

#ifdef MODULE_FLASH512
	#define START1B 5
#else
	#define START1B 3
#endif
#define CGI_START03 (START1B+NUM_NET_UART+NUM_ADC+NUM_SMTP+NUM_UDP_TELNET+NUM_DIDO)

#ifdef MODULE_FLASH512
	#define START2B (CGI_START03+6)
#else
	#define START2B (CGI_START03+3)
#endif

#ifdef MODULE_CLI_CUSTOMIAZE
#else

SFILENAME code files_table03[] = {
//1a.Common file:
{"login2.htm",          login2_htm_file,  sizeof(login2_htm_file),   0},
{"errormsg.htm",        errormsg_htm_file,sizeof(errormsg_htm_file),1},
{"Status.htm",          Status_htm_file,  sizeof(Status_htm_file),  2},
#ifdef MODULE_FLASH512
{"setIP.htm",           setIP_htm_file,   sizeof(setIP_htm_file),   3},
{"setPASS.htm",         setPASS_htm_file, sizeof(setPASS_htm_file), 4},
#endif
{"index.htm",           index_htm_file,   sizeof(index_htm_file),   NO_EX_FUNC},
{"left.htm",            left_htm_file,    sizeof(left_htm_file),    NO_EX_FUNC},
#ifdef MODULE_FLASH512
{"setDef.htm",          setDef_htm_file,  sizeof(setDef_htm_file),  NO_EX_FUNC},
#endif
{"login.htm",           login_htm_file,   sizeof(login_htm_file),   NO_EX_FUNC},//chance20080620
#ifdef MODULE_FIRMWARE
{"fw_up_wait.htm",      fw_up_wait_htm_file,   sizeof(fw_up_wait_htm_file),   NO_EX_FUNC},
#endif
//1b.Option file:
//The order must be: (1)SMTP, (2)ADC, (3)UDP_TELNET
#ifdef MODULE_64K_HTM
#else
	#ifdef MODULE_NET_UART
	{"uart.htm",            uart_htm_file,    sizeof(uart_htm_file),    START1B},
	{"telnet.htm",          telnet_htm_file,  sizeof(telnet_htm_file),  START1B+1},
	#endif
#endif

#ifdef MODULE_SMTP
{"smtp.htm",            smtp_htm_file,    sizeof(smtp_htm_file),    START1B+NUM_NET_UART},
#endif

#ifdef MODULE_ADC
{"adc.htm",             adc_htm_file,     sizeof(adc_htm_file),     START1B+NUM_NET_UART+NUM_SMTP},
#endif

#ifdef MODULE_UDP_TELNET
{"udp_tel.htm",         udp_tel_htm_file,     sizeof(udp_tel_htm_file),     START1B+NUM_NET_UART+NUM_SMTP+NUM_ADC},
#endif

#ifdef MODULE_DIDO
{"dido.htm",            dido_htm_file,    sizeof(dido_htm_file),   START1B+NUM_NET_UART+NUM_SMTP+NUM_ADC+NUM_UDP_TELNET},
#endif //MODULE_DIDO

//2a.Common CGI:
{"cgi/login.cgi",       NULL,             NULL,                    CGI_START03},
{"cgi/status.cgi",      NULL,             NULL,                    CGI_START03+1},
{"cgi/reset.cgi",       NULL,             NULL,                    CGI_START03+2},
#ifdef MODULE_FLASH512
{"cgi/setDef.cgi",      NULL,             NULL,                    CGI_START03+3},
{"cgi/setting_ip.cgi",  NULL,             NULL,                    CGI_START03+4},
{"cgi/setting_pass.cgi",NULL,             NULL,                    CGI_START03+5},
#endif
//{"cgi/reboot.cgi",		NULL,			  NULL,					   CGI_START03+6},

//2b.Option CGI:
//The order must be: (1)SMTP, (2)ADC, (3)UDP_TELNET
#ifdef MODULE_64K_HTM
#else
	#ifdef MODULE_NET_UART
	{"cgi/uart.cgi",        NULL,             NULL,                    START2B},
	{"cgi/telnet.cgi",      NULL,             NULL,                    START2B+1},
	#endif
#endif

#ifdef MODULE_SMTP
{"cgi/smtp.cgi",        NULL,             NULL,                    START2B+NUM_NET_UART},
#endif
#ifdef MODULE_ADC
{"cgi/adc.cgi",         NULL,             NULL,                    START2B+NUM_NET_UART+NUM_SMTP},
#endif
#ifdef MODULE_UDP_TELNET
{"cgi/udp_tel.cgi",     NULL,             NULL,                    START2B+NUM_NET_UART+NUM_SMTP+NUM_ADC},
#endif
#ifdef MODULE_DIDO
{"cgi/dido.cgi",        NULL,             NULL,                    START2B+NUM_NET_UART+NUM_SMTP+NUM_ADC+NUM_UDP_TELNET},
#endif
{ NULL,                 NULL,             NULL,                    NO_EX_FUNC},
};
//*************CGI service function table*********
//3a.Common SSI VEC:
CGI_FUNC_T code cgi_table03[] = {
{ssi_login,   login2_htm_tags},   //00
{ssi_errormsg,errormsg_htm_tags},//01
{ssi_status,  Status_htm_tags},  //04
#ifdef MODULE_FLASH512
{ssi_setip,   setIP_htm_tags},   //02
{ssi_setpass, setPASS_htm_tags}, //03
#endif
//3b.Option SSI VEC:
//The order must be: (1)SMTP, (2)ADC, (3)UDP_TELNET
#ifdef MODULE_64K_HTM
#else
	#ifdef MODULE_NET_UART
	{ssi_uart,    uart_htm_tags},    //START1B
	{ssi_telnet,  telnet_htm_tags},  //START1B+1
	#endif
#endif
#ifdef MODULE_SMTP
{ssi_smtp,    smtp_htm_tags},    //START1B+NUM_NET_UART
#endif
#ifdef MODULE_ADC
{ssi_adc,     adc_htm_tags},     //START1B+NUM_NET_UART+NUM_SMTP
#endif
#ifdef MODULE_UDP_TELNET
{ssi_udp_tel,     udp_tel_htm_tags},     //START1B+NUM_NET_UART+NUM_SMTP+NUM_ADC
#endif
#ifdef MODULE_DIDO
{ssi_dido,    dido_htm_tags},             //START1B+NUM_NET_UART+NUM_SMTP+NUM_ADC+NUM_UDP_TELNET
#endif //MODULE_DIDO
//4a.Common CGI VEC:
{cgi_login,   NULL},             //CGI_START03
{cgi_status,  NULL},             //CGI_START03+4
{cgi_reset,   NULL},             //CGI_START03+5
#ifdef MODULE_FLASH512
{cgi_setDef,  NULL},             //CGI_START03+1
{cgi_setip,   NULL},             //CGI_START03+2
{cgi_setpass, NULL},             //CGI_START03+3
#endif
//{cgi_reboot,  NULL},			 //CGI_START03+6
//4b.Option CGI VEC:
//The order must be: (1)SMTP, (2)ADC, (3)UDP_TELNET
#ifdef MODULE_64K_HTM
#else
	#ifdef MODULE_NET_UART
	{cgi_uart,    NULL},             //START2B
	{cgi_telnet,  NULL},             //START2B+1
	#endif
#endif

#ifdef MODULE_SMTP
{cgi_smtp,    NULL},             //START2B+NUM_NET_UART
#endif
#ifdef MODULE_ADC
{cgi_adc,   NULL},               //START2B+NUM_NET_UART+NUM_SMTP
#endif
#ifdef MODULE_UDP_TELNET
{cgi_udp_tel,   NULL},               //START2B+NUM_NET_UART+NUM_SMTP+NUM_ADC
#endif
#ifdef MODULE_DIDO
{cgi_dido,    NULL},             //START2B+NUM_NET_UART+NUM_SMTP+NUM_ADC+NUM_UDP_TELNET
#endif //MODULE_DIDO
{NULL,        NULL},
};

#endif
/******************************************************************************
*
*  Function:    fs_open03
*
*  Description:  find request filename or cgi in bank03
*               
*  Parameters:  address of filename
*               
*  Returns:     address of filesystem
*               
*******************************************************************************/
#ifdef MODULE_CLI_CUSTOMIAZE
#else

SFILENAME *  fs_open03(char *name)
{
    SFILENAME  *ptr;
    HS->cgi_func_id=NO_EX_FUNC;
    if (name[0] == '/')
    {
        name++;
        if (*name == '\0')
            return NULL;
    }
    for (ptr = files_table03; ptr->file_name != NULL; ptr++)
    {
        if (strcmp(name, ptr->file_name) == 0)
        {    	
			HS->file=*ptr;
        	if(ptr->func_id!=NO_EX_FUNC)
        	{
        		HS->cgi_func=cgi_table03[ptr->func_id];
        		HS->cgi_func_id=ptr->func_id;
        	}
           	return &HS->file;
	  	}
    }
    return NULL;
}

/******************************************************************************
*
*  Function:    datacpy03
*
*  Description: data copy function for Bank03
*               
*  Parameters:  data copy structure
*               
*  Returns:     None
*               
*******************************************************************************/
void datacpy03(mem_cpy_fs *inputdata)
{
    memcpy(inputdata->dest_ptr, inputdata->source_ptr, inputdata->len ) ;
}

/******************************************************************************
*
*  Function:    get_tag03
*
*  Description: data copy function for Bank03
*               
*  Parameters:  index
*               
*  Returns:     None
*               
*******************************************************************************/
struct html_tags get_tag03(u8_t index)
{
		return HS->cgi_func.tags[index];
}
#endif

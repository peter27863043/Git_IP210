#include "type_def.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "type_def.h"
#include "..\sys\eeprom.h"
#include "..\main\eepdef.h"
#include "fs.h"
#include "httpd.h"
#include "uip.h"
#include "net_uart.h"
#ifdef MODULE_FLASH512
#ifdef MODULE_CLI_CUSTOMIAZE
#else

u16_t asc_to_hex(char asc[]);
extern u8_t errmsgflag ;
void eepromerr();
extern struct httpd_info *hs;

/******************************************************************************
*
*  Function:    cgi_setDef
*
*  Description: Handle restore EEPROM default value.
*
*  Parameters:  cnt: number of the cgi data, vp: CGI name and value.
*
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*
*******************************************************************************/
extern u8_t web_def;
u8_t cgi_setDef(u8_t cnt, req_data *vp)
{
	if(cnt||vp){}

	if (EE_Result == 1) // EEPROM is OK
	{
		web_def=1;
		EEPROMLoadDefault();
	    hs->errormsg = "RESET";
	    errmsgflag = ERRORMSG_SETOK;
	    httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
	}
	else
	{
		eepromerr();
	}
	return OK;
}
#endif
#endif

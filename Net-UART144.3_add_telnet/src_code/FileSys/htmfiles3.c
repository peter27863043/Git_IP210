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

 
extern u8_t cgi_login(u8_t cnt, void *vptr);
extern u8_t ssi_login(u8_t varid, data_value *vp);

#include "errormsg_htm.c"
extern u8_t ssi_errormsg(u8_t varid, data_value *vp);

#include "fw_up_wait_htm.c"
 
 
#include "setDef_htm.c"
extern u8_t cgi_setDef(u8_t cnt, req_data *vp);
	
#include "setIP_htm.c"
extern u8_t ssi_setip(u8_t varid, data_value *vp);
extern u8_t cgi_setip(u8_t cnt, req_data *vp);
	
#include "setPASS_htm.c"
extern u8_t ssi_setpass(u8_t varid, data_value *vp);
extern u8_t cgi_setpass(u8_t cnt, req_data *vp);
 
#include "Status_htm.c"
extern u8_t ssi_status(u8_t varid, data_value *vp);
extern u8_t cgi_status(u8_t cnt, req_data *vp);

    
u8_t cgi_errormsg(u8_t cnt, req_data *vp);
u8_t cgi_reset(u8_t cnt, req_data *vp);


#include "NoService_htm.c"

extern u8_t cgi_noservice(u8_t cnt, void *vptr);	  
extern u8_t ssi_noservice(u8_t varid, data_value *vp);

#include "NoService2_htm.c"

extern u8_t cgi_noservice2(u8_t cnt, void *vptr);	  
extern u8_t ssi_noservice2(u8_t varid, data_value *vp);

#include "NoService3_htm.c"
extern u8_t cgi_noservice3(u8_t cnt, void *vptr);	  
extern u8_t ssi_noservice3(u8_t varid, data_value *vp);


#include "NoService4_htm.c"
extern u8_t cgi_noservice4(u8_t cnt, void *vptr);	  
extern u8_t ssi_noservice4(u8_t varid, data_value *vp);

#include "NoService5_htm.c"
extern u8_t cgi_noservice5(u8_t cnt, void *vptr);	  
extern u8_t ssi_noservice5(u8_t varid, data_value *vp);

#include "NoService6_htm.c"
extern u8_t cgi_noservice6(u8_t cnt, void *vptr);	  
extern u8_t ssi_noservice6(u8_t varid, data_value *vp);






 
 
SFILENAME code files_table03[] = {
//1a.Common file:
{"login2.htm",          login2_htm_file,  		sizeof(login2_htm_file),   	0},
{"errormsg.htm",        errormsg_htm_file,		sizeof(errormsg_htm_file),	1},
{"Status.htm",          Status_htm_file,  		sizeof(Status_htm_file),  	2},
{"setIP.htm",           setIP_htm_file,   		sizeof(setIP_htm_file),   	3},
{"setPASS.htm",         setPASS_htm_file, 		sizeof(setPASS_htm_file), 	4},
{"index.htm",           index_htm_file,   		sizeof(index_htm_file),   	NO_EX_FUNC},
{"left.htm",            left_htm_file,    		sizeof(left_htm_file),    	NO_EX_FUNC},
{"setDef.htm",          setDef_htm_file,  		sizeof(setDef_htm_file),  	NO_EX_FUNC},
{"login.htm",           login_htm_file,   		sizeof(login_htm_file),   	NO_EX_FUNC},//chance20080620
{"fw_up_wait.htm",      fw_up_wait_htm_file,   	sizeof(fw_up_wait_htm_file),NO_EX_FUNC},

//2a.Common CGI:
{"cgi/login.cgi",       NULL,             		NULL,                    	5},
{"cgi/status.cgi",      NULL,             		NULL,                    	6},
{"cgi/reset.cgi",       NULL,             		NULL,                    	7},
{"cgi/setDef.cgi",      NULL,             		NULL,                    	8},
{"cgi/setting_ip.cgi",  NULL,             		NULL,                    	9},
{"cgi/setting_pass.cgi",NULL,             		NULL,                    	10},
{"NoService.htm",       NoService_htm_file,  	sizeof(NoService_htm_file), 11},
{"NoService2.htm",		NoService2_htm_file, 	sizeof(NoService2_htm_file),12},
{"NoService3.htm",		NoService3_htm_file,	sizeof(NoService3_htm_file),13},
{"NoService4.htm",		NoService4_htm_file, 	sizeof(NoService4_htm_file),14},
{"NoService5.htm",		NoService5_htm_file, 	sizeof(NoService5_htm_file),15},
{"NoService6.htm",		NoService6_htm_file, 	sizeof(NoService6_htm_file),16},
{ NULL,                 NULL,             		NULL,                    	NO_EX_FUNC},
};
//*************CGI service function table*********
//3a.Common SSI VEC:
CGI_FUNC_T code cgi_table03[] = {
{ssi_login,   	login2_htm_tags},   //00
{ssi_errormsg,	errormsg_htm_tags},	//01
{ssi_status,  	Status_htm_tags},  	//02
{ssi_setip,   	setIP_htm_tags},   	//03
{ssi_setpass, 	setPASS_htm_tags}, 	//04
{cgi_login,   	NULL},             	//05
{cgi_status,  	NULL},             	//06
{cgi_reset,   	NULL},             	//07
{cgi_setDef,  	NULL},             	//08
{cgi_setip,   	NULL},             	//09
{cgi_setpass, 	NULL},             	//10
{ssi_noservice,	NoService_htm_tags},//11
{ssi_noservice2,NoService2_htm_tags},//12
{ssi_noservice3,NoService3_htm_tags},//13
{ssi_noservice4,NoService4_htm_tags},//14
{ssi_noservice5,NoService5_htm_tags},//15
{ssi_noservice6,NoService6_htm_tags},//16

	



{NULL,        	NULL},
};

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

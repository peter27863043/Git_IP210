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
* (for example bank04)
* files_table04 (x2) in array and in fs_open04()
* cgi_table04   (x2) in array and in fs_open04()
* fs_open04     (x1) in fs_open04()
* datacpy04     (x1) in datacpy04()
* get_tag04     (x1) in get_tag04()
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
#include <stdlib.h>
#include "type_def.h"
#include "fs.h"
#include "uip.h"
#include "httpd.h"
#include <string.h>
//*****include html files*********
#include "fw_up_htm.c"
extern u8_t cgi_fw_up(u8_t cnt, req_data *vp);
#include "fu_up_htm.c"
extern u8_t cgi_fu_up(u8_t cnt, req_data *vp);
//*************File System*********
#define CGI_START04 0
SFILENAME code files_table04[] = {
{"fw_up.htm",           fw_up_htm_file,   sizeof(fw_up_htm_file),  NO_EX_FUNC},
{"cgi/fw_up.cgi",       NULL,             NULL,                    CGI_START04},
{"fu_up.htm",           fu_up_htm_file,   sizeof(fu_up_htm_file),  NO_EX_FUNC},
{"cgi/fu_up.cgi",       NULL,             NULL,                    CGI_START04+1},
{ NULL,                 NULL,             NULL,                    NO_EX_FUNC},
};
//*************CGI service function table*********
CGI_FUNC_T code cgi_table04[] = {
{cgi_fw_up,   NULL},             //CGI_START04+0
{cgi_fu_up,   NULL},             //CGI_START04+1
{NULL,        NULL},
};

/******************************************************************************
*
*  Function:    fs_open04
*
*  Description:  find request filename or cgi in bank04
*               
*  Parameters:  address of filename
*               
*  Returns:     address of filesystem
*               
*******************************************************************************/
SFILENAME *  fs_open04(char *name)
{
    SFILENAME  *ptr;
    HS->cgi_func_id=NO_EX_FUNC;
    if (name[0] == '/')
    {
        name++;
        if (*name == '\0')
            return NULL;
    }
    for (ptr = files_table04; ptr->file_name != NULL; ptr++)
    {
        if (strcmp(name, ptr->file_name) == 0)
        {
					 HS->file=*ptr;
        	 if(ptr->func_id!=NO_EX_FUNC)
        	 {
        	 		HS->cgi_func=cgi_table04[ptr->func_id];
        	 		HS->cgi_func_id=ptr->func_id;
        	 }
           return &HS->file;
	  	}
    }
    return NULL;
}

/******************************************************************************
*
*  Function:    datacpy04
*
*  Description: data copy function for Bank04
*               
*  Parameters:  data copy structure
*               
*  Returns:     None
*               
*******************************************************************************/
void datacpy04(mem_cpy_fs *inputdata)
{
    memcpy(inputdata->dest_ptr, inputdata->source_ptr, inputdata->len ) ;      
}

/******************************************************************************
*
*  Function:    get_tag04
*
*  Description: data copy function for Bank04
*               
*  Parameters:  index
*               
*  Returns:     None
*               
*******************************************************************************/
struct html_tags get_tag04(u8_t index)
{
		return HS->cgi_func.tags[index];
}
void CPU_GOTO_FUU(void);
void FUU_Run(void)
{
    CPU_GOTO_FUU();
}

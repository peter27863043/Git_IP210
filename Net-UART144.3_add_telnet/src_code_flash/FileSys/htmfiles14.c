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
* (for example bank14)
* files_table07 (x2) in array and in fs_open14()
* cgi_table07   (x2) in array and in fs_open14()
* fs_open07     (x1) in fs_open14()
* datacpy07     (x1) in datacpy14()
* get_tag07     (x1) in get_tag14()
******************************************************************************/







/******************************************************************************
*
*   Name:           htmfiles.c
*
*   Description:    htmfiles in bank14
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

#include "adc_htm.c"
extern u8_t ssi_adc(u8_t varid, data_value *vp);
u8_t cgi_adc(u8_t cnt, req_data *vp);
 
//*************File System*********
SFILENAME code files_table14[] = {
{"adc.htm",           	adc_htm_file,     		sizeof(adc_htm_file),     	0}, 
{"cgi/adc.cgi",         NULL,             		NULL,                    	1},
{ NULL,                 NULL,             		NULL,                    	NO_EX_FUNC},
};
//*************CGI service function table*********
CGI_FUNC_T code cgi_table14[] = {
{ssi_adc,     	adc_htm_tags},     	
{cgi_adc,   	NULL},             
{NULL,        	NULL},
};

/******************************************************************************
*
*  Function:    fs_open14
*
*  Description:  find request filename or cgi in bank14
*               
*  Parameters:  address of filename
*               
*  Returns:     address of filesystem
*               
*******************************************************************************/
SFILENAME *  fs_open14(char *name)
{
    SFILENAME  *ptr;
    HS->cgi_func_id=NO_EX_FUNC;
    if (name[0] == '/')
    {
        name++;
        if (*name == '\0')
            return NULL;
    }
    for (ptr = files_table14; ptr->file_name != NULL; ptr++)
    {
        if (strcmp(name, ptr->file_name) == 0)
        {
					 HS->file=*ptr;
        	 if(ptr->func_id!=NO_EX_FUNC)
        	 {
        	 		HS->cgi_func=cgi_table14[ptr->func_id];
        	 		HS->cgi_func_id=ptr->func_id;
        	 }
           return &HS->file;
	  	}
    }
    return NULL;
}

/******************************************************************************
*
*  Function:    datacpy14
*
*  Description: data copy function for Bank14
*               
*  Parameters:  data copy structure
*               
*  Returns:     None
*               
*******************************************************************************/
void datacpy14(mem_cpy_fs *inputdata)
{
    memcpy(inputdata->dest_ptr, inputdata->source_ptr, inputdata->len ) ;      
}

/******************************************************************************
*
*  Function:    get_tag14
*
*  Description: data copy function for Bank14
*               
*  Parameters:  index
*               
*  Returns:     None
*               
*******************************************************************************/
struct html_tags get_tag14(u8_t index)
{
		return HS->cgi_func.tags[index];
}

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
* (for example bank16)
* files_table07 (x2) in array and in fs_open16()
* cgi_table07   (x2) in array and in fs_open16()
* fs_open07     (x1) in fs_open16()
* datacpy07     (x1) in datacpy16()
* get_tag07     (x1) in get_tag16()
******************************************************************************/







/******************************************************************************
*
*   Name:           htmfiles.c
*
*   Description:    htmfiles in bank16
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

//*************File System*********
SFILENAME code files_table16[] = {
{ NULL,                 NULL,             NULL,                    NO_EX_FUNC},
};
//*************CGI service function table*********
CGI_FUNC_T code cgi_table16[] = {
{NULL,        NULL},
};

/******************************************************************************
*
*  Function:    fs_open16
*
*  Description:  find request filename or cgi in bank16
*               
*  Parameters:  address of filename
*               
*  Returns:     address of filesystem
*               
*******************************************************************************/
SFILENAME *  fs_open16(char *name)
{
    SFILENAME  *ptr;
    HS->cgi_func_id=NO_EX_FUNC;
    if (name[0] == '/')
    {
        name++;
        if (*name == '\0')
            return NULL;
    }
    for (ptr = files_table16; ptr->file_name != NULL; ptr++)
    {
        if (strcmp(name, ptr->file_name) == 0)
        {
					 HS->file=*ptr;
        	 if(ptr->func_id!=NO_EX_FUNC)
        	 {
        	 		HS->cgi_func=cgi_table16[ptr->func_id];
        	 		HS->cgi_func_id=ptr->func_id;
        	 }
           return &HS->file;
	  	}
    }
    return NULL;
}

/******************************************************************************
*
*  Function:    datacpy16
*
*  Description: data copy function for Bank16
*               
*  Parameters:  data copy structure
*               
*  Returns:     None
*               
*******************************************************************************/
void datacpy16(mem_cpy_fs *inputdata)
{
    memcpy(inputdata->dest_ptr, inputdata->source_ptr, inputdata->len ) ;      
}

/******************************************************************************
*
*  Function:    get_tag16
*
*  Description: data copy function for Bank16
*               
*  Parameters:  index
*               
*  Returns:     None
*               
*******************************************************************************/
struct html_tags get_tag16(u8_t index)
{
		return HS->cgi_func.tags[index];
}
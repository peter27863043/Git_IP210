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
* (for example bank06)
* files_table06 (x2) in array and in fs_open06()
* cgi_table06   (x2) in array and in fs_open06()
* fs_open06     (x1) in fs_open06()
* datacpy06     (x1) in datacpy06()
* get_tag06     (x1) in get_tag06()
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

//*************File System*********
SFILENAME code files_table06[] = {
{ NULL,                 NULL,             NULL,                    NO_EX_FUNC},
};
//*************CGI service function table*********
CGI_FUNC_T code cgi_table06[] = {
{NULL,        NULL},
};

/******************************************************************************
*
*  Function:    fs_open06
*
*  Description:  find request filename or cgi in bank06
*               
*  Parameters:  address of filename
*               
*  Returns:     address of filesystem
*               
*******************************************************************************/
SFILENAME *  fs_open06(char *name)
{
    SFILENAME  *ptr;
    HS->cgi_func_id=NO_EX_FUNC;
    if (name[0] == '/')
    {
        name++;
        if (*name == '\0')
            return NULL;
    }
    for (ptr = files_table06; ptr->file_name != NULL; ptr++)
    {
        if (strcmp(name, ptr->file_name) == 0)
        {
					 HS->file=*ptr;
        	 if(ptr->func_id!=NO_EX_FUNC)
        	 {
        	 		HS->cgi_func=cgi_table06[ptr->func_id];
        	 		HS->cgi_func_id=ptr->func_id;
        	 }
           return &HS->file;
	  	}
    }
    return NULL;
}

/******************************************************************************
*
*  Function:    datacpy06
*
*  Description: data copy function for Bank06
*               
*  Parameters:  data copy structure
*               
*  Returns:     None
*               
*******************************************************************************/
void datacpy06(mem_cpy_fs *inputdata)
{
    memcpy(inputdata->dest_ptr, inputdata->source_ptr, inputdata->len ) ;      
}

/******************************************************************************
*
*  Function:    get_tag06
*
*  Description: data copy function for Bank06
*               
*  Parameters:  index
*               
*  Returns:     None
*               
*******************************************************************************/
struct html_tags get_tag06(u8_t index)
{
		return HS->cgi_func.tags[index];
}
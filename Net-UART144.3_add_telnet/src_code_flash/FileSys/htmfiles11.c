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
* (for example bank11)
* files_table07 (x2) in array and in fs_open11()
* cgi_table07   (x2) in array and in fs_open11()
* fs_open07     (x1) in fs_open11()
* datacpy07     (x1) in datacpy11()
* get_tag07     (x1) in get_tag11()
******************************************************************************/







/******************************************************************************
*
*   Name:           htmfiles.c
*
*   Description:    htmfiles in bank11
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


#include "udp_tel_htm.c"
extern u8_t ssi_udp_tel(u8_t varid, data_value *vp);
u8_t cgi_udp_tel(u8_t cnt, req_data *vp);

//*************File System*********
SFILENAME code files_table11[] = {
	{"udp_tel.htm", 		udp_tel_htm_file,	  	sizeof(udp_tel_htm_file), 	0},
	{"cgi/udp_tel.cgi",     NULL,             		NULL,                    	1},	
	{ NULL,                 NULL,             		NULL,                    	NO_EX_FUNC},
};
//*************CGI service function table*********
CGI_FUNC_T code cgi_table11[] = {
	{ssi_udp_tel,			udp_tel_htm_tags},
	{cgi_udp_tel,			NULL},	
	{NULL,        			NULL},
};

struct html_tags  udp_tel_htm_tags_Buffer =  { 0, 0 }; 


/******************************************************************************
*
*  Function:    fs_open11
*
*  Description:  find request filename or cgi in bank11
*               
*  Parameters:  address of filename
*               
*  Returns:     address of filesystem
*               
*******************************************************************************/
SFILENAME *  fs_open11(char *name)
{
    SFILENAME  *ptr;
    HS->cgi_func_id=NO_EX_FUNC;
    if (name[0] == '/')
    {
        name++;
        if (*name == '\0')
            return NULL;
    }
    for (ptr = files_table11; ptr->file_name != NULL; ptr++)
    {
        if (strcmp(name, ptr->file_name) == 0)
        {
					 HS->file=*ptr;
        	 if(ptr->func_id!=NO_EX_FUNC)
        	 {
        	 		HS->cgi_func=cgi_table11[ptr->func_id];
        	 		HS->cgi_func_id=ptr->func_id;
        	 }
           return &HS->file;
	  	}
    }
    return NULL;
}

/******************************************************************************
*
*  Function:    datacpy11
*
*  Description: data copy function for Bank11
*               
*  Parameters:  data copy structure
*               
*  Returns:     None
*               
*******************************************************************************/
void datacpy11(mem_cpy_fs *inputdata)
{
    memcpy(inputdata->dest_ptr, inputdata->source_ptr, inputdata->len ) ;      
}

/******************************************************************************
*
*  Function:    get_tag11
*
*  Description: data copy function for Bank11
*               
*  Parameters:  index
*               
*  Returns:     None
*               
*******************************************************************************/
struct html_tags get_tag11(u8_t index)
{
		udp_tel_htm_tags_Buffer.pos 	=udp_tel_htm_tags[index].pos;
		udp_tel_htm_tags_Buffer.varid=udp_tel_htm_tags[index].varid;
		return udp_tel_htm_tags_Buffer;
}

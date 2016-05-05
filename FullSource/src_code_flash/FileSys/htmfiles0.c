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
* (for example bank00)
* files_table00 (x2) in array and in fs_open00()
* cgi_table00   (x2) in array and in fs_open00()
* fs_open00     (x1) in fs_open00()
* datacpy00     (x1) in datacpy00()
* get_tag00     (x1) in get_tag00()
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
#include "public.h"
//*****include html files*********

//*************File System*********
SFILENAME code files_table00[] = {
{ NULL,                 NULL,             NULL,                    NO_EX_FUNC},
};
//*************CGI service function table*********
CGI_FUNC_T code cgi_table00[] = {
{NULL,        NULL},
};

/******************************************************************************
*
*  Function:    fs_open00
*
*  Description:  find request filename or cgi in bank00
*               
*  Parameters:  address of filename
*               
*  Returns:     address of filesystem
*               
*******************************************************************************/
SFILENAME *  fs_open00(char *name)
{
    SFILENAME  *ptr;
    HS->cgi_func_id=NO_EX_FUNC;
    if (name[0] == '/')
    {
        name++;
        if (*name == '\0')
            return NULL;
    }
    for (ptr = files_table00; ptr->file_name != NULL; ptr++)
    {
        if (strcmp(name, ptr->file_name) == 0)
        {
					 HS->file=*ptr;
        	 if(ptr->func_id!=NO_EX_FUNC)
        	 {
        	 		HS->cgi_func=cgi_table00[ptr->func_id];
        	 		HS->cgi_func_id=ptr->func_id;
        	 }
           return &HS->file;
	  	}
    }
    return NULL;
}

/******************************************************************************
*
*  Function:    datacpy00
*
*  Description: data copy function for Bank00
*               
*  Parameters:  data copy structure
*               
*  Returns:     None
*               
*******************************************************************************/
void datacpy00(mem_cpy_fs *inputdata)
{
    memcpy(inputdata->dest_ptr, inputdata->source_ptr, inputdata->len ) ;      
}

/******************************************************************************
*
*  Function:    get_tag00
*
*  Description: data copy function for Bank00
*               
*  Parameters:  index
*               
*  Returns:     None
*               
*******************************************************************************/
struct html_tags get_tag00(u8_t index)
{
		return HS->cgi_func.tags[index];
}
void FUU_Run(void);
void FU_Check(void)
{
  u16_t i;
  u8_t bRun_FUU = 0;
  u8_t checksum = 0;
  u8_t magic_code[4] = {0x13, 0xf0, 0xf0, 0x13};


  do{
/*
     printf("\n");
     for(i=0;i<0x10;i++)
         printf("%02x ", (u16_t)CBYTE[i+0xfff0]);
*/
  	 //check magic code
     for(i=0;i<4;i++)
     {
         if(CBYTE[i+0xfff9] != magic_code[i])
	         break;
     }
     if(i != 4)
     {
	     printf("\n Check Magic Code Error!!!");
         bRun_FUU = 1;
	     break;
     }
	 //inspect checksum
     for(i=0;i<0xA000-8;i++)
	 {
	     checksum += CBYTE[0x6000+i];
	 }
	     printf("\n Checksum=[%02x] ", (u16_t)checksum);
	 if(checksum != CBYTE[0xfff8])
	 {
	     printf("\n Checksum Error!!!, CBYTE[0xfff8]=[%02x] ", (u16_t)CBYTE[0xfff8]);
         bRun_FUU = 1;
	 }
  }while(0);

  	if(bRun_FUU)
  	{
	    printf("\nRun_FUU ");
	    FUU_Run();
	}
}

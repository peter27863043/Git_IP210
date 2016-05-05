



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
#include <string.h>
//*****include html files*********
//#include "Warn_htm.c"
//extern u8_t cgi_callerase(u8_t cnt, void *vptr);

#include "errormsg_htm.c"
extern u8_t ssi_errormsg(u8_t varid, data_value *vp);
#include "firmwareUpdate_htm.c"
extern u8_t ssi_firmwareUpdate(u8_t varid, data_value *vp);
extern u8_t cgi_firmwareUpdate(u8_t cnt, void *vptr);

#include "EraseFlash_htm.c"
extern u8_t ssi_eraseflash(u8_t varid, data_value *vp);
#include "UpdateComplete_htm.c"
//extern u8_t ssi_updatecomplete(u8_t varid, data_value *vp);
#include "TFTPInProgress_htm.c"
extern u8_t ssi_tftpinprogress(u8_t varid, data_value *vp);
#include "TF_htm.c"
//*************File System*********
//#define CGI_START03 1
SFILENAME code files_table03[] = {
{"errormsg.htm"          , errormsg_htm_file      , sizeof(errormsg_htm_file)      , 0          },
{"firmwareUpdate.htm"    , firmwareUpdate_htm_file, sizeof(firmwareUpdate_htm_file), 1          },
{"cgi/firmwareUpdate.cgi", NULL                   , NULL                           , 2          },
{"EraseFlash.htm"        , EraseFlash_htm_file    , sizeof(EraseFlash_htm_file)    , 3          },
{"TFTPInProgress.htm"    , TFTPInProgress_htm_file, sizeof(TFTPInProgress_htm_file), 4          },
{"TF.htm"                , TF_htm_file            , sizeof(TF_htm_file)            , 5 			},
{"UpdateComplete.htm"    , UpdateComplete_htm_file, sizeof(UpdateComplete_htm_file), NO_EX_FUNC },
};
//*************CGI service function table*********
CGI_FUNC_T code cgi_table03[] = {
{ssi_errormsg		,errormsg_htm_tags},//00
{ssi_firmwareUpdate	,firmwareUpdate_htm_tags},//01
{cgi_firmwareUpdate	,NULL},    //02
{ssi_eraseflash		,EraseFlash_htm_tags},    //03
{ssi_tftpinprogress	,TFTPInProgress_htm_tags},    //04
{ssi_firmwareUpdate, TF_htm_tags                   },
//{cgi_callerase		,NULL					},    //04
//{ssi_updatecomplete,   UpdateComplete_htm_tags},    //05
{NULL,        NULL},
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

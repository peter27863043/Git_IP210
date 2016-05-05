



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


//#include "errormsg_htm.c"
//extern u8_t ssi_errormsg(u8_t varid, data_value *vp);
#include "fu_htm.c"
extern u8_t ssi_firmwareUpdate(u8_t varid, data_value *vp);
extern u8_t cgi_firmwareUpdate(u8_t cnt, void *vptr);

#include "Warn_htm.c"
extern u8_t cgi_callerase(u8_t varid, data_value *vp);
extern u8_t ssi_warn(u8_t varid, data_value *vp);

#include "Erase_htm.c"
extern u8_t ssi_eraseflash(u8_t varid, data_value *vp);
#include "OK_htm.c"
extern u8_t ssi_updatecomplete(u8_t varid, data_value *vp);
//#include "TFTPGoing_htm.c"
//extern u8_t ssi_tftpinprogress(u8_t varid, data_value *vp);
#include "sb_gif.c"
//chance add 20100322 for top frame
#include "main_htm.c"
extern u8_t ssi_main(u8_t varid, data_value *vp);
#include "TF_htm.c"
//#include "test1_JPG.c"
//*************File System*********
//#define CGI_START03 1
SFILENAME code files_table03[] = {
//{"errormsg.htm"          , errormsg_htm_file      , sizeof(errormsg_htm_file)      , 0          },
{"fu.htm"    			, fu_htm_file		, sizeof(fu_htm_file)		, 0  },
{"cgi/FUpdate.cgi"  	, NULL              , NULL                      , 1  },
{"Erase.htm"        	, Erase_htm_file    , sizeof(Erase_htm_file)    , 2  },
{"OK.htm"    			, OK_htm_file		, sizeof(OK_htm_file)		, 3  },
//{"TFTPInProgress.htm"   , TFTPGoing_htm_file, sizeof(TFTPGoing_htm_file), 4  },
{"cgi/callerase.cgi"	, NULL				, NULL						, 4	 },
{"Warn.htm"				, Warn_htm_file		, sizeof(Warn_htm_file)     , 5  },
{"main.htm"				, main_htm_file		, sizeof(main_htm_file)     , 6  },//chance add 20100322 for top frame
{"TF.htm"               , TF_htm_file       , sizeof(TF_htm_file)       , 7  },
{"sb.gif"               , sb_gif_file       , sizeof(sb_gif_file)       , 8  },
//{"test1.JPG"               , test1_JPG_file       , sizeof(test1_JPG_file)       , 8  },
}; 
//*************CGI service function table*********
CGI_FUNC_T code cgi_table03[] = {
//{ssi_errormsg      ,   errormsg_htm_tags      },//00
{ssi_firmwareUpdate,   fu_htm_tags},//01
{cgi_firmwareUpdate,   NULL                   },    //02
{ssi_eraseflash    ,   Erase_htm_tags    },    //03
{ssi_updatecomplete,   OK_htm_tags},    //04
//{ssi_tftpinprogress,   TFTPGoing_htm_tags},    //05
{cgi_callerase	   ,   NULL					},			//05
{ssi_warn		   ,   Warn_htm_tags		},    		//06
{ssi_main		   ,   main_htm_tags		},    		//07//chance add 20100322 for top frame
{ssi_firmwareUpdate,   TF_htm_tags                   },
{NULL              ,   NULL                   },
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
//20080304**********************************
#define FSOP_INDEX_NUM 1
_FS fsop[FSOP_INDEX_NUM]=
{
		fs_open03,datacpy03,get_tag03
};
mem_cpy_fs indata;
//**********************************************
/******************************************************************************
*
*  Function:    fs_open
*
*  Description:  find request filename or cgi in bank0
*               
*  Parameters:  address of filename
*               
*  Returns:     address of filesystem
*               
*******************************************************************************/
/*
SFILENAME*  fs_open(char *name, u8_t* fsop_idx)
{
    SFILENAME  *ptr;	
	*fsop_idx=0;
	for(HS->cur_fsop_idx=0;HS->cur_fsop_idx<FSOP_INDEX_NUM;HS->cur_fsop_idx++)
	{
			ptr=fsop[HS->cur_fsop_idx].open(name);
			if(ptr!=NULL)
			{
				 *fsop_idx=HS->cur_fsop_idx;
				 return ptr;
			}
	}		
    return NULL;
}
*/

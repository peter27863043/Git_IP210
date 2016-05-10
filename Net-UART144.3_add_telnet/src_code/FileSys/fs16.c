/******************************************************************************
*
*   Name:           fs.c
*
*   Description:    file system for bank0
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "uip.h"
#include "fs.h"
#include "type_def.h"

#ifdef MODULE_CLI_CUSTOMIAZE
#else

#ifdef FLASH_1M
SFILENAME *   fs_open00(char *name);
SFILENAME *   fs_open01(char *name);
SFILENAME *   fs_open02(char *name);
SFILENAME *   fs_open03(char *name);
SFILENAME *   fs_open04(char *name);
SFILENAME *   fs_open05(char *name);
SFILENAME *   fs_open06(char *name);
SFILENAME *   fs_open07(char *name);
SFILENAME *   fs_open08(char *name);
SFILENAME *   fs_open09(char *name);
SFILENAME *   fs_open10(char *name);
SFILENAME *   fs_open11(char *name);
SFILENAME *   fs_open12(char *name);
SFILENAME *   fs_open13(char *name);
SFILENAME *   fs_open14(char *name);
SFILENAME *   fs_open15(char *name);

void datacpy00(mem_cpy_fs *inputdata);
void datacpy01(mem_cpy_fs *inputdata);
void datacpy02(mem_cpy_fs *inputdata);
void datacpy03(mem_cpy_fs *inputdata);
void datacpy04(mem_cpy_fs *inputdata);
void datacpy05(mem_cpy_fs *inputdata);
void datacpy06(mem_cpy_fs *inputdata);
void datacpy07(mem_cpy_fs *inputdata);
void datacpy08(mem_cpy_fs *inputdata);
void datacpy09(mem_cpy_fs *inputdata);
void datacpy10(mem_cpy_fs *inputdata);
void datacpy11(mem_cpy_fs *inputdata);
void datacpy12(mem_cpy_fs *inputdata);
void datacpy13(mem_cpy_fs *inputdata);
void datacpy14(mem_cpy_fs *inputdata);
void datacpy15(mem_cpy_fs *inputdata);

struct html_tags get_tag00(u8_t index);
struct html_tags get_tag01(u8_t index);
struct html_tags get_tag02(u8_t index);
struct html_tags get_tag03(u8_t index);
struct html_tags get_tag04(u8_t index);
struct html_tags get_tag05(u8_t index);
struct html_tags get_tag06(u8_t index);
struct html_tags get_tag07(u8_t index);
struct html_tags get_tag08(u8_t index);
struct html_tags get_tag09(u8_t index);
struct html_tags get_tag10(u8_t index);
struct html_tags get_tag11(u8_t index);
struct html_tags get_tag12(u8_t index);
struct html_tags get_tag13(u8_t index);
struct html_tags get_tag14(u8_t index);
struct html_tags get_tag15(u8_t index);

#define FSOP_INDEX_NUM 16
_FS fsop[FSOP_INDEX_NUM]=
{
		fs_open00,datacpy00,get_tag00,
		fs_open01,datacpy01,get_tag01,
		fs_open02,datacpy02,get_tag02,
		fs_open03,datacpy03,get_tag03,
		fs_open04,datacpy04,get_tag04,
		fs_open05,datacpy05,get_tag05,
		fs_open06,datacpy06,get_tag06,
		fs_open07,datacpy07,get_tag07,
		fs_open08,datacpy08,get_tag08,
		fs_open09,datacpy09,get_tag09,
		fs_open10,datacpy10,get_tag10,
		fs_open11,datacpy11,get_tag11,
		fs_open12,datacpy12,get_tag12,
		fs_open13,datacpy13,get_tag13,
		fs_open14,datacpy14,get_tag14,
		fs_open15,datacpy15,get_tag15
};
#else
SFILENAME *   fs_open03(char *name);
void datacpy03(mem_cpy_fs *inputdata);
struct html_tags get_tag03(u8_t index);
#define FSOP_INDEX_NUM 1
_FS fsop[FSOP_INDEX_NUM]=
{
		fs_open03,datacpy03,get_tag03
};
#endif

#endif
mem_cpy_fs indata;
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

#ifdef MODULE_CLI_CUSTOMIAZE
#else
SFILENAME*  fs_open(char *name, u8_t* fsop_idx)
{
//	u8_t contenttype[12]=0;

    SFILENAME  *ptr;	
	*fsop_idx=0;
	for(HS->cur_fsop_idx=0;HS->cur_fsop_idx<FSOP_INDEX_NUM;HS->cur_fsop_idx++)
	{
			ptr=fsop[HS->cur_fsop_idx].open(name);
			if(ptr!=NULL)
			{
/*
				memset(contenttype,0,12);
				if(!strncmp(ptr->file_name, "xml", 3)){
			    	sprintf(contenttype,"text/xml");
			    	hs->contentType=contenttype;									
				}
			    if(!strncmp(&name[strlen(name)-3], "htm", 3)){
			    	sprintf(contenttype,"text/html");
			    	hs->contentType=contenttype;
			    }					    
			    if(!strncmp(&name[strlen(name)-3], "jpg", 3)){
			    	sprintf(contenttype,"image/jpeg");
			    	hs->contentType=contenttype;
			    }		
			    if(!strncmp(&name[strlen(name)-3], "gif", 3)){
			    	sprintf(contenttype,"image/gif");
			    	hs->contentType=contenttype;
			    }
*/
				*fsop_idx=HS->cur_fsop_idx;
				return ptr;
			}
	}		
    return NULL;
}
#endif

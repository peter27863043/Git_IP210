/******************************************************************************
*
*   Name:           fs.c
*
*   Description:    file system for bank0
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "uip.h"
#include "fs.h"
#include "type_def.h"

SFILENAME *   fs_open03(char *name);
/*
SFILENAME *   fs_open01(char *name);
SFILENAME *   fs_open02(char *name);
SFILENAME *   fs_open03(char *name);
SFILENAME *   fs_open04(char *name);
SFILENAME *   fs_open05(char *name);
SFILENAME *   fs_open06(char *name);
SFILENAME *   fs_open07(char *name);
*/
void datacpy03(mem_cpy_fs *inputdata);
/*
void datacpy00(mem_cpy_fs *inputdata);
void datacpy01(mem_cpy_fs *inputdata);
void datacpy02(mem_cpy_fs *inputdata);

void datacpy04(mem_cpy_fs *inputdata);
void datacpy05(mem_cpy_fs *inputdata);
void datacpy06(mem_cpy_fs *inputdata);
void datacpy07(mem_cpy_fs *inputdata);
*/
struct html_tags get_tag03(u8_t index);
/*
struct html_tags get_tag00(u8_t index);
struct html_tags get_tag01(u8_t index);
struct html_tags get_tag02(u8_t index);

struct html_tags get_tag04(u8_t index);
struct html_tags get_tag05(u8_t index);
struct html_tags get_tag06(u8_t index);
struct html_tags get_tag07(u8_t index);
*/
/*
#ifdef FLASH_512
#define FSOP_INDEX_NUM 8
_FS fsop[FSOP_INDEX_NUM]=
{
		fs_open00,datacpy00,get_tag00,
		fs_open01,datacpy01,get_tag01,
		fs_open02,datacpy02,get_tag02,
		fs_open03,datacpy03,get_tag03,
		fs_open04,datacpy04,get_tag04,
		fs_open05,datacpy05,get_tag05,
		fs_open06,datacpy06,get_tag06,
		fs_open07,datacpy07,get_tag07
};
#else
*/
#define FSOP_INDEX_NUM 1
_FS fsop[FSOP_INDEX_NUM]=
{
		fs_open03,datacpy03,get_tag03
};
//#endif

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


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
void datacpy03(mem_cpy_fs *inputdata);
struct html_tags get_tag03(u8_t index);


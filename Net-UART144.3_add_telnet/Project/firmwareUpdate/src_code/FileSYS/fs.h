/******************************************************************************
*
*   Name:           fs.h
*
*   Description:    file system header
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
#ifndef __FS_H__
#define __FS_H__
//define bank0~7
#define BANK00 0x00
#define BANK01 0x01
#define BANK02 0x02
#define BANK03 0x03
#define BANK04 0x04
#define BANK05 0x05
#define BANK06 0x06
#define BANK07 0x07
//typedefine cgi funciton pointer
typedef  u8_t (*CGI_FUNC_PTR)(u8_t, void *);

//HTML cgi tag structure
struct html_tags {
    u16_t   pos;
//    u8_t   len;
    u8_t   varid;
};
#define NO_EX_FUNC 255
//File System
typedef struct _sfilesystem
{	 
    char *file_name;//file name
    char *file_ptr; // address of the file 
    unsigned int  file_len;//length of file
    u8_t func_id; //function vector ID
} SFILENAME;

typedef struct _cgi_func_t
{	 
    u8_t    (*func)(u8_t, void *);//call back function
    struct html_tags *tags;//address of the tag structure
}CGI_FUNC_T;

//define find function in the common area, then it can called in each bank
//SFILENAME *   fs_open(char *name,u8_t* fsop_idx);
SFILENAME *   fs_open03(char *name);//,u8_t* fsop_idx);
//extern each CGI table, then find function can find cgi request in each bank
//******************
//memory copy file system

typedef struct mem_cpy_fs
{
  u8_t * dest_ptr;//address of destination buffer
  u8_t * source_ptr;//address of source buffer
  u16_t len;//data length
} mem_cpy_fs;
extern mem_cpy_fs indata;

typedef struct __FS
{
		SFILENAME* (*open)(char* filename);
		void (*Mem_copy)(mem_cpy_fs* inputdata);
		struct html_tags (*get_tag)(u8_t index);
}_FS;
extern _FS fsop[];
//file system operator, it use for memory copy and find filename
typedef struct fs_o
{
    SFILENAME *    (*Open_func)(char* filename);
    void    (*Mem_copy)(mem_cpy_fs *inputdata);
} FILE_Operator;
//******************
#define FILE_NUM 0x1
#endif

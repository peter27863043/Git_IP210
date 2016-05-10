/******************************************************************************
*
*   Name:           httpd.h
*
*   Description:    HTTP Header
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
#ifndef __HTTPD_H__
#define __HTTPD_H__
#include "fs.h"
//define data
//******error message*********************************
#define ERRORMSG_LOGIN          0
#define ERRORMSG_HISTORY_BACK   1
#define ERRORMSG_SETUP          2
#define ERRORMSG_NOTE           3
#define ERRORMSG_EEP            4
#define ERRORMSG_CHALLENGE      5
#define ERRORMSG_SETNO          6
#define ERRORMSG_SETOK          7
#define ERRORMSG_TIMEOUT        8
//******SetON message*********************************
#define SETON_DATARATE          0
#define SETON_VLANGROUP         1
//******Cookie and Challenge expire time**************
#define EXPIRE_TIME 60000//60000//system tick = 10 min
//****************************************************
#define MAX_PASSWORD_LEN    (30)
#define MAX_USERS           1
#define SUPER_USER_NAME  "admin"
#define DFLT_PASSWORD   "system"
//**********Cookie*********
#define MAX_COOKIE_LEN      5
//**************Function Prototype********************
void httpd_init(void);
void http_server(void);
void httpd_write(char *str, u16_t len);
bit_t httpd_init_file(SFILENAME *fsfile);

extern u8_t *httpd_set_cookie();
extern bit_t httpd_parse_cookie();
extern u8_t *httpd_set_challenge();
extern bit_t httpd_parse_challenge(void *check_value);
//structure area**************************************
//ssi return value and type
typedef enum {
    undefined    = 0,
    digits_3_int = 1,//3 digits integer
    string       = 2
} data_type;

typedef struct value_struct {
    union {
        int    digits_3_int;
        char  *string;
    } value;
    data_type    type;
} data_value;
//http request data item and value
typedef struct variable_struct {
    char  *item;
    char  *value;
} req_data;
//http in packet
typedef struct HTTP_IN_PACKET {
    u8_t  *http_Indata;
    u8_t  *http_cgiInfo;
    u8_t  *Cookie_item;
    u8_t  *Cookie_data;
} http_in_packet;
struct httpd_info{
  SFILENAME   *hsfile;//chance
  u8_t  op_index; //chance for file operater
  u8_t  state; //chance 
  u16_t count;//chance for count total html size
  u8_t  *dataptr;//chance for not tag html use to point current address
  u8_t  header_flag; //chance
  u8_t   *response;
  //for tag information
  u16_t html_pre_pos;   /* count of unsent bytes */
  u8_t  row_num;    // row number for a table; start from 1
  u8_t  row_num_unack;
  u8_t  tag_index; //for tag index
  u8_t  tag_index_unack; //for tag index un ack for retransmit
  u8_t  tag_index_start;
  u16_t html_current_pos;//chance
  u16_t tag_current_pos;//chance
  u8_t  tag_flag; //for tag start and end
  u8_t  table_flag;   //for first dimention
  u16_t count_unack;//chance for count unack data size
  
  //second dimention
  u8_t  col_num;    // col number for a table; start from 1
  u8_t  col_num_unack;
  u8_t  table2_flag;  //for second dimention
//  u8_t  tag2_index; //for tag index
//  u8_t  tag2_index_unack; //for tag index un ack for retransmit
  u8_t  tag2_index_start;

//*********RETX Count**********
  u8_t  retx_count;
//*****************************
//*********Polling Count**********
  u8_t  poll_count;
//*****************************
  http_in_packet INpacket;
    //u8_t  *http_Indata;
    //u8_t  *http_cgiInfo;
//*************header information
    u8_t  header_length;
    u8_t   *contentType;
    u8_t   *errormsg;
//********Challenge**********
//    u8_t   *challenge_value;
//********COOKIE_SUPPORT**********
    //u8_t   *cookie_name;
    //u8_t   *cookie_value;
//******************************
//**************************
#ifdef	UIP_REXMIT_DEBUG
    u8_t   file_name[20];
#endif
//**************************
	u16_t last_len;
//*************MULTIPACKET*************
	u16_t multipacket_datasize;
	u16_t  multipacket_count;		
//**************************
	SFILENAME  file;
	CGI_FUNC_T cgi_func;
	u8_t       cgi_func_id;
	u8_t       cur_fsop_idx;
};
typedef struct cookie_struct {
    u32_t   age;
    char    value[MAX_COOKIE_LEN];
} COOKIE_Type;

//extern variable
extern u32_t img512k;
extern u32_t inspect_img512k;
//extern COOKIE_Type cookie_data;
//extern COOKIE_Type challenge_data;
extern struct httpd_info *hs;
#define HS ((struct httpd_info *)(uip_conn->appstate))
#endif 


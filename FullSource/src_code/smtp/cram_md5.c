/******************************************************************************
*
*   Name:           cram_md5.c
*
*   Description:    CRAM MD5
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#include "cram_md5.h"
#include "base64.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "module.h"
#ifdef MODULE_SMTP
unsigned char buf_md5[100];
s_cram_md5_param _cram_md5_param;
void cram_md5(s_cram_md5_param* cram_md5_param)
{  
  u8_t *challenge;
  u8_t *decoded;
  u8_t *encoded;

  challenge = b64decode_alloc(cram_md5_param->greeting);
  b64decode(cram_md5_param->greeting, challenge);

  cram_md5_param->decoded_greeting=challenge;
  
  decoded = buf_md5;

  strcpy(decoded, cram_md5_param->username);

  encoded = b64encode_alloc(decoded);
  b64encode(decoded, encoded);
  cram_md5_param->encoded_username=encoded;
}
void cram_md5_encode(u8_t* in,u8_t* out)
{
	_cram_md5_param.username=in;
	_cram_md5_param.password="";
	_cram_md5_param.greeting="";
	cram_md5(&_cram_md5_param);
	strcpy(out,_cram_md5_param.encoded_username);
}

void cram_md5_decode(u8_t* in,u8_t* out)
{
	_cram_md5_param.username="";
	_cram_md5_param.password="";
	_cram_md5_param.greeting=in;
	cram_md5(&_cram_md5_param);
	strcpy(out,_cram_md5_param.decoded_greeting);
}
#endif //MODULE_SMTP

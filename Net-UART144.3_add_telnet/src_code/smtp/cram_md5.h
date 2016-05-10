#ifndef CRAM_MD5_H
#define CRAM_MD5_H
#include <type_def.h>
typedef struct _s_cram_md5_param
{
	u8_t* username;
	u8_t* password;
	u8_t* greeting;
	u8_t* decoded_greeting;
	u8_t* encoded_username;
}s_cram_md5_param;

void cram_md5(s_cram_md5_param* cram_md5_param);
void cram_md5_encode(u8_t* in,u8_t* out);
void cram_md5_decode(u8_t* in,u8_t* out);
#endif

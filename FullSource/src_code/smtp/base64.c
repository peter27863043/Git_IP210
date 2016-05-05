/******************************************************************************
*
*   Name:           base64.c
*
*   Description:    base 64
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <type_def.h>
#include "module.h"
#ifdef MODULE_SMTP
u8_t buf_en[100];
u8_t buf_de[100];
static u8_t b64e[] = {
'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
'w', 'x', 'y', 'z', '0', '1', '2', '3',
'4', '5', '6', '7', '8', '9', '+', '/' 
};

static u8_t b64d[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,0x00,0x00,0x3f,
0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,
0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x00,0x00,0x00,0x00,0x00,
0x00,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x00,0x00,0x00,0x00,0x00
};

static void b64enc(u8_t *in, int inlen, u8_t *out)
{
  u8_t t0 = (inlen > 0) ? in[0] : 0;
  u8_t t1 = (inlen > 1) ? in[1] : 0;
  u8_t t2 = (inlen > 2) ? in[2] : 0;

  if (inlen <= 0) return;
  out[0] = b64e[(t0 >> 2) & 0x3f];
  out[1] = b64e[((t0 << 4) & 0x30) | (t1 >> 4)];

  if (inlen <= 1){
	out[2]=0x3D;
	out[3]=0x3D;
	return;
  }
  out[2] = b64e[((t1 << 2) & 0x3c) | (t2 >> 6)];

  if (inlen <= 2){
	out[3]=0x3D;
	return;
  }
  out[3] = b64e[t2 & 0x3f];
}

static void b64dec(u8_t *in, int inlen, u8_t *out)
{
  u8_t t0 = (inlen > 0) ? b64d[in[0] & 0x7f] : 0;
  u8_t t1 = (inlen > 1) ? b64d[in[1] & 0x7f] : 0;
  u8_t t2 = (inlen > 2) ? b64d[in[2] & 0x7f] : 0;
  u8_t t3 = (inlen > 3) ? b64d[in[3] & 0x7f] : 0;

  if (inlen <= 0) return;
  out[0] = (t0 << 2) | (t1 >> 4);
  if (inlen <= 1) { 
    out[1] = 0;
    return;
  }
  out[1] = (t1 << 4) | ((t2 & 0x3c) >> 2);
  if (inlen <= 2) {
    out[2] = 0;
    return;
  }
  out[2] = (t2 << 6) | t3;
}

int b64encode_len(u8_t *in)
{
  int l = strlen(in);
  return 4*((l+2)/3);
}

int b64decode_len(u8_t *in)
{
  int l = strlen(in);
  return 3*((l+3)/4);
}

u8_t *b64encode_alloc(u8_t *in)
{
  int l = b64encode_len(in);
  u8_t *n = buf_en;//malloc(l+1);
  if (n != NULL) {
    n[l--] = 0;
    while (l >= 0) {
      n[l--] = '=';
    }
  }
  return n;
}

void b64encode(u8_t *in, u8_t *out)
{
  int inlen = strlen(in);
  while (inlen > 0) {
    b64enc(in, inlen, out);
    inlen -= 3;
    in += 3;
    out += 4;
  }
}

u8_t *b64decode_alloc(u8_t *in)
{
  int l = b64decode_len(in);
  u8_t *n = buf_de;//malloc(l+1);
  if (n != NULL) {
    while (l >= 0) {
      n[l--] = 0;
    }
  }
  return n;
}

void b64decode(u8_t *in, u8_t *out) {
  int inlen = strlen(in);
  while (inlen > 0) {
    b64dec(in, inlen, out);
    inlen -= 4;
    in += 4;
    out += 3;
  }
}
#endif //MODULE_SMTP

#ifndef _TFTPD_H_
#define _TFTPD_H_
#include <stdio.h>
#include <string.h>
#include "..\net.h"
/* opcodes */
#define TFTP_RRQ                0x0001      /* read request */
#define TFTP_WRQ                0x0002      /* write request */
#define TFTP_DATA               0x0003      /* data packet */
#define TFTP_ACK                0x0004      /* acknowledgement */
#define TFTP_ERROR              0x0005      /* error code */

#define ICMP_Header_Len         0x08
#define TFTP_Header_Len         0x04
/*packet length*/
/*
#define SEGSIZE                 512     
#define TFTPPKTSIZE             (SEGSIZE+4)
*/
/*error code*/
/*
#define ERR_OTHER		        0x0000
#define ERR_NOTFOUND	        0x0001
#define ERR_ACCESS	            0x0002
#define ERR_NOSPACE	            0x0003
#define ERR_ILLEGAL	            0x0004
#define ERR_PORT		        0x0005
#define ERR_EXISTS	            0x0006
#define ERR_NOUSER	            0x0007
*/
#define TFTP_WRQ_ACK_LEN 0x04

/*tftp packet header format*/
typedef struct _TFTPD_Header{
  u16_t      opcode;                /* packet type */
  union {
          u16_t    block_num;       /* block # */
          u16_t    errcode;         /* error code */
          char     filename[1];     /* request packet filename and mode */
        } str;
  u8_t indata[1];                    /* data or error string */
} tftpd_header;

//u8_t tftp_app(u8_t *indata,u16_t indata_len);

#define TFTPD_PKT ((tftpd_header *)&uip_fw_buf[UIP_LLH_LEN+IP_HEADER_LENGTH+UDP_HEADER_LENGTH])
#endif

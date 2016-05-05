#ifndef _FM_WEB_COM_H_
#define _FM_WEB_COM_H_
#include "uip.h"
#define UIP_BUFSIZE      1520
extern u8_t xdata uip_buf_real_mem[UIP_BUFSIZE+2+6];
//sorbica071123 #define uip_fw_buf uip_buf_real_mem
extern volatile u8_t uip_acc32[4];
#endif
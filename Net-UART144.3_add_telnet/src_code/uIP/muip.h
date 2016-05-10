/******************************************************************************
*
*   Name:           muip.h
*
*   Description:    multiple buffer for increase uip transmit throughput
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#ifndef MUIP_H
#include "module.h"
#ifdef MODULE_MUIP
#include "uip.h"
//multiple uIP structure
#define MAX_MBUF 1

extern struct uip_conn* muip_conn;
extern u8_t muip_buf[MAX_MBUF][UIP_BUFSIZE+2];
extern u8_t muip_buf_cnt;                 //total muip_buf need to send
extern u8_t muip_need_send;
extern u16_t muip_len[MAX_MBUF];
extern u8_t muip_snd_nxt0[4];
extern u8_t muip_snd_nxt1[MAX_MBUF][4];
extern u16_t uip_window_size;

void muip_send();
void uip_add32(u8_t* op32,u16_t op16);
void muip_process();
void muip_init();
#define muip_mss() ((MAX_MBUF+1)*uip_mss())>uip_window_size?uip_window_size:((MAX_MBUF+1)*uip_mss())
#define MUIP_APPDATA (UIP_LLH_LEN+40)
#endif //MODULE_MUIP
#endif //MUIP_H
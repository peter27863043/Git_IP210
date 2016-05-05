/******************************************************************************
*
*   Name:           
*
*   Description:    
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#ifndef UDP_TEL_H
#define UDP_TEL_H

#include "module.h"

#ifdef  MODULE_UDP_TELNET
#include "uip.h"
#include "..\main\eepdef.h"
#include "ring.h"
void udp_tel_init();
void udp_tel_close();
void udp_tel_chk_send();
void udp_tel_rx();
void UDP_TEL_Timer();
u8_t udp_get_dns();

extern u8_t udp_dns;
//#define UDP_TEL_MAX_IP_RANGE
#define MAX_ARP_RETRY 5
typedef struct _udp_tel_data_t
{
	u32_t valid_ip;
	u32_t is_send;
	u8_t  current_ip_num;
	udp_tel_info_t* pEEPROM_UDPTel;
	u32_t nxt_run_start;
	u32_t nxt_blk_start;//next udp block start
	u8_t initial_flag;
//----------Add for DNS----------------------------
	u8_t dns_wait;
	u8_t dns_retry;
	u8_t dns_idle;
}udp_tel_data_t;

//IP210 uip telnet times (ms)
//Advance to next UDP timer, it will also drop last udp block
#define ADV2NXT_UDP_BLK_TIMER 50
//Re-try to send remote IP, which not in ARP table
#define SAME_BLK_NXT_RUN_TIMER 50

extern udp_tel_data_t udp_tel_data;
#endif //MODULE_UDP_TELNET

#endif //UDP_TEL_H
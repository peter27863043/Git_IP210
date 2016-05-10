/******************************************************************************
*
*   Name:           uipopt.h
*
*   Description:    options of uip layer
*
*   Copyright:      
*
*******************************************************************************/
/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved.
 *
 * $Id: uipopt.h,v 1.6 2002/01/11 12:43:14 adam Exp $
 *
 */

#ifndef __UIPOPT_H__
#define __UIPOPT_H__

#include "type_def.h"



typedef struct
{
    u8_t   dest_mac[6];
    u8_t   src_mac[6];
#ifdef OEM_SWITCH_MANAGEMENT_MODE
    u8_t   b_type[2];     // BCM5328's Type
    u8_t   b_opcode[4];   // BCM5328's OPCode
#endif
    u16_t  type;
} ETH_HDR;

/*---------------------------------------------------------------------------*/
/* This is where you configure if your CPU is big or little endian.          */
/*---------------------------------------------------------------------------*/
#ifdef _MSC_VER
#define BIG_ENDIAN   0
#else
#define BIG_ENDIAN   1
#endif


/*-----------------------------------------------------------------------------------*/
/* The configuration options for a specific node. This includes IP
 * address, netmask and default router as well as the Ethernet
 * address. The netmask, default router and Ethernet address are
 * appliciable only if uIP should be run over Ethernet.
 *
 * All of these should be changed to suit your project.
*/

#include "httpd.h"

/* The maximum number of simultaneously active connections. */
#define UIP_CONNS        10


/* The maximum number of simultaneously listening TCP ports. */
#define UIP_LISTENPORTS  7


/* The link level header length; this is the offset into
   the uip_buf where the IP header can be found. For Ethernet, this
   should be set to 14. For SLIP, this should be set to 0. */
#define UIP_LLH_LEN    (sizeof(ETH_HDR))//14   //length of ethernet header

/* The maximum number of times a segment should be
   retransmitted before the connection should be aborted. */
#define UIP_MAXRTX      8

//IP210 will invoke uip_timer every  IP210_TIMER_SCALE=10 ==> 100msecond
#define IP210_TIMER_SCALE 10

/* The retransmission timeout counted in timer pulses (i.e.,
   the speed of the periodic timer, usually one second). */
#define UIP_RTO         3

/* The TCP maximum segment size. This should be set to
   at most UIP_BUFSIZE - UIP_LLH_LEN - 40. */
#define UIP_BUFSIZE      (1520)
   
#define UIP_TCP_MSS     (1460)

/* The IP TTL (time to live) of IP packets sent by uIP. */
#define UIP_TTL         255

/* How long a connection should stay in the TIME_WAIT state.
   Has no real implication, so it should be left untouched. */
#define UIP_TIME_WAIT_TIMEOUT  60


#endif

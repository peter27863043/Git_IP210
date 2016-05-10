#ifndef TYPE_DEF_H
#define TYPE_DEF_H
/*
 * Copyright (c) 2004, ICPlus Crop.	DeNa Lim 
 * All rights reserved.
 * Author: DeNa Lim  <dena@icplus.com.tw>
 *
 */
#include "type_def2.h"
#define	IP210_ON		1
//packet types
#define		pkt_802_3		0x00
#define		pkt_TCP_IPv4	0x01
#define		pkt_UDP_IPv4	0x02
#define		pkt_ICMP_IPv4	0x03
#define		pkt_TCP_IPv6	0x05
#define		pkt_UDP_IPv6	0x06
#define		pkt_ICMP_IPv6	0x07
#define		pkt_ARP			0x08
#define		pkt_BPDU		0x09
#define		pkt_RARP		0x0a

#define		ETHER_TYPE_OFFSET	12
#define		IP_TYPE_OFFSET		0x17
/****************************************************************************
*
*	Name:			TFTYPES
*
****************************************************************************/
#ifndef _U8_T
typedef unsigned long       u32_t;
typedef unsigned short      u16_t;
typedef unsigned char       u8_t;
#define _U8_T
#endif
typedef signed long         s32_t;
typedef signed short        s16_t;
typedef signed char         s8_t;
typedef u8_t                bool_t;
typedef bit                 bit_t;

#define TRUE            0x01
#define FALSE           0x00

#endif


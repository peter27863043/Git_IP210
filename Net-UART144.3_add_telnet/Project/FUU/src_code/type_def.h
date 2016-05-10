#ifndef TYPE_DEF_H
#define TYPE_DEF_H
/*
 * Copyright (c) 2004, ICPlus Crop.	DeNa Lim 
 * All rights reserved.
 * Author: DeNa Lim  <dena@icplus.com.tw>
 *
 */
//#include "Reg51.h"
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
/*
#ifndef _INC_TYPE_H
#define _INC_TYPE_H

//User Definition for Project need
typedef unsigned char volatile code CodeBYTE;
typedef unsigned char volatile data DataBYTE;
typedef unsigned char volatile pdata PDataBYTE;
typedef unsigned char volatile xdata XDataBYTE;

typedef unsigned int volatile code CodeWORD;
typedef unsigned int volatile data DataWORD;
typedef unsigned int volatile pdata PDataWORD;
typedef unsigned int volatile xdata XDataWORD;
*/
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
/*
// BIT MASK PATTERN 
#define BIT0            0X01
#define BIT1            0X02
#define BIT2            0X04
#define BIT3            0X08
#define BIT4            0X10
#define BIT5            0X20
#define BIT6            0X40
#define BIT7            0X80

#ifndef NULL
#define NULL (void*)(0)
#endif

//------------------------------------------------------------------------------
//  HIGHBYTE: MSB
//  LOWBYTE : LSB
// -----------------------------------------------------------------------------
#define HIGHBYTE(int_var)       ((((u16_t)int_var) >> 8) & 0x00ff) //[MSB]
#define LOWBYTE(int_var)        (((u16_t)int_var) & 0x00ff)        //[LSB]

#ifndef OK
#define OK  0
#endif

#ifndef ERROR
#define ERROR  (-1)
#endif

#endif
*/
#define TRUE            0x01
#define FALSE           0x00

#endif


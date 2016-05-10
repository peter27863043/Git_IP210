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
#ifndef MODULE_H
#define MODULE_H

//sorted by name
//#define CUSTOMER_EEPROM
//#define DIS_CHALLENGE
//#define DIS_COOKIE
//#define ET0_INTERRUPT
//#define IC_PACKAGE_128_PIN
//#define MODULE_ADC
//#define MODULE_DHCPC
//#define MODULE_DIDO
//#define MODULE_DNS
//#define MODULE_FULL_MSG
//#define MODULE_GPIO
//#define MODULE_MUIP
//#define MODULE_NET_CMD
//#define MODULE_NET_CMD_EEP_CFG
//#define MODULE_NET_UART_CTRL
//#define MODULE_NET_UART
//#define MODULE_NET_UART_INT
//#define MODULE_RS485
//#define MODULE_SMALL_UDP
//#define MODULE_SMTP
//#define MODULE_SNMP_TRAP
//#define MODULE_REVERSE_TELNET
//#define MODULE_UDP_TELNET
//#define MULTI_PACKET
#include "image_mode.h"

//================================================================
#ifdef IC_PACKAGE_128_PIN
#define CHIP_CFG_VALUE 0x87
#else
#define CHIP_CFG_VALUE 0x44
#endif 
//==================Module Description================================
//Name:					MODULE_DHCPC
//Size:					2773 Bytes (without MODULE_SMALL_UDP)
//reference module:	MODULE_SMALL_UDP.
//Description:		DHCP client program
//Options:
#ifdef  MODULE_DHCPC
#endif//MODULE_DHCPC

//==================Module Description================================
//Name:					MODULE_DNS
//Size:					1290 Bytes
//reference module:	MODULE_DNS.
//Description:		DNS client
//Options:
#ifdef  MODULE_DNS
#endif//MODULE_DNS


//==================Module Description================================
//Name:					MODULE_GPIO
//Size:					? Bytes
//reference module:	
//Description:		GPIO
//Options:
#ifdef  MODULE_GPIO
#endif//MODULE_GPIO

//==================Module Description================================
//Name:					MODULE_NET_UART
//Size:					3788 KBytes
//reference module:	    None.
//Description:		    UART to Telnet s/c
//Options:
#ifdef  MODULE_NET_UART
#define MODULE_TELNET_RE_TX_TIMEOUT 10
#endif//MODULE_NET_UART

//==================Module Description================================
//Name:					MODULE_NET_CMD
//Size:					 KBytes
//reference module:	    None.
//Description:		    TCP and Local command
//Options:
#ifdef  MODULE_NET_CMD
#endif//MODULE_NET_CMD


//==================Module Description================================
//Name:					MODULE_SMALL_UDP
//Size:					0493 Bytes
//reference module:       None.
//Description:		    UART to Telnet s/c
//Options:
#ifdef  MODULE_SMALL_UDP
#endif//MODULE_SMALL_UDP

//==================Module Description================================
//Name:					MODULE_SMTP
//Size:					3797 Bytes 
//reference module:	    None.
//Description:		
//Options:
#ifdef  MODULE_SMTP
#endif//MODULE_SMTP

//==================Module Description================================
//Name:					MODULE_SNMP_TRAP
//Size:					0518 Bytes 
//reference module:	    None.
//Description:		
//Options:
#ifdef  MODULE_SNMP_TRAP
#endif//MODULE_SNMP_TRAP

//-----------------------------------------------------------------------------
#define SYS_PARAM_EXPIRE_TIME 1
void* get_system_parameter(unsigned int ID);
#endif//MODULE_H

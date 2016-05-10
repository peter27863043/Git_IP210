#ifndef _PUBLIC_H
#define _PUBLIC_H

#include <stdio.h>
#include "ip210reg.h"
#include "serial.h"
#define FW_CODE_VERSION "V2.8"
#define CBYTE ((unsigned char volatile code*) 0)
#define DBYTE ((unsigned char volatile data*) 0)
#define XBYTE ((unsigned char volatile xdata*) 0)

#define BIT_0 0x01
#define BIT_1 0x02
#define BIT_2 0x04
#define BIT_3 0x08
#define BIT_4 0x10
#define BIT_5 0x20
#define BIT_6 0x40
#define BIT_7 0x80

void delay_ms(unsigned short);

//trace
//#define DBG_TEST
//#define DBG_EEPROM
//#define DBG_UART
//#define DBG_FLASH
//#define DBG_SERIAL
//#define DBG_INTERRUPT
//#define DBG_TIMER
//#define DBG_DMA
//-------------------
//#define TRACE_N(fmt)    {TI=1;printf fmt;}
#define TRACE_N(fmt)    {printf fmt;}
#ifdef DBG_TEST
#define TRACE_TEST(fmt) {TI=1;printf fmt;}
#else 
#define TRACE_TEST(fmt)
#endif

#ifdef DBG_EEPROM
#define TRACE_EEPROM(fmt) {TI=1;printf fmt;}
#else 
#define TRACE_EEPROM(fmt)
#endif

#ifdef DBG_UART
#define TRACE_UART(fmt) {TI=1;printf fmt;}
#else 
#define TRACE_UART(fmt)
#endif

#ifdef DBG_FLASH
#define TRACE_FLASH(fmt) {TI=1;printf fmt;}
#else 
#define TRACE_FLASH(fmt)
#endif

#ifdef DBG_SERIAL
#define TRACE_SERIAL(fmt) {TI=1;printf fmt;}
#else 
#define TRACE_SERIAL(fmt)
#endif

#ifdef DBG_INTERRUPT
#define TRACE_INTERRUPT(fmt) {TI=1;printf fmt;}
#else 
#define TRACE_INTERRUPT(fmt)
#endif

#ifdef DBG_TIMER
#define TRACE_TIMER(fmt) {TI=1;printf fmt;}
#else 
#define TRACE_TIMER(fmt)
#endif

#ifdef DBG_DMA
#define TRACE_DMA(fmt) {TI=1;printf fmt;}
#else 
#define TRACE_DMA(fmt)
#endif

//#define DB_UDP_TEL

//#define DB_UART_FIFO_OVERFLOW

//#define DB_SHOW_EEPROM_SIZE

//#define DB_DUMP_EEPROM

//#define DB_MP

//#define DEBUG_MUIP

//#define DB_NET_UART_CTRL

void CPU_GOTO_0000H(void);
void CPU_GOTO_FFFDH(void);

extern unsigned char in_isr;
#define IN_ISR_UART 0x01
#endif
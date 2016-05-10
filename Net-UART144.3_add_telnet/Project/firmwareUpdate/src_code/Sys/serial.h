/******************************************************************************
*
*   Name:           serial.h
*
*   Description:    Serial Port setting Header
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/

#ifndef _SERIAL_H
#define _SERIAL_H

//Prototype function
void Serial_Init( void );
u8_t Serial_Check_Rx(u8_t);
u8_t Serial_Polling_Rx(void);

#endif
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
unsigned char Serial_Check_Rx(unsigned char);
unsigned char Serial_Polling_Rx(void);

#endif
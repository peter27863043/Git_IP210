/******************************************************************************
*
*   Name:           serial.c
*
*   Description:    Driver functions for Serial port operations
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/
//include data
#include "ip210reg.h"
#include "serial.h"

//constants
#if SYSTEM_CLOCK_59M
#define SERIAL_19200 240 //50M Hz
#else
#define SERIAL_19200 244 //44.2M Hz
#endif

//functions
/******************************************************************************
*
*  Function:    Serial_Init
*
*  Description: to initialize Timer1 & Serial port 0 settings
*               
*  Parameters: none
*               
*  Returns: none
*               
*******************************************************************************/
void Serial_Init( void )
{
    SCON  = 0x50;  //Mode 1, 8-bit UART, Baudrate decided by Timer Overflow
    PCON |= 0x80;  //Double Baudrate
    TMOD &= 0x0F;  //Set to Timer1
    TMOD |= 0x20;  //Timer1 as 8-bit Auto-reload, reload from TH1 to TL1 upon every overflow
    TH1 = SERIAL_19200;     //Set to 19200 bps 
    TR1 = 1;       //Timer1 Run
    TI=1;//for using printf()
}


/******************************************************************************
*
*  Function:    Serial_Check_Rx
*
*  Description: to examine whether the received data on Serial Port 0 the one that is expected
*               
*  Parameters: the expected char on Serial Port0
*               
*  Returns: 
*  1: succeeded
*  0: failed
*               
*******************************************************************************/
unsigned char Serial_Check_Rx(unsigned char rx_byte)
{
    if(RI==1)
	{
	    RI=0;
        return (rx_byte==SBUF)?1:0;
	}  
	return 0;
}

/******************************************************************************
*
*  Function:    Serial_Check_Rx
*
*  Description: to check whether any received data on Serial Port 0 
*               
*  Parameters: none
*               
*  Returns: 
*  1: succeeded
*  0: failed
*               
*******************************************************************************/
unsigned char Serial_Polling_Rx(void)
{
    if(RI==1)
	{
	    RI=0;
        return SBUF;
	}  
	return 0;
}

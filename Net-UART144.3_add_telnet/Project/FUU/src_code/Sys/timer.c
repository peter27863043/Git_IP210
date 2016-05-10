/******************************************************************************
*
*   Name:           timer.c
*
*   Description:    functions to utilize the timercounter 
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/
//include data
#include "public.h"
#include "timer.h"

//variables
unsigned long  timercounter, high_timercounter;
//------------------------------------------------------
//For Timer Counter
//------------------------------------------------------
//overflow interval = (12*count)/Hz
//functions
/******************************************************************************
*
*  Function:    SeriTimer2_Inital_Init
*
*  Description: to initialize Timer2
*               
*  Parameters: none
*               
*  Returns: none
*               
*******************************************************************************/
void Timer2_Init(void)
{
    RCAP2H = 0x3E;  //set the scale for timer counter 
    RCAP2L = 0xFC; //for 40.32MHz, 10ms overflow intervals.
    TH2 = 0x0;
    TL2 = 0x0;
    TR2 = 0x1;
}

void Timer0_Init(void)
{
	TMOD &= 0xF0;
	TMOD |= 0x1;
	TL0=0x00;
	TH0=0x00;
//	EA=0x1;
//	ET0=0x1;
//	TR0=0x1;
}
/******************************************************************************
*
*  Function:    IP210_Write_Timer_Counter
*
*  Description: to change the value in timercounter register
*               
*  Parameters: 
*    timercounter: the value to be updated to timercounter
*               
*  Returns: none
*               
*******************************************************************************/
void IP210_Write_Timer_Counter(void)
{
      IP210RegWrite(REG_TIMER_COUNTER_0, timercounter&0xff);
      IP210RegWrite(REG_TIMER_COUNTER_1, (timercounter>>8)&0xff);
      IP210RegWrite(REG_TIMER_COUNTER_2, (timercounter>>16)&0xff);
      IP210RegWrite(REG_TIMER_COUNTER_3, (timercounter>>24)&0xff);
}

/******************************************************************************
*
*  Function:    Timercounter_Init
*
*  Description: to initialize the Timercounter
*               
*  Parameters: 
*    timercounter: the value to be updated to timercounter
*               
*  Returns: none
*               
*******************************************************************************/
void Timercounter_Init(void)
{
    TRACE_TIMER(("\n<-Timercounter_Init()~"));

	Timer2_Init();

    high_timercounter=0;
    timercounter=0;
	IP210_Write_Timer_Counter();

    TRACE_TIMER(("\n<-Timercounter_Init(%x %x)~", high_timercounter, timercounter));
}

void Timercounter0_Init(void)
{
    TRACE_TIMER(("\n<-Timercounter_Init()~"));

	Timer0_Init();
//    high_timercounter=0;
//    timercounter=0;

    TRACE_TIMER(("\n<-Timercounter_Init(%x %x)~", high_timercounter, timercounter));
}
/******************************************************************************
*
*  Function:    Timer_Counter_Overflow
*
*  Description: to call when timercounter overflows
*               
*  Parameters: 
*    high_timercounter: the number of overflows happened
*               
*  Returns: none
*               
*******************************************************************************/
void Timer_Counter_Overflow()
{
    high_timercounter++;
}



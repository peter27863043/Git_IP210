/******************************************************************************
*
*   Name:           timer.h
*
*   Description:    timer Header
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/

#ifndef _TIMER_H
#define _TIMER_H
#include "ip210reg.h"

//Prototype Functions
void Timercounter_Init(void);
void IP210_Write_Timer_Counter(void);
//void IP210_Update_Timer_Counter(void);
#define  IP210_Update_Timer_Counter() \
      IP210RegWrite(REG_LATCH_TIMER_COUNTER, 0x1);\
      ((u8_t*)(&timercounter))[0] = IP210RegRead(REG_TIMER_COUNTER_3);\
      ((u8_t*)(&timercounter))[1] = IP210RegRead(REG_TIMER_COUNTER_2);\
      ((u8_t*)(&timercounter))[2] = IP210RegRead(REG_TIMER_COUNTER_1);\
      ((u8_t*)(&timercounter))[3] = IP210RegRead(REG_TIMER_COUNTER_0);

//unsigned long Delta_Time(unsigned long old_time);
#define Delta_Time(old_time)    (u32_t)(timercounter + (~old_time+1))
extern u32_t  timercounter, high_timercounter;
#endif
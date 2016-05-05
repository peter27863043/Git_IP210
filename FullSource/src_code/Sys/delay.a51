/******************************************************************************
*
*   Name:           delay.a51
*
*   Description:    delay function
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/
;include data
$NOMOD51
$INCLUDE  (REG52.INC)

                NAME    ?delay_ms
                
		PUBLIC _delay_ms

?delay     SEGMENT   CODE

RSEG ?delay

;functions
/******************************************************************************
*
*  Function:    _delay_ms
*
*  Description: to delay a period of time
*               
*  Parameters: 
*    unsigned short int: how many ms of time to be delayed
*               
*  Returns: none
*               
*******************************************************************************/
_delay_ms:
mov  a, r5
push acc
mov  a, r4
push acc

inc r6
inc r7

MS_LOOP:

      MOV     R4, #0Eh
D1:
      MOV     R5, #051h
D2:   DJNZ    R5, D2
      DJNZ    R4, D1

      DJNZ    R7, MS_LOOP
      DJNZ    R6, MS_LOOP
POP acc
mov r4, a
POP acc
mov r5, a
      RET
END

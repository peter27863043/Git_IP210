/******************************************************************************
*
*   Name:           flashsum.a51
*
*   Description:    checksum calculation function
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/
;include data
$NOMOD51
$INCLUDE  (REG52.INC)
WDTCON      DATA    0D8H

                NAME    ?FlashSum


?FlashSum   SEGMENT   CODE



                PUBLIC  _FlashBankSum
                PUBLIC  _FlashBlankCheck

		RSEG ?FlashSum
;---------------------------------------------
;functions
/******************************************************************************
*
*  Function:    _FlashBankSum
*
*  Description: the sum up the current bank of Flash data
*               
*  Parameters: 
*    unsigned short int: the upper limit address of data that is to be summed up
*               
*  Returns: the checksum so far
*               
*******************************************************************************/
_FlashBankSum:
	      MOV      R5, #0h
	      MOV      DPTR, #0h
	      INC      R6
	      INC      R7
              ;redirect data bus to flash
	      orl      8Eh, #07h
	      SETB     WDTCON.4
;LOOP_B:
;jnb TI, LOOP_B
;mov SBUF, #34h
;clr TI
SUM_LOOP:
CLR      WDTCON.1

	      MOVX     A, @DPTR
	      ADD      A, R5
	      MOV      R5, A
              INC      DPTR

	      DJNZ     R7, SUM_LOOP
	      DJNZ     R6, SUM_LOOP
SETB     WDTCON.1
        MOV      A, R5
        MOV      R7, A

	      ;switch data bus back to normal
     	  CLR      WDTCON.4
	      anl      8Eh, #0F8h
        ret
;---------------------------------------------

_FlashBlankCheck:
	      MOV      DPTR, #0h
	      INC      R6
	      INC      R7

              ;redirect data bus to flash
	      mov      8Eh, #07h
	      SETB     WDTCON.4

FlashBlankCheck_LOOP:
              MOVX     A, @DPTR
              XRL      A, #0ffh
              JNZ      NOT_BLANK
              INC      DPTR

	      DJNZ     R7, FlashBlankCheck_LOOP
	      DJNZ     R6, FlashBlankCheck_LOOP


              MOV      R7, #1h
	      ;switch data bus back to normal
              CLR      WDTCON.4
	      mov      8Eh, #0h
              ret
NOT_BLANK:
              MOV      R7, #0h
	      ;switch data bus back to normal
              CLR      WDTCON.4
	      mov      8Eh, #0h              
              ret
;---------------------------------------------
END

$NOMOD51
$INCLUDE  (REG52.INC)
$INCLUDE  (image_module.inc)

P4      DATA    0E8H
P4ENON  DATA    0DAH

WDTEN   BIT     0D9H
;  User-defined Power-On Initialization of Memory
;
;  With the following EQU statements the initialization of memory
;  at processor reset can be defined:
;
;               ; the absolute start-address of IDATA memory is always 0
IDATALEN        EQU     80H     ; the length of IDATA memory in bytes.
;
XDATASTART      EQU     0H      ; the absolute start-address of XDATA memory
;XDATALEN        EQU     8000H      ; the length of XDATA memory in bytes.
XDATALEN        EQU     7FFEH      ; the length of XDATA memory in bytes.
;
PDATASTART      EQU     0H      ; the absolute start-address of PDATA memory
PDATALEN        EQU     0H      ; the length of PDATA memory in bytes.
;
;  Notes:  The IDATA space overlaps physically the DATA and BIT areas of the
;          8051 CPU. At minimum the memory space occupied from the C51 
;          run-time routines must be set to zero.
;------------------------------------------------------------------------------
;
;  Reentrant Stack Initilization
;
;  The following EQU statements define the stack pointer for reentrant
;  functions and initialized it:
;
;  Stack Space for reentrant functions in the SMALL model.
IBPSTACK        EQU     0       ; set to 1 if small reentrant is used.
IBPSTACKTOP     EQU     0FFH+1  ; set top of stack to highest location+1.
;
;  Stack Space for reentrant functions in the LARGE model.      
XBPSTACK        EQU     0       ; set to 1 if large reentrant is used.
XBPSTACKTOP     EQU     0FFFFH+1; set top of stack to highest location+1.
;
;  Stack Space for reentrant functions in the COMPACT model.    
PBPSTACK        EQU     0       ; set to 1 if compact reentrant is used.
PBPSTACKTOP     EQU     0FFFFH+1; set top of stack to highest location+1.
;
;------------------------------------------------------------------------------
;
;  Page Definition for Using the Compact Model with 64 KByte xdata RAM
;
;  The following EQU statements define the xdata page used for pdata
;  variables. The EQU PPAGE must conform with the PPAGE control used
;  in the linker invocation.
;
PPAGEENABLE     EQU     0       ; set to 1 if pdata object are used.
;
PPAGE           EQU     0       ; define PPAGE number.
;
PPAGE_SFR       DATA    0A0H    ; SFR that supplies uppermost address byte
;               (most 8051 variants use P2 as uppermost address byte)
;
;------------------------------------------------------------------------------

                NAME    ?C_STARTUP

?C_C51STARTUP   SEGMENT   CODE
?STACK          SEGMENT   IDATA

                EXTRN CODE (?C_START)
$IF RS485
;                EXTRN CODE (ISR_ET0)		
                EXTRN CODE (ip210_uart_isr_process)
$ENDIF

                PUBLIC  ?C_STARTUP
                PUBLIC  CPU_GOTO_FFF7H
                PUBLIC  CPU_GOTO_0000H
                PUBLIC  CPU_GOTO_FFFDH
		PUBLIC  CPU_REBOOT

$if A51_D_FLASH_512
                PUBLIC  CPU_GOTO_FUU
$endif

;------------------------------------------------------------
;Reserve Bit-Addressable Space
                    BSEG    AT    20h.0
;BitAddr:                DBIT    80  
BitAddr:                DBIT    40  
;------------------------------------------------------------
;Reserve Data memory Space
                    DSEG  AT  08h
                        DS    24

                    DSEG  AT  30h
Reserved_DATA:
                        DS    48
;------------------------------------------------------------
                    RSEG    ?STACK
STACK_LABEL:
                        DS      1
;-------------------------------------------------------------

                CSEG    AT      0
?C_STARTUP:     LJMP    STARTUP1

                CSEG    AT      03h
                jmp vec_clean_uart_isr

                CSEG    AT      0fffdh
                JMP 0808Dh

                CSEG    AT      0Bh
	      jmp VECTOR_INT0_ET0
;-------------------------------------------------------------
                RSEG    ?C_C51STARTUP
;-----------------------------
VECTOR_INT0_ET0:
              clr EA

	      push acc
	      push b
	      push dph
	      push dpl
	      push psw
	      mov  psw, #08h;using register bank 1
$IF RS485
;              call ISR_ET0
$ENDIF
	      pop  psw
	      pop  dpl
	      pop  dph
	      pop  b
	      pop  acc

	      setb EA
	      reti
	      
vec_clean_uart_isr:
              clr EA

	      push acc
	      push b
	      push dph
	      push dpl
	      push psw
	      mov  psw, #08h;using register bank 1
$IF RS485
	      call ip210_uart_isr_process
$ENDIF
	      pop  psw
	      pop  dpl
	      pop  dph
	      pop  b
	      pop  acc

	      setb EA
	      reti	      
;--------------------------end of interrupt isr
END_POINT:
                JMP     END_POINT
STARTUP1:
                CLR    WDTEN;for disabling WDT when there was a WDT overflow-reset
;Initialize DATAs
IF IDATALEN <> 0
                MOV     R0,#IDATALEN - 1
                CLR     A
IDATALOOP:      MOV     @R0,A
                DJNZ    R0,IDATALOOP
ENDIF

IF XDATALEN <> 0
                MOV     DPTR,#XDATASTART
                MOV     R7,#LOW (XDATALEN)
  IF (LOW (XDATALEN)) <> 0
                MOV     R6,#(HIGH (XDATALEN)) +1
  ELSE
                MOV     R6,#HIGH (XDATALEN)
  ENDIF
                CLR     A
XDATALOOP:      MOVX    @DPTR,A
                INC     DPTR
                DJNZ    R7,XDATALOOP
                DJNZ    R6,XDATALOOP
ENDIF

IF PPAGEENABLE <> 0
                MOV     PPAGE_SFR,#PPAGE
ENDIF

IF PDATALEN <> 0
                MOV     R0,#LOW (PDATASTART)
                MOV     R7,#LOW (PDATALEN)
                CLR     A
PDATALOOP:      MOVX    @R0,A
                INC     R0
                DJNZ    R7,PDATALOOP
ENDIF

IF IBPSTACK <> 0
EXTRN DATA (?C_IBP)

                MOV     ?C_IBP,#LOW IBPSTACKTOP
ENDIF

IF XBPSTACK <> 0
EXTRN DATA (?C_XBP)

                MOV     ?C_XBP,#HIGH XBPSTACKTOP
                MOV     ?C_XBP+1,#LOW XBPSTACKTOP
ENDIF

IF PBPSTACK <> 0
EXTRN DATA (?C_PBP)
                MOV     ?C_PBP,#LOW PBPSTACKTOP
ENDIF
;Initialize DATAs
           
                MOV     SP,#?STACK-1

		CLR     P3.4;A16
		CLR     P3.5;A17
		CLR     P1.7;A18
                CLR     P4.1;A19
                CLR     P4.6;A20

; This code is required if you use L51_BANK.A51 with Banking Mode 4
EXTRN CODE (?B_SWITCH0)
               CALL    ?B_SWITCH0      ; init bank mechanism to code bank 0


                CALL    ?C_START
                JMP     END_POINT

;for WebCtrl jumping to Firmware Update Project while doing firmware update
CPU_GOTO_FFF7H:
		CLR     EA
		CLR     ET0
		CLR     P3.4;A16
		CLR     P3.5;A17
		CLR     P1.7;A18
                CLR     P4.1;A19
                CLR     P4.6;A20
;------------------------------------------------------------
;for P4 output en
;------------------------------------------------------------
                mov a, P4ENON
                orl a, #042h
                mov P4ENON, a
;------------------------------------------------------------

                JMP 08087h
;redirect CPU to C:0000h
CPU_GOTO_0000H:
               JMP 0000h
CPU_GOTO_FFFDH:
CPU_REBOOT:
		CLR     EA
		CLR     ET0
		CLR     P3.4;A16
		CLR     P3.5;A17
		CLR     P1.7;A18
                CLR     P4.1;A19
                CLR     P4.6;A20
                JMP 0808Dh
;image id 20100406
                CSEG    AT      00050h
                DB 031h, 033h, 066h, 030h, 066h, 030h, 031h, 033h,000h

$if A51_D_FLASH_512
CPU_GOTO_FUU:

;=============================================================================
PUSH ACC 
;SR
mov SCON, #50h
orl PCON, #80h
anl TMOD, #0Fh
orl TMOD, #20h
mov TH1, #0F0h
setb TR1
setb TI

;H18-------------------------------
mov a, P1
anl a, #80h
jnz H18_1
mov SBUF, #30h
jmp H18_SR

H18_1:
mov SBUF, #31h

H18_SR:
clr TI
jnb TI, H18_CHECK_TI
H18_CHECK_TI:
jnb TI, H18_CHECK_TI
;---------------------------
;H17-------------------------------
mov a, P3
anl a, #20h
jnz H17_1
mov SBUF, #30h
jmp H17_SR

H17_1:
mov SBUF, #31h

H17_SR:
clr TI
jnb TI, H17_CHECK_TI
H17_CHECK_TI:
jnb TI, H17_CHECK_TI
;---------------------------
;H16-------------------------------
mov a, P3
anl a, #10h
jnz H16_1
mov SBUF, #30h
jmp H16_SR

H16_1:
mov SBUF, #31h

H16_SR:
clr TI
jnb TI, H16_CHECK_TI
H16_CHECK_TI:
jnb TI, H16_CHECK_TI
;---------------------------

POP ACC
;=============================================================================
                JMP 0808Fh;for go to FUU
$endif

                END

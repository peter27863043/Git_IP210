$NOMOD51
$INCLUDE  (REG52.INC)

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
XDATALEN        EQU     7ffeH      ; the length of XDATA memory in bytes.
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
		
                PUBLIC  ?C_STARTUP
		PUBLIC CPU_GOTO_0000H
		PUBLIC CPU_GOTO_FFFDH
		PUBLIC  CPU_REBOOT

;------------------------------------------------------------
;Reserve Bit-Addressable Space
                    BSEG    AT    20h.0
BitAddr:                DBIT    80  
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
;Starting point, if it jumps from WebCtrl
;-------------------------------------------------------------
                CSEG    AT      08087h
		MOV     30h,#0A6h
		LJMP    STARTUPF7 ;STARTUP1
;*******************************************
;For go to Bank0 from Bank4, therefore clr A18, CSeg at 4:0808DH, it takes 2bytes for CLR bit op
;it has to have the same code at the same address in FU code for FUU to go to FU
;*******************************************
		CSEG    AT      0808DH
		CLR     P1.7;A18
;*******************************************
;For go to FUU, not bank jumping
;*******************************************
		CSEG    AT      0808FH
;*******************************************
?C_STARTUP:    
		LJMP    STARTUP1
;*******************************************
;-------------------------------------------------------------
                RSEG    ?C_C51STARTUP
END_POINT:
                JMP     END_POINT
STARTUP1:
;Initialize DATAs
IF IDATALEN <> 0
                MOV     R0,#IDATALEN - 1
                CLR     A
IDATALOOP:      MOV     @R0,A
                DJNZ    R0,IDATALOOP
ENDIF
STARTUPF7:



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

;-------------------------------------------------                
                MOV     SP,#?STACK-1
;Call main()
                CALL    ?C_START
                JMP     END_POINT


;redirect CPU to C:0000h
CPU_GOTO_0000H:
               JMP 0000h

;redirect CPU to C:0FFFDh for WebCtrl firmware update operation
CPU_GOTO_FFFDH:
CPU_REBOOT:
               	JMP 0FFFDh
        CSEG    AT      0FFFDH
                JMP     0808DH
                END

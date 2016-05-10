$NOMOD51
$INCLUDE  (REG52.INC)
                NAME    ?MP_FLASH

CKCON  data  08Eh
WDTCON  data  0D8h

?MP_FLASH       SEGMENT   CODE

		PUBLIC mpCloseFlash
                
		RSEG ?MP_FLASH
mpCloseFlash:
                ;push all used registers
                push acc
		push dpl
		push dph

		;into mirror mode
		mov dptr, #8006h
		mov a, #HIGH mpCloseFlash
;		mov SBUF, a
		movx @dptr, a

		mov dptr, #8000h
		movx a, @dptr
		setb  acc.5
		movx @dptr, a;do mirroring

		;enable flash
		orl CKCON, #7h
		setb WDTCON.4

		;update flash
;      XBYTE[0x5555] = 0xAA;
                mov dptr, #05555h
		mov a, #0aah
		movx @dptr, a
;      XBYTE[0x2AAA] = 0x55;
                mov dptr, #02aaah
		mov a, #055h
		movx @dptr, a
;      XBYTE[0x5555] = 0xA0;
                mov dptr, #05555h
		mov a, #0a0h
		movx @dptr, a
;      XBYTE[flash_write_addr] = DBYTE[Xxxx];
                mov dptr, #0FFF6h
		mov a, #84h
		movx @dptr, a
;delay
      mov     r6, #10
      mov     r7, #10
MS_LOOP:
      MOV     R4, #10h
D1:
      MOV     R5, #065h
D2:   DJNZ    R5, D2
      DJNZ    R4, D1

      DJNZ    R7, MS_LOOP
      DJNZ    R6, MS_LOOP


		;disable flash
		anl CKCON, #0f8h
		clr WDTCON.4

		;de-mirroring
		mov dptr, #8000h
		movx a, @dptr
		clr  acc.5
		movx @dptr, a;do de-mirroring

		;pop all used registers back
		pop dph
		pop dpl
		pop acc
		ret
                END
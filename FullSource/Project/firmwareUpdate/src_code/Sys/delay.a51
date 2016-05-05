$NOMOD51
$INCLUDE  (REG52.INC)

                NAME    ?delay_ms
                
		PUBLIC _delay_ms

?delay     SEGMENT   CODE

RSEG ?delay

_delay_ms:
mov  a, r5
push acc
mov  a, r4
push acc

inc  r6

MS_LOOP:
$if SYSTEM_CLOCK_59M=1
      MOV     R4, #10h
$else
      MOV     R4, #0Ch
$endif
D1:
      MOV     R5, #065h
D2:   DJNZ    R5, D2
      DJNZ    R4, D1

      DJNZ    R7, MS_LOOP
      DJNZ    R6, MS_LOOP
R6EZ:
POP acc
mov r4, a
POP acc
mov r5, a
      RET
END

@echo off

cd ..\..\src_code_flash\HtmSrcPage
call Cod2C uart.htm

copy uart_htm.c ..\..\src_code\HtmCfiles /y
copy uart_htm.h ..\..\src_code\HtmCfiles /y
del *.c
del *.h

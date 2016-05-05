@echo off
rem cd ..\..\src_code\HtmSrcPage
rem call Cod2C.bat

cd ..\..\src_code_flash\HtmSrcPage
call Cod2C fw_up.htm
call Cod2C left.htm
call Cod2C adc.htm
call Cod2C uart.htm
call Cod2C telnet.htm
call Cod2C setIP.htm
call Cod2C fu_up.htm
call Cod2C fw_up_wait.htm
call Cod2C setPASS.htm
call Cod2C smtp.htm
call Cod2C Status.htm
call Cod2C udp_tel.htm

copy *.c ..\..\src_code\HtmCfiles /y
copy *.h ..\..\src_code\HtmCfiles /y
del *.c
del *.h

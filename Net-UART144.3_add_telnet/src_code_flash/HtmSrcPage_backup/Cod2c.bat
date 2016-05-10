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
call Cod2C index.htm
call Cod2C dido.htm
call Cod2C NoService.htm
call Cod2C NoService2.htm
call Cod2C NoService3.htm
call Cod2C NoService4.htm
call Cod2C NoService5.htm
call Cod2C NoService6.htm

copy *.c ..\..\src_code\HtmCfiles /y
copy *.h ..\..\src_code\HtmCfiles /y
del *.c
del *.h

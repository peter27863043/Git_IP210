@echo off

del /Q left.htm
copy adc_left.htm left.htm
copy dido_64.htm dido.htm
call common.bat
call Cod2C udp_tel.htm
call Cod2C smtp.htm
call Cod2C setPASS.htm
call Cod2C Status.htm

copy adc_48pin.htm adc.htm
call Cod2C adc.htm

copy *.c ..\HtmCfiles /y
copy *.h ..\HtmCfiles /y
del *.c
del *.h

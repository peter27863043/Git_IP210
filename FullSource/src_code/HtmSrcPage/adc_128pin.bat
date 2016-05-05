@echo off

call udp_tel.bat
del /Q left.htm
copy adc_left.htm left.htm
call common.bat
copy adc_128pin.htm adc.htm
call Cod2C adc.htm


copy *.c ..\HtmCfiles /y
copy *.h ..\HtmCfiles /y
del *.c
del *.h

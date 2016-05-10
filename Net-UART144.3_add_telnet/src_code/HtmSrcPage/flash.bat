@echo off

copy dido_512.htm dido.htm
call common.bat
call Cod2C smtp.htm
call Cod2C udp_tel.htm

copy *.c ..\HtmCfiles /y
copy *.h ..\HtmCfiles /y
del *.c
del *.h

@echo off

del /Q left.htm
copy u_left.htm left.htm
call common.bat
call Cod2C udp_tel.htm

copy *.c ..\HtmCfiles /y
copy *.h ..\HtmCfiles /y
del *.c
del *.h

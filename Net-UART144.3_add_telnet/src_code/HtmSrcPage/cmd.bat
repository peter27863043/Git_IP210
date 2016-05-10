@echo off

del /Q left.htm
copy cmd_left.htm left.htm
copy dido_64.htm dido.htm
call common.bat

copy *.c ..\HtmCfiles /y
copy *.h ..\HtmCfiles /y
del *.c
del *.h

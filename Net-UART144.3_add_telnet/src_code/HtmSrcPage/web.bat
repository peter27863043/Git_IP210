@echo off

del /Q left.htm
copy w_left.htm left.htm
call common.bat

copy *.c ..\HtmCfiles /y
copy *.h ..\HtmCfiles /y
del *.c
del *.h

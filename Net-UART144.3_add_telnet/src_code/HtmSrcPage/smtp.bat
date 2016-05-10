@echo off

del /Q left.htm
copy s_left.htm left.htm
call common.bat
call Cod2C smtp.htm

copy *.c ..\HtmCfiles /y
copy *.h ..\HtmCfiles /y
del *.c
del *.h

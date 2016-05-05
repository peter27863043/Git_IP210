@echo off

rem call ..\..\..\bin\Cod2C errormsg.htm
call ..\..\..\bin\Cod2C main.htm
call ..\..\..\bin\Cod2C fu.htm
call ..\..\..\bin\Cod2C Warn.htm
call ..\..\..\bin\Cod2C Erase.htm
call ..\..\..\bin\Cod2C OK.htm
call ..\..\..\bin\Cod2C TFTPGoing.htm
call ..\..\..\bin\Cod2C TF.htm
call ..\..\..\bin\Cod2C sb.gif
rem call ..\..\..\bin\Cod2C test1.JPG

del ..\HtmFiles\*.* /y

copy *.c ..\HtmFiles /y
copy *.h ..\HtmFiles /y
copy *.tag ..\HtmFiles /y
del *.c
del *.h
del *.tag

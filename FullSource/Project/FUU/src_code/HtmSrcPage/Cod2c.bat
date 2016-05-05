@echo off

call Cod2C errormsg.htm
call Cod2C firmwareUpdate.htm
call Cod2C EraseFlash.htm
call Cod2C UpdateComplete.htm
call Cod2C TFTPInProgress.htm
call Cod2C Warn.htm
call Cod2C TF.htm

del ..\HtmFiles\*.* /y

copy *.c ..\HtmFiles /y
copy *.h ..\HtmFiles /y
copy *.tag ..\HtmFiles /y
del *.c
del *.h
del *.tag

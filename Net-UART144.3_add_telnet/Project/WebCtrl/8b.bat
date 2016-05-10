@echo off
echo [Hex2Bin]:
.\hex2bin.exe /Q /L65536 .\obj\NetUART.hex .\exe\NetUART.bin
echo [otpchksum]:
.\otpchksum .\obj\NetUART.hex
echo [copy to .\exe]:
copy /y .\obj\NetUART.bin .\exe\NetUARTc.bin
echo [copy to %1]:
copy /y .\obj\NetUART.bin %1\NetUARTc.bin
copy /y .\exe\NetUART.bin %1\NetUART.bin
echo [dir]:
dir %1\NETUART*.BIN

echo ImageMode:
type ..\..\src_code\include\image_mode.h
echo .
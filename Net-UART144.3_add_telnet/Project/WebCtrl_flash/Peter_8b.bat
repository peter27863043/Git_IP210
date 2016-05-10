@echo off
copy /y .\obj\NETUART.H00 .\obj\tmp00.hex > build_msg.txt
copy /y ..\firmwareUpdate\obj\firmwareUpdate.hex .\obj\tmpfu.hex >> build_msg.txt
copy /y ..\FUU\obj\firmwareUpdate.hex .\obj\fuu.hex >> build_msg.txt

copy /y .\obj\NETUART.H01 .\obj\tmp01.hex >> build_msg.txt
copy /y .\obj\NETUART.H02 .\obj\tmp02.hex >> build_msg.txt
copy /y .\obj\NETUART.H03 .\obj\tmp03.hex >> build_msg.txt
copy /y .\obj\NETUART.H04 .\obj\tmp04.hex >> build_msg.txt
copy /y .\obj\NETUART.H05 .\obj\tmp05.hex >> build_msg.txt
copy /y .\obj\NETUART.H06 .\obj\tmp06.hex >> build_msg.txt
copy /y .\obj\NETUART.H07 .\obj\tmp07.hex >> build_msg.txt


C:\IP210\hex2bin.exe /Q /L524288    .\obj\tmp00.hex .\obj\tmp09.bin >> build_msg.txt
C:\IP210\hex2bin.exe /Q /O65536  /M .\obj\tmp01.hex .\obj\tmp09.bin >> build_msg.txt
C:\IP210\hex2bin.exe /Q /O131072 /M .\obj\tmp02.hex .\obj\tmp09.bin >> build_msg.txt
C:\IP210\hex2bin.exe /Q /O196608 /M .\obj\tmp03.hex .\obj\tmp09.bin >> build_msg.txt
C:\IP210\hex2bin.exe /Q /O262144 /M .\obj\tmp04.hex .\obj\tmp09.bin >> build_msg.txt
C:\IP210\hex2bin.exe /Q /O327680 /M .\obj\tmp05.hex .\obj\tmp09.bin >> build_msg.txt
C:\IP210\hex2bin.exe /Q /O393216 /M .\obj\tmp06.hex .\obj\tmp09.bin >> build_msg.txt
C:\IP210\hex2bin.exe /Q /O458752 /M .\obj\tmp07.hex .\obj\tmp09.bin >> build_msg.txt
C:\IP210\hex2bin.exe /Q /O262144 /M .\obj\fuu.hex   .\obj\tmp09.bin >> build_msg.txt

C:\IP210\nchksum.exe .\obj\tmp09.bin
copy /y .\obj\tmp09.bin .\exe\NETUARTAPP.bin >> build_msg.txt

C:\IP210\hex2bin.exe /Q /O0000 /M .\obj\tmpfu.hex .\obj\tmp09.bin >> build_msg.txt

C:\IP210\nchksum.exe .\obj\tmp09.bin
copy /y .\obj\tmp09.bin .\exe\NETUART.bin >> build_msg.txt

rem .\obj\NETUART.H0* >> build_msg.txt
del .\obj\tmp*.* >> build_msg.txt
rem del .\obj\fu.hex >> build_msg.txt

rem if errorlevel==1 goto OUT
rem EXIT
rem :OUT
rem  ..\..\bin\er.exe

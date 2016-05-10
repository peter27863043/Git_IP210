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
copy /y .\obj\NETUART.H08 .\obj\tmp08.hex >> build_msg.txt
copy /y .\obj\NETUART.H09 .\obj\tmp09.hex >> build_msg.txt
copy /y .\obj\NETUART.H10 .\obj\tmp10.hex >> build_msg.txt
copy /y .\obj\NETUART.H11 .\obj\tmp11.hex >> build_msg.txt
copy /y .\obj\NETUART.H12 .\obj\tmp12.hex >> build_msg.txt
copy /y .\obj\NETUART.H13 .\obj\tmp13.hex >> build_msg.txt
copy /y .\obj\NETUART.H14 .\obj\tmp14.hex >> build_msg.txt
copy /y .\obj\NETUART.H15 .\obj\tmp15.hex >> build_msg.txt


..\..\bin\hex2bin.exe /Q /L1048576    .\obj\tmp00.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O65536  /M .\obj\tmp01.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O131072 /M .\obj\tmp02.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O196608 /M .\obj\tmp03.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O262144 /M .\obj\tmp04.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O327680 /M .\obj\tmp05.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O393216 /M .\obj\tmp06.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O458752 /M .\obj\tmp07.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O524288 /M .\obj\tmp08.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O589824 /M .\obj\tmp09.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O655360 /M .\obj\tmp10.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O720896 /M .\obj\tmp11.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O786432 /M .\obj\tmp12.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O851968 /M .\obj\tmp13.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O917504 /M .\obj\tmp14.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O983040 /M .\obj\tmp15.hex .\obj\tmp16.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O262144 /M .\obj\fuu.hex   .\obj\tmp16.bin >> build_msg.txt

rem ..\..\bin\nchksum.exe .\obj\tmp16.bin
rem copy /y .\obj\tmp16.bin .\exe\NETUARTAPP.bin >> build_msg.txt

..\..\bin\hex2bin.exe /Q /O0000 /M .\obj\tmpfu.hex .\obj\tmp16.bin >> build_msg.txt

..\..\bin\nchksum.exe .\obj\tmp16.bin
copy /y .\obj\tmp16.bin .\exe\NETUART.bin >> build_msg.txt

rem .\obj\NETUART.H0* >> build_msg.txt
del .\obj\tmp*.* >> build_msg.txt
rem del .\obj\fu.hex >> build_msg.txt

if errorlevel==1 goto OUT
EXIT
:OUT
 ..\..\bin\er.exe

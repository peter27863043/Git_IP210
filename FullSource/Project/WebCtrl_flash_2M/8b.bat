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
copy /y .\obj\NETUART.H16 .\obj\tmp16.hex >> build_msg.txt
copy /y .\obj\NETUART.H17 .\obj\tmp17.hex >> build_msg.txt
copy /y .\obj\NETUART.H18 .\obj\tmp18.hex >> build_msg.txt
copy /y .\obj\NETUART.H19 .\obj\tmp19.hex >> build_msg.txt
copy /y .\obj\NETUART.H20 .\obj\tmp20.hex >> build_msg.txt
copy /y .\obj\NETUART.H21 .\obj\tmp21.hex >> build_msg.txt
copy /y .\obj\NETUART.H22 .\obj\tmp22.hex >> build_msg.txt
copy /y .\obj\NETUART.H23 .\obj\tmp23.hex >> build_msg.txt
copy /y .\obj\NETUART.H24 .\obj\tmp24.hex >> build_msg.txt
copy /y .\obj\NETUART.H25 .\obj\tmp25.hex >> build_msg.txt
copy /y .\obj\NETUART.H26 .\obj\tmp26.hex >> build_msg.txt
copy /y .\obj\NETUART.H27 .\obj\tmp27.hex >> build_msg.txt
copy /y .\obj\NETUART.H28 .\obj\tmp28.hex >> build_msg.txt
copy /y .\obj\NETUART.H29 .\obj\tmp29.hex >> build_msg.txt
copy /y .\obj\NETUART.H30 .\obj\tmp30.hex >> build_msg.txt
copy /y .\obj\NETUART.H31 .\obj\tmp31.hex >> build_msg.txt


..\..\bin\hex2bin.exe /Q /L2097152    .\obj\tmp00.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O65536  /M .\obj\tmp01.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O131072 /M .\obj\tmp02.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O196608 /M .\obj\tmp03.hex .\obj\tmp32.bin  >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O262144 /M .\obj\tmp04.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O327680 /M .\obj\tmp05.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O393216 /M .\obj\tmp06.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O458752 /M .\obj\tmp07.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O524288 /M .\obj\tmp08.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O589824 /M .\obj\tmp09.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O655360 /M .\obj\tmp10.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O720896 /M .\obj\tmp11.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O786432 /M .\obj\tmp12.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O851968 /M .\obj\tmp13.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O917504 /M .\obj\tmp14.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O983040 /M .\obj\tmp15.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1048576 /M .\obj\tmp16.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1114112 /M .\obj\tmp17.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1179648 /M .\obj\tmp18.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1245184 /M .\obj\tmp19.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1310720 /M .\obj\tmp20.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1376256 /M .\obj\tmp21.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1441792 /M .\obj\tmp22.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1507328 /M .\obj\tmp23.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1572864 /M .\obj\tmp24.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1638400 /M .\obj\tmp25.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1703936 /M .\obj\tmp26.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1769472 /M .\obj\tmp27.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1835008 /M .\obj\tmp28.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1900544 /M .\obj\tmp29.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O1966080 /M .\obj\tmp30.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O2031616 /M .\obj\tmp31.hex .\obj\tmp32.bin >> build_msg.txt
..\..\bin\hex2bin.exe /Q /O262144 /M .\obj\fuu.hex   .\obj\tmp32.bin >> build_msg.txt

rem ..\..\bin\nchksum.exe .\obj\tmp32.bin
rem copy /y .\obj\tmp32.bin .\exe\NETUARTAPP.bin >> build_msg.txt

..\..\bin\hex2bin.exe /Q /O0000 /M .\obj\tmpfu.hex .\obj\tmp32.bin >> build_msg.txt

..\..\bin\nchksum.exe .\obj\tmp32.bin
copy /y .\obj\tmp32.bin .\exe\NETUART.bin >> build_msg.txt

rem .\obj\NETUART.H0* >> build_msg.txt
del .\obj\tmp*.* >> build_msg.txt
rem del .\obj\fu.hex >> build_msg.txt

if errorlevel==1 goto OUT
EXIT
:OUT
 ..\..\bin\er.exe

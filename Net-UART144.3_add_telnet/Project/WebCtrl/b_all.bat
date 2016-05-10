del /Q exe\OTP_64k.bin

del /Q obj\*.*
echo //ModuleList >..\..\src_code\include\image_mode.h
echo #define DIS_CHALLENGE >>..\..\src_code\include\image_mode.h
echo #define MODULE_DHCPC >>..\..\src_code\include\image_mode.h
rem echo #define MODULE_DNS >>..\..\src_code\include\image_mode.h
rem echo #define MODULE_FULL_MSG >>..\..\src_code\include\image_mode.h
echo #define MODULE_NET_CMD >>..\..\src_code\include\image_mode.h
echo #define MODULE_NET_UART >>..\..\src_code\include\image_mode.h
echo #define MODULE_SMALL_UDP >>..\..\src_code\include\image_mode.h
rem echo #define MODULE_NET_UART_INT >>..\..\src_code\include\image_mode.h

echo #define MODULE_RS485 >>..\..\src_code\include\image_mode.h
echo #define ET0_INTERRUPT >>..\..\src_code\include\image_mode.h
echo #define MODULE_UDP_TELNET >>..\..\src_code\include\image_mode.h
rem echo #define MODULE_ADC >>..\..\src_code\include\image_mode.h
echo #define MODULE_MUIP >>..\..\src_code\include\image_mode.h
rem echo #define MODULE_DIDO >>..\..\src_code\include\image_mode.h
echo #define MODULE_UDP_NET_CMD >>..\..\src_code\include\image_mode.h
echo #define MODULE_UART_AUTO_SEND>>..\..\src_code\include\image_mode.h

rem echo #define MODULE_NET_UART_CTRL >>..\..\src_code\include\image_mode.h

cd ..\..\src_code\HtmSrcPage
call adc_48pin.bat

cd ..\..\Project\WebCtrl
call ..\_mov_lib udp_tel_adc48_muip
c:\keil\uv2\uv2 %1 IP210WebCtrl.Uv2
cd exe
del /Q NETUART.BIN
ren NETUARTc.BIN OTP_64k.bin
cd ..


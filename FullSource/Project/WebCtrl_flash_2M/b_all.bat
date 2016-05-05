del /Q exe\*.*
del /Q obj\*.*
echo //ModuleList >..\..\src_code\include\image_mode.h
echo #define DIS_CHALLENGE >>..\..\src_code\include\image_mode.h
echo #define MODULE_NET_UART >>..\..\src_code\include\image_mode.h
echo #define ET0_INTERRUPT >>..\..\src_code\include\image_mode.h
rem echo #define MODULE_NET_UART_INT >>..\..\src_code\include\image_mode.h
echo #define MODULE_RS485 >>..\..\src_code\include\image_mode.h
echo #define MODULE_FLASH_485>>..\..\src_code\include\image_mode.h
echo #define MODULE_UDP_TELNET >>..\..\src_code\include\image_mode.h

echo #define MODULE_NET_CMD >>..\..\src_code\include\image_mode.h
echo #define MODULE_SMALL_UDP >>..\..\src_code\include\image_mode.h
echo #define MODULE_ADC >>..\..\src_code\include\image_mode.h
rem echo #define MODULE_DIDO >>..\..\src_code\include\image_mode.h
echo #define MODULE_SIGNAL_GPIO >>..\..\src_code\include\image_mode.h
echo #define MODULE_TELNET_RFC >>..\..\src_code\include\image_mode.h
echo #define MODULE_UDP_NET_CMD >>..\..\src_code\include\image_mode.h
echo #define MODULE_FIRMWARE >>..\..\src_code\include\image_mode.h

echo #define MODULE_DNS >>..\..\src_code\include\image_mode.h
echo #define MODULE_SMTP >>..\..\src_code\include\image_mode.h

echo #define MODULE_DHCPC >>..\..\src_code\include\image_mode.h
echo #define MODULE_FULL_MSG >>..\..\src_code\include\image_mode.h
echo #define MODULE_COMMAND_LINE >>..\..\src_code\include\image_mode.h
echo #define IC_PACKAGE_128_PIN >>..\..\src_code\include\image_mode.h

echo #define MODULE_MUIP >>..\..\src_code\include\image_mode.h
echo #define MODULE_NET_UART_CTRL >>..\..\src_code\include\image_mode.h
echo #define MODULE_REVERSE_TELNET >>..\..\src_code\include\image_mode.h
echo #define MODULE_FLASH512 >>..\..\src_code\include\image_mode.h
 
echo #define MULTI_PACKET >>..\..\src_code\include\image_mode.h
echo #define MODULE_CLI_UDP>>..\..\src_code\include\image_mode.h

rem echo #define SW_UART_AUTO_LOOBACK>>..\..\src_code\include\image_mode.h
echo #define MODULE_UART_AUTO_SEND>>..\..\src_code\include\image_mode.h

rem echo #define MODULE_TCP_DIDO>>..\..\src_code\include\image_mode.h

echo #define MODULE_VLAN_TAG>>..\..\src_code\include\image_mode.h

rem echo #define DB_FIFO>>..\..\src_code\include\image_mode.h

echo //ModuleList >..\..\src_code\include\image_module.inc
echo $SET (A51_D_FLASH_512) >>..\..\src_code\include\image_module.inc
echo $SET (RS485) >>..\..\src_code\include\image_module.inc

cd ..\..\src_code\HtmSrcPage
call flash.bat
cd ..\..\src_code_flash\HtmSrcPage
call Cod2c.bat
cd ..\..\Project\WebCtrl_flash_2M
call ..\_mov_lib flash
c:\keil\uv2\uv2 %1 IP210WebCtrl.Uv2
cd exe
ren NETUART.BIN flash_2M.bin
rem ren NETUARTAPP.BIN nu_flashapp.bin
cd ..

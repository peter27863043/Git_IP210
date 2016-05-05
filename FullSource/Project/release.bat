del /Q ..\exe\*.*

cd firmwareUpdate
call build.bat
cd ..

cd FUU
call build.bat
cd ..

cd WebCtrl
call build.bat
copy exe\*.* ..\..\exe
cd ..

cd WebCtrl_flash
call build.bat
copy exe\*.* ..\..\exe
cd ..

cd WebCtrl_flash_1M
call build.bat
copy exe\*.* ..\..\exe
cd ..

cd WebCtrl_flash_2M
call build.bat
copy exe\*.* ..\..\exe
cd ..
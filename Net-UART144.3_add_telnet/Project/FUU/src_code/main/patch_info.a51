size_EEchip_id EQU 2
size_EEapp_id  EQU 2
EE_LENGTH equ 256
EXTRN CODE (EE_Default)
     CSEG  AT  0FFE0H
_patch_info:
     DB    0H         ;BANK NUMBER of Main Application Default EE Sturcture (1byte),
     DW    EE_Default ;ADDRESS of Main Application Default EE Structure(2bytes), 
     DW    EE_LENGTH  ;LENGTH of Main Application Default EE Structure(2bytes), 
     DW    EE_Default+size_EEchip_id+size_EEapp_id ;ADDRESS of NetIF Struture of Main Application Default EE Structure(2byte), 
     DW    0H         ;ADDRESS of NetIF Struture of FirmwareUpdate Default EE Structure(2byte) 
     DB    0ffh,0ffh,0ffh,0ffh,0ffh,0ffh,0ffh ;reserve
     ;reserve for HMerger
     DB    0ffH,0ffH,0ffH,0ffH,0ffH,0ffH,0ffH,0ffH,0ffH,0ffH,0ffH
     END

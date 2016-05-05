/******************************************************************************
*
*   Name:           eedefault.c
*
*   Description:    EEPROM default value
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/
#include "type_def.h"
#include "eepdef.h"

uEE_Info code EE_Default=
{                          //0x82 app_id
 0x10,0xf0,0x13,0xf0,0x18,0x26,           //0x84 MACID
 192,168,2,1,                             //0x8A IP Address
 255,255,255,0,                           //0x8E Sub Mask
 192,168,2,254,                           //0x92 Gateway
 168,95,192,1,                            //0x96 DNS
 0x00,		                              //dhcp enable=0x1,disable=0x0
 'a','d','m','i','n',0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, //0x9A login name     Valid Length=15
 's','y','s','t','e','m',0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,  //0xAA login password Valid Length=15
 's','w','i','t','c','h',0x0,0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,   //comment        Valid Length=15
};


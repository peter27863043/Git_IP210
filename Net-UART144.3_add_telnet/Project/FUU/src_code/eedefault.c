#include "eepdef.h"
uFW_EE_Info code EE_Default=
{
 // Netif
 0x50,0x80,0x17,0x80,0x13,0xf0,           //0x84 MACID
 192,168,2,1,                             //0x8A IP Address
 255,255,255,0,                           //0x8E Sub Mask
 192,168,2,254,                           //0x92 Gateway
 168,95,192,1,                            //0x96 DNS
 0x00,		                              //dhcp enable=0x1,disable=0x0
 "admin",                                 //user name
 "system",                                //password
 "Net Uart"                               //comment
};
//eeprom default data in CODE SEGMENT
#ifdef CUSTOMER_EEPROM
#include "..\..\src_code_customer\device\main\ee_default.c"
#else
#include "eepdef.h"
//----------------------------------------------------------------------------
// Do not touch this EE_RESERVED_0 area, or something unexpected would wrongly happen
//----------------------------------------------------------------------------
#define EE_RESERVED_0 \
0x00, 0x01,\
0x00, 0x00,\
0x00, 0x01,\
0x00, 0x00\
//----------------------------------------------------------------------------

uEE_Info code EE_Default =
{
    EE_RESERVED_0,//EE_RESERVED_0 area must be placed at the offset 0 of this structure.
	              //Or someting unexpected would wrongly happen.
	0x00,0x00,0x11,0x33,0xff,0x00,		  //0x4- MACID
	//192,168,2,1,						  //0xA- IPAddress
	169,254,1,1,						  //0xA- IPAddress
	255,255,0,0,
	169,254,1,1,						 //gateway	
	169,254,1,1,						//dns
	0x0,
	'a','d','m','i','n',0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	's','y','s','t','e','m',0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	'N','e','t','U','A','R','T', 0x0, 0x0, 0x0, 0x0, 0x0,
	"",                        //reserve
	DEFAULT_UARTINFO,
	DEFAULT_TELNETINFO,
	DEFAULT_NETCMDINFO,
	DEFAULT_UDP_TELNET,
	DEFAULT_DIDO,
	DEFAULT_SMTPINFO,
	DEFAULT_VLANINFO,
};
#endif //CUSTOMER_EEPROM
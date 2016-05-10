/******************************************************************************
*
*   Name:           eeprominit
*
*   Description:    EEPROM intialization Header
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/

#ifndef _EEPROMINIT_H
#define _EEPROMINIT_H

// data offset in EEPROM
#define EE_MAGIC_CODE		0x40 /* EE_MAGIC_CODE, Valid EEPROM               */
#define EE_USER				0xe0 /* Password 10 bytes,     */
#define EE_USERNAME			0xe0/* Username 6  bytes,     */
#define EE_PASSWORD			0xf0 /* Password 10 bytes,     */
#define EE_COMMENT			0xb0 /* Comment 12 bytes,     */
#define EE_VLANGROUP        0xc0 /* VLAN GROUP 4 bytes */
#define EE_VLANMTO1         0xd0 /* VLAN Multi to 1 1 byte*/
#define EE_MAC12			0x44 /* MAC1(_00), MAC2(_0B)                      */
#define EE_MAC34			0x45 /* MAC3(_78), MAC4(66)                       */
#define EE_MAC56			0x46 /* MAC5(77) , MAC5(88)                       */
#define EE_IP				0x8e /* IP1(_C0) , IP2(_A8)                       */
#define EE_IP12				0x47 /* IP1(_C0) , IP2(_A8)                       */
#define EE_IP34				0x48 /* IP1(_00) , IP2(_64)                       */
#define EE_NETMASK12		0x49 /* NETMASK1 , NETMASK2						 */
#define EE_NETMASK34		0x4a /* NETMASK3 , NETMASK4						 */
#define EE_GATEWAY12		0x4b /* GATEWAY1 , GATEWAY2						 */
#define EE_GATEWAY34		0x4c /* GATEWAY3 , GATEWAY4						 */
#define EE_DNS12        	0x4d /* DNS1, DNS2						 */
#define EE_DNS34        	0x4e /* DNS3, DNS4						 */
#define EE_DHCPEN			0x4f /* System Configuration-DHCP Enable/Disable */
#define EE_DATARATE         0x74 /* Data rate address*/

#define EE_RS232CMD1		0x90 				/* RS232 Command 1 			 */
#define EE_RS232CMD2		0x91+CMD_MAX_SIZE	/* RS232 Command 2 			 */

#define	EE_PORT_SETTING		0xa0
#define	EE_MODE				0xaf
//#define EE_EMAIL_RECEIVER	0x9e
//#define EE_EMAIL_SERVER     0x11e+MaxEmailReceiverLen
//#define EE_EMAIL_BODYTEXT   0x100+EE_EMAIL_SERVER+MaxEmailServerLen
//#define EE_EMAIL_SERVER     0xd7
//#define EE_EMAIL_BODYTEXT   0xe0

//define username and password length
#define USER_LENGTH			16
#define PASS_LENGTH			16
#define COMMENT_LEN			12
#define DATARATE_LEN        8
// 24c16 Max: 256 Block (256 x 8 bits)
#define EE_TEST				0xde		/* Write to eeprom space, for test which device it is (93lc66, 24c16, or none!) */

/*-----------------------------------------------------------------------------
The Content of user-defined 93C66 EEPROM
-----------------------------------------------------------------------------*/
#define EE_MAGIC_CODE_DATA    0x1781

#define   EEPROM24C16           0x01

//Sturcture
/*struct _EEConfigInfo{
    unsigned char MACID[6];//  0x88-0x8d
    unsigned char IPAddress[4];//  0x8E-0x91
    unsigned char SubnetMask[4];//  0x92-0x95
    unsigned char Gateway[4];//  0x96-0x99
    unsigned char DNSAddress[4];//  0x9a-0x9d
    unsigned char LoginName[16];//  0xe0-0xef
    unsigned char LoginPassward[16];//  0xf0-0xff
    unsigned char Comment[12];
    unsigned char EepromDataValid;
};*/

//variables
extern struct _EEConfigInfo EEConfigInfo;
extern code struct _EEConfigInfo EEConfigInfoDefault;
extern unsigned char EE_result;

// Prototype functions
void EEPROMInit(void);



#endif
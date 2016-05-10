/******************************************************************************
*
*   Name:           switch.h
*
*   Description:    switch Header
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
#ifndef _SWITCH_H_
#define _SWITCH_H_


#define MAX_PORT_NUM						8
#define	FIBER_PORT							7
#define	VLAN_GROUP							4

#define SWITCH_EEPROM_SIZE					0x80

#define	MODE_NORMAL							0x01
#define	MODE_MII							0x02


#define INT									int
#define UINT								unsigned int
#define CHAR								char
#define UCHAR								unsigned char

//PHY address
#define		IP178_PORT1					0
#define		IP178_PORT2					1
#define		IP178_PORT3					2
#define		IP178_PORT4					3
#define		IP178_PORT5					4
#define		IP178_PORT6					5
#define		IP178_PORT7					6
#define		IP178_PORT8					7

#define		PHY_30						30
#define		PHY_31						31

#define 	PHY_1F						0x1F

//MII of PHY 0~7
#define		MII_CONTROL					0
#define		MII_STATUS					1
#define		MII_ID1						2
#define		MII_ID2						3
#define		MII_AUTO_N					4
#define		MII_LINK_PART				5
#define		MII_LINK_STATUS				18

//MII of PHY 30
#define		MII_EEPROM_EN				0
#define		MII_SW_CTRL1				1
#define		MII_SW_CTRL2				2
#define		MII_SW_CTRL2				2
#define		MII_TAG_REG					3
#define		MII_TAG_REG1				3
#define		MII_TAG_REG2				4
#define		MII_TAG_REG3				5
#define		MII_TAG_REG4				6
#define		MII_TAG_REG5				7
#define		MII_TAG_REG6				8
#define		MII_TAG_REG7				9
#define		MII_TAG_REG8				10
#define		MII_TAG_REG9				11
#define		MII_TAG_REG10				12
#define		MII_TAG_REG11				13
#define		MII_TAG_VLAN1				13
#define		MII_TAG_VLAN2				14
#define		MII_TVLANMASK				14
#define		MII_TVLANMASK1				15
#define		MII_TVLANMASK2				16
#define		MII_TVLANMASK3				17
#define		MII_TVLANMASK4				18
#define		MII_TVLANMASK5				19
#define		MII_TVLANMASK6				20
#define		MII_TVLANMASK7				21
#define		MII_TVLANMASK8				22
#define		MII_TVLANMASK9				23
#define		MII_TVLANMASKA				24
#define		MII_TVLANMASKB				25
#define		MII_TVLANMASKC				26
#define		MII_TVLANMASKD				27
#define		MII_TVLANMASKE				28
#define		MII_TVLANMASKF				29
#define		MII_ROUTE_CTRL1				30
#define		MII_ROUTE_CTRL2				31

//MII of PHY 31
#define		MII_POSTVLAN				0
#define		MII_COSnPVLAN0				0
#define		MII_COSnPVLAN1				1
#define		MII_COSnPVLAN2				2
#define		MII_COSnPVLAN3				3
#define		MII_COSnPVLAN4				4
#define		MII_COSnPVLAN5				5
#define		MII_COSnPVLAN6				6
#define		MII_COSnPVLAN7				7
#define		MII_COSnPVLAN8				8
#define		MII_STP						13
#define		MII_STPDF1					14
#define		MII_STPDF1_MAC12			14
#define		MII_STPDF1_MAC34			15
#define		MII_STPDF1_MAC56			16
#define		MII_STPDF1_PORT				17
#define		MII_STPDF2					18
#define		MII_STPDF2_MAC12			18
#define		MII_STPDF2_MAC34			19
#define		MII_STPDF2_MAC56			20
#define		MII_STPDF2_PORT				21
#define		MII_DSCP					22
#define		MII_DATARATE				26
#define		MII_3130					30

#endif

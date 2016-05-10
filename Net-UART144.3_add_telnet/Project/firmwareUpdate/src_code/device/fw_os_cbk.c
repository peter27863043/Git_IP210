/******************************************************************************
*
*   Name:           fw_os_cbk.c
*
*   Description:    Firmware Update Project:os call back routine for different devices
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Grace
*
*******************************************************************************/
#include "type_def.h"
#include "IP210reg.h"

//for Firmware Update project only
void os_cbk_fu_switch_init(void)
{
	/*****************2008-07-18 Thanks for DeNa*****************************/
IP210RegWrite(REG_CHIP_CONFIGURE_0, (IP210RegRead(REG_CHIP_CONFIGURE_0)| 0x8) ); // PHY Mode
IP210RegWrite(REG_CHIP_CONFIGURE_0, (IP210RegRead(REG_CHIP_CONFIGURE_0)&~0x4) ); // PHY Mode
/**************************************************************************/
}
u32_t os_cbk_arp_timeout()
{
  return 3000; //300 seconds
  //need to add to kernelsettings???
}

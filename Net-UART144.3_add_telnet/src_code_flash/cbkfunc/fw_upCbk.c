#include "type_def.h"
#include "httpd.h"	     
#include "..\..\src_code\main\eepdef.h"
#include "..\..\src_code\sys\eeprom.h"
#include "uip.h"
#include <stdio.h>
#ifdef MODULE_FIRMWARE
/******************************************************************************
*
*  Function:    cgi_fw_up
*
*  Description: Handle restore EEPROM default value.
*
*  Parameters:  cnt: number of the cgi data, vp: CGI name and value.
*
*  Returns:     If run successfully return OK. Otherwise, return Fail.
*
*******************************************************************************/
extern u8_t firm_flag;
extern u8_t boot_flag;
//void CPU_GOTO_FFF7H(void);
u8_t Setup_BLUpgrade(u8_t *);
u8_t cgi_fw_up(u8_t cnt, req_data *vp)
{
	u8_t i=cnt;
    req_data *y=vp;
    for(i=0;i<4;i++)
	{
		EEPROM_Write_Byte(loc_EEIPAddress+i,((u8_t*)uip_hostaddr)[i]);
	}
    for(i=0;i<4;i++)
	{
		EEPROM_Write_Byte(loc_EESubnetMask+i,((u8_t*)uip_arp_netmask)[i]);
	}
    for(i=0;i<4;i++)
	{
		EEPROM_Write_Byte(loc_EEGateway+i,((u8_t*)uip_arp_draddr)[i]);
	}
    for(i=0;i<4;i++)
	{
		EEPROM_Write_Byte(loc_EEDNSAddress+i,((u8_t*)uip_dnsaddr)[i]);
	}
	firm_flag=1;
	httpd_init_file(fs_open("fw_up_wait.htm",&hs->op_index));
    //CPU_GOTO_FFF7H();
	return OK;
}
u8_t Setup_BLUpgrade(u8_t *uip_hostaddr);
//void FUU_Run(void);
u8_t cgi_fu_up(u8_t cnt, req_data *vp)
{
	u8_t i=cnt;
    req_data *y=vp;
    for(i=0;i<4;i++)
	{
		EEPROM_Write_Byte(loc_EEIPAddress+i,((u8_t*)uip_hostaddr)[i]);
	}
    for(i=0;i<4;i++)
	{
		EEPROM_Write_Byte(loc_EESubnetMask+i,((u8_t*)uip_arp_netmask)[i]);
	}
    for(i=0;i<4;i++)
	{
		EEPROM_Write_Byte(loc_EEGateway+i,((u8_t*)uip_arp_draddr)[i]);
	}
    for(i=0;i<4;i++)
	{
		EEPROM_Write_Byte(loc_EEDNSAddress+i,((u8_t*)uip_dnsaddr)[i]);
	}
	//Setup_BLUpgrade((u8_t *)uip_hostaddr);
//  FUU_Run();
	boot_flag=1;
	httpd_init_file(fs_open("fw_up_wait.htm",&hs->op_index));

	return OK;
}
#endif

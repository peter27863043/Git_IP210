/******************************************************************************
*
*   Name:           eeprom.c
*
*   Description:    Driver functions for EEPROM access operations
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/
#include "public.h"
#include "kernelsetting.h"
/******************************************************************************
*
*  Function:    EEPROM_Enable
*
*  Description: to enable EEPROM to be accessed
*               
*  Parameters: none
*               
*  Returns: none
*               
*******************************************************************************/
void EEPROM_Enable(void)
{
  unsigned char byte_data;

    byte_data = IP210RegRead(REG_CHIP_CONFIGURE_1);
#if EEPROM_P3_6_7==1
    IP210RegWrite(REG_CHIP_CONFIGURE_1, byte_data|0x10);
#else
    IP210RegWrite(REG_CHIP_CONFIGURE_1, byte_data|0x08);
#endif
 
}


/******************************************************************************
*
*  Function:    EEPROM_Disable
*
*  Description: to not allowed EEPROM to be accessed
*               
*  Parameters: none
*               
*  Returns: none
*               
*******************************************************************************/
void EEPROM_Disable(void)
{
  	u8_t byte_data;

    byte_data = IP210RegRead(REG_CHIP_CONFIGURE_1);
	byte_data = byte_data&~0x18;
    IP210RegWrite(REG_CHIP_CONFIGURE_1, byte_data);
}
/******************************************************************************
*
*  Function:    EEPROM_Read_Byte
*
*  Description: to read a byte of data from EEPROM
*               
*  Parameters: 
*    u16_t addr: the offset of EEPROM data to be read
*    u8_t *read_data: the pointer of unsinged int to put the read data
*               
*  Returns: 
*  1: succeeded
*  0: failed
*               
*******************************************************************************/
u8_t EEPROM_Read_Byte(u16_t addr, u8_t *read_data)
{
	u8_t EE_addr, EE_id;
	u8_t byte_data;
	u16_t ii;
    u8_t i;
	
    EE_addr = (u8_t)(addr&0xff);
    EE_id = (u8_t)((addr>>8)&0x7);

    IP210RegWrite(REG_EEPROM_ADDR, EE_addr);
    IP210RegWrite(REG_EEPROM_ID, EE_id);
  
    
    for(i=0;i<200;i++)    
    {
        IP210RegWrite(REG_EEPROM_CMD, EE_CMD_OP_READ);

	    ii=0;
	    do{
	    	byte_data = IP210RegRead(REG_EEPROM_CMD);
	    	if(byte_data & (EE_CMD_ABORT|EE_CMD_NEXT) )
			{
	    	    break;
			}
	    	ii++;
	    }while(ii<10000);
	    if(byte_data & EE_CMD_NEXT)
	    {
	    		*read_data = IP210RegRead(REG_EEPROM_DATA);
	    		return 1;
	    }
    }
    return 0;
}

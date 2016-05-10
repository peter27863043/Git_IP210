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
//include data
#include "public.h"
#include "eeprom.h"



//functions
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
/*void EEPROM_Disable(void)
{
  unsigned char byte_data;

    byte_data = IP210RegRead(REG_CHIP_CONFIGURE_1);
#if EEPROM_P3_6_7==1
    byte_data &= ~0x10;
#else
    byte_data &= ~0x08;
#endif
    IP210RegWrite(REG_CHIP_CONFIGURE_1, byte_data);

}*/


/******************************************************************************
*
*  Function:    EEPROM_Read_Byte
*
*  Description: to read a byte of data from EEPROM
*               
*  Parameters: 
*    unsigned short addr: the offset of EEPROM data to be read
*    unsigned char *read_data: the pointer of unsinged int to put the read data
*               
*  Returns: 
*  1: succeeded
*  0: failed
*               
*******************************************************************************/
unsigned char EEPROM_Read_Byte(unsigned short addr, unsigned char *read_data)
{
	unsigned char EE_addr, EE_id;
	unsigned char byte_data;
	unsigned short ii;
    unsigned char i;
	
    EE_addr = (unsigned char)(addr&0xff);
    EE_id = (unsigned char)((addr>>8)&0x7);

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
		//TRACE_EEPROM(("\nEEPROM_Read_Byte(byte_data=%x)", (unsigned short)byte_data));//debug
		    	    break;
				}
		    	ii++;
		    }while(ii<10000);
		//TRACE_EEPROM(("\nEEPROM_Read_Byte(ii=%x)", (unsigned short)ii));//debug
		
		    if(byte_data & EE_CMD_NEXT)
		    {
		    		*read_data = IP210RegRead(REG_EEPROM_DATA);
		    		return 1;
		    }
//		    TRACE_EEPROM(("\nEEPROM_Read_Byte(byte_data=%x)", (unsigned short)byte_data));//debug

    }

    return 0;
}


/******************************************************************************
*
*  Function:    EEPROM_Write_Byte
*
*  Description: to write a byte of data to EEPROM
*               
*  Parameters: 
*    unsigned short addr: the offset of EEPROM data to be written
*    unsigned char write_data: data to be written to EERPOM
*               
*  Returns: 
*  1: succeeded
*  0: failed
*               
*******************************************************************************/
unsigned char EEPROM_Write_Byte(unsigned short addr, unsigned char write_data)
{
  unsigned char EE_addr, EE_id;	
  unsigned char byte_data;
  unsigned short ii;
  unsigned char i;
  
    EE_addr = (unsigned char)(addr&0xff);
    EE_id = (unsigned char)((addr>>8)&0x7);

    IP210RegWrite(REG_EEPROM_DATA, write_data);
    IP210RegWrite(REG_EEPROM_ADDR, EE_addr);
    IP210RegWrite(REG_EEPROM_ID, EE_id);

	

    for(i=0;i<200;i++)
    {
            IP210RegWrite(REG_EEPROM_CMD, EE_CMD_OP_WRITE); 

		    ii=0;
		    do{
		    	byte_data = IP210RegRead(REG_EEPROM_CMD);
		    	if(byte_data & (EE_CMD_ABORT|EE_CMD_NEXT) )
				{
		//TRACE_EEPROM(("\nEEPROM_Read_Byte(byte_data=%x)", (unsigned short)byte_data));//debug
		    	    break;
				}
		    	ii++;
		    }while(ii<10000);
		//TRACE_EEPROM(("\nEEPROM_Read_Byte(ii=%x)", (unsigned short)ii));//debug
		
		    if(byte_data & EE_CMD_NEXT)
		    {
		    		return 1;
		    }

//		    TRACE_EEPROM(("\nEEPROM_Write_Byte(byte_data=%x)(%x)", (unsigned short)byte_data, (unsigned short)byte_data));//debug
    }
    return 0;

}

#ifndef FIRMWARE_UPDATE
/******************************************************************************
*
*  Function:    EEPROM_Read_Multi
*
*  Description: to read a block of data from EEPROM
*               
*  Parameters: 
*    unsigned short addr: the offset of EEPROM data to be read
*    unsigned char *read_data: the begining pointer of char to put the read data
*    unsigned char length: the length of this block of data
*               
*  Returns: 
*  1: succeeded
*  0: failed
*               
*******************************************************************************/
/*unsigned char EEPROM_Read_Multi(unsigned short addr, unsigned char *read_data, unsigned char length)
{
  unsigned char EE_addr, EE_id;	
  unsigned char byte_data, n;
  unsigned short ii;
  unsigned char i;
	  
    EE_addr = (unsigned char)(addr&0xff);
    EE_id = (unsigned char)((addr>>8)&0x7);
    
    IP210RegWrite(REG_EEPROM_ADDR, EE_addr);
    IP210RegWrite(REG_EEPROM_ID, EE_id);
    

    
    for(i=0;i<200;i++)		
	{
        IP210RegWrite(REG_EEPROM_CMD, EE_CMD_OP_READ|((length-1)&EE_CMD_BYTE_COUNT));

		    n=0;
		    ii=0;
		    do{
		    	byte_data = IP210RegRead(REG_EEPROM_CMD);
		    	if(byte_data & EE_CMD_ABORT)
				  {
		    	    break;
				  }    	
		    	if(byte_data & EE_CMD_NEXT)
				  {
		    		read_data[n] = IP210RegRead(REG_EEPROM_DATA);
		    		n++;
				  }      
				  ii++; 	
		    }while(n<length&&ii<10000);     
		    
		    if(n==length)
			    return 1;
	}
	return 0;
}
*/

/******************************************************************************
*
*  Function:    EEPROM_Write_Multi
*
*  Description: to write a block of data to EEPROM
*               
*  Parameters: 
*    unsigned short addr: the offset of EEPROM data to be written
*    unsigned char *write_data: the begining pointer of char that stores the data to be written
*    unsigned char length: the length of this block of data
*               
*  Returns: 
*  1: succeeded
*  0: failed
*               
*******************************************************************************/
/*unsigned char EEPROM_Write_Multi(unsigned short addr, unsigned char *write_data, unsigned char length)
{
  unsigned char EE_addr, EE_id;	
  unsigned char byte_data, n;
  unsigned short ii;
  unsigned char i;
	  
    EE_addr = (unsigned char)(addr&0xff);
    EE_id = (unsigned char)((addr>>8)&0x7);
    
    IP210RegWrite(REG_EEPROM_DATA, write_data[0]);    
    IP210RegWrite(REG_EEPROM_ADDR, EE_addr);
    IP210RegWrite(REG_EEPROM_ID, EE_id);
    

    
    if(length==1)
        return 1;
    
    for(i=0;i<200;i++)
	{
        IP210RegWrite(REG_EEPROM_CMD, EE_CMD_OP_WRITE|((length-1)&EE_CMD_BYTE_COUNT));

		    n=1;
		    ii=0;
		    do{
		    	byte_data = IP210RegRead(REG_EEPROM_CMD);
		    	if(byte_data & EE_CMD_ABORT)
				  {
		    	    break;
				  }    	
		    	if(byte_data & EE_CMD_NEXT)
				  {
				  	if(n<length)
		                IP210RegWrite(REG_EEPROM_DATA, write_data[n]);		
		    		n++;
				  }      
				  ii++; 	
		    }while(n<=length&&ii<10000);     
		
		    if(n==length+1)
			    return 1;
	}
	return 0;
}
*/
#endif//FIRMWARE_UPDATE

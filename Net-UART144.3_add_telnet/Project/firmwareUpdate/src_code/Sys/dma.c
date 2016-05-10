/******************************************************************************
*
*   Name:           dma.c
*
*   Description:    Driver functions for DMA operations
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/
#include "public.h"
#include "dma.h"
/******************************************************************************
*
*  Function:    IP210_DMA_Set_Idle
*
*  Description: set IP210 into idle mode so that it can perform Flash2Memory DMA
*               
*  Parameters:  none
*               
*  Returns: none
*               
*******************************************************************************/
void IP210_DMA_Set_Idle(void)
{
	u8_t temp_ier;

    TRACE_DMA(("\nIP210_DMA_Set_Idle~"));
    DBYTE[SFR_IE_TEMP] = IE;
    temp_ier = IP210RegRead(REG_STATUS);
    temp_ier = IP210RegRead(REG_INTERRUPT_ENABLE);
    IP210RegWrite(REG_INTERRUPT_ENABLE, temp_ier | BIT_DMA_ACCESS_DONE_ENABLE);
	PDCON |= PCON_JWP;
    EX0=1;
    EA=1;
    PCON |= PCON_IDL;//put IP210 into idle mode action
}
/******************************************************************************
*
*  Function:    IP210_DMA_Idle_Restore
*
*  Description: handling SFR & IO registers' settings while restoring from Idle
*               
*  Parameters:  none
*               
*  Returns: none
*               
*******************************************************************************/
void IP210_DMA_Idle_Restore(void)
{
	u8_t temp_ier;

    IE = DBYTE[SFR_IE_TEMP];//restore IE value
    IP210RegRead(REG_STATUS);
    temp_ier = IP210RegRead(REG_INTERRUPT_ENABLE);
    IP210RegWrite(REG_INTERRUPT_ENABLE, temp_ier & ~BIT_DMA_ACCESS_DONE_ENABLE);
	PDCON &= ~PCON_JWP;
}

/******************************************************************************
*
*  Function:    Fill_DMA_Setting
*
*  Description: fill informations into DMA IO registers for DMA operation
*               
*  Parameters:  
*    u16_t src_addr: source address where DMA operation moves data from
*    u16_t des_addr: destination address where DMA operation moves data to
*    u16_t length: length of data to be moved
*               
*  Returns: none
*               
*******************************************************************************/
void Fill_DMA_Setting(u16_t src_addr, u16_t des_addr, u16_t length)
{
    TRACE_DMA(("\nFill_DMA_Setting(0x%X, 0x%X, 0x%X)~", src_addr, des_addr, length));
    IP210RegWrite(REG_DMA_SOURCE_ADDRESS_H, (src_addr>>8) & 0xff );
    IP210RegWrite(REG_DMA_SOURCE_ADDRESS_L, src_addr & 0xff);

    IP210RegWrite(REG_DMA_DESTINATION_ADDRESS_H, (des_addr>>8) & 0xff);
    IP210RegWrite(REG_DMA_DESTINATION_ADDRESS_L, des_addr & 0xff);

    IP210RegWrite(REG_DMA_LENGTH_H, (length>>8) & 0xff);
    IP210RegWrite(REG_DMA_LENGTH_L, length & 0xff);
}

/******************************************************************************
*
*  Function:    IP210_Wait_DMA_Complete
*
*  Description: waits for the competion of DMA operation
*               
*  Parameters:  none
*               
*  Returns: none
*               
*******************************************************************************/
void IP210_Wait_DMA_Complete(void)
{
	u8_t b;

    TRACE_DMA(("\n-->IP210_Wait_DMA_Complete()~"));
    do{
	    b = IP210RegRead(REG_DMA_COMMAND);
	}while((b&0x03)!=0);
    TRACE_DMA(("\n<--IP210_Wait_DMA_Complete()~"));
}

/******************************************************************************
*
*  Function:    IP210_DMA
*
*  Description: do DMA operation
*               
*  Parameters:  
*    u16_t src_addr: source address where DMA operation moves data from
*    u16_t des_addr: destination address where DMA operation moves data to
*    u16_t length: length of data to be moved*               
*  Returns: 
*  1: DMA succeeded
*  0: DMA failed
*******************************************************************************/
u8_t IP210_DMA(u16_t src_addr, u16_t des_addr, u16_t length, u8_t cmd)
{
  	u8_t i, loop_count, mode;

    TRACE_DMA(("\n->IP210_DMA(%x,%x,%x,%x)~", src_addr, des_addr, length, (u16_t)cmd));

    mode = cmd & 0x07;
    if(mode==BIT_CMD_MODE_CRC || mode==BIT_CMD_MODE_CHECKSUM)
    {
        TRACE_DMA(("\n->IP210_DMA(fail)~"));
	    return 0;
    }
    loop_count = (u8_t)(length/DMA_DATA_MAXIMUM) + 1;
    for(i=0;i<loop_count;i++)
    {
        if(length==0)
	        break;
        if(i==loop_count-1)
            Fill_DMA_Setting(src_addr, des_addr, length);
        else
            Fill_DMA_Setting(src_addr, des_addr, DMA_DATA_MAXIMUM);
        IP210RegWrite(REG_DMA_COMMAND, cmd);
		if(mode==BIT_CMD_MODE_F2M)
		{
    	    IP210_DMA_Set_Idle();
    	    TRACE_DMA(("\nBack from Idle_mode_Ex0_active~"));
		}
        IP210_Wait_DMA_Complete();
		if(mode==BIT_CMD_MODE_F2M)
            IP210_DMA_Idle_Restore();
        length-=DMA_DATA_MAXIMUM;
        src_addr+=DMA_DATA_MAXIMUM;
        des_addr+=DMA_DATA_MAXIMUM;
    }
    TRACE_DMA(("\n->IP210_DMA(ok)~"));
  	return 1;
}

/******************************************************************************
*
*  Function:    CRC_Calculation
*
*  Description: do CRC Calculation operation
*               
*  Parameters:
*    u16_t src_addr: the address of data to be calculated CRC
*    u16_t length: length of data to be calcultaed
*    u32_t *p_crc: the pointer of u32_t int to put CRC result
*               
*  Returns: 
*  1: DMA succeeded, crc result are put into the 4-byte varibale
*  0: DMA failed
*******************************************************************************/
u8_t CRC_Calculation(u16_t src_addr, u16_t length, u32_t *p_crc)
{
  	u8_t b, ii, *p;

    TRACE_DMA(("\n->CRC_Calculation(0x%x, 0x%x)~", src_addr, length));

    IP210RegWrite(REG_DMA_SOURCE_ADDRESS_H, (src_addr>>8) & 0xff );
    IP210RegWrite(REG_DMA_SOURCE_ADDRESS_L, src_addr & 0xff);

    IP210RegWrite(REG_DMA_LENGTH_H, (length>>8) & 0xff);
    IP210RegWrite(REG_DMA_LENGTH_L, length & 0xff);

    IP210RegWrite(REG_DMA_COMMAND, BIT_CMD_MODE_CRC);

    ii=0;
    do{
	  	b = IP210RegRead(REG_DMA_COMMAND);
      	ii++;
	}while((b&BIT_CMD_MODE)!=0 && ii<60000);
    if(ii>=60000)
	{
        TRACE_DMA(("\n<-CRC_Calculation(failed)~"));
        return 0;
	}
    p = (u8_t*)p_crc;
    for(ii=0;ii<4;ii++)
    {
        p[3-ii] = IP210RegRead(REG_CRC_RESULT_0+ii);
    }
  	TRACE_DMA(("\n<-CRC_Calculation()~"));
}

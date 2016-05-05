/******************************************************************************
*
*   Name:           dma.h
*
*   Description:    DMA operations Header
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By sorbica
*
*******************************************************************************/

#ifndef _DMA_H
#define _DMA_H

//DMA
//Constants
#define DMA_DATA_MAXIMUM          0x800//use with DMA function

//Functions
void IP210_Read_CRC_Result(void);
unsigned char IP210_DMA(unsigned short src_addr, unsigned short des_addr, unsigned short length, unsigned char cmd);
void Copy_Flash_XRAM(unsigned short src_addr, unsigned short des_addr);
void Copy_XRAM_XRAM(unsigned short src_addr, unsigned short des_addr);
unsigned char CRC_Calculation(unsigned short src_addr, unsigned short length, unsigned long *p_crc);

#endif
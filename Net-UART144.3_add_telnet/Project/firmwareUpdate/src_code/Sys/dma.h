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
u8_t IP210_DMA(u16_t src_addr, u16_t des_addr, u16_t length, u8_t cmd);
void Copy_Flash_XRAM(u16_t src_addr, u16_t des_addr);
void Copy_XRAM_XRAM(u16_t src_addr, u16_t des_addr);
u8_t CRC_Calculation(u16_t src_addr, u16_t length, u32_t *p_crc);

#endif
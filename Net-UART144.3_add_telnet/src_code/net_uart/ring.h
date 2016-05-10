/******************************************************************************
*
*   Name:           
*
*   Description:    
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
#ifndef RING_H
#define RING_H
typedef struct _SRingBuffer
{
	u8_t* buf;	
	u16_t TotalLen;//current data length
	u16_t MaxLen;//max avalible space
	u16_t ri;//Read Index
	u16_t wi;//Write Index
}SRingBuffer;

//distant between wi to button
#define RingWBDistance(RingBuffer)  (RingBuffer.MaxLen-RingBuffer.wi)
//Avallible write space
#define RingAvallibleSpace(RingBuffer)	 (RingBuffer.MaxLen-RingBuffer.TotalLen)
//distant between ri to button
#define RingRBDistance(RingBuffer)  (RingBuffer.MaxLen-RingBuffer.ri)
//check wi is over boundery or not
#define RingCheckWi(RingBuffer) if(RingBuffer.wi>=RingBuffer.MaxLen)RingBuffer.wi-=RingBuffer.MaxLen;
//check ri is over boundery or not
#define RingCheckRi(RingBuffer) if(RingBuffer.ri>=RingBuffer.MaxLen)RingBuffer.ri-=RingBuffer.MaxLen;
//Ring initial function
#define RingInit(RingBuffer,buff,max_len){RingBuffer.ri=0;RingBuffer.wi=0;RingBuffer.TotalLen=0;RingBuffer.buf=buff;RingBuffer.MaxLen=max_len;}
//Increase ri
#define RingIncRi(RingBuffer,len) {RingBuffer.ri+=len;RingCheckRi(RingBuffer);RingBuffer.TotalLen-=len;}
//Increase wi
#define RingIncWi(RingBuffer,len) {RingBuffer.wi+=len;RingCheckWi(RingBuffer);RingBuffer.TotalLen+=len;}
#endif //RING_H
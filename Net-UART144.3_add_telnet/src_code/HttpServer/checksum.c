#include "type_def.h"
#include "uip.h"
#include "muip.h"
//#include "checksum.h"
#ifdef MODULE_VLAN_TAG
/******************************************************************************
*
*   Name:           checksum.c
*
*   Description:    Software checksum function
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Chance
*
*******************************************************************************/
#define Checksum_BUF ((uip_tcpip_hdr *)&uip_buf_real[UIP_LLH_LEN])
#define Checksum_MBUF ((uip_tcpip_hdr *)&muip_buf[0][UIP_LLH_LEN+4])
u8_t muip_check_flag=0;
/*-----------------------------------------------------------------------------------*/
/******************************************************************************
*
*  Function:    uip_chksum
*
*  Description:  checksum function for protocol
*               
*  Parameters:  sdata : the start address of data which user want to caculate the checksum
*               len   : The length of the data        
*               
*  Returns:     checksum value
*               
*******************************************************************************/
u16_t uip_chksum(u16_t *sdata, u16_t len)
{
  u16_t acc;
  
  for(acc = 0; len > 1; len -= 2) {
    acc += *sdata;
    if(acc < *sdata) {
      /* Overflow, so we add the carry to acc (i.e., increase by
         one). */
      ++acc;
    }
    ++sdata;
  }

  /* add up any odd byte */
  if(len == 1) {
    acc += htons(((u16_t)(*(u8_t *)sdata)) << 8);
    if(acc < htons(((u16_t)(*(u8_t *)sdata)) << 8)) {
      ++acc;
    }
  }

  return acc;
}
/******************************************************************************
*
*  Function:    uip_ipchksum
*
*  Description:  checksum function for IP protocol
*               
*  Parameters:   None       
*               
*  Returns:     checksum value
*               
*******************************************************************************/
/*-----------------------------------------------------------------------------------*/
u16_t
uip_ipchksum(void)
{
	if(muip_check_flag)
	{ return uip_chksum((u16_t *)&muip_buf[0][UIP_LLH_LEN+4], 20); }
	else
	{ return uip_chksum((u16_t *)&uip_buf_real[UIP_LLH_LEN], 20); }
}
/*-----------------------------------------------------------------------------------*/
#define UIP_PROTO_UDP   17
/******************************************************************************
*
*  Function:    uip_udpchksum
*
*  Description:  checksum function for UDP protocol
*               
*  Parameters:   None       
*               
*  Returns:     checksum value
*               
*******************************************************************************/
u16_t
uip_udpchksum(void)
{
  u16_t hsum, sum;

  /* Compute the checksum of the UDP header. */
  hsum = uip_chksum((u16_t *)&uip_buf_real[20 + UIP_LLH_LEN], 8);

  /* Compute the checksum of the data in the TCP packet and add it to
     the TCP header checksum. */
  sum = uip_chksum((u16_t *)(&uip_buf_real[UIP_LLH_LEN+28]),//uip_appdata, //JC:change form uip_appdata to (&uip_buf[UIP_LLH_LEN+40])
		   (u16_t)(((((u16_t)(Checksum_BUF->len[0]) << 8) + Checksum_BUF->len[1]) - 28)));

  if((sum += hsum) < hsum) {
    ++sum;
  }
  
  if((sum += Checksum_BUF->srcipaddr[0]) < Checksum_BUF->srcipaddr[0]) {
    ++sum;
  }
  if((sum += Checksum_BUF->srcipaddr[1]) < Checksum_BUF->srcipaddr[1]) {
    ++sum;
  }
  if((sum += Checksum_BUF->destipaddr[0]) < Checksum_BUF->destipaddr[0]) {
    ++sum;
  }
  if((sum += Checksum_BUF->destipaddr[1]) < Checksum_BUF->destipaddr[1]) {
    ++sum;
  }
  if((sum += (u16_t)htons((u16_t)UIP_PROTO_UDP)) < (u16_t)htons((u16_t)UIP_PROTO_UDP)) {
    ++sum;
  }
  hsum = (u16_t)htons((((u16_t)(Checksum_BUF->len[0]) << 8) + Checksum_BUF->len[1]) - 20);

  if((sum += hsum) < hsum) {
    ++sum;
  }
  return sum;
}


u16_t
uip_tcpchksum(void)
{
  u16_t hsum, sum;

  /* Compute the checksum of the TCP header. */
  if(muip_check_flag)
  { hsum = uip_chksum((u16_t *)&muip_buf[0][20 + UIP_LLH_LEN+4], 20); }
  else
  { hsum = uip_chksum((u16_t *)&uip_buf_real[20 + UIP_LLH_LEN], 20); }

  /* Compute the checksum of the data in the TCP packet and add it to
     the TCP header checksum. */
  if(muip_check_flag)
  {
	sum = uip_chksum((u16_t *)(&muip_buf[0][UIP_LLH_LEN+40+4]),//uip_appdata, //JC:change form uip_appdata to (&uip_buf[UIP_LLH_LEN+40])
		   (u16_t)(((((u16_t)(Checksum_MBUF->len[0]) << 8) + Checksum_MBUF->len[1]) - 40)));
  }
  else
  {
	sum = uip_chksum((u16_t *)(&uip_buf_real[UIP_LLH_LEN+40]),//uip_appdata, //JC:change form uip_appdata to (&uip_buf[UIP_LLH_LEN+40])
		   (u16_t)(((((u16_t)(Checksum_BUF->len[0]) << 8) + Checksum_BUF->len[1]) - 40)));
  }

  if((sum += hsum) < hsum) {
    ++sum;
  }
  if(muip_check_flag)
  { 
	  if((sum += Checksum_MBUF->srcipaddr[0]) < Checksum_MBUF->srcipaddr[0]) {
	    ++sum;
	  }
	  if((sum += Checksum_MBUF->srcipaddr[1]) < Checksum_MBUF->srcipaddr[1]) {
	    ++sum;
	  }
	  if((sum += Checksum_MBUF->destipaddr[0]) < Checksum_MBUF->destipaddr[0]) {
	    ++sum;
	  }
	  if((sum += Checksum_MBUF->destipaddr[1]) < Checksum_MBUF->destipaddr[1]) {
	    ++sum;
	  }
	  if((sum += (u16_t)htons((u16_t)UIP_PROTO_TCP)) < (u16_t)htons((u16_t)UIP_PROTO_TCP)) {
	    ++sum;
	  }
	  hsum = (u16_t)htons((((u16_t)(Checksum_MBUF->len[0]) << 8) + Checksum_MBUF->len[1]) - 20);
  }
  else
  { 
	  if((sum += Checksum_BUF->srcipaddr[0]) < Checksum_BUF->srcipaddr[0]) {
	    ++sum;
	  }
	  if((sum += Checksum_BUF->srcipaddr[1]) < Checksum_BUF->srcipaddr[1]) {
	    ++sum;
	  }
	  if((sum += Checksum_BUF->destipaddr[0]) < Checksum_BUF->destipaddr[0]) {
	    ++sum;
	  }
	  if((sum += Checksum_BUF->destipaddr[1]) < Checksum_BUF->destipaddr[1]) {
	    ++sum;
	  }
	  if((sum += (u16_t)htons((u16_t)UIP_PROTO_TCP)) < (u16_t)htons((u16_t)UIP_PROTO_TCP)) {
	    ++sum;
	  }
	  hsum = (u16_t)htons((((u16_t)(Checksum_BUF->len[0]) << 8) + Checksum_BUF->len[1]) - 20);
  }

  if((sum += hsum) < hsum) {
    ++sum;
  }
  return sum;
}
#endif

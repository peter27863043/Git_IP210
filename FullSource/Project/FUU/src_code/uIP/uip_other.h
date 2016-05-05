/******************************************************************************
*
*   Name:           uip_other.h
*
*   Description:    This file is not directerly related to uip, just for compatible issue.
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Jesse
*
*******************************************************************************/
/****************************************************************************************************
****************************************************************************************************/

#define REMOVE_FOR_NOT_NEED

//====================================================================================================
#define	DEBUG_RS232		1
#define	DEBUG_UART			1		/* debug information output from UART for DEBUGF() 1:enable	*/


#define DEBUG
#ifdef DEBUG
#define DEBUGF(debug, x)    do { if (debug) { P3TXD=1; printf x; } } while (0)
#else
#define DEBUGF(debug, x)
#endif

typedef struct config_s {
    IPADDR_T    ipaddr;
    IPADDR_T    netmask;
    IPADDR_T    gateway;
    IPADDR_T    dnsserver;
    bool_t      dhcp_en;
    u16_t       ctlport;
    u16_t       videoport;
	u8_t		trunk;
} CONFIG_T;

extern CONFIG_T    config_g;

//#define memcpy4(dst4,src4)  *(u32_t *)(dst4) = *(u32_t *)(src4)
#define memcpy4(dst4,src4)  \
    ((u8_t *)dst4)[0] = ((u8_t *)src4)[0], \
    ((u8_t *)dst4)[1] = ((u8_t *)src4)[1], \
    ((u8_t *)dst4)[2] = ((u8_t *)src4)[2], \
    ((u8_t *)dst4)[3] = ((u8_t *)src4)[3]

/*
 * <num> should always be an integer constant, otherwise
 *  it will be very slooooow.
 */
#ifndef REMOVE_FOR_NOT_NEED
#define IPADDR_SET(ia,num)  \
    ((u8_t *)ia)[0] = (u8_t)((u32_t)num >> 24), \
    ((u8_t *)ia)[1] = (u8_t)((u32_t)num >> 16), \
    ((u8_t *)ia)[2] = (u8_t)((u32_t)num >> 8), \
    ((u8_t *)ia)[3] = (u8_t)((u32_t)num)
#endif //REMOVE_FOR_NOT_NEED

#define DEBUG_IO_PAGE		0	//

#define DEBUG_CHECK_NET_ID  0

#define DEBUG_HTTPD			0

#ifndef REMOVE_FOR_NOT_NEED
struct uip_conn_s
{
    u8_t tcpstateflags; /* TCP state and flags. */
    u16_t lport, rport; /* The local and the remote port. */
    u16_t ripaddr[2];   /* The IP address of the remote peer. */
    u8_t rmacaddr[6];     /* The mac address to the remote peer. */
    u8_t rcv_nxt[4];    /* The sequence number that we expect to receive next. */
    u8_t snd_nxt[4];    /* The sequence number that was last sent by us. */
    u8_t ack_nxt[4];    /* The sequence number that should be ACKed by
                           next ACK from peer. */
    u8_t snd_seq[4];    /* The sequence number of current tx window. */
    u8_t ack_seq[4];    /* The sequence number that was ACKed by peer. */
    u8_t timer;         /* The retransmission timer. */
    u8_t nrtx;          /* Counts the number of retransmissions for a
                           particular segment. */
    u16_t mss;          /* max segment size */
    u16_t wnd;          /* peer receive window */

    u8_t appstate[sizeof(struct httpd_info)];
};
#define uip_connw       ((struct uip_conn_s  xdata *)0x6080)
#endif// REMOVE_FOR_NOT_NEED

//void jc_eth_send();

#ifndef NETUART_CTRL_H
#define NETUART_CTRL_H
#include "module.h"
#ifdef MODULE_NET_UART_CTRL
#include "type_def.h"

/*
#define DCD_MASK 1
#define DTR_MASK 4
#define DSR_MASK 6
#define RTS_MASK 7
#define CTS_MASK 8
*/
extern u32_t tcp_check_ip;

#define NUC_SERVER		0
#define NUC_CLIENT		1

#define NUC_TOLISTEN	2
#define NUC_TOCONN		3

#define NUC_CONNECT		4
#define NUC_DISABLE		5
#define NUC_IDLE		6

void nuc_init();
u8_t nuc_on_tcp_rx();
void nuc_on_loop();

typedef struct _netuart_ctrl_t
{
	u8_t mode;
	u8_t status;
	u8_t on_change;
	u8_t ack;
	

}netuart_ctrl_t;
extern netuart_ctrl_t netuart_ctrl;

typedef struct _NUC_signal
{
	u8_t ri;
	u8_t dcd;
	u8_t cts;
	u8_t dsr;
	u8_t old;
}NUC_signal, *pNUC_signal;


#endif //MODULE_NET_UART_CTRL
#endif //NETUART_CTRL_H
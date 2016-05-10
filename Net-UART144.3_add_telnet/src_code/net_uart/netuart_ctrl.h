#ifndef NETUART_CTRL_H
#define NETUART_CTRL_H
#include "module.h"
#ifdef MODULE_NET_UART_CTRL
#include "type_def.h"

#define DCD_MASK 1
#define DTR_MASK 4
#define DSR_MASK 6
#define RTS_MASK 7
#define CTS_MASK 8

typedef struct _netuart_ctrl_t
{
	u8_t enable;
	u8_t last_uart_signal;
	u8_t result;
	u8_t event_queue[1];
	u8_t event_queue_wait_ack[1];
//[0]: bit0:op0x11
}netuart_ctrl_t;

typedef struct _unc_uart_config_t
{
	u8_t op;
	u8_t baudrate;
	u8_t data_size;
	u8_t parity;
	u8_t stop_size;
	u8_t flow_ctrl;
	u8_t password[17];
}unc_uart_config_t;

typedef struct _unc_uart_signal_t
{
	u8_t op;
	u8_t signal;
	u8_t password[17];
}unc_uart_signal_t;

typedef struct _unc_uart_result_t
{
	u8_t op;
	u8_t result;
}unc_uart_result_t;

void nuc_init();
u8_t nuc_on_tcp_rx();
void nuc_on_loop();
extern netuart_ctrl_t netuart_ctrl;
#endif //MODULE_NET_UART_CTRL
#endif //NETUART_CTRL_H
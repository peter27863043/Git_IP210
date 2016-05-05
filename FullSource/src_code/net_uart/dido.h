#include "module.h"
#ifndef DIDO_H
#define DIDO_H
#ifdef MODULE_DIDO
#include "gpio.h"
#ifdef FLASH_512
#define DIDO_MAX_PIN 2
#define DIDO_0	P3_3
#define DIDO_1	P3_2
#define DIDO_2	P1_6
#define DIDO_3	P1_5
#define DIDO_4	P1_3
#define DIDO_5	P1_2
//#define DIDO_4	P4_7
//#define DIDO_5	P4_6
#else //FLASH_512
#define DIDO_MAX_PIN 2
#define DIDO_0 P3_5
#define DIDO_1 P1_2
#define DIDO_2 ERROR_UNDEFINE
#endif //FLASH_512

void dido_init();
u8_t dido_read(u8_t mask);
u8_t dido_tcp_rx_fun();
void dido_write(u8_t value,u8_t mask);

#ifdef MODULE_TCP_DIDO
#define TD_NUM	8
//extern STCPDIDO TCPDIDO;

typedef struct _STD
{
	u8_t number_count;
	char Name[16];
}STD;
typedef struct _STCPDIDO
{
	u8_t connect;
	u8_t waitAck;
	u8_t last_status;
	u8_t pin_num;
	u8_t pin_value;
	u8_t echo;
   	u8_t* pkt_rx_data_offset;
   	u8_t* pkt_tx_data_offset;
}STCPDIDO;

#define W0	0
#define C0	1
#define R0	2
#define WA	3
#define EH	4
#define CT	5
#define ST	6
#define AL	7
#define MAX_DODI_PIN	6
#endif

#endif //MODULE_DIDO
#endif //DIDO_H

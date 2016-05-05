#ifndef _UART_H
#define _UART_H


//UART
#if SYSTEM_CLOCK_59M//58.9MHz
#define UART_RATE_110           0x82B9
#define UART_RATE_300           0x2FEE
#define UART_RATE_1200          0xBFB
#define UART_RATE_2400          0x5FD
#define UART_RATE_4800          0x2FE
#define UART_RATE_9600          0x17F
#define UART_RATE_19200         0xC0
#define UART_RATE_38400         0x60
#define UART_RATE_57600         0x40
#define UART_RATE_115200        0x20
#define UART_RATE_230400        0x10
#define UART_RATE_460800        0x8
#define UART_RATE_921600        0x4
#else//44.2MHz
#define UART_RATE_110           0x6219
#define UART_RATE_300           0x23F8
#define UART_RATE_1200          0x8FE
#define UART_RATE_2400          0x480
#define UART_RATE_4800          0x240
#define UART_RATE_9600          0x120
#define UART_RATE_19200         0x90
#define UART_RATE_38400         0x48
#define UART_RATE_57600         0x30
#define UART_RATE_115200        0x18
#define UART_RATE_230400        0xC
#define UART_RATE_460800        0x6
#define UART_RATE_921600        0x3
#endif

#define PARITY_ODD    0x08
#define PARITY_EVEN   0x18
#define PARITY_SPACE  0x38
#define PARITY_MARK   0x28

struct _UART_Settings{
  unsigned char UART_Enable;//1:Enable, 0:Disable
  unsigned short divisor;//the value set to determine the baudrate
	unsigned char Rx_FIFO_threshold;//the number of chars in RX FIFO set to issue interrupt when interrupt is enabled 
	unsigned char UART_IER;//the value set to UIER registers,0x0:Disable,BIT0-4 stand for each interrupt event
	unsigned char Character_Bits;//LCR_CHARACTER_5BITS, LCR_CHARACTER_6BITS, LCR_CHARACTER_7BITS, LCR_CHARACTER_8BITS
	unsigned char Parity_Type;//0x0:None, 0x02:PARITY_EVEN(LCR[3:5]=[110]), 0x03:PARITY_ODD(LCR[3:5]=[100]), 0x04:PARITY_MARK(LCR[3:5]=[101]), 0x05:PARITY_SPACE(LCR[4:5]=[111])
	unsigned char Stop_Bit;//0x0:1bit, 0x1:1.5bit or 2 bit
	unsigned char HW_Flow_Control;//0x0:None, 0x1:Enable
};

struct _Baudrate_string{
    unsigned short divisor;
	unsigned char  string[10];
};
extern struct _UART_Settings UART_Settings;
extern struct _Baudrate_string UART_Baudrate[13];

void UART_Set_Property(void);

unsigned char UART_Tx_FIFO_Status(void);
unsigned char UART_Rx_FIFO_Status(void);

void UART_Transmit_Multi(unsigned char *ptr_src, unsigned short length);
void UART_Receive_Multi(unsigned char *ptr_des, unsigned char length);

unsigned char UART_Receive_Byte(unsigned char *Rx_byte);
void UART_Transmit_Byte(unsigned char Tx_byte);

void UART_Empty_Rx_FIFO(void);

void UART_Set_RTS();
void UART_Clear_RTS();
//---------------------------------------------------------------------------

#endif
#ifndef _IP210REG_H
#define _IP210REG_H
#include <Reg52.h>

#define IP210RegWrite(_Dest, _data)     *(volatile unsigned char xdata *)(_Dest) = _data
#define IP210RegRead(_Src) (*(volatile unsigned char xdata *)(_Src))

#define IP210RegRead16(SrcReg_) ( ( (u16_t)IP210RegRead(SrcReg_+1)<<8 ) | IP210RegRead(SrcReg_) )
#define IP210RegWrite16(DestReg_, data_) \
    *(volatile unsigned char xdata *)(DestReg_) = (data_&0xff);\
    *(volatile unsigned char xdata *)(DestReg_+1) = (data_>>8)

//-------------------------------------------------------------------
// IP210 Registers definitions
//-------------------------------------------------------------------
//CPU
//Additional definitaions from standard REG52.h

//sfr PCON  = 0x87;
#define PCON_IDL            0x01

sfr PDCON         = 0xE1;


sfr CKCON         = 0x8E;
sfr WDTCON        = 0xD8;
sfr WDTWCYC       = 0xEF;

//Bit
//sfr WDTCON        = 0xD8;
sbit WDTRST          = WDTCON^0;
sbit WDTEN           = WDTCON^1;
sbit WDTCLR          = WDTCON^2;
sbit WDT_DIS         = WDTCON^3;

//sfr PDCON         = 0xE1;
#define PCON_JWP            0x02


sbit FLASH_ACCESS_EN = WDTCON^4;

sbit FLASH_ADDR_16 = P3^5;
sbit FLASH_ADDR_17 = P3^4;
sbit FLASH_ADDR_18 = P1^7;


//-------------------------------------------------------------------
//IO Registers
//
//CONTROL
#define REG_CHIP_CONFIGURE_0              0x8000
#define REG_CHIP_CONFIGURE_1              0x8001
#define REG_CPU_CONTROL                   0x8002

#define REG_STATUS                        0x8003
#define REG_INTERRUPT_ENABLE              0x8004
#define REG_SW_RESET                      0x8005

#define REG_MIRROR_ADDRESS                0x8006

//TIMER COUNTER
#define REG_LATCH_TIMER_COUNTER           0x800F
#define REG_TIMER_COUNTER_0               0x8010//LSB
#define REG_TIMER_COUNTER_1               0x8011
#define REG_TIMER_COUNTER_2               0x8012
#define REG_TIMER_COUNTER_3               0x8013//MSB
#define REG_PAD_CONTROL					  0x8014

//MAC
#define REG_MAC_CONTROL_0                 0x8100
#define REG_MAC_CONTROL_1                 0x8101
#define REG_MAC_CONTROL_2                 0x8102
#define REG_FLOW_ON_THRESHOLD             0x8103
#define REG_FLOW_OFF_THRESHOLD            0x8104
//MAC TX
#define REG_TXD_START						  0x8110
#define REG_TX_DES_0_0_BUFFER_PTR_L           0x8110
#define REG_TX_DES_0_1_BUFFER_PTR_H           0x8111
#define REG_TX_DES_0_2_PACKET_LENGTH          0x8112
#define REG_TX_DES_0_3_CONTROL_LENGTH         0x8113

#define REG_TX_DES_1_0_BUFFER_PTR_L           0x8114
#define REG_TX_DES_1_1_BUFFER_PTR_H           0x8115
#define REG_TX_DES_1_2_PACKET_LENGTH          0x8116
#define REG_TX_DES_1_3_CONTROL_LENGTH         0x8117

#define REG_TX_DES_2_0_BUFFER_PTR_L           0x8118
#define REG_TX_DES_2_1_BUFFER_PTR_H           0x8119
#define REG_TX_DES_2_2_PACKET_LENGTH          0x811A
#define REG_TX_DES_2_3_CONTROL_LENGTH         0x811B

#define REG_TX_DES_3_0_BUFFER_PTR_L           0x811C
#define REG_TX_DES_3_1_BUFFER_PTR_H           0x811D
#define REG_TX_DES_3_2_PACKET_LENGTH          0x811E
#define REG_TX_DES_3_3_CONTROL_LENGTH         0x811F

//RX MAC
#define	REG_RX_START                          0x8130
#define	REG_RX_READ_PTR_L                     0x8131
#define	REG_RX_READ_PTR_H                     0x8132
#define	REG_RX_WRITE_PTR_L                    0x8133
#define	REG_RX_WRITE_PTR_H                    0x8134

#define	REG_RX_FILTER_0                       0x8135
#define	REG_RX_FILTER_1                       0x8136
#define	REG_ETH_TYPE_OFFSET                   0x8137
//Special Tag
#define REG_SPECIAL_TAG_TYPE_L                0x8138
#define REG_SPECIAL_TAG_TYPE_H                0x8139

//MAC Debug
#define REG_MAC_DEBUG_L                0x81D0
#define REG_MAC_DEBUG_H                0x81D1
//DMA
#define REG_DMA_COMMAND                   0x8200
#define REG_DMA_SOURCE_ADDRESS_L          0x8201
#define REG_DMA_SOURCE_ADDRESS_H          0x8202
#define REG_DMA_DESTINATION_ADDRESS_L     0x8203
#define REG_DMA_DESTINATION_ADDRESS_H     0x8204
#define REG_DMA_LENGTH_L                  0x8205
#define REG_DMA_LENGTH_H                  0x8206

#define REG_CRC_RESULT_0                  0x8207
#define REG_CRC_RESULT_1                  0x8208
#define REG_CRC_RESULT_2                  0x8209
#define REG_CRC_RESULT_3                  0x820A
#define REG_PRESET_CRC_VALUE              0x820B

//EEPROM
#define REG_EEPROM_DATA					          0x8310
#define REG_EEPROM_ADDR					          0x8311
#define REG_EEPROM_ID  					          0x8312
#define REG_EEPROM_CMD					          0x8313
#define REG_EEPROM_CONTROL			          0x8314

//MDC/MDIO 
#define	REG_MDC_CONTROL					0x8320
#define	REG_MDC_PHYADDR					0x8321
#define	REG_MDC_REGADDR					0x8322
#define	REG_MDC_DATA_LOW				0x8323
#define	REG_MDC_DATA_HIGH				0x8324


//MAC ADDRESS
#define REG_MAC_ADDR					  0x8330
#define REG_MAC_ADDR_0					0x8330
#define REG_MAC_ADDR_1					0x8331
#define REG_MAC_ADDR_2					0x8332
#define REG_MAC_ADDR_3					0x8333
#define REG_MAC_ADDR_4					0x8334
#define REG_MAC_ADDR_5					0x8335

//IP ADDRESSv4
#define	REG_IP_ADDR						0x8336
#define	REG_IP_ADDR_0					0x8336
#define	REG_IP_ADDR_1					0x8337
#define	REG_IP_ADDR_2					0x8338
#define	REG_IP_ADDR_3					0x8339

//IP ADDRESSv6
#define	REG_IPV6_ADDR					0x8340
#define	REG_IPV6_ADDR_0					0x8340
#define	REG_IPV6_ADDR_1					0x8341
#define	REG_IPV6_ADDR_2					0x8342
#define	REG_IPV6_ADDR_3					0x8343
#define	REG_IPV6_ADDR_4					0x8344
#define	REG_IPV6_ADDR_5					0x8345
#define	REG_IPV6_ADDR_6					0x8346
#define	REG_IPV6_ADDR_7					0x8347
#define	REG_IPV6_ADDR_8					0x8348
#define	REG_IPV6_ADDR_9					0x8349
#define	REG_IPV6_ADDR_a					0x834a
#define	REG_IPV6_ADDR_b					0x834b
#define	REG_IPV6_ADDR_c					0x834c
#define	REG_IPV6_ADDR_d					0x834d
#define	REG_IPV6_ADDR_e					0x834e
#define	REG_IPV6_ADDR_f					0x834f


//MAC ADDRESS
#define REG_RMT_MAC_ADDR					0x8350
#define REG_RMT_MAC_ADDR_0					0x8350
#define REG_RMT_MAC_ADDR_1					0x8351
#define REG_RMT_MAC_ADDR_2					0x8352
#define REG_RMT_MAC_ADDR_3					0x8353
#define REG_RMT_MAC_ADDR_4					0x8354
#define REG_RMT_MAC_ADDR_5					0x8355

//IP ADDRESSv4
#define	REG_RMT_IP_ADDR						0x8356
#define	REG_RMT_IP_ADDR_0					0x8356
#define	REG_RMT_IP_ADDR_1					0x8357
#define	REG_RMT_IP_ADDR_2					0x8358
#define	REG_RMT_IP_ADDR_3					0x8359

//IP ADDRESSv6
#define	REG_RMT_IPV6_ADDR					0x8360
#define	REG_RMT_IPV6_ADDR_0					0x8360
#define	REG_RMT_IPV6_ADDR_1					0x8361
#define	REG_RMT_IPV6_ADDR_2					0x8362
#define	REG_RMT_IPV6_ADDR_3					0x8363
#define	REG_RMT_IPV6_ADDR_4					0x8364
#define	REG_RMT_IPV6_ADDR_5					0x8365
#define	REG_RMT_IPV6_ADDR_6					0x8366
#define	REG_RMT_IPV6_ADDR_7					0x8367
#define	REG_RMT_IPV6_ADDR_8					0x8368
#define	REG_RMT_IPV6_ADDR_9					0x8369
#define	REG_RMT_IPV6_ADDR_a					0x836a
#define	REG_RMT_IPV6_ADDR_b					0x836b
#define	REG_RMT_IPV6_ADDR_c					0x836c
#define	REG_RMT_IPV6_ADDR_d					0x836d
#define	REG_RMT_IPV6_ADDR_e					0x836e
#define	REG_RMT_IPV6_ADDR_f					0x836f


//UART
#define REG_UART_RECEIVE_BUFFER           0x8800
#define REG_UART_TRANSMIT_BUFFER          0x8801
#define REG_UART_INTERRUPT_ENABLE         0x8802
#define REG_UART_INTERRUPT_IDENTIFICATION 0x8803
#define REG_UART_FIFO_CONTROL             0x8804
#define REG_UART_LINE_CONTROL             0x8805
#define REG_UART_MODEM_CONTROL            0x8806
#define REG_UART_LINE_STATUS              0x8807
#define REG_UART_MODEM_STATUS             0x8808
#define REG_UART_TX_FIFO_STATUS           0x8809
#define REG_UART_RX_FIFO_STATUS           0x880A
#define REG_UART_CLOCK_DEVISOR_L          0x880B
#define REG_UART_CLOCK_DEVISOR_H          0x880C
//-------------------------------------------------------------------
// IP210 Registers bit definitions
//-------------------------------------------------------------------
//CONTROL
//#define REG_CHIP_CONFIGURE_0              0x8000
  #define BIT_EXT_FLASH_EN                         0x01
  #define BIT_CPU_REDIRECT_EN                      0x02
  #define BIT_MIRROR_EN                            0x20
//#define REG_CHIP_CONFIGURE_1              0x8001
//#define REG_CPU_CONTROL                   0x8002

//#define REG_STATUS                        0x8003
  #define BIT_STATUS_TIMER_COUNTER_OVERFLOW        0x08
  #define BIT_STATUS_UART_STATUS_CHANGE            0x10
//#define REG_INTERRUPT_ENABLE              0x8004
  #define BIT_DMA_ACCESS_DONE_ENABLE               0x04
  #define BIT_UART_STATUS_CHANGE                   0x10
//#define REG_SW_RESET                      0x8005

//#define REG_MIRROR_ADDRESS                0x8006


//#define REG_TIMER_COUNTER_0               0x8010//LSB
//#define REG_TIMER_COUNTER_1               0x8011
//#define REG_TIMER_COUNTER_2               0x8012
//#define REG_TIMER_COUNTER_3               0x8013//MSB


//MAC
//#define REG_MAC_CONTROL_0                 0x8100
#define	MC0_SPEED100                                  0x01
#define	MC0_DUPLEX_F                                  0x02
#define	MC0_TX_ENABLE                                 0x04
#define	MC0_RX_ENABLE                                 0x08
#define	MC0_FLOW_CTRL_EN                              0x10
#define	MC0_BOFF_16_OFF                               0x20
#define	MC0_LOOPBACK                                  0x40
#define	MC0_MAX_FRAME_1536                            0x80

//#define REG_MAC_CONTROL_1                 0x8101
#define	MC1_FCS_APPEND_DISABLE                        0x01
#define	MC1_FCS_RECIEVE_ENABLE                        0x02
#define	MC1_RESET_TX_MAC                              0x80

//#define REG_MAC_CONTROL_2                 0x8102
#define	MC2_IP_CHECKSUM_INSP                          0x01
#define	MC2_TCP_CHECKSUM_INSP                         0x02
#define	MC2_UDP_CHECKSUM_INSP                         0x04
#define	MC2_ICMP_CHECKSUM_INSP                        0x08

//#define REG_FLOW_ON_THRESHOLD             0x8103
//#define REG_FLOW_OFF_THRESHOLD            0x8104
//MAC TX
//#define REG_TX_DES_0_0_BUFFER_PTR_L           0x8110
//#define REG_TX_DES_0_1_BUFFER_PTR_H           0x8111
//#define REG_TX_DES_0_2_PACKET_LENGTH          0x8112
//#define REG_TX_DES_0_3_CONTROL_LENGTH         0x8113
#define	TFD_LEN_H		                    0x07
#define	TFD_PKT_TYPE		                0x38
#define	TFD_ERROR		                    0x40
#define	TFD_START		                    0x80

//#define REG_TX_DES_1_0_BUFFER_PTR_L           0x8114
//#define REG_TX_DES_1_1_BUFFER_PTR_H           0x8115
//#define REG_TX_DES_1_2_PACKET_LENGTH          0x8116
//#define REG_TX_DES_1_3_CONTROL_LENGTH         0x8117

//#define REG_TX_DES_2_0_BUFFER_PTR_L           0x8118
//#define REG_TX_DES_2_1_BUFFER_PTR_H           0x8119
//#define REG_TX_DES_2_2_PACKET_LENGTH          0x811A
//#define REG_TX_DES_2_3_CONTROL_LENGTH         0x811B

//#define REG_TX_DES_3_0_BUFFER_PTR_L           0x811C
//#define REG_TX_DES_3_1_BUFFER_PTR_H           0x811D
//#define REG_TX_DES_3_2_PACKET_LENGTH          0x811E
//#define REG_TX_DES_3_3_CONTROL_LENGTH         0x811F

//RX MAC
//#define	REG_RX_START                          0x8130
//#define	REG_RX_READ_PTR_L                     0x8131
//#define	REG_RX_READ_PTR_H                     0x8132
//#define	REG_RX_WRITE_PTR_L                    0x8133
//#define	REG_RX_WRITE_PTR_H                    0x8134

//#define	REG_RX_FILTER_0                       0x8135
#define RF0_MY_MAC_EN                  0x01
#define RF0_MSCT_EN                    0x02
#define RF0_BSCT_EN                    0x04
#define RF0_ALL_EN                     0x08
#define RF0_PAUSE_EN                   0x10
#define RF0_RX_REMOTE_MAC              0x20

//#define	REG_RX_FILTER_1                       0x8136
#define RF1_RX_MY_IP_EN                0x01
#define RF1_RX_REMOTE_IP_EN            0x02
#define RF1_RX_IP_TYPE_EN              0x04
#define RF1_RX_REMOTE_IP_EN            0x02
//#define	REG_ETH_TYPE_OFFSET                   0x8137

//RX BUFFER
//Rx_buf types  //RFD
#define	RX_VALID                       0x80
#define	RX_PKT_TYPE                    0x78
#define	RX_LEN_H                       0x07

#define	HEADER_LEN                     2
#define	RX_BUF_END                     0x7fff
//#define	RX_BUF_START                   0x4000
#define	RX_BUF_START                   ((u16_t)IP210RegRead(REG_RX_START)<<8)

//#define REG_DMA_COMMAND                   0x8200
  #define BIT_CMD_MODE                      0x07
  #define BIT_CMD_MODE_NO_DMA               0x00
  #define BIT_CMD_MODE_M2M                  0x01
  #define BIT_CMD_MODE_M2IO                 0x02
  #define BIT_CMD_MODE_IO2M                 0x03
  #define BIT_CMD_MODE_F2M                  0x04
  #define BIT_CMD_MODE_CRC                  0x05
  #define BIT_CMD_MODE_CHECKSUM             0x06

//#define	REG_MDC_CONTROL					0x8320
#define	MDC_SPEED_40		0x00
#define	MDC_SPEED_80		0x01
#define	MDC_SPEED_160		0x02
#define	MDC_SPEED_320		0x03
#define	MDC_SPEED_640		0x04
#define	MDC_SPEED_1280		0x05
#define	MDC_SPEED_2560		0x06

#define	MDC_READ			0x10
#define	MDC_WRITE			0x00

#define	MDC_PREAMABLE_OFF	0x20
#define	MDC_START			0x80


//EEPROM
//#define REG_EEPROM_DATA					          0x8310
//#define REG_EEPROM_ADDR					          0x8311
//#define REG_EEPROM_ID  					          0x8312
//#define REG_EEPROM_CMD					          0x8313
  #define EE_CMD_BYTE_COUNT                    0x1F
  #define EE_CMD_OP_READ                       0x20  
  #define EE_CMD_OP_WRITE                      0x00  
  #define EE_CMD_ABORT                         0x40
  #define EE_CMD_NEXT                          0x80    
//#define REG_EEPROM_CONTROL			          0x8314


//UART
//#define REG_UART_RECEIVE_BUFFER           0x8800
//#define REG_UART_TRANSMIT_BUFFER          0x8801
//#define REG_UART_INTERRUPT_ENABLE         0x8802
#define IER_RECEIVE_DATA_AVAILABLE                  0x1
#define IER_TRANSMITTER_HOLDING_REGISTER_EMPTY      0x2
#define IER_RECEIVE_LINE_STATUS                     0x4
#define IER_MODEM_STATUS                            0x8
#define IER_RECEIVE_TIMEOUT_INDICATION              0x10

//#define REG_UART_INTERRUPT_IDENTIFICATION         0x8803
#define IIR_RECEIVE_LINE_STATUS                     0x6
#define IIR_RECEIVE_DATA                            0x4
#define IIR_TIMEOUT_INDICATION                      0xC
#define IIR_TRANSMITTER_HOLDING_REGISTER_EMPTY      0x2
#define IIR_MODEM_STATUS                            0x0

//#define REG_UART_FIFO_CONTROL             0x8804
#define FCR_RESET_RX_FIFO                           0x1
#define FCR_RESET_TX_FIFO                           0x2
#define FCR_RX_FIFO_THRESHOLD                       0xC8 // FCR C8 is 200 , original is FC

//#define REG_UART_LINE_CONTROL             0x8805
#define LCR_CHARACTER_5BITS                           0x0
#define LCR_CHARACTER_6BITS                           0x1
#define LCR_CHARACTER_7BITS                           0x2
#define LCR_CHARACTER_8BITS                           0x3
#define LCR_STOP_BIT                                  0x4
#define LCR_PARITY_ENABLE                             0x8
#define LCR_EVEN_PARITY_SELECT                        0x10
#define LCR_STICK_PARITY_BIT                          0x20
#define LCR_BREAK_CONTROL_BIT                         0x40
#define LCR_DIVISOR_LATCH_BIT                         0x80

//#define REG_UART_MODEM_CONTROL            0x8806
#define MCR_DATA_TERMINAL_READY                        0x1
#define MCR_REQUEST_TO_SEND                            0x2
#define MCR_OUT1                                       0x4
#define MCR_OUT2                                       0x8
#define MCR_LOOPBACK                                   0x10

//#define REG_UART_LINE_STATUS              0x8807
#define LSR_DATA_READY_INDICATOR                       0x1
#define LSR_OVERRUN_ERROR_INDICATOR                    0x2
#define LSR_PARITY_ERROR_INDICATOR                     0x4
#define LSR_FRAMING_ERROR_INDICATOR                    0x8
#define LSR_BREAK_INTERRUPT_INDICATOR                  0x10
#define LSR_TRANSMIT_FIFO_EMPTY                        0x20
#define LSR_TRANSMIT_EMPTY_INDICATOR                   0x40
#define LSR_AT_LEAST_ONE_ERROR                         0x80

//#define REG_UART_MODEM_STATUS             0x8808
#define MSR_DELTA_CTS                                   0x1
#define MSR_DELTA_DSR                                   0x2
#define MSR_TERI                                        0x4
#define MSR_DELTA_DCD                                   0x8
#define MSR_COMPLEMENT_CTS                              0x10
#define MSR_COMPLEMENT_DSR                              0x20
#define MSR_COMPLEMENT_RI                               0x40
#define MSR_COMPLEMENT_DCD                              0x80
//#define REG_UART_TX_FIFO_STATUS           0x8809
//#define REG_UART_CLOCK_DEVISOR_L          0x880A
//#define REG_UART_CLOCK_DEVISOR_H          0x880B









//-------------------------------------------
//Reserved Bit-Addressible
#define IP210_Status            0x26//for any routines that are polling to stats reg
//Reserved DATA
#define SFR_IE_TEMP             0x30//only for CPU Idle mode sfr IE recovery use
#define TEMP_0                  0x38
#define TEMP_1                  0x39
#define TEMP_2                  0x3A
#define TEMP_3                  0x3B

#define SRC_L                   0x40
#define SRC_H                   0x41
#define DES_L                   0x42
#define DES_H                   0x43
#endif
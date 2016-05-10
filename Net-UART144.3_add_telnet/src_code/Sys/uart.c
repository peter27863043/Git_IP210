#include "public.h"
#include "uart.h"
//typedef unsigned short      u16_t; //debug
//typedef unsigned char       u8_t; // debug

struct _UART_Settings UART_Settings;
struct _Baudrate_string UART_Baudrate[13] = {UART_RATE_110, "110",
                                       UART_RATE_300, "300",
                                       UART_RATE_1200, "1200",
                                       UART_RATE_2400, "2400",
                                       UART_RATE_4800, "4800",
                                       UART_RATE_9600, "9600",
                                       UART_RATE_19200, "19200",
                                       UART_RATE_38400, "38400",
                                       UART_RATE_57600, "57600",
                                       UART_RATE_115200, "115200",
                                       UART_RATE_230400, "230400",
                                       UART_RATE_460800, "460800",
                                       UART_RATE_921600, "921600"};

void UART_Set_Divisor(unsigned char divisor_H, unsigned char divisor_L)
{
  unsigned char byte_data;

	  //enable divisor write
    byte_data = IP210RegRead(REG_UART_LINE_CONTROL);
    byte_data |= LCR_DIVISOR_LATCH_BIT;
	  IP210RegWrite(REG_UART_LINE_CONTROL, byte_data); 

	  IP210RegWrite(REG_UART_CLOCK_DEVISOR_L, divisor_L); 
	  IP210RegWrite(REG_UART_CLOCK_DEVISOR_H, divisor_H); 
    TRACE_UART(("\nWrite divisor=(0x%x%x)", (unsigned short)divisor_H, (unsigned short)divisor_L));

    byte_data &= ~LCR_DIVISOR_LATCH_BIT;
	  IP210RegWrite(REG_UART_LINE_CONTROL, byte_data); 

}

void UART_Set_Property(void)
{
  unsigned char divisor_L, divisor_H, byte_data;

    TRACE_UART(("\n=>UART_Set_Property()"));

//Set UART Baudrate
    if(UART_Settings.UART_Enable == 1)
    {
        byte_data = IP210RegRead(REG_CHIP_CONFIGURE_1);
        IP210RegWrite(REG_CHIP_CONFIGURE_1, byte_data|0x02); 
        byte_data = IP210RegRead(REG_CHIP_CONFIGURE_1);
        TRACE_UART(("\n REG_CHIP_CONFIGURE_1=(0x%x)", (unsigned short)byte_data));

        divisor_L = IP210RegRead(REG_UART_CLOCK_DEVISOR_L); 
        divisor_H = IP210RegRead(REG_UART_CLOCK_DEVISOR_H); 
        TRACE_UART(("\nOld divisor=(0x%x) (0x%x)", (unsigned short)divisor_H, (unsigned short)divisor_L));

        divisor_L = (unsigned char)(UART_Settings.divisor&0xFF);
        divisor_H = (unsigned char)((UART_Settings.divisor>>8)&0xFF);

        UART_Set_Divisor(divisor_H, divisor_L);

        divisor_L = IP210RegRead(REG_UART_CLOCK_DEVISOR_L); 
        divisor_H = IP210RegRead(REG_UART_CLOCK_DEVISOR_H); 
        TRACE_UART(("\nNew divisor=(0x%x) (0x%x)", (unsigned short)divisor_H&0xff, (unsigned short)divisor_L));

//set LCR
        byte_data = IP210RegRead(REG_UART_LINE_CONTROL);
        TRACE_UART(("\nLCR=(0x%x)",(unsigned short)byte_data));
        
        //Parity
        byte_data &= ~(0x38);        
		    if(UART_Settings.Parity_Type!=0x0)
		        byte_data |= UART_Settings.Parity_Type;

        //Stop Bit
		    if(UART_Settings.Stop_Bit==1)
            byte_data |= LCR_STOP_BIT;
		    else
            byte_data &= ~LCR_STOP_BIT;
            
        //Number of bit of Character
        byte_data &= ~(0x03);   
        byte_data |= UART_Settings.Character_Bits;
        IP210RegWrite(REG_UART_LINE_CONTROL, byte_data); 

        byte_data = IP210RegRead(REG_UART_LINE_CONTROL);
        TRACE_UART(("\nNew LCR=(0x%x)",(unsigned short)byte_data));

//Set Interrupt
        if(UART_Settings.UART_IER!=0)
	      {
            byte_data = UART_Settings.UART_IER;
            IP210RegWrite(REG_UART_INTERRUPT_ENABLE, byte_data); 
            byte_data = IP210RegRead(REG_UART_INTERRUPT_ENABLE);
            TRACE_UART(("\nNew UART IER=(0x%x)",(unsigned short)byte_data));

            byte_data = IP210RegRead(REG_INTERRUPT_ENABLE);
            TRACE_UART(("\nOld IER=(0x%x)",(unsigned short)byte_data));
            
		        byte_data |= BIT_UART_STATUS_CHANGE;
            IP210RegWrite(REG_INTERRUPT_ENABLE, byte_data); 
            byte_data = IP210RegRead(REG_INTERRUPT_ENABLE);
            TRACE_UART(("\nNew IER=(0x%x)",(unsigned short)byte_data));

	      }
        if(UART_Settings.Rx_FIFO_threshold!=0)
        {
            byte_data = IP210RegRead(REG_UART_FIFO_CONTROL);
            TRACE_UART(("\nOld FCR=(0x%x), set rx threshold(%x)",(unsigned short)byte_data, (unsigned short)UART_Settings.Rx_FIFO_threshold));
	          byte_data = UART_Settings.Rx_FIFO_threshold & FCR_RX_FIFO_THRESHOLD;
            IP210RegWrite(REG_UART_FIFO_CONTROL, byte_data); 
            byte_data = IP210RegRead(REG_UART_FIFO_CONTROL);
            TRACE_UART(("\nNew FCR=(0x%x)",(unsigned short)byte_data));
        }

//set MCR
        byte_data = IP210RegRead(REG_UART_MODEM_CONTROL); 
        byte_data &= ~0x80;
        TRACE_UART(("\nOld MCR=(0x%x), set HW_Flow_Control=(%x)",(unsigned short)byte_data, (unsigned short)UART_Settings.HW_Flow_Control));
        //HW_FlowControl
        if(UART_Settings.HW_Flow_Control!=0)
	          byte_data |= 0x80;
        IP210RegWrite(REG_UART_MODEM_CONTROL, byte_data); 
        byte_data = IP210RegRead(REG_UART_MODEM_CONTROL);
        TRACE_UART(("\nNew FCR=(0x%x)",(unsigned short)byte_data));        
    }
    TRACE_UART(("\n<=UART_Set_Property()"));
}

void UART_Transmit_Multi(unsigned char *ptr_src, unsigned short length)
{
  unsigned short tx_count=0;
  unsigned char  tx_byte=0;

    for(tx_count=0;tx_count<length;tx_count++)
	{
        tx_byte = ptr_src[tx_count];
        IP210RegWrite(REG_UART_TRANSMIT_BUFFER, tx_byte);
	}
}

void UART_Receive_Multi(unsigned char *ptr_des, unsigned char length)
{
  unsigned char rx_count=0, rx_byte;

    for(rx_count=0;rx_count<length;rx_count++)
	{
	    rx_byte = IP210RegRead(REG_UART_RECEIVE_BUFFER);
	
//printf("rx_byte is %x\n\r", (unsigned short)rx_byte); //debug
		*((unsigned char *)(ptr_des+rx_count)) = rx_byte;
	}
}



unsigned char UART_Tx_FIFO_Status(void)
{
    return IP210RegRead(REG_UART_TX_FIFO_STATUS);
}

unsigned char UART_Rx_FIFO_Status(void)
{
    return IP210RegRead(REG_UART_RX_FIFO_STATUS);
}

void UART_Empty_Rx_FIFO(void)
{
  unsigned char rx_count=0, rx_byte, i;

        rx_count = UART_Rx_FIFO_Status();
        if(rx_count!=0)
        {
            for(i=0;i<rx_count;i++)
            {
                rx_byte = IP210RegRead(REG_UART_RECEIVE_BUFFER);
            }
        }

}

unsigned char UART_Receive_Byte(unsigned char *Rx_byte)
{

    if(UART_Rx_FIFO_Status()!=0)
	{
        *Rx_byte = IP210RegRead(REG_UART_RECEIVE_BUFFER);
		return 1;
	}
	else
	{
	    return 0;
	}

}


void UART_Transmit_Byte(unsigned char Tx_byte)
{
    IP210RegWrite(REG_UART_TRANSMIT_BUFFER, Tx_byte);
}
void UART_Set_RTS()
{
	unsigned char tmp;
	tmp=IP210RegRead(REG_UART_MODEM_CONTROL);
	IP210RegWrite(REG_UART_MODEM_CONTROL, tmp|0x2);
}

void UART_Clear_RTS()
{
	unsigned char tmp;
	tmp=IP210RegRead(REG_UART_MODEM_CONTROL);
	IP210RegWrite(REG_UART_MODEM_CONTROL, tmp&0xfd);
}

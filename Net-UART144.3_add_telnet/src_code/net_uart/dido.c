#include "module.h"
#ifdef MODULE_DIDO
#include "dido.h"
#include <stdio.h>
#include <string.h>
#include "uip.h"
#include "eepdef.h"

void DIDO_data_config();
void DIDO_send();
extern void up2lwr(u8_t *buf, u8_t len);

#ifdef MODULE_TCP_DIDO

STCPDIDO TCPDIDO;

STD code TD[TD_NUM]={
W0,		"w0",
C0,		"c0",
R0,		"r0",
WA,		"wa",
EH,		"eh",
CT,		"ct",
ST,		"st",
AL,		"",		//use for when first connect send all status
};
#endif

void dido_init()
{
#ifdef MODULE_TCP_DIDO
	u8_t mask=0;
	uip_listen(ModuleInfo.UDPTelInfo.dido_port);
  	TCPDIDO.pkt_rx_data_offset=uip_appdata;
	TCPDIDO.pkt_tx_data_offset=uip_appdata;
	TCPDIDO.echo=ModuleInfo.UDPTelInfo.dido_echo;
	dido_write(ModuleInfo.UDPTelInfo.dido_value, mask);
#endif
}
u8_t dido_read(u8_t mask)
{
	u8_t value=0, demo=0;
	
	if((mask&0x01)!=0){ DIDO_0=1; }
	value=DIDO_0;
	if((mask&0x02)!=0){ DIDO_1=1; }
	demo=DIDO_1;
	value|=(demo&0x01)<<1;
    if((mask&0x04)!=0){ DIDO_2=1; }
	demo=DIDO_2;
	value|=(demo&0x01)<<2;
	if((mask&0x08)!=0){ DIDO_3=1; }
	demo=DIDO_3;
	value|=(demo&0x01)<<3;
    if((mask&0x10)!=0){ DIDO_4=1; }
	demo=DIDO_4;
	value|=(demo&0x01)<<4;
	if((mask&0x20)!=0){ DIDO_5=1; }
	demo=DIDO_5;
	value|=(demo&0x01)<<5;
/*
    if((mask&0x40)!=0){ DIDO_6=1; }
	demo=DIDO_6;
	value|=(demo&0x01)<<6;
	if((mask&0x80)!=0){ DIDO_7=1; }
	demo=DIDO_7;
	value|=(demo&0x01)<<7;
*/
	return value;
}
void dido_write(u8_t value,u8_t mask)
{
    if((mask&0x01)==0){ DIDO_0=(value&0x01)?1:0; }
	if((mask&0x02)==0){ DIDO_1=(value&0x02)?1:0; }
    if((mask&0x04)==0){ DIDO_2=(value&0x04)?1:0; }
	if((mask&0x08)==0){ DIDO_3=(value&0x08)?1:0; }
    if((mask&0x10)==0){ DIDO_4=(value&0x10)?1:0; }
	if((mask&0x20)==0){ DIDO_5=(value&0x20)?1:0; }
/*
    if((mask&0x40)==0){ DIDO_6=(value&0x40)?1:0; }
	if((mask&0x80)==0){ DIDO_7=(value&0x80)?1:0; }
*/
}
#ifdef MODULE_TCP_DIDO
u8_t dido_buf[10]={0};
u8_t dido_wi=0;
u8_t dido_tcp_rx_fun()
{
	u8_t i=0, j=0, find=0;
	u8_t error[9]="\n\rError";
	u8_t len=0, error_flag=0;

	if(uip_conn->lport!=ModuleInfo.UDPTelInfo.dido_port){return 0;}
	if(uip_connected())	//alway use new connect
	{
//		TCPDIDO.connect=uip_conn;
		TCPDIDO.last_status=7;

		DIDO_data_config();
		uip_slen=uip_len;
	}
	if(uip_acked())
	{
		TCPDIDO.waitAck=0;
	}
	if(uip_newdata())
	{
		TCPDIDO.last_status=10;
		memcpy(&dido_buf[dido_wi], TCPDIDO.pkt_rx_data_offset, uip_len);
		dido_wi+=uip_len;

		for(j=0;j<10;j++)
		{  if(dido_buf[j]==0x0D){ find=1; dido_wi=0; } } 
		
		if(find)
		{
			up2lwr(dido_buf, 10);
//			for(i=0;i<10;i++) //A - Z to a - z
//			{	dido_buf[i]=up2lwr(dido_buf[i]);	}
	
			for(i=0;i<7;i++)
			{
				if(!strncmp(dido_buf, TD[i].Name, 2))
				{
					TCPDIDO.last_status=i;
					if(i<5){ TCPDIDO.pin_num=dido_buf[2]-0x30; }
					if(i<3){ TCPDIDO.pin_value=dido_buf[3]-0x30; }
					break;
				}
			}
			len=strlen(dido_buf);
			memset(dido_buf, 0, 10);
			if(TCPDIDO.last_status==0 || TCPDIDO.last_status==1){ if(len>5){ error_flag=1; } }
			if(TCPDIDO.last_status>=2 && TCPDIDO.last_status<=4){ if(len>4){ error_flag=1; } }
			if(TCPDIDO.last_status==5 || TCPDIDO.last_status==6){ if(len>3){ error_flag=1; } }
			if(TCPDIDO.last_status<3)
			{
				if(TCPDIDO.last_status==2){TCPDIDO.pin_value=0;}
				if( TCPDIDO.pin_num<=0 || TCPDIDO.pin_num>=9 || (TCPDIDO.pin_value!=0 && TCPDIDO.pin_value!=1))
				{ error_flag=1; }
			}
			if(error_flag==1)
			{
				memcpy(TCPDIDO.pkt_tx_data_offset, error, 9);
				uip_len=9;
				uip_slen=uip_len;
				return 1;
			}
			else
			{
				DIDO_data_config();
				uip_slen=uip_len;
			}
		}
		else
		{ uip_len=0; }
	}
	if(uip_rexmit())
	{
		DIDO_data_config();
		uip_slen=uip_len;
	}
	if(uip_closed()||uip_aborted())
	{
		TCPDIDO.waitAck=0;
	}
/*
	if(uip_newdata() || uip_acked())
	{
	}
	if(uip_poll())
	{
	}
	if(uip_timedout())
	{
	}
*/
	return 1;
}
//ModuleInfo.dido_info.direction[1]
//: 0x3A, ( 0x28, ) 0x29
void DIDO_data_config()
{
	u8_t i=0, value=0, count=0, j=0;
	u8_t pin[6]={0x0D, 0x0A, 0x30, 0x30, 0x3A, 0x30};
	u8_t pin_n[13]={0x0D, 0x0A, 0x30, 0x30, 0x28, 0x30, 0x30, 0x30, 0x30, 0x30, 0x29, 0x3A, 0x30};
	u8_t pin_de[13]={0x0D, 0x0A, 0x30, 0x30, 0x28, 0x30, 0x30, 0x30, 0x30, 0x30, 0x29, 0x3A, 0x30};
	u8_t error[9]="\n\rError";
	u8_t end[2]={0x0D, 0x0A};
	u8_t dido_value=0;
	
	uip_len=0;

	switch(TD[TCPDIDO.last_status].number_count)
	{
		case W0:
			if(ModuleInfo.dido_info.direction[(TCPDIDO.pin_num-1)]==0)
			{
				pin[3]=0x30+TCPDIDO.pin_num;
				pin[5]=0x30+TCPDIDO.pin_value;
				memcpy(TCPDIDO.pkt_tx_data_offset, pin, 6);
				switch(TCPDIDO.pin_num)
				{
					case 1:
						DIDO_0=TCPDIDO.pin_value;
						dido_value|=TCPDIDO.pin_value&0x01;
						break;
					case 2:
						DIDO_1=TCPDIDO.pin_value;
						dido_value|=(TCPDIDO.pin_value&0x01)<<1;
						break;
					case 3:
						DIDO_2=TCPDIDO.pin_value;
						dido_value|=(TCPDIDO.pin_value&0x01)<<2;
						break;
					case 4:
						DIDO_3=TCPDIDO.pin_value;
						dido_value|=(TCPDIDO.pin_value&0x01)<<3;
						break;
					case 5:
						DIDO_4=TCPDIDO.pin_value;
						dido_value|=(TCPDIDO.pin_value&0x01)<<4;
						break;
					case 6:
						DIDO_5=TCPDIDO.pin_value;
						dido_value|=(TCPDIDO.pin_value&0x01)<<5;
						break;
/*
					case 7:
						DIDO_6=TCPDIDO.pin_value;
						dido_value|=(TCPDIDO.pin_value&0x01)<<6;
						break;
					case 8:
						DIDO_7=TCPDIDO.pin_value;
						dido_value|=(TCPDIDO.pin_value&0x01)<<7;
						break;
*/
				}
				ModuleInfo.UDPTelInfo.dido_value=dido_value;
				uip_len=6;
			}
			else
			{
				memcpy(TCPDIDO.pkt_tx_data_offset, error, 9);
				uip_len=9;
			}
			break;
		case R0:
			if(ModuleInfo.dido_info.direction[(TCPDIDO.pin_num-1)]==1)
			{
				pin[3]=0x30+TCPDIDO.pin_num;
				switch(TCPDIDO.pin_num)
				{
					case 1:
						TCPDIDO.pin_value=DIDO_0;
						break;
					case 2:
						TCPDIDO.pin_value=DIDO_1;
						break;
					case 3:
						TCPDIDO.pin_value=DIDO_2;
						break;
					case 4:
						TCPDIDO.pin_value=DIDO_3;
						break;
					case 5:
						TCPDIDO.pin_value=DIDO_4;
						break;
					case 6:
						TCPDIDO.pin_value=DIDO_5;
						break;
/*
					case 7:
						TCPDIDO.pin_value=DIDO_6;
						break;
					case 8:
						TCPDIDO.pin_value=DIDO_7;
						break;
*/
				}
				pin[5]=0x30+TCPDIDO.pin_value;
				memcpy(TCPDIDO.pkt_tx_data_offset, pin, 6);
				uip_len=6;
			}
			else
			{
				memcpy(TCPDIDO.pkt_tx_data_offset, error, 9);
				uip_len=9;
			}
			break;
		case WA:
			for(i=0;i<8;i++)
			{
				if(ModuleInfo.dido_info.direction[i]==0)
				{
					count=1;
					if(TCPDIDO.pin_num!=0 && TCPDIDO.pin_num!=1){ count=0; break; }
					switch(i+1)
					{
						case 1:
							DIDO_0=TCPDIDO.pin_num;
							dido_value|=TCPDIDO.pin_num&0x01;
							break;
						case 2:
							DIDO_1=TCPDIDO.pin_num;
							dido_value|=(TCPDIDO.pin_num&0x01)<<1;
							break;
						case 3:
							DIDO_2=TCPDIDO.pin_num;
							dido_value|=(TCPDIDO.pin_num&0x01)<<2;
							break;
						case 4:
							DIDO_3=TCPDIDO.pin_num;
							dido_value|=(TCPDIDO.pin_num&0x01)<<3;
							break;
						case 5:
							DIDO_4=TCPDIDO.pin_num;
							dido_value|=(TCPDIDO.pin_num&0x01)<<4;
							break;
						case 6:
							DIDO_5=TCPDIDO.pin_num;
							dido_value|=(TCPDIDO.pin_num&0x01)<<5;
							break;
/*
						case 7:
							DIDO_6=TCPDIDO.pin_num;
							dido_value|=(TCPDIDO.pin_num&0x01)<<6;
							break;
						case 8:
							DIDO_7=TCPDIDO.pin_num;
							dido_value|=(TCPDIDO.pin_num&0x01)<<7;
							break;
*/
					}
				}
			}
			if(count==1)
			{
				ModuleInfo.UDPTelInfo.dido_value=dido_value;
				memcpy(TCPDIDO.pkt_tx_data_offset, end, 2);
				*(TCPDIDO.pkt_tx_data_offset+2)=0x30;
				uip_len=3;
			}
			else
			{
				memcpy(TCPDIDO.pkt_tx_data_offset, end, 2);
				*(TCPDIDO.pkt_tx_data_offset+2)=0x31;
				uip_len=3;
			}
			break;
		case ST:
			if(TCPDIDO.echo)
			{
				for(i=0;i<8;i++)
				{
					for(j=0;j<13;j++){ pin_n[j]=pin_de[j]; }
					pin_n[3]=0x31+i;
					memcpy(&pin_n[5], &ModuleInfo.UDPTelInfo.dido_name[5*i], 5);
					memcpy(TCPDIDO.pkt_tx_data_offset+(13*i), pin_n, 13);
				}
				value=DIDO_0;
				*(TCPDIDO.pkt_tx_data_offset+12+(13*0))=0x30+value;
				value=DIDO_1;
				*(TCPDIDO.pkt_tx_data_offset+12+(13*1))=0x30+value;
				value=DIDO_2;
				*(TCPDIDO.pkt_tx_data_offset+12+(13*2))=0x30+value;
				value=DIDO_3;
				*(TCPDIDO.pkt_tx_data_offset+12+(13*3))=0x30+value;
				value=DIDO_4;
				*(TCPDIDO.pkt_tx_data_offset+12+(13*4))=0x30+value;
				value=DIDO_5;
				*(TCPDIDO.pkt_tx_data_offset+12+(13*5))=0x30+value;
/*
				value=DIDO_6;
				*(TCPDIDO.pkt_tx_data_offset+12+(13*6))=0x30+value;
				value=DIDO_7;
				*(TCPDIDO.pkt_tx_data_offset+12+(13*7))=0x30+value;
*/
				uip_len=78;
			}
			else
			{
				for(i=0;i<8;i++)
				{
					pin[3]=0x31+i;
					memcpy(TCPDIDO.pkt_tx_data_offset+(6*i), pin, 6);
				}
				value=DIDO_0;
				*(TCPDIDO.pkt_tx_data_offset+5+(6*0))=0x30+value;
				value=DIDO_1;
				*(TCPDIDO.pkt_tx_data_offset+5+(6*1))=0x30+value;
				value=DIDO_2;
				*(TCPDIDO.pkt_tx_data_offset+5+(6*2))=0x30+value;
				value=DIDO_3;
				*(TCPDIDO.pkt_tx_data_offset+5+(6*3))=0x30+value;
				value=DIDO_4;
				*(TCPDIDO.pkt_tx_data_offset+5+(6*4))=0x30+value;
				value=DIDO_5;
				*(TCPDIDO.pkt_tx_data_offset+5+(6*5))=0x30+value;
/*
				value=DIDO_6;
				*(TCPDIDO.pkt_tx_data_offset+5+(6*6))=0x30+value;
				value=DIDO_7;
				*(TCPDIDO.pkt_tx_data_offset+5+(6*7))=0x30+value;
*/
				uip_len=36;
			}
			break;
		case CT:	//1:input 0:output
			for(i=0;i<8;i++)
			{
				pin[3]=0x31+i;
				pin[5]=0x30+ModuleInfo.dido_info.direction[i];
				memcpy(TCPDIDO.pkt_tx_data_offset+(6*i), pin, 6);
			}
			uip_len=48;	
			break;
		case C0:
			ModuleInfo.dido_info.direction[TCPDIDO.pin_num-1]=TCPDIDO.pin_value;
			memcpy(TCPDIDO.pkt_tx_data_offset, end, 2);
			*(TCPDIDO.pkt_tx_data_offset+2)=0x30;
			uip_len=3;
			break;
		case EH:
			TCPDIDO.echo=TCPDIDO.pin_num;
			ModuleInfo.UDPTelInfo.dido_echo=TCPDIDO.pin_num;
			if(TCPDIDO.echo==1 || TCPDIDO.echo==0)
			{
				memcpy(TCPDIDO.pkt_tx_data_offset, end, 2);
				*(TCPDIDO.pkt_tx_data_offset+2)=0x30;
				uip_len=3;
			}
			else
			{
				memcpy(TCPDIDO.pkt_tx_data_offset, end, 2);
				*(TCPDIDO.pkt_tx_data_offset+2)=0x31;
				uip_len=3;
			}
			break;
		case AL:	//start send all status
			for(i=0;i<8;i++)
			{
				pin[3]=0x31+i;
				memcpy(TCPDIDO.pkt_tx_data_offset+(6*i), pin, 6);
			}
			value=DIDO_0;
			*(TCPDIDO.pkt_tx_data_offset+5+(6*0))=0x30+value;
			value=DIDO_1;
			*(TCPDIDO.pkt_tx_data_offset+5+(6*1))=0x30+value;
			value=DIDO_2;
			*(TCPDIDO.pkt_tx_data_offset+5+(6*2))=0x30+value;
			value=DIDO_3;
			*(TCPDIDO.pkt_tx_data_offset+5+(6*3))=0x30+value;
			value=DIDO_4;
			*(TCPDIDO.pkt_tx_data_offset+5+(6*4))=0x30+value;
			value=DIDO_5;
			*(TCPDIDO.pkt_tx_data_offset+5+(6*5))=0x30+value;
/*
			value=DIDO_6;
			*(TCPDIDO.pkt_tx_data_offset+5+(6*6))=0x30+value;
			value=DIDO_7;
			*(TCPDIDO.pkt_tx_data_offset+5+(6*7))=0x30+value;
*/
			uip_len=36;	
			break;
		default:
			memcpy(TCPDIDO.pkt_tx_data_offset, error, 9);
			uip_len=9;
			break;
	}
	SaveModuleInfo2EEPROM(&ModuleInfo.dido_info,sizeof(ModuleInfo.dido_info));
}

void DIDO_send()
{
	TCPDIDO.waitAck=1;
}
#endif
#endif //MODULE_DIDO
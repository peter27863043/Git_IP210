#include <module.h>
#include "type_def.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>								  
#include "type_def.h"
#include "fs.h"
#include "httpd.h"
#include "uip.h"
#include "..\sys\eeprom.h"
#include "..\main\eepdef.h"
#include "..\HtmCfiles\uart_htm.h"
#ifdef MODULE_NET_UART
#include "net_uart.h"
#endif //MODULE_NET_UART
#ifdef MODULE_RS485
#include "gpio.h"
#endif //MODULE_RS485
void hex_to_asc(u8_t hex, s8_t * buf);
u16_t asc_to_hex(char asc[]);

extern struct httpd_info *hs;
char s_tmp[5];
char *itoah(u8_t bv);
#define ROW_BR          10
#define ROW_CHAR_BITS   30
#define ROW_PARITY      50
#define ROW_STOP_BIT    70
#define ROW_HW_FLOWCTRL 90
#define ROW_MD          110

char selected[]="selected";
u8_t code pr_v[5]={8,24,0,56,40};
char code pr_n[5][6]={"odd","even","none","space","mark"};	
u8_t code sb_v[2]={0,1};
char code sb_n[2][9]={"1","1.5 or 2"};		
u8_t code fc_v[2]={1,0};
char code fc_n[2][9]={"hardware","none"};

#ifdef MODULE_RS485
u8_t code md_v[3]={0,1,2};
char code md_n[3][6]={"RS232","RS422","RS485"};
#endif
#ifdef MODULE_64K_HTM	//for CLI 64K
#else	
u8_t ssi_uart(u8_t varid, data_value *vp)
{
#ifdef MODULE_NET_UART
	switch(hs->row_num)
	{
			case ROW_BR+10:
			case ROW_CHAR_BITS+4:
			case ROW_PARITY+5:
			case ROW_STOP_BIT+2:
			case ROW_HW_FLOWCTRL+2:
#ifdef MODULE_RS485				
			case ROW_MD+3:
#endif				
			return ERROR;
  	}
	vp->value.string ="";
	vp->type = string;
    switch (varid)
    {
    	case CGI_SELECT:
    		if(hs->row_num-ROW_BR+1==TelnetData.pEEPROM_UART->Baudrate)vp->value.string=selected;
    		break;
    	case CGI_BR1:
   		case CGI_BR2:
   			if(hs->row_num==1)hs->row_num=ROW_BR;
    		vp->value.string=BaudrateTable[hs->row_num-ROW_BR+1].Name;
    		break;
    		
    	case CGI_CB1:
    	case CGI_CB2:
    		if(hs->row_num==1)hs->row_num=ROW_CHAR_BITS;
        vp->value.digits_3_int=hs->row_num-ROW_CHAR_BITS+5;
   		  vp->type=digits_3_int;    		
    		break;
    	case CGI_CBS:
    		if(hs->row_num-ROW_CHAR_BITS==TelnetData.pEEPROM_UART->Character_Bits)vp->value.string=selected;
    		break;    		 
    		
    	case CGI_PR1:
    		if(hs->row_num==1)hs->row_num=ROW_PARITY;
        vp->value.digits_3_int=pr_v[hs->row_num-ROW_PARITY];
   		  vp->type=digits_3_int;    		
    		break;
    	case CGI_PR2:    		
    		vp->value.string=pr_n[hs->row_num-ROW_PARITY];
    		break;
    	case CGI_PRS:
    		if(pr_v[hs->row_num-ROW_PARITY]==TelnetData.pEEPROM_UART->Parity_Type)vp->value.string=selected;
    		break;    	   		
    		
    	case CGI_SB1:
    		if(hs->row_num==1)hs->row_num=ROW_STOP_BIT;
        vp->value.digits_3_int=sb_v[hs->row_num-ROW_STOP_BIT];
   		  vp->type=digits_3_int;    		
    		break;
    	case CGI_SB2:    		
    		vp->value.string=sb_n[hs->row_num-ROW_STOP_BIT];
    		break;
    	case CGI_SBS:
    		if(sb_v[hs->row_num-ROW_STOP_BIT]==TelnetData.pEEPROM_UART->Stop_Bit)vp->value.string=selected;
    		break;     		
    		
    	case CGI_FC1:
    		if(hs->row_num==1)hs->row_num=ROW_HW_FLOWCTRL;
        vp->value.digits_3_int=fc_v[hs->row_num-ROW_HW_FLOWCTRL];
   		  vp->type=digits_3_int;    		
    		break;
    	case CGI_FC2:    		
    		vp->value.string=fc_n[hs->row_num-ROW_HW_FLOWCTRL];
    		break;
    	case CGI_FCS:
    		if(fc_v[hs->row_num-ROW_HW_FLOWCTRL]==TelnetData.pEEPROM_UART->HW_Flow_Control)vp->value.string=selected;
    		break;     		    		
#ifdef MODULE_RS485
    	case CGI_MD1:
    		if(hs->row_num==1)hs->row_num=ROW_MD;
        vp->value.digits_3_int=md_v[hs->row_num-ROW_MD];
   		  vp->type=digits_3_int;    		
    		break;
    	case CGI_MD2:    		
    		vp->value.string=md_n[hs->row_num-ROW_MD];
    		break;
    	case CGI_MDS:
    		if(md_v[hs->row_num-ROW_MD]==TelnetData.pEEPROM_UART->OP_Mode)vp->value.string=selected;
    		break;
#endif //MODULE_RS485
#ifdef MODULE_FLASH_485		
		case CGI_UART_MEM_OF_B:
		vp->value.digits_3_int=TelnetData.UART_MEM_OF_B;
   		vp->type=digits_3_int;
		break;
		
		case CGI_UART_MEM_OF_K:
		vp->value.digits_3_int=TelnetData.UART_MEM_OF_K;
   		vp->type=digits_3_int;
		break;
		
		case CGI_UART_MEM_OF_M:
		vp->value.digits_3_int=TelnetData.UART_MEM_OF_M;
   		vp->type=digits_3_int;
		break;				
		
		case CGI_UART_FIFO_OF:
		vp->value.digits_3_int=TelnetData.UART_FIFO_Overflow;
   		vp->type=digits_3_int;
		break;		
#endif
		case CGI_D1CHK:
			if(TelnetData.pEEPROM_UART->delimiter_en&UART_INFO_DILIMITER_EN_CHAR1)
			{
				vp->value.string ="checked";
			}
		break;

		case CGI_D1DATA:
			hex_to_asc(TelnetData.pEEPROM_UART->delimiter[0],s_tmp);
			vp->value.string =s_tmp;
		break;

		case CGI_D2CHK:
			if(TelnetData.pEEPROM_UART->delimiter_en&UART_INFO_DILIMITER_EN_CHAR2)
			{
				vp->value.string ="checked";
			}

		break;

		case CGI_D2DATA:
			hex_to_asc(TelnetData.pEEPROM_UART->delimiter[1],s_tmp);
			vp->value.string =s_tmp;
		break;

		case CGI_D3CHK:
			if(TelnetData.pEEPROM_UART->delimiter_en&UART_INFO_DILIMITER_EN_SILENT)
			{
				vp->value.string ="checked";
			}

		break;

		case CGI_D3DATA:
			vp->value.digits_3_int=TelnetData.pEEPROM_UART->delimiter[2];
		   	vp->type=digits_3_int;
		break;
		case CGI_D4CHK:
			if(TelnetData.pEEPROM_UART->delimiter_drop)
			{
				vp->value.string ="checked";
			}
		break;
#ifdef MODULE_NET_UART_CTRL
		case CGI_INCHK:
			if(TelnetData.pEEPROM_UART->rd_io==1)
				vp->value.string="checked";
			break;
		case CGI_OUTCHK:
			if(TelnetData.pEEPROM_UART->rd_io==0)
				vp->value.string="checked";
			break;
#endif
    }
#else //MODULE_NET_UART
int temp=(int)varid;
temp=(int)vp;
#endif //MODULE_NET_UART
    return OK;
}

u8_t cgi_uart(u8_t cnt, void *vptr)
{
#ifdef MODULE_NET_UART
	req_data *vp = vptr;
	int i,j;
	u8_t value;
	TelnetData.pEEPROM_UART->delimiter_en=0;
	TelnetData.pEEPROM_UART->delimiter_drop=0;
    for (i=0; i < cnt; i++, vp++)
    {
#ifdef MODULE_RS485
        if (!strcmp(vp->item, "mode"))
        {        	
					NetUARTData.pTelnetData->pEEPROM_UART->OP_Mode=atoi(vp->value);
					switch(TelnetData.pEEPROM_UART->OP_Mode)
					{
						case UART_INFO_OP_M_RS232:
							P4_0=1;
							P4_5=1;
							break;
						case UART_INFO_OP_M_RS422:
							P4_0=0;
							P4_5=0;
							break;
						case UART_INFO_OP_M_RS485:
							P4_0=0;
							P4_5=0;
							break;
					}
        }
#endif //MODULE_RS485
        if (!strcmp(vp->item, "set_baudrate"))
        {        	
			for(j=0;j<BAUDRATE_NUM;j++)
			{
			  if(!strcmp(BaudrateTable[j].Name,vp->value))
			  {
				  NetUARTData.pTelnetData->pEEPROM_UART->Baudrate=j;
			  	UART_Settings.divisor = BaudrateTable[TelnetData.pEEPROM_UART->Baudrate].Divisor;
			  }
  			}
        }
		if (!strcmp(vp->item, "set_cb"))
        { 
			NetUARTData.pTelnetData->pEEPROM_UART->Character_Bits=atoi(vp->value)-5;
			UART_Settings.Character_Bits=atoi(vp->value)-5;
		}
		if (!strcmp(vp->item, "set_pt"))
        {
			NetUARTData.pTelnetData->pEEPROM_UART->Parity_Type=atoi(vp->value);
			UART_Settings.Parity_Type=atoi(vp->value);
		}
		if (!strcmp(vp->item, "set_sb"))
        {
			NetUARTData.pTelnetData->pEEPROM_UART->Stop_Bit=atoi(vp->value);
			UART_Settings.Stop_Bit=atoi(vp->value);
		}
		if (!strcmp(vp->item, "set_hf"))
    	{
			NetUARTData.pTelnetData->pEEPROM_UART->HW_Flow_Control=atoi(vp->value);
			UART_Settings.HW_Flow_Control=atoi(vp->value);
#ifdef MODULE_RS485
	    	if(NetUARTData.pTelnetData->pEEPROM_UART->OP_Mode!=UART_INFO_OP_M_RS232)
	    	{
	    		NetUARTData.pTelnetData->pEEPROM_UART->HW_Flow_Control=0;
	    		UART_Settings.HW_Flow_Control=0;
	    	}
#endif //MODULE_RS485    	
		}
		if(!strcmp(vp->item, "D1"))
		{
			TelnetData.pEEPROM_UART->delimiter_en|=UART_INFO_DILIMITER_EN_CHAR1;
		}
		if(!strcmp(vp->item, "D1V"))
		{
			TelnetData.pEEPROM_UART->delimiter[0]=asc_to_hex(vp->value);
		}
		if(!strcmp(vp->item, "D2"))
		{
			TelnetData.pEEPROM_UART->delimiter_en|=UART_INFO_DILIMITER_EN_CHAR2;
		}
		if(!strcmp(vp->item, "D2V"))
		{
			TelnetData.pEEPROM_UART->delimiter[1]=asc_to_hex(vp->value);			
		}
		if(!strcmp(vp->item, "D3"))
		{
			TelnetData.pEEPROM_UART->delimiter_en|=UART_INFO_DILIMITER_EN_SILENT;
		}
		if(!strcmp(vp->item, "D3V"))
		{
			value=atoi(vp->value);
			if(value==0)value=1;
			if(value>255)value=255;
			TelnetData.pEEPROM_UART->delimiter[2]=value;			
		}
		if(!strcmp(vp->item, "D4"))
		{
			TelnetData.pEEPROM_UART->delimiter_drop=1;
		}
		if(!strcmp(vp->item, "inout")){
			if(!strcmp(vp->value, "INPUT"))
				TelnetData.pEEPROM_UART->rd_io=1;
			if(!strcmp(vp->value, "OUTPUT"))
				TelnetData.pEEPROM_UART->rd_io=0;
		}
    }//for (i=0; i < cnt; i++, vp++)
  	UART_Set_Property();
	UART_Clear_RTS();
  	SaveModuleInfo2EEPROM(TelnetData.pEEPROM_UART,sizeof(SUARTInfo));
    httpd_init_file(fs_open("uart.htm",&hs->op_index));
#else //MODULE_NET_UART
int temp=(int)cnt;
temp=(int)vptr;
#endif //MODULE_NET_UART
    return OK;
}
#endif

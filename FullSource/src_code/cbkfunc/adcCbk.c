#include <module.h>
#ifdef MODULE_ADC
#include "type_def.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>								  
#include "type_def.h"
#include "public.h"
#include "fs.h"
#include "httpd.h"
#include "uip.h"
#include "..\sys\eeprom.h"
#include "..\main\eepdef.h"

#include "..\HtmCfiles\adc_htm.h"

extern struct httpd_info *hs;
#define CODING_DBG_MESSAGE_1 0
u8_t current_channel=1, current_vrs=4;//ADC0, vrs3

u16_t adc_convert(void)
{
  u8_t byte_data, adc_h, adc_l;
  u16_t adcValue;
#if 0
  u8_t;
#endif

    //Turn on ADC
    byte_data = IP210RegRead(0x8370);
    byte_data = ((current_vrs-1)<<2) | ((current_channel-1)<<4) | 0x1;
    IP210RegWrite(0x8370, byte_data); 
    //Wait Calibration
    //bit7 calibrated status
    do{
        byte_data = IP210RegRead(0x8370);
    }while(!(byte_data&0x80));

    	//Start ADC 
        byte_data = IP210RegRead(0x8370);
        byte_data |= 0x2;
        IP210RegWrite(0x8370, byte_data); 

   	    
		do{
            byte_data = IP210RegRead(0x8370);
      	}while(byte_data&0x02);

     	adc_l= IP210RegRead(0x8371);
    	adc_h= IP210RegRead(0x8372);
        //printf("\nadc_h=[%x],adc_l=[%x]", (u16_t)adc_h, (u16_t)adc_l);
        adc_h=(adc_h&0x03);
        //printf("\nadc_h=[%x],adc_l=[%x]", (u16_t)adc_h, (u16_t)adc_l);
		if(adc_h&0x02)
		{
            //printf("\nnegative");
#if 0
		    if(!(adc_h&0x01))
			{
			        //printf("\nnegative-h01");
               // adc_h|=0x01;
			    byte_data=adc_l;
				for(i=0;i<8;i++)
				{
				    if(byte_data&0x80)
					{
					    adc_l|=0x80>>i;
					}
					else
					{
					    break;
					}
				    ;byte_data<<=1;
				}
			}
#endif
            adc_h|=0xfc;
			adcValue=(u16_t)adc_h<<8|adc_l;
    		adcValue=~adcValue+1;
			adcValue=512-adcValue;
		}
		else
		{
            //printf("\npositive");
            adcValue=((u16_t)adc_h&0x03)<<8|adc_l;
			adcValue+=512;
		}
        //printf("\nadcValue=[%x]", (u16_t)adcValue);

    //Turn off ADC
    byte_data = IP210RegRead(0x8370);
    byte_data &= ~0x1;
    IP210RegWrite(0x8370, byte_data); 
    
	adcValue>>=3;

	return adcValue;
}
#define ADC_VRS_0 "0.9375 V - 1.5625 V" 
#define ADC_VRS_1 "0.625 V - 1.875 V" 
#define ADC_VRS_2 "0.3125 V - 2.1875 V" 
#define ADC_VRS_3 "0 V - 2.5 V" 
u8_t ssi_adc(u8_t varid, data_value *vp)
{

#if CODING_DBG_MESSAGE_1
               printf("\nssi_adc[%02x]", (u16_t)varid);
#endif
    switch (varid)
    {

        case CGI_CURRENT_CHANNEL:
#if CODING_DBG_MESSAGE_1
               printf("CGI_CURRENT_CHANNEL");
#endif
        	   vp->value.digits_3_int=current_channel;
   			   vp->type=digits_3_int;
               break;
        case CGI_CURRENT_VALUE:
#if CODING_DBG_MESSAGE_1
               printf("CGI_CURRENT_VALUE");
#endif
        	   vp->value.digits_3_int=adc_convert();
   			   vp->type=digits_3_int;
#if CODING_DBG_MESSAGE_1
               printf("\adc_convert[%d] ", (u16_t)vp->value.digits_3_int);
#endif
               break;

        default:
                break;
    }
    return OK;
}
u8_t cgi_adc(u8_t cnt, void *vptr)
{
	req_data *vp = vptr;
	u8_t temp_channel;
	int i;	


    for (i=0; i < cnt; i++, vp++)
	{
	  if(i==0) temp_channel=atoi(vp->value);
#if CODING_DBG_MESSAGE_1
	  printf("\n[%d, %s, %s]", i, vp->item, vp->value);
#endif
	}
	vp = vptr;

			if(!strcmp(vp[1].item, "Change"))
			{
#if CODING_DBG_MESSAGE_1
                 printf("\nChange");
#endif
                current_channel=temp_channel;
#if CODING_DBG_MESSAGE_1
                 printf("\ncurrent_channel=[%d],current_vrs=[%d] ",(u16_t)current_channel, (u16_t)current_vrs);
#endif
			}
			if(!strcmp(vp[0].item, "ADCUpadte"))
			{
#if CODING_DBG_MESSAGE_1
                 printf("\nADCUpadte");
#endif
			}

    hs->op_index=0;
    httpd_init_file(fs_open("adc.htm",&hs->op_index));

    return OK;
}
#endif//MODULE_ADC

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

#include "..\HttpServer\application.h"
extern u8_t errmsgflag ;

extern u8_t  m_sync_data  ;
extern u8_t  m_src_data   ;
extern u8_t  m_hms_data   ;
extern u8_t  m_vms_data   ;


eSCALER_ST SendGetAllSettingCommand(void);


void hex_to_asc(u8_t hex, s8_t * buf);
u16_t asc_to_hex(char asc[]);

extern struct httpd_info *hs;
char s_tmp[5];
char *itoah(u8_t bv);
#define ROW_PWR_SAVE    10
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
u8_t code pwr_save_v[4]={0,1,2,3};
char code pwr_diso_n[4][4]={"Low","High","On","Off"};
char code fc_n[2][9]={"hardware","none"};

#ifdef MODULE_RS485
u8_t code md_v[3]={0,1,2};
char code md_n[3][6]={"RS232","RS422","RS485"};
#endif
#ifdef MODULE_64K_HTM	//for CLI 64K
#else	

u8_t  m_mid_data	=4;
//u8_t  m_plk_data	=0;
u8_t  m_pws_data	=0;
u8_t  m_imr_data	=0;
u8_t  m_lts_data	=0;
u8_t  m_osd_data	=0;
u8_t  m_lag_data	=0;
u8_t  m_iro_data	=0;
u8_t  m_tch_data	=0;
u8_t  m_apd_data	=0;
//u8_t  m_osr_data	=0;
//u8_t  m_psi_data	=0;
//u8_t  m_dps_data	=0;
//u8_t  m_dpa_data	=0;
//u8_t  m_dp_strech_data=0;

//u8_t  m_bls_data;
//u8_t  m_fre_data;
extern u8_t  m_asp_data;
extern u8_t  m_src_mode_data;

//u8_t  m_asp_mode;
u8_t  m_pw_save_mode;


u8_t ssi_uart(u8_t varid, data_value *vp)
{
	u8_t select_chk;	

	// exit from contorl code oxf2
	switch(hs->row_num)
	{
		case ROW_PWR_SAVE+4:
		return ERROR;
		
	}


	vp->value.string ="";
	vp->type = string;

	
    switch (varid)
    {
//    	case  CGI_UART_HDMI_AUDIO:
//			    if (m_asp_data>2)
//					vp->value.string ="disabled"; 
//				break;
//    	case  CGI_UART_DISPLAY_AUTO:
//			    if ((m_src_data>2) || (m_hms_data>1) || (m_vms_data>1))
//					vp->value.string ="disabled"; 
//				break;
//		case  CGI_UART_DISPLAY_43:
//			    if (m_dps_data==0)
//					vp->value.string ="disabled"; 
//				break;
//    	case  CGI_UART_SYNC_ASPCT:
//			    if ((m_sync_data==0) || (m_tch_data==1) || (m_hms_data>1) || (m_vms_data>1))
//					vp->value.string ="disabled"; 
//				break;
		case  CGI_UART_SYNC_AUTO_ADJ:	
			    if ((m_sync_data==0) || (m_src_data!=4) || (m_hms_data>1) || (m_vms_data>1))
					vp->value.string ="disabled"; 
				break;
//    	case  CGI_UART_SYNC:
//			    if (m_sync_data==0)
//					vp->value.string ="disabled"; 
//				break;
    
//		case CGI_UART_PLK_1:
//		case CGI_UART_PLK_2:
//			select_chk= varid-CGI_UART_PLK_1;
//			if(select_chk==m_plk_data) { 
//				vp->value.string ="Selected"; 
//			}	
			
			break;
			
    	case CGI_UART_MID:			
			vp->type=digits_3_int;
			vp->value.digits_3_int=m_mid_data;
			break;

		//CGI_TB1	-- Power Saving --
    	case CGI_UART_PWS_VAL:
    			if(hs->row_num==1)
					hs->row_num=ROW_PWR_SAVE;
				if(m_pw_save_mode==0){
					if(hs->row_num==ROW_PWR_SAVE) {
						hs->row_num=ROW_PWR_SAVE+2;
					}
				}	
				else {
					if(hs->row_num==(ROW_PWR_SAVE+2))
						hs->row_num=ROW_PWR_SAVE+3;
				}
        		vp->value.digits_3_int=pwr_save_v[hs->row_num-ROW_PWR_SAVE];
   		  		vp->type=digits_3_int;    		
    			break;
		case CGI_UART_PWS_SELECT:
				select_chk= pwr_save_v[hs->row_num-ROW_PWR_SAVE];
				if(select_chk>1)
				   select_chk--;	
				if(select_chk==m_pws_data)
					vp->value.string=selected;
				break;						
			
    	case CGI_UART_PWS_DISP:    		
    			vp->value.string=pwr_diso_n[hs->row_num-ROW_PWR_SAVE];
    			break;
		//CGI_TB1

						
		case CGI_UART_IMR_1:
		case CGI_UART_IMR_2:
			select_chk= varid-CGI_UART_IMR_1;
			if(select_chk==m_imr_data) { 
				vp->value.string ="Selected"; 
			}				
			break;
			
		case CGI_UART_LTS_1:
		case CGI_UART_LTS_2:
			select_chk= varid-CGI_UART_LTS_1;
			if(select_chk==m_lts_data) { 
				vp->value.string ="Selected"; 
			}				
			break;

		case CGI_UART_OSD_1:
		case CGI_UART_OSD_2:
			select_chk= varid-CGI_UART_OSD_1;
			if(select_chk==m_osd_data) { 
				vp->value.string ="Selected"; 
			}				
			break;

		case CGI_UART_LAG_1:
		case CGI_UART_LAG_2:
		case CGI_UART_LAG_3:
		case CGI_UART_LAG_4:
		case CGI_UART_LAG_5:
		case CGI_UART_LAG_6:
		case CGI_UART_LAG_7:
		case CGI_UART_LAG_8:
		case CGI_UART_LAG_9:
//		case CGI_UART_LAG_10:
			select_chk= varid-CGI_UART_LAG_1;
			if(select_chk==m_lag_data) { 
				vp->value.string ="Selected"; 
			}				
			break;


		case CGI_UART_IRO_1:
		case CGI_UART_IRO_2:
			select_chk= varid-CGI_UART_IRO_1;
			if(select_chk==m_iro_data) { 
				vp->value.string ="Selected"; 
			}				
			break;

		case CGI_UART_TCH_CONTROL:
			if(m_src_mode_data==0)
				vp->value.string ="disabled";
			else
				vp->value.string ="";
			break;	
		
		case CGI_UART_TCH_1:
		case CGI_UART_TCH_2:
			select_chk= varid-CGI_UART_TCH_1;
			if(select_chk==m_tch_data) { 
				vp->value.string ="Selected"; 
			}				
			break;
		
		
		case CGI_UART_APD_1:
		case CGI_UART_APD_2:
			select_chk= varid-CGI_UART_APD_1;
			if(select_chk==m_apd_data) { 
				vp->value.string ="Selected"; 
			}				
			break;

		
//		case CGI_UART_OSR_1:
//		case CGI_UART_OSR_2:
//			select_chk= varid-CGI_UART_OSR_1;
//			if(select_chk==m_osr_data) { 
//				vp->value.string ="Selected"; 
//			}				
//			break;

//		case CGI_UART_PSI_1:
//		case CGI_UART_PSI_2:
//			select_chk= varid-CGI_UART_PSI_1;
//			if(select_chk==m_psi_data) { 
//				vp->value.string ="Selected"; 
//			}				
//			break;
		
//		case CGI_UART_DPS_1:
//		case CGI_UART_DPS_2:
//		case CGI_UART_DPS_3:	
//			select_chk= varid-CGI_UART_DPS_1;
//			if(select_chk==m_dps_data) { 
//				vp->value.string ="Selected"; 
//			}				
//			break;

				
//		case CGI_UART_BLS_1:
//		case CGI_UART_BLS_2:
//			select_chk= varid-CGI_UART_BLS_1;
//			if(select_chk==m_bls_data) { 
//				vp->value.string ="Selected"; 
//			}	
//			break;


//		case CGI_UART_FRE_1:
//		case CGI_UART_FRE_2:
//			select_chk= varid-CGI_UART_FRE_1;
//			if(select_chk==m_fre_data) { 
//				vp->value.string ="Selected"; 
//			}	
//			break;


		
//		case CGI_UART_HMA_1:
//		case CGI_UART_HMA_2:
//			select_chk= varid-CGI_UART_HMA_1;
//			if(select_chk==m_hpa_data) { 
//				vp->value.string ="Selected"; 
//			}	
//			break;

		
//		case CGI_UART_ASP_1:
//		case CGI_UART_ASP_2:
//		case CGI_UART_ASP_3:
//		case CGI_UART_ASP_4:
//		case CGI_UART_ASP_5:
//		case CGI_UART_ASP_6:	
//		case CGI_UART_ASP_7:		
//			select_chk= varid-CGI_UART_ASP_1;
//			if(select_chk==m_asp_data) { 
//				vp->value.string ="Selected"; 
//			}
//			else if (m_asp_mode==0){
//				if(varid>CGI_UART_ASP_4)
//					vp->value.string ="hidden";	
//				
//			}
//			else  { 
//				if(varid<CGI_UART_ASP_5)
//					vp->value.string ="hidden";	 
//			}	
//			break;

					
//		case CGI_UART_DPA_1:
//		case CGI_UART_DPA_2:
//			select_chk= varid-CGI_UART_DPA_1;
//			if(select_chk==m_dpa_data) { 
//				vp->value.string ="Selected"; 
//			}				
//			break;

	
		
//		case CGI_UART_DPS_S_1:
//		case CGI_UART_DPS_S_2:
//			select_chk= varid-CGI_UART_DPS_S_1;
//			if(select_chk==m_dp_strech_data) { 
//				vp->value.string ="Selected"; 
//			}				
//			break;
			
    }
    return OK;
}

u8_t cgi_uart(u8_t cnt, void *vptr)
{
	req_data *vp = vptr;
	int i;
	eSCALER_ST eCMDStatus=GetTVState();
	
	TelnetData.pEEPROM_UART->delimiter_en=0;
	TelnetData.pEEPROM_UART->delimiter_drop=0;
	if(eCMDStatus==eScaler_PowerOn){
    for (i=0; i < cnt; i++, vp++){

		
//		if(!strcmp(vp->item, "dps_s_sel")){
//			m_dp_strech_data=atoi(vp->value);
//			eCMDStatus=SendTVSetCommand('s',TV_CMD_DISP_4_3,(m_dp_strech_data));
//		}	

				
//		if(!strcmp(vp->item, "dpa_sel")){
//			m_dpa_data=atoi(vp->value);
//			eCMDStatus=SendTVSetCommand('s',TV_CMD_DISP_AUTO,(m_dpa_data));
//		}	
		
		
//		if(!strcmp(vp->item, "dps_sel")){
//			m_dps_data=atoi(vp->value);
//			eCMDStatus=SendTVSetCommand('s',TV_CMD_DISP_SETTINGS,(m_dps_data));
//		}	
		
					
//		if(!strcmp(vp->item, "psi_sel")){
//			m_psi_data=atoi(vp->value);
//			eCMDStatus=SendTVSetCommand('s',TV_CMD_PWR_SAVEING_ID,(m_psi_data));
//		}	
		
//		if(!strcmp(vp->item, "osr_sel")){
//			m_apd_data=atoi(vp->value);
//			eCMDStatus=SendTVSetCommand('s',TV_CMD_OSD_ROTATION,(m_osr_data));
//		}	
		
		if(!strcmp(vp->item, "apd_sel")){
			m_apd_data=atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_AUTO_POWER_DOWN,(m_apd_data));
		}	
				
		
		if(!strcmp(vp->item, "tch_sel")){
			m_tch_data=atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_TOUCH_FEATURE,(m_tch_data));
		}	
		
		
		if(!strcmp(vp->item, "iro_sel")){
			m_iro_data=atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_IR_OUT,(m_iro_data));
		}	

		
		if(!strcmp(vp->item, "lag_sel")){
			m_lag_data=atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_LANGUAGE,(m_lag_data));
		}	

		if(!strcmp(vp->item, "osd_sel")){
			m_osd_data=atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_OSD_INFO_BOX,(m_osd_data));
		}	

		
		if(!strcmp(vp->item, "lts_sel")){
			m_lts_data=atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_AMBIENT_LT_SNS,(m_lts_data));
		}	
		
		if(!strcmp(vp->item, "imr_sel")){
			m_imr_data=atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_IMAGE_RETENTION,(m_imr_data));
		}	
		
		if(!strcmp(vp->item, "pws_sel")){
			m_pws_data=atoi(vp->value);
			if(m_pws_data>1)
			   m_pws_data--;
			eCMDStatus=SendTVSetCommand('s',TV_CMD_POWER_SAVE,(m_pws_data));
		}	
//		if(!strcmp(vp->item, "plk_sel")){
//			m_plk_data=atoi(vp->value);
//			eCMDStatus=SendTVSetCommand('s',TV_CMD_PANEL_LOCK,(m_plk_data));
//		}	
		        	
		if(!strcmp(vp->item, "mid_val")){
			m_mid_data=atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_MONITOR_ID,(m_mid_data));
		}	
		
		if(!strcmp(vp->item, "aae_act")){
			eCMDStatus=SendTVSetCommand('s',TV_CMD_AUTO_EXCUTE,0);
		}	
		
		if(!strcmp(vp->item, "are_act")){
			eCMDStatus=SendTVSetCommand('s',TV_CMD_ALL_RESET,0);
			SendGetAllSettingCommand();
			if(eCMDStatus!=eScaler_RS232_No_Service)
			   eCMDStatus=eScaler_Mode_Change;
		}	
		
//		if(!strcmp(vp->item, "bls_sel")){
//			m_bls_data= atoi(vp->value);
//			eCMDStatus=SendTVSetCommand('s',TV_CMD_BLUE_SCREEN,(m_bls_data));
//		}	

//		if(!strcmp(vp->item, "fre_sel")){
//			m_fre_data= atoi(vp->value);
//			eCMDStatus=SendTVSetCommand('s',TV_CMD_FREEZE,(m_fre_data));
//		}	
				
//		if(!strcmp(vp->item, "asp_sel")){
//			m_asp_data= atoi(vp->value);
//			eCMDStatus=SendTVSetCommand('s',TV_CMD_ASPECT,(m_asp_data));
//		}	

		
//		if(!strcmp(vp->item, "hma_sel")){
//			m_hpa_data= atoi(vp->value);
//			eCMDStatus=SendTVSetCommand('s',TV_CMD_HDMI_PC_AUDIO,(m_hpa_data));
//		}	
		
    }//for (i=0; i < cnt; i++, vp++)
	}
	if(eCMDStatus==eScaler_Standy){		
		hs->errormsg = "Standby!";
		errmsgflag = ERRORMSG_WAIT5;
		httpd_init_file(fs_open("NoService3.htm",&hs->op_index));    
	}	
	else if(eCMDStatus==eScaler_Mode_Change){
		hs->errormsg = "Change the Setting";
		errmsgflag = ERRORMSG_WAIT3;		
		httpd_init_file(fs_open("NoService3.htm",&hs->op_index));
	}	
	else{	
    	httpd_init_file(fs_open("uart.htm",&hs->op_index));
	}
  	//UART_Set_Property();
	//UART_Clear_RTS();
  	//SaveModuleInfo2EEPROM(TelnetData.pEEPROM_UART,sizeof(SUARTInfo));
    //httpd_init_file(fs_open("uart.htm",&hs->op_index));
    return OK;
}
#endif

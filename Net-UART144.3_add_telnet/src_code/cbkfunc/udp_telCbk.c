#include <module.h>
#ifdef MODULE_CLI_CUSTOMIAZE
#else
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
///#ifdef MODULE_NET_UART		//20081215 joe for module netuart testing
#include "net_uart.h"
//#endif //MODULE_NET_UART
#ifdef MODULE_UDP_TELNET
#include "..\HtmCfiles\udp_tel_htm.h"
#endif //MODULE_UDP_TELNET

#include "..\HttpServer\application.h"

extern u8_t errmsgflag ;
extern u8_t m_tch_data;

void eepromerr();
extern struct httpd_info *hs;
#define IPB1 0
#define IPB2 1
#define IPB3 2
#define IPB4 3
#define IPE1 4
#define IPE2 5
#define IPE3 6
#define IPE4 7
#define IPP1 8

u8_t  m_hms_data=1;
u8_t  m_vms_data=1;
u8_t  m_hps_data=1;
u8_t  m_vps_data=1;
u8_t  m_frc_data;
u8_t  m_led_data;
u8_t  m_pod_data;
u8_t  m_cdp_data;

u8_t  m_asp_data;
u8_t  m_asp_mode;
u8_t  m_fre_data;
u8_t  m_bls_data;


extern u8_t  m_sync_data  ;
extern u8_t  m_tch_data	  ;


u8_t ssi_udp_tel(u8_t varid, data_value *vp)
{
	u8_t index=hs->row_num-1;
	u8_t ee_value=0, i=0;
	u16_t ee_port=0;
	u8_t name_buf[31]=0;
	u8_t select_chk;	
	if(hs->row_num>UDP_TEL_MAX_IP_RANGE)return ERROR;
	vp->value.string ="";
    vp->type = string;
	switch(varid)
	{

    	case  CGI_UDP_TEL_TOUCH_H:
			    if ((m_tch_data==1)||(m_hms_data==1))
					vp->value.string ="disabled"; 
				break;

    	case  CGI_UDP_TEL_TOUCH_V:
			    if ((m_tch_data==1)||(m_vms_data==1))
					vp->value.string ="disabled"; 
				break;
		
    	case  CGI_UDP_TEL_TOUCH_HV:
			    if ((m_tch_data==1) ||((m_vms_data==1)&&(m_hms_data==1)))
					vp->value.string ="disabled"; 
				break;
	    						
							
		case CGI_UDP_TEL_FRC_1:
		case CGI_UDP_TEL_FRC_2:
			select_chk= varid-CGI_UDP_TEL_FRC_1;
			if(select_chk==m_frc_data) { 
				vp->value.string ="Selected"; 
			}	
			break;


		case CGI_UDP_TEL_LED_1:
		case CGI_UDP_TEL_LED_2:
			select_chk= varid-CGI_UDP_TEL_LED_1;
			if(select_chk==m_led_data) { 
				vp->value.string ="Selected"; 
			}	
			break;

		case CGI_UDP_TEL_POD_1:
		case CGI_UDP_TEL_POD_2:
			select_chk= varid-CGI_UDP_TEL_POD_1;
			if(select_chk==m_pod_data) { 
				vp->value.string ="Selected"; 
			}	
			break;

			
		case CGI_UDP_TEL_VPS:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_vps_data;
				break;	

		case CGI_UDP_TEL_HPS:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_hps_data;
				break;	
				
		case CGI_UDP_TEL_VMS:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_vms_data;
				break;	
				
		case CGI_UDP_TEL_HMS:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_hms_data;
				break;	

		case CGI_UDP_TEL_CDP_1:
		case CGI_UDP_TEL_CDP_2:
			select_chk= varid-CGI_UDP_TEL_CDP_1;
			if(select_chk==m_cdp_data) { 
				vp->value.string ="Selected"; 
			}				
			break;
		
		case CGI_UDP_TEL_ASP_1:
		case CGI_UDP_TEL_ASP_2:
		case CGI_UDP_TEL_ASP_3:
		case CGI_UDP_TEL_ASP_4:
		case CGI_UDP_TEL_ASP_5:
		case CGI_UDP_TEL_ASP_6:	
			select_chk= varid-CGI_UDP_TEL_ASP_1;
			if (m_asp_mode==0){
				
				if(varid==CGI_UDP_TEL_ASP_4)
					vp->value.string ="hidden";	
				else if(varid==CGI_UDP_TEL_ASP_5)
					vp->value.string ="hidden";	
				else if(select_chk==m_asp_data) 
					vp->value.string ="Selected"; 
				
			}
			else  { 
				if(varid==CGI_UDP_TEL_ASP_1)
					vp->value.string ="hidden";	
				else if(varid==CGI_UDP_TEL_ASP_2)
					vp->value.string ="hidden";
				else if(varid==CGI_UDP_TEL_ASP_3)
					vp->value.string ="hidden";
				else if(select_chk==m_asp_data) 
					vp->value.string ="Selected"; 
			}	
			break;
		
		case CGI_UDP_TEL_FRE_1:
		case CGI_UDP_TEL_FRE_2:
			select_chk= varid-CGI_UDP_TEL_FRE_1;
			if(select_chk==m_fre_data) { 
				vp->value.string ="Selected"; 
			}	
			break;
		
		case CGI_UDP_TEL_BLS_1:
		case CGI_UDP_TEL_BLS_2:
			select_chk= varid-CGI_UDP_TEL_BLS_1;
			if(select_chk==m_bls_data) { 
				vp->value.string ="Selected"; 
			}	
			break;
		case  CGI_UDP_TEL_SYNC_ASPCT:
			if ((m_sync_data==0) || (m_tch_data==1) || (m_hms_data>1) || (m_vms_data>1))
				vp->value.string ="disabled"; 
			break;

		
    	case  CGI_UDP_TEL_SYNC:
			    if (m_sync_data==0)
					vp->value.string ="disabled"; 
				break;


					
				
	}
    return OK;
}
//extern void CGIstrcpy(char* str,char* cgistr);
u8_t cgi_udp_tel(u8_t cnt, void *vptr)
{
	u8_t i=0,begin=0, j=0, count=0;
	u8_t ee_value=0;
	u16_t ee_port=0;
	req_data* vp=vptr;
	eSCALER_ST eCMDStatus=GetTVState();
	//NetUartTelnetClose();
	if(eCMDStatus==eScaler_PowerOn){
	for(i=0;i<cnt;i++,vp++)
	{
		
		if(!strcmp(vp->item, "hms_val")){
			 m_hms_data=atoi(vp->value);
		}	
		if(!strcmp(vp->item, "hms_act")){
			if((*vp->value)=='1') 	m_hms_data++;
			if((*vp->value)=='0') 	m_hms_data--;
			eCMDStatus=SendTVSetCommand('s',TV_CMD_H_MONITOR,(m_hms_data));
			if(m_hps_data>m_hms_data){
				m_hps_data=m_hms_data;
				SendTVSetCommand('s',TV_CMD_H_POSITION,(m_hps_data));
			}
			if((m_hms_data==1)&&(m_vms_data==1)) {
				m_frc_data=0;
				m_led_data=0;
				m_pod_data=0;
				eCMDStatus=SendTVSetCommand('s',TV_CMD_FRAME_COMP,(m_frc_data));
				eCMDStatus=SendTVSetCommand('s',TV_CMD_DISP_WALL_LED,(m_led_data)); 
				eCMDStatus=SendTVSetCommand('s',TV_CMD_DISP_WALL_PW_ON,(m_pod_data));
			}	
    	}	

		
		if(!strcmp(vp->item, "vms_val")){
			 m_vms_data=atoi(vp->value);
		}	
		if(!strcmp(vp->item, "vms_act")){
			if((*vp->value)=='1') 	m_vms_data++;
			if((*vp->value)=='0') 	m_vms_data--;
			eCMDStatus=SendTVSetCommand('s',TV_CMD_V_MONITOR,(m_vms_data));			
			if(m_vps_data>m_vms_data){
				m_vps_data=m_vms_data;
				SendTVSetCommand('s',TV_CMD_V_POSITION,(m_vps_data));
			}				
			if((m_hms_data==1)&&(m_vms_data==1)) {
				m_frc_data=0;
				m_led_data=0;
				m_pod_data=0;
				eCMDStatus=SendTVSetCommand('s',TV_CMD_FRAME_COMP,(m_frc_data));
				eCMDStatus=SendTVSetCommand('s',TV_CMD_DISP_WALL_LED,(m_led_data)); 
				eCMDStatus=SendTVSetCommand('s',TV_CMD_DISP_WALL_PW_ON,(m_pod_data));
			}	
    	}	

		if(!strcmp(vp->item, "hps_val")){
			 m_hps_data=atoi(vp->value);
		}			
		if(!strcmp(vp->item, "hps_act")){
			if((*vp->value)=='1') 	m_hps_data++;
			if((*vp->value)=='0') 	m_hps_data--;
			eCMDStatus=SendTVSetCommand('s',TV_CMD_H_POSITION,(m_hps_data));
    	}	

		if(!strcmp(vp->item, "vps_val")){
			 m_vps_data=atoi(vp->value);
		}					
		if(!strcmp(vp->item, "vps_act")){
			if((*vp->value)=='1') 	m_vps_data++;
			if((*vp->value)=='0') 	m_vps_data--;
			eCMDStatus=SendTVSetCommand('s',TV_CMD_V_POSITION,(m_vps_data));
    	}	
		if(!strcmp(vp->item, "frc_sel")){
			m_frc_data= atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_FRAME_COMP,(m_frc_data));
		}	
		
		if(!strcmp(vp->item, "led_sel")){
			m_led_data= atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_DISP_WALL_LED,(m_led_data)); 
		}	

		if(!strcmp(vp->item, "pod_sel")){
			m_pod_data= atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_DISP_WALL_PW_ON,(m_pod_data));
		}	

		
		if(!strcmp(vp->item, "cdp_sel")){
			m_cdp_data=atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_CLOCK_DISP,(m_cdp_data));
		}	

		
		if(!strcmp(vp->item, "bls_sel")){
			m_bls_data= atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_BLUE_SCREEN,(m_bls_data));
		}	

		if(!strcmp(vp->item, "fre_sel")){
			m_fre_data= atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_FREEZE,(m_fre_data));
		}	
				
		if(!strcmp(vp->item, "asp_sel")){
			m_asp_data= atoi(vp->value);
			eCMDStatus=SendTVSetCommand('s',TV_CMD_ASPECT,(m_asp_data));
		}	
				
		
	}
	}
//	NetUartInit();
	//SaveModuleInfo2EEPROM(&ModuleInfo.UDPTelInfo, sizeof(udp_tel_info_t));
	//SaveModuleInfo2EEPROM(TelnetData.pEEPROM_Telnet, sizeof(STelnetInfo));
	//SaveModuleInfo2EEPROM(&ModuleInfo.dido_info, sizeof(ModuleInfo.dido_info));
	//httpd_init_file(fs_open("udp_tel.htm",&hs->op_index));
	
	//hs->errormsg = "RESET to effect";
	//errmsgflag = ERRORMSG_SETOK;
	//httpd_init_file(fs_open("errormsg.htm",&hs->op_index));
	
	if(eCMDStatus==eScaler_Standy){		
		hs->errormsg = "Standby!";
		errmsgflag = ERRORMSG_WAIT5;
		httpd_init_file(fs_open("NoService2.htm",&hs->op_index));    
	}	
	else if(eCMDStatus==eScaler_Mode_Change){
		hs->errormsg = "Change the Setting";
		errmsgflag = ERRORMSG_WAIT3;		
		httpd_init_file(fs_open("NoService2.htm",&hs->op_index));
	}	
	else{	
    	httpd_init_file(fs_open("udp_tel.htm",&hs->op_index));
	}
	
    return OK;
}
#endif

#include <module.h>
#ifdef MODULE_64K_HTM	//for CLI 64K
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
#include "..\HtmCfiles\telnet_htm.h"
#ifdef MODULE_NET_UART
#include "net_uart.h"
#endif //MODULE_NET_UART
extern u8_t errmsgflag ;
#include "..\smtp\smtp.h"

#include "telnet.h"
#include "..\HttpServer\application.h"
#include "..\net_uart\dhcpc.h"
#include "option.h"


void eepromerr();
extern struct httpd_info *hs;
extern u8_t force_reset_countdown;
extern u8_t DHCP_get_ip;
extern u8_t m_touch_mode_data;

 u8_t  m_sync_data  ;
 u8_t  m_src_mode_data	;
 u8_t  m_src_data	;
 u8_t  m_pic_data	;
 u8_t  m_cot_data	;
// u8_t  m_nra_data	;
// u8_t  m_nrm_data	;
// u8_t  m_nrd_data	;


 u8_t  m_bkl_data	;
 u8_t  m_ct_data	;
 u8_t  m_br_data	;
 int   m_sr_data	;
 int   m_hue_data	;
 int   m_sat_data	;
 u8_t  m_ctb_data	;
// u8_t  m_m3d_data	;
// u8_t  m_abc_data	;



 u8_t  m_sm_data	;
 int   m_tre_data	;
 int   m_bas_data	;
 int   m_bal_data	;

 u8_t  m_sur_data	;
 u8_t  m_mut_data	;
 //u8_t  m_aud_data	;
 

 u8_t  m_vol_data	;
// int   m_alo_data	=-5;



//Screen
extern u8_t  m_hms_data;
extern u8_t  m_vms_data;
extern u8_t  m_hps_data;
extern u8_t  m_vps_data;
extern u8_t  m_frc_data;
extern u8_t  m_led_data;
extern u8_t  m_pod_data;

extern u8_t  m_bls_data;
extern u8_t  m_fre_data;
extern u8_t  m_asp_data;
extern u8_t  m_asp_mode;

//Setting
extern u8_t  m_mid_data ;
//extern u8_t  m_plk_data ;
extern u8_t  m_pws_data	;
extern u8_t  m_imr_data	;
extern u8_t  m_lts_data	;
extern u8_t  m_osd_data	;
extern u8_t  m_lag_data	;
u8_t  m_act_data	;
extern u8_t  m_iro_data	;
//extern u8_t  m_ovs_data	;
//extern u8_t  m_irc_data	;
extern u8_t  m_tch_data	;
//extern u8_t  m_bir_data	;
extern u8_t  m_cdp_data	;
extern u8_t  m_apd_data	;
//extern u8_t  m_osr_data	;
//extern u8_t  m_psi_data	;
//extern u8_t  m_dps_data	;
//extern u8_t  m_aad_data	;
//extern u8_t  m_dpa_data	;
//extern u8_t  m_btc_data	;
//extern u8_t  m_dp_strech_data;
u8_t  m_hpa_data	;

//Schedule
extern u16_t m_rtc_year	;
extern u8_t  m_rtc_month;
extern u8_t  m_rtc_date	;
extern u8_t  m_rtc_hour	;
extern u8_t  m_rtc_min	;
extern u8_t  tmier1[9];
extern u8_t  tmier2[9];
extern u8_t  tmier3[9];
extern u8_t  tmier4[9];

//Info
extern u8_t  m_mode_name[15];
extern u8_t  m_firmware_version[15];
extern u8_t  m_serial_number[15];
extern u16_t m_operation_time_H;
extern u16_t m_operation_time_L;
extern u8_t  m_pwr_data;


extern u8_t m_smtp_day;
extern u8_t m_smtp_hour;
extern u8_t m_smtp_min;
extern u8_t m_smtp_check;

extern SSMTPData SMTPData;

extern u8_t smtp_send_mail(u8_t MailNo);

extern u8_t  m_pw_save_mode;


u8_t code m_GetTVData[]={
 //Info
 TV_GET_POWER,				TV_GET_OPERATION_TIME,
 
 //Picture&Sound	
 TV_GET_LAN_GET_MODE,
 TV_GET_SRC, 				
 TV_GET_PIC_MODE, 		
 TV_GET_BACKLIGHT,			TV_GET_CONTRAST,		TV_GET_BRIGHTNESS, 		TV_GET_SHARPNESS,		TV_GET_COLOR_TEMP,
 TV_GET_HUE,				TV_GET_SATURATION,
 TV_GET_CONTRAST_BOOSTER,	TV_GET_ADAPTIVE_CONT,
 TV_GET_SOUND_MODE,
 TV_GET_TREBLE,     		TV_GET_BASS,			TV_GET_BALANCE,			TV_GET_VOLUME,			TV_GET_MUTE,
 TV_GET_HDMI_PC_AUDIO,	
 
 //Application
 TV_GET_H_MONITOR,			TV_GET_V_MONITOR,		TV_GET_H_POSITION,		TV_GET_V_POSITION,		TV_GET_FRAME_COMP,
 TV_GET_DISP_WALL_LED,		TV_GET_DISP_WALL_PW_ON, TV_GET_CLOCK_DISP,      TV_GET_ASPECT_MODE,     TV_GET_ASPECT,
 TV_GET_BLUE_SCREEN,
 
 
 //Set Up
 TV_GET_MONITOR_ID,			TV_GET_POWER_SAVE,		TV_GET_AMBIENT_LT_SNS,	TV_GET_LANGUAGE,		TV_GET_IR_OUT,
 TV_GET_AUTO_POWER_DOWN,	
 TV_GET_IMAGE_RETENTION,	TV_GET_OSD_INFO_BOX,	
 TV_GET_TOUCH_FEATURE,		
 					
 
 
 //Schedule
 TV_GET_RTC_YEAR,			TV_GET_RTC_MONTH,		TV_GET_RTC_DAY,			TV_GET_RTC_HOUR,		TV_GET_RTC_MINUTE,	
 0xff
};


#define ALERT_MSG_01 "Temperature Alert"
#define ALERT_MSG_02 "Monitor ID: "
#define ALERT_MSG_03 "Degree: "


#define SEND_MSG_01 "Email Test"
#define SEND_MSG_02 "Monitor ID: "


char *uitos3Digital(u16_t int_value)
{
	int i=0;
	int k=0;
	static char str[7];
    int base=100,last_div=0,div=0;

	
	for(i=k;base>0;i++)
	{
		div=int_value/base;
		str[i]='0'+(div-last_div*10);
		last_div=div;
		base/=10;
	}
	if(i!=0)str[i]='\0';
	else strcpy(str,"0");
	return str;
}

eSCALER_ST m_eScaler_State=eScaler_Init;
u8_t	   m_ReGetScalerData;	
char TVSetCMD[24];

void Send_Status_Report_Message(u8_t cType, u8_t * pData)
{
		u8_t *pByte;
		u8_t *pSourceName;
		u8_t CRLFS[]="<br>";
		
		if(EEConfigInfo.DHCPEnable!=sDHCP_DISABLE) {
				if(DHCP_get_ip==1){
					pByte=(u8_t *)&DHCPCInfo.MyIPAddr;
				}
				else
					pByte = EEConfigInfo.IPAddress;
		}	
		else
		{	pByte = EEConfigInfo.IPAddress;	}


		switch(m_src_data)
		{
			case 0		: pSourceName = "HDMI1";	break;
			case 1		: pSourceName = "HDMI2";	break;
			case 2		: pSourceName = "HDMI3";	break;
			case 3		: pSourceName = "Mode";		break;
			case 4		: pSourceName = "VGA"; 		break;
			case 5		: pSourceName = "YPbPr";	break;
			default 	: pSourceName = "AV";		break;
		}


			
		if(cType==0)	
			sprintf(pData, "Status Report%sModel Name:%s%sSerial No:%s%sInput:%s%sMonitor id:%d%sIP Address:%d.%d.%d.%d%sF/W%s; %s%sOperation:%03d%04dH%s",CRLFS,m_mode_name,CRLFS,
																																						   m_serial_number,CRLFS,
																																						   pSourceName,CRLFS,
																							  															  (u16_t)m_mid_data,CRLFS,
																							  															  (u16_t)pByte[0],
																							  															  (u16_t)pByte[1],
																							  															  (u16_t)pByte[2],
																							  															  (u16_t)pByte[3],CRLFS,
																							  															  m_firmware_version,
		 																					  															  WEB_CODE_VERSION,CRLFS,
																								  														  m_operation_time_H,
																							  															  m_operation_time_L,CRLFS);
		else if(cType==1)
			sprintf(pData, "Thermal Error%sModel Name:%s%sSerial No:%s%sInput:%s%sMonitor id:%d%sIP Address:%d.%d.%d.%d%sF/W%s; %s%sOperation:%03d%04dH%s",CRLFS,m_mode_name,CRLFS,
																																						   m_serial_number,CRLFS,
																																						   pSourceName,CRLFS,
																							  															  (u16_t)m_mid_data,CRLFS,
																							  															  (u16_t)pByte[0],
																							  															  (u16_t)pByte[1],
																							  															  (u16_t)pByte[2],
																							  															  (u16_t)pByte[3],CRLFS,
																							  															  m_firmware_version,
		 																					  															  WEB_CODE_VERSION,CRLFS,
																								  														  m_operation_time_H,
																							  															  m_operation_time_L,CRLFS);
		else if(cType==2)
			sprintf(pData, "Mail Test%sModel Name:%s%sSerial No:%s%sInput:%s%sMonitor id:%d%sIP Address:%d.%d.%d.%d%sF/W%s; %s%sOperation:%03d%04dH%s",CRLFS,m_mode_name,CRLFS,
																																						   m_serial_number,CRLFS,
																																						   pSourceName,CRLFS,
																							  															  (u16_t)m_mid_data,CRLFS,
																							  															  (u16_t)pByte[0],
																							  															  (u16_t)pByte[1],
																							  															  (u16_t)pByte[2],
																							  															  (u16_t)pByte[3],CRLFS,
																							  															  m_firmware_version,
		 																					  															  WEB_CODE_VERSION,CRLFS,
																								  														  m_operation_time_H,
																							  															  m_operation_time_L,CRLFS);
		

		
		
}


void Send_Message(u8_t * pData)
{
		u16_t MoniterID;
		MoniterID=m_mid_data;
		
		sprintf(pData, "%s\n%s%d\n%",SEND_MSG_01,SEND_MSG_02,MoniterID);
}

void Alert_Message(u8_t * pData,u8_t value)
{
	u16_t MoniterID;
	u16_t Degree;
	MoniterID=m_mid_data;
	Degree=value;
	
	sprintf(pData, "%s\n%s%d\n%s%d\n",ALERT_MSG_01,ALERT_MSG_02,MoniterID,ALERT_MSG_03,Degree);
}

eSCALER_ST GetTVState(void)
{
	return m_eScaler_State;
}

u8_t init_cmd;
u16_t init_timer_count;

eSCALER_ST SendTVReplyNetWorkCommand(u8_t value1,u8_t *pValue2,u8_t len)
{
	u16_t k,j;
	if(m_eScaler_State==eScaler_RS232_No_Service)
		return eScaler_RS232_No_Service;
	
	{
		TVSetCMD[0]=0x3e;
		TVSetCMD[1]='0';
		TVSetCMD[2]='1';
		TVSetCMD[3]='r';
		TVSetCMD[4]=TV_CMD_NETWORK_SETTING;
		// Data 
		TVSetCMD[5] =value1;
		for(j=0;j<8;j++){
			if(j<len)
			  	TVSetCMD[6+j] = pValue2[j];
			else
				TVSetCMD[6+j] =0;
		}

		for(j=0;j<14;j++){
			k=UartTxBuf.wi+j;
			if(k>=UartTxBuf.MaxLen){
				k=k-UartTxBuf.MaxLen;	
			}	
			UartTxBuf.buf[k]=TVSetCMD[j];
		}	
		RingIncWi(UartTxBuf,14);	
		j=TV_CMD_NETWORK_SETTING;
		printf("send wi=%d ri=%d tal=%d max=%d cmd=>01r_%02x_\n",UartTxBuf.wi,UartTxBuf.ri,UartTxBuf.TotalLen,UartTxBuf.MaxLen,j);
	}	
	return m_eScaler_State;

}


eSCALER_ST SendTVGetNetWorkCommand(u8_t cType)
{
	u16_t k,j;
	if(m_eScaler_State==eScaler_RS232_No_Service)
		return eScaler_RS232_No_Service;
	
	{
		TVSetCMD[0]=0x3e;
		TVSetCMD[1]='0';
		TVSetCMD[2]='1';
		TVSetCMD[3]='g';
		TVSetCMD[4]=TV_CMD_NETWORK_SETTING;
		// Data 
		TVSetCMD[5] =cType;
		TVSetCMD[6] =0;
		TVSetCMD[7] =0;
		TVSetCMD[8] =0;
		TVSetCMD[9] =0;
		TVSetCMD[10]=0;
		TVSetCMD[11]=0;
		TVSetCMD[12]=0;
		TVSetCMD[13]=0;

		for(j=0;j<14;j++){
			k=UartTxBuf.wi+j;
			if(k>=UartTxBuf.MaxLen){
				k=k-UartTxBuf.MaxLen;	
			}	
			UartTxBuf.buf[k]=TVSetCMD[j];
		}	
		RingIncWi(UartTxBuf,14);	
		j=TV_CMD_NETWORK_SETTING;
		printf("send wi=%d ri=%d tal=%d max=%d cmd=>01g_%02x_\n",UartTxBuf.wi,UartTxBuf.ri,UartTxBuf.TotalLen,UartTxBuf.MaxLen,j);
	}	
	return m_eScaler_State;
}





eSCALER_ST SendTVGetInfoCommand(u8_t TypeNo)
{
	u16_t k,j;
	if(m_eScaler_State==eScaler_RS232_No_Service)
		return eScaler_RS232_No_Service;
	
	{
		TVSetCMD[0]=0x44;
		TVSetCMD[1]='0';
		TVSetCMD[2]='1';
		TVSetCMD[3]='g';
		TVSetCMD[4]=TV_GET_MODEL_INFO;
		// Data 
		TVSetCMD[5] =TypeNo;
		TVSetCMD[6] =0;
		TVSetCMD[7] =0;
		TVSetCMD[8] =0;
		TVSetCMD[9] =0;
		TVSetCMD[10]=0;
		TVSetCMD[11]=0;
		TVSetCMD[12]=0;
		TVSetCMD[13]=0;
		TVSetCMD[14]=0;
		TVSetCMD[15]=0;
		TVSetCMD[16]=0;
		TVSetCMD[17]=0;
		TVSetCMD[18]=0;
		TVSetCMD[19]=0;
		TVSetCMD[20]=0;
		TVSetCMD[21]=0;
		TVSetCMD[22]=0;
		TVSetCMD[23]=0;

		for(j=0;j<20;j++){
			k=UartTxBuf.wi+j;
			if(k>=UartTxBuf.MaxLen){
				k=k-UartTxBuf.MaxLen;	
			}	
			UartTxBuf.buf[k]=TVSetCMD[j];
		}	
		RingIncWi(UartTxBuf,20);	
		j=TV_GET_MODEL_INFO;
		printf("send wi=%d ri=%d tal=%d max=%d cmd=>01g_%02x_\n",UartTxBuf.wi,UartTxBuf.ri,UartTxBuf.TotalLen,UartTxBuf.MaxLen,j);
	}	
	return m_eScaler_State;


}


eSCALER_ST SendTVGetTimerCommand(u8_t TimerNo)
{
	u16_t k,j;

	if(m_eScaler_State==eScaler_RS232_No_Service)
		return eScaler_RS232_No_Service;
	
	{
		TVSetCMD[0]=0x3e;
		TVSetCMD[1]='0';
		TVSetCMD[2]='1';
		TVSetCMD[3]='g';
		TVSetCMD[4]=TV_GET_TIMER_ON_OFF;
		// Data 
		TVSetCMD[5] =TimerNo;
		TVSetCMD[6] =0;
		TVSetCMD[7] =0;
		TVSetCMD[8] =0;
		TVSetCMD[9] =0;
		TVSetCMD[10]=0;
		TVSetCMD[11]=0;
		TVSetCMD[12]=0;
		TVSetCMD[13]=0;

		for(j=0;j<14;j++){
			k=UartTxBuf.wi+j;
			if(k>=UartTxBuf.MaxLen){
				k=k-UartTxBuf.MaxLen;	
			}	
			UartTxBuf.buf[k]=TVSetCMD[j];
		}	
		RingIncWi(UartTxBuf,14);	
		j=TV_GET_TIMER_ON_OFF;
		printf("send wi=%d ri=%d tal=%d max=%d cmd=>01g_%02x_\n",UartTxBuf.wi,UartTxBuf.ri,UartTxBuf.TotalLen,UartTxBuf.MaxLen,j);
	}	
	return m_eScaler_State;
}



eSCALER_ST SendTVSetTimerCommand(u8_t TimerNo, u8_t *SetValue)
{
	u16_t k,j;

	if(m_eScaler_State==eScaler_RS232_No_Service)
		return eScaler_RS232_No_Service;
	
	{
		TVSetCMD[0]=0x3e;
		TVSetCMD[1]='0';
		TVSetCMD[2]='1';
		TVSetCMD[3]='s';
		TVSetCMD[4]=TV_CMD_TIMER_ON_OFF;
		// Data 
		TVSetCMD[5] =(TimerNo | (SetValue[5]<<6)|(SetValue[6]<<5)|(SetValue[7]<<4));
		TVSetCMD[6] =SetValue[4];
		TVSetCMD[7] =SetValue[0];
		TVSetCMD[8] =SetValue[1];
		TVSetCMD[9] =SetValue[2];
		TVSetCMD[10]=SetValue[3];
		TVSetCMD[11]=SetValue[8];
		TVSetCMD[12]=0;
		TVSetCMD[13]=0;

		for(j=0;j<14;j++){
			k=UartTxBuf.wi+j;
			if(k>=UartTxBuf.MaxLen){
				k=k-UartTxBuf.MaxLen;	
			}	
			UartTxBuf.buf[k]=TVSetCMD[j];
		}	
		RingIncWi(UartTxBuf,14);	
		j=TV_CMD_TIMER_ON_OFF;
		printf("send wi=%d ri=%d tal=%d max=%d cmd=>01s_%02x_\n",UartTxBuf.wi,UartTxBuf.ri,UartTxBuf.TotalLen,UartTxBuf.MaxLen,j);
	}	
	return m_eScaler_State;
}


eSCALER_ST Get_smtp_status_day_hour_min(void)
{
	char *pData;
	u16_t k,j;

	if(m_eScaler_State==eScaler_RS232_No_Service)
		return eScaler_RS232_No_Service;
	
	
	{
		TVSetCMD[0]=0x39;
		TVSetCMD[1]='0';
		TVSetCMD[2]='1';
		TVSetCMD[3]='g';
		TVSetCMD[4]=TV_GET_SMTP_TIME;
		TVSetCMD[5]=0x28;
		TVSetCMD[6]=0;
		TVSetCMD[7]=0;
		TVSetCMD[8]=0;
		//TVSetCMD[9] =0;
		//TVSetCMD[10]=0;


		for(j=0;j<9;j++){
			k=UartTxBuf.wi+j;
			if(k>=UartTxBuf.MaxLen){
				k=k-UartTxBuf.MaxLen;	
			}	
			UartTxBuf.buf[k]=TVSetCMD[j];
		}	
		RingIncWi(UartTxBuf,9);	
		j=TV_GET_SMTP_TIME;
		printf("send wi=%d ri=%d tal=%d max=%d cmd=801s_%02x_%s\n",UartTxBuf.wi,UartTxBuf.ri,UartTxBuf.TotalLen,UartTxBuf.MaxLen,j,&TVSetCMD[5]);
	}	
	return m_eScaler_State;


}



eSCALER_ST smtp_status_day_hour_min(u8_t cStatus, u8_t cDay, u8_t cHour,u8_t cMin)
{
	char *pData;
	u16_t k,j;

	if(m_eScaler_State==eScaler_RS232_No_Service)
		return eScaler_RS232_No_Service;
	
	
	{
		TVSetCMD[0]=0x39;
		TVSetCMD[1]='0';
		TVSetCMD[2]='1';
		TVSetCMD[3]='s';
		TVSetCMD[4]=TV_CMD_SMTP_TIME;
		TVSetCMD[5]=0x28;
		if(cStatus==0){
			TVSetCMD[6]=0;
		}
		else{
				switch(cDay){
					case 0: TVSetCMD[6]=1;   break;
					case 1: TVSetCMD[6]=2;   break;
					case 2: TVSetCMD[6]=4;   break;
					case 3: TVSetCMD[6]=8;   break;
					case 4: TVSetCMD[6]=16;  break;
					case 5: TVSetCMD[6]=32;  break;
					case 6: TVSetCMD[6]=64;  break;
					default:TVSetCMD[6]=128; break;
				}
			}
					
		TVSetCMD[7]=cHour;
		TVSetCMD[8]=cMin;


		for(j=0;j<9;j++){
			k=UartTxBuf.wi+j;
			if(k>=UartTxBuf.MaxLen){
				k=k-UartTxBuf.MaxLen;	
			}	
			UartTxBuf.buf[k]=TVSetCMD[j];
		}	
		RingIncWi(UartTxBuf,9);	
		j=TV_CMD_SMTP_TIME;
		printf("send wi=%d ri=%d tal=%d max=%d cmd=801s_%02x_%s\n",UartTxBuf.wi,UartTxBuf.ri,UartTxBuf.TotalLen,UartTxBuf.MaxLen,j,&TVSetCMD[5]);
	}	
	return m_eScaler_State;


}

eSCALER_ST SendTVSetCommand(u8_t cType, u8_t SetCmd, u8_t SetValue)
{
	char *pData;
	u16_t k,j;

	if(m_eScaler_State==eScaler_RS232_No_Service)
		return eScaler_RS232_No_Service;
	
	
	pData =uitos3Digital((u16_t)SetValue);
	//if(UartTxBuf.TotalLen==0)
	{
		TVSetCMD[0]='8';
		TVSetCMD[1]='0';
		TVSetCMD[2]='1';
		TVSetCMD[3]=cType;
		TVSetCMD[4]=SetCmd;
		TVSetCMD[5]=pData[0];
		TVSetCMD[6]=pData[1];
		TVSetCMD[7]=pData[2];
		TVSetCMD[8]=0;

		for(j=0;j<8;j++){
			k=UartTxBuf.wi+j;
			if(k>=UartTxBuf.MaxLen){
				k=k-UartTxBuf.MaxLen;	
			}	
			UartTxBuf.buf[k]=TVSetCMD[j];
		}	
		RingIncWi(UartTxBuf,8);	
		j=SetCmd;
		printf("send wi=%d ri=%d tal=%d max=%d cmd=801%c_%02x_%s\n",UartTxBuf.wi,UartTxBuf.ri,UartTxBuf.TotalLen,UartTxBuf.MaxLen,cType,j,&TVSetCMD[5]);
	}	
	return m_eScaler_State;
}


eSCALER_ST SendGetAllPictureSettingCommand(void)
{
	int i;
	i=0;
	while(m_GetTVData[i]!=TV_GET_PIC_MODE)
		i++;
	while(m_GetTVData[i]!=TV_GET_SOUND_MODE){
		SendTVSetCommand('g', m_GetTVData[i],0);
		i++;
	}
	return m_eScaler_State;
}


eSCALER_ST SendGetAllSoundSettingCommand(void)
{
	int i;
	i=0;
	while(m_GetTVData[i]!=TV_GET_SOUND_MODE)
		i++;
	while(m_GetTVData[i]!=TV_GET_H_MONITOR){
		SendTVSetCommand('g', m_GetTVData[i],0);
		i++;
	}
	return m_eScaler_State;
}


eSCALER_ST SendGetAllSettingCommand(void)
{
	init_cmd=0;
	m_eScaler_State=eScaler_Init_Data;
	return m_eScaler_State;
}



void SendTVACK(void)
{
	u16_t k,j;

	TVSetCMD[0]='4';
	TVSetCMD[1]='0';
	TVSetCMD[2]='1';
	TVSetCMD[3]='+';
	TVSetCMD[4]=0;
	for(j=0;j<4;j++){
		k=UartTxBuf.wi+j;
		if(k>=UartTxBuf.MaxLen){
			k=k-UartTxBuf.MaxLen;	
		}	
		UartTxBuf.buf[k]=TVSetCMD[j];
	}	
	RingIncWi(UartTxBuf,4);
}

eSCALER_ST SendTVGetCommand(u8_t GetCmd)
{
	u16_t k,j;
	
	
	{
		TVSetCMD[0]='8';
		TVSetCMD[1]='0';
		TVSetCMD[2]='1';
		TVSetCMD[3]='g';
		TVSetCMD[4]=GetCmd;
		TVSetCMD[5]='0';
		TVSetCMD[6]='0';
		TVSetCMD[7]='0';
		TVSetCMD[8]=0;

		for(j=0;j<8;j++){
			k=UartTxBuf.wi+j;
			if(k>=UartTxBuf.MaxLen){
				k=k-UartTxBuf.MaxLen;	
			}	
			UartTxBuf.buf[k]=TVSetCMD[j];
		}	
		RingIncWi(UartTxBuf,8);	
		j=GetCmd;
		printf("send wi=%d ri=%d tal=%d max=%d cmd=801g_%02x_%s\n",UartTxBuf.wi,UartTxBuf.ri,UartTxBuf.TotalLen,UartTxBuf.MaxLen,j,&TVSetCMD[5]);
	}	
	return m_eScaler_State;
}




void TVStateInitial(void)
{
	switch(m_eScaler_State)
	{
		case eScaler_Init		: init_cmd=0;
								  init_timer_count=0;
								  m_eScaler_State=eScaler_RS232_No_Service; 
								  break;
		case eScaler_RS232_No_Service: 
								  init_timer_count++;
								  if(init_timer_count>200){
								  	 init_timer_count=0;	
								  	 SendTVGetCommand(TV_GET_READY_TST);
								  }
								  break; 
		case eScaler_Init_Data	: if(m_GetTVData[init_cmd]!=0xff) {
									 SendTVSetCommand('g', m_GetTVData[init_cmd],0);
									 init_cmd++;
								  }
								  else{
								    SendTVSetCommand('g', TV_GET_PWR_SAVE_LOW,0); 
									SendTVSetCommand('g', TV_GET_TOUCH_FEATURE,0); 
								  	m_eScaler_State=eScaler_Init_Timer;
									init_cmd=0;
								  }	
								  break;
								  
		case eScaler_Init_Timer : init_cmd++;
								  SendTVGetTimerCommand(init_cmd);
								  if(init_cmd==4)
								  	m_eScaler_State=eScaler_Init_Info;
								  break;
		case eScaler_Init_Info	: SendTVGetInfoCommand(1);
								  SendTVGetInfoCommand(2);
								  SendTVGetInfoCommand(3);
								  Get_smtp_status_day_hour_min();
								  m_eScaler_State=eScaler_Standy;	
								  break;
		default					: if(m_ReGetScalerData==1){
										if(m_GetTVData[init_cmd]!=0xff) {
									 		SendTVSetCommand('g', m_GetTVData[init_cmd],0);
									 		init_cmd++;
								  		}
								  		else{
											SendTVSetCommand('g', TV_GET_PWR_SAVE_LOW,0); 
											SendTVSetCommand('g', TV_GET_TOUCH_FEATURE,0); 
											SendTVGetTimerCommand(1);
											SendTVGetTimerCommand(2);
											SendTVGetTimerCommand(3);
											SendTVGetTimerCommand(4);
											SendTVGetInfoCommand(1);
								  			SendTVGetInfoCommand(2);
								  			SendTVGetInfoCommand(3);
								  			Get_smtp_status_day_hour_min();
								  			m_ReGetScalerData=0;
											init_cmd=0;
								  		}	
								  }
								  break;
	}
}

void ParsingTVCommand(u8_t data_len, u8_t *SetValue)
{
	u8_t Set_to_ACK;
	u8_t TimeerNo;
	u8_t *pData;
	u8_t i,ctemp;
	u8_t cdata[4];

	if(data_len==20){
		if((SetValue[3]=='r')&&(SetValue[4]==TV_GET_MODEL_INFO)){
			if(SetValue[5]==1) 		pData =m_mode_name;
			else if(SetValue[5]==2) pData =m_firmware_version;
			else 					pData =m_serial_number;
			for(i=0;i<14;i++)
				pData[i]=SetValue[6+i];
		}	
	}	
	
	if(data_len==4){
		if(SetValue[3]=='-')
			printf("Invalid CMD=%s\n",SetValue);
		if(SetValue[3]=='+')
			// RS232 is ready! 
			// eScaler_RS232_Ready
			if(m_eScaler_State==eScaler_RS232_No_Service){
				m_eScaler_State=eScaler_Init_Data;
				printf("Sclaer is ready!\n");
			}	
	}	
	if(data_len==0x0e){
		if(SetValue[3]=='s'){
			Set_to_ACK=1;
			switch (SetValue[4]){
				case TV_CMD_NETWORK_SETTING	: if (SetValue[5]==0) {
											  		if(SetValue[6]==1)	{ 		//DHCP
											  		    //printf("Now DHCPEnable=%d ",(u16_t)EEConfigInfo.DHCPEnable);     //peter debug
														if(EEConfigInfo.DHCPEnable==sDHCP_DISABLE)
														{
															EEConfigInfo.DHCPEnable=sDHCPC_NEEDDHCP;
															EEPROM_Write_Byte(loc_EEDHCPAddress,EEConfigInfo.DHCPEnable);
															//printf("New DHCPEnable=%d\n",(u16_t)EEConfigInfo.DHCPEnable); //peter debug
														}
											  		}
													else if(SetValue[6]==2) {	//Manual IP
													    //printf("Now DHCPEnable=%d ",(u16_t)EEConfigInfo.DHCPEnable);     //peter debug
														if(EEConfigInfo.DHCPEnable!=sDHCP_DISABLE)
														{
															EEConfigInfo.DHCPEnable=sDHCP_DISABLE;
															EEPROM_Write_Byte(loc_EEDHCPAddress,EEConfigInfo.DHCPEnable);
														}	
														//printf("New DHCPEnable=%d\n",(u16_t)EEConfigInfo.DHCPEnable); //peter debug
														Set_to_ACK=0;
														SendTVACK();
														SendTVGetNetWorkCommand(0x01);//get ip
														SendTVGetNetWorkCommand(0x02);//get submask
														SendTVGetNetWorkCommand(0x03);//get gatway
														SendTVGetNetWorkCommand(0x04);//get dns
													}
													else if(SetValue[6]==6) {	//MAC Address
															for(i=0;i<6;i++){ 
																EEConfigInfo.MACID[i]=SetValue[7+i];
																EEPROM_Write_Byte((loc_EEMACID+i),EEConfigInfo.MACID[i]);
															}
													}
													else if(SetValue[6]==0x09){	//Send Status Report Mail
														Send_Status_Report_Message(0,&ModuleInfo.SMTPInfo.MAIL_MSG[0][0]);
														smtp_send_mail(0);
													}
													else if(SetValue[6]==0x0b){	//Reset IP210
														force_reset_countdown=10;
													}
											  }
											  break;
					
				case TV_CMD_TIMER_ON_OFF	: TimeerNo = SetValue[4+1]&0xf;
											  switch(TimeerNo)	
											  {
											  	case 1	: pData =tmier1; break;
												case 2	: pData =tmier2; break;								
												case 3	: pData =tmier3; break;
												case 4	: pData =tmier4; break;	
												default : pData =tmier1; break;
											  }
											  if((SetValue[4+1]&0x40)!=0) pData[5]=1; else pData[5]=0;
											  if((SetValue[4+1]&0x20)!=0) pData[6]=1; else pData[6]=0;
											  if((SetValue[4+1]&0x10)!=0) pData[7]=1; else pData[7]=0;
											  pData[4]=	SetValue[4+2];  // Select Day
											  pData[0]=	SetValue[4+3];  // On Hour
											  pData[1]=	SetValue[4+4];  // On Min.
											  pData[2]=	SetValue[4+5];  // Off Houur
											  pData[3]=	SetValue[4+6];  // Off Min.
											  pData[8]=	SetValue[4+7];  // Select Source
											  break;
				default						: Set_to_ACK=0;break;	
				
			}	
			if(Set_to_ACK==1)
				SendTVACK();
		}	
		if(SetValue[3]=='g'){
			switch (SetValue[4]){
				case TV_CMD_NETWORK_SETTING	: if (SetValue[5]==0) {
													if(SetValue[6]==3) {		//Excute Result
														SendTVACK();
														if((EEConfigInfo.DHCPEnable==sDHCP_DISABLE)||(DHCPCInfo.Status==sDHCPC_DONE)){
														  cdata[0]=1;//done
														  SendTVReplyNetWorkCommand(0x06,&cdata[0],1);
														}  
														else{
															cdata[0]=0;//Ongoing
															SendTVReplyNetWorkCommand(0x06,&cdata[0],1);
														}
													}	
													else if(SetValue[6]==4) {	//Netwrok Setting 
													    SendTVACK();
														if(EEConfigInfo.DHCPEnable!=sDHCP_DISABLE) {
															SendTVReplyNetWorkCommand(0x07,&DHCPCInfo.MyIPAddr,4); 			//IP
															SendTVReplyNetWorkCommand(0x08,&DHCPCInfo.SubnetMask,4); 		//Submask
															SendTVReplyNetWorkCommand(0x09,&DHCPCInfo.GetwayAddr,4); 		//Gatway
															SendTVReplyNetWorkCommand(0x0a,&DHCPCInfo.DNSAddr,4); 			//DNS
														}	
														else {
															SendTVReplyNetWorkCommand(0x07,&EEConfigInfo.IPAddress[0],4); 	//IP
															SendTVReplyNetWorkCommand(0x08,&EEConfigInfo.SubnetMask[0],4); 	//Submask
															SendTVReplyNetWorkCommand(0x09,&EEConfigInfo.Gateway[0],4); 	//Gatway
															SendTVReplyNetWorkCommand(0x0a,&EEConfigInfo.DNSAddress[0],4); 	//DNS
														}
													}
													else if(SetValue[6]==5) { 	//MAC Address
														SendTVACK();
														SendTVReplyNetWorkCommand(0x0c,&EEConfigInfo.MACID[0],6); 		//MAC Address 
													}
													else if(SetValue[6]==7) { 	//Lan Firmware Version
														SendTVACK();
														cdata[0]=1; cdata[1]=44; cdata[2]=3;							//V1.44.3
														SendTVReplyNetWorkCommand(0x0d,cdata,3); 						//Land Firmware Version
													}
													else if(SetValue[6]==0x0a) {//IP=Manual/DHCP
														SendTVACK();
														// printf("Now DHCPEnable=%d\n",(u16_t)EEConfigInfo.DHCPEnable);     //peter debug
														if(EEConfigInfo.DHCPEnable!=sDHCP_DISABLE)
															cdata[0]=1; //DHCP
														else
															cdata[0]=0;//Manual
														SendTVReplyNetWorkCommand(0x0e,&cdata[0],1); 					//IP=Manual/DHCP
													}
											  }
											  break;
				default:					  break;
			}
		}
		if(SetValue[3]=='r'){
			switch (SetValue[4]){
				case TV_CMD_NETWORK_SETTING	: if (SetValue[5]==1) {				//IP		
													for(i=0;i<4;i++){ 
														EEConfigInfo.IPAddress[i]=SetValue[6+i];
														EEPROM_Write_Byte((loc_EEIPAddress+i),EEConfigInfo.IPAddress[i]);
													} 
											  }
											  else if (SetValue[5]==2) {		//Submask
													for(i=0;i<4;i++){ 
														EEConfigInfo.SubnetMask[i]=SetValue[6+i];
														EEPROM_Write_Byte((loc_EESubnetMask+i),EEConfigInfo.SubnetMask[i]);
													} 
											  }
											  else if (SetValue[5]==3) {		//Gatway
													for(i=0;i<4;i++){ 
														EEConfigInfo.Gateway[i]=SetValue[6+i];
														EEPROM_Write_Byte((loc_EEGateway+i),EEConfigInfo.Gateway[i]);
													} 
											  }
											  else if (SetValue[5]==4) {		//DNS
												   for(i=0;i<4;i++){ 
													   EEConfigInfo.DNSAddress[i]=SetValue[6+i];
													   EEPROM_Write_Byte((loc_EEDNSAddress+i),EEConfigInfo.DNSAddress[i]);
												   } 
											  }
											  break;
				case TV_GET_TIMER_ON_OFF	: TimeerNo = SetValue[4+1]&0xf;
											  switch(TimeerNo)	
											  {
											  	case 1	: pData =tmier1; break;
												case 2	: pData =tmier2; break;								
												case 3	: pData =tmier3; break;
												case 4	: pData =tmier4; break;	
												default : pData =tmier1; break;
											  }
											  if((SetValue[4+1]&0x40)!=0) pData[5]=1; else pData[5]=0;
											  if((SetValue[4+1]&0x20)!=0) pData[6]=1; else pData[6]=0;
											  if((SetValue[4+1]&0x10)!=0) pData[7]=1; else pData[7]=0;
											  pData[4]=	SetValue[4+2];  // Select Day
											  pData[0]=	SetValue[4+3];  // On Hour
											  pData[1]=	SetValue[4+4];  // On Min.
											  pData[2]=	SetValue[4+5];  // Off Houur
											  pData[3]=	SetValue[4+6];  // Off Min.
											  pData[8]=	SetValue[4+7];  // Select Source
											  break;
				default						: break;	
				
			}	
		}	
		
	}	
	if(data_len==11){
		if((SetValue[3]=='s')||(SetValue[3]=='r')){
			switch (SetValue[4]){
				case TV_CMD_SMTP_TIME		: cdata[0]=SetValue[6];
											  cdata[1]=0;   
											  m_smtp_day=atoi(&cdata[0]);	
											  cdata[0]=SetValue[7];
											  cdata[1]=SetValue[8];	
											  cdata[2]=0;  
											  m_smtp_hour=atoi(&cdata[0]);	
											  cdata[0]=SetValue[9];
											  cdata[1]=SetValue[10];	
											  cdata[2]=0;  
											  m_smtp_min=atoi(&cdata[0]);	
			}
		}
		
	}
	if(data_len==12){
		if(SetValue[3]=='s'){
			switch (SetValue[4]){
				case TV_GET_OPERATION_TIME	: cdata[0]=SetValue[5];
											  cdata[1]=SetValue[6];
											  cdata[2]=SetValue[7];
											  cdata[3]=0;
											  m_operation_time_H=atoi(&cdata[0]);
											  m_operation_time_L=atoi(&SetValue[8]);
											  SendTVACK();
											  break;
				default						: break; 
			}
		}	
		if(SetValue[3]=='r'){
			switch (SetValue[4]){
				case TV_GET_OPERATION_TIME	: cdata[0]=SetValue[5];
											  cdata[1]=SetValue[6];
											  cdata[2]=SetValue[7];
											  cdata[3]=0;
											  m_operation_time_H=atoi(&cdata[0]);
											  m_operation_time_L=atoi(&SetValue[8]);
											  break;
				default						: break; 
			}
		}
	}	
	if(data_len==9){
		if(SetValue[3]=='r'){
			if((SetValue[4]==TV_GET_SMTP_TIME) &&(SetValue[5]==0x28)){
				if(SetValue[6]==0){
					m_smtp_day=0;
					m_smtp_check=0;
				}
				else{
					m_smtp_check=1;
					switch(SetValue[6]){
						case 1:  m_smtp_day=0;break;
						case 2:  m_smtp_day=1;break;
						case 4:  m_smtp_day=2;break;
						case 8:  m_smtp_day=3;break;
						case 16: m_smtp_day=4;break;
						case 32: m_smtp_day=5;break;
						case 64: m_smtp_day=6;break;	
						default: m_smtp_day=7;break;		
					}
				}	
				m_smtp_hour=SetValue[7];
				m_smtp_min =SetValue[8];
				SMTPData.pEEPROM_SMTP->SMTPEnable=m_smtp_check;
				
			}	
		}
	}	
	if(data_len==8){
		if(SetValue[3]=='s'){
			//Set command to Update Data
			Set_to_ACK=1;
			switch (SetValue[4]){
				case TV_CMD_TOUCH_MODE		: m_touch_mode_data=atoi(&SetValue[5]);break;
					
				case TV_CMD_PWR_SAVE_LOW	: // Set the power saving mode 
											  m_pw_save_mode=atoi(&SetValue[5]); break;
				case TV_CMD_TEMP_ALERT		: //Alert_Message(&ModuleInfo.SMTPInfo.MAIL_MSG[0][0],SetValue[7]);
											  Send_Status_Report_Message(1,&ModuleInfo.SMTPInfo.MAIL_MSG[0][0]);
											  smtp_send_mail_test(0);
											  break;
				case TV_CMD_SEND_MAIL		: //Send_Message(&ModuleInfo.SMTPInfo.MAIL_MSG[0][0]);
											  Send_Status_Report_Message(2,&ModuleInfo.SMTPInfo.MAIL_MSG[0][0]);
											  smtp_send_mail_test(0);
											  break;
				//Picture&Sound
				case TV_CMD_POWER			: m_pwr_data=atoi(&SetValue[5]);
											  if(m_pwr_data==0){
											  	m_eScaler_State=eScaler_Standy;
											    m_sync_data=0;
											  }	
											  else
											  	m_eScaler_State=eScaler_PowerOn;
											  break;
				case TV_CMD_SYNC			: m_sync_data=atoi(&SetValue[5]);break;
				case TV_CMD_LAN_GET_MODE	: m_src_mode_data=atoi(&SetValue[5]);break;
				
				
				case TV_CMD_SRC				: m_src_data=atoi(&SetValue[5]); break;
				case TV_CMD_PIC_MODE		: m_pic_data=atoi(&SetValue[5]); break;
				case TV_CMD_BACKLIGHT		: m_bkl_data=atoi(&SetValue[5]); break;
				case TV_CMD_CONTRAST		: m_ct_data=atoi(&SetValue[5]);  break;
				case TV_CMD_BRIGHTNESS		: m_br_data=atoi(&SetValue[5]);  break;
				case TV_CMD_SHARPNESS		: m_sr_data=atoi(&SetValue[5]);  m_sr_data -=50; break;
				case TV_CMD_COLOR_TEMP		: m_cot_data=atoi(&SetValue[5]); break;
//				case TV_CMD_NR_AUTO_CLEAN	: m_nra_data=atoi(&SetValue[5]); break;
//				case TV_CMD_NR_MPEG			: m_nrm_data=atoi(&SetValue[5]); break;
//				case TV_CMD_NR_DNR			: m_nrd_data=atoi(&SetValue[5]); break;
				case TV_CMD_HUE				: m_hue_data=atoi(&SetValue[5]); m_hue_data -=50; break;
				case TV_CMD_SATURATION		: m_sat_data=atoi(&SetValue[5]); break;
				case TV_CMD_SOUND_MODE		: m_sm_data=atoi(&SetValue[5]);  break;
				case TV_CMD_TREBLE			: m_tre_data=atoi(&SetValue[5]); m_tre_data -=50; break;
				case TV_CMD_BASS			: m_bas_data=atoi(&SetValue[5]); m_bas_data -=50; break;
				case TV_CMD_BALANCE			: m_bal_data=atoi(&SetValue[5]); m_bal_data -=50; break;
				case TV_CMD_VOLUME			: m_vol_data=atoi(&SetValue[5]); break;
				case TV_CMD_MUTE			: m_mut_data=atoi(&SetValue[5]); break;
//				case TV_CMD_AUDIO_DISTORTION: m_aud_data=atoi(&SetValue[5]); break;
//				case TV_CMD_AUDIO_LEVEL_OFS	: m_alo_data=atoi(&SetValue[5]); m_alo_data -=11; break;
				case TV_CMD_HDMI_PC_AUDIO	: m_hpa_data=atoi(&SetValue[5]); break;

				//Screen
				case TV_CMD_H_MONITOR		: m_hms_data=atoi(&SetValue[5]); break;
				case TV_CMD_V_MONITOR		: m_vms_data=atoi(&SetValue[5]); break;
				case TV_CMD_H_POSITION		: m_hps_data=atoi(&SetValue[5]); break;
				case TV_CMD_V_POSITION		: m_vps_data=atoi(&SetValue[5]); break;
				case TV_CMD_FRAME_COMP		: m_frc_data=atoi(&SetValue[5]); break;
				case TV_CMD_DISP_WALL_LED	: m_led_data=atoi(&SetValue[5]); break;
				case TV_CMD_DISP_WALL_PW_ON	: m_pod_data=atoi(&SetValue[5]); break;
				case TV_CMD_CONTRAST_BOOSTER: m_ctb_data=atoi(&SetValue[5]); break;
				case TV_CMD_BLUE_SCREEN		: m_bls_data=atoi(&SetValue[5]); break;
				case TV_CMD_FREEZE			: m_fre_data=atoi(&SetValue[5]); break;
//				case TV_CMD_3D_COLOR		: m_m3d_data=atoi(&SetValue[5]); break;
				case TV_CMD_ADAPTIVE_CONT	: m_act_data=atoi(&SetValue[5]); break;
				case TV_CMD_ASPECT			: m_asp_data=atoi(&SetValue[5]); break;
				case TV_CMD_ASPECT_MODE     : ctemp=atoi(&SetValue[5]); 
											  if(ctemp==0x00) m_asp_mode =0; else m_asp_mode =1;
											  printf("Set Aspect Mode=%d\n",(u16_t)m_asp_mode);
											  break;
				//Setting
				case TV_CMD_MONITOR_ID		: m_mid_data=atoi(&SetValue[5]); break;
				case TV_CMD_POWER_SAVE		: m_pws_data=atoi(&SetValue[5]); break;
				case TV_CMD_AMBIENT_LT_SNS	: m_lts_data=atoi(&SetValue[5]); break;
				case TV_CMD_LANGUAGE		: m_lag_data=atoi(&SetValue[5]); break;
				case TV_CMD_IR_OUT			: m_iro_data=atoi(&SetValue[5]); break;
				case TV_CMD_AUTO_POWER_DOWN	: m_apd_data=atoi(&SetValue[5]); break;
//				case TV_CMD_PWR_SAVEING_ID	: m_psi_data=atoi(&SetValue[5]); break;
//				case TV_CMD_PANEL_LOCK		: m_plk_data=atoi(&SetValue[5]); break;
				case TV_CMD_IMAGE_RETENTION	: m_imr_data=atoi(&SetValue[5]); break;
				case TV_CMD_OSD_INFO_BOX	: m_osd_data=atoi(&SetValue[5]); break;
				case TV_CMD_TOUCH_FEATURE	: m_tch_data=atoi(&SetValue[5]); break;
				case TV_CMD_CLOCK_DISP		: m_cdp_data=atoi(&SetValue[5]); break;
//				case TV_CMD_OSD_ROTATION	: m_osr_data=atoi(&SetValue[5]); break;
//				case TV_CMD_DISP_SETTINGS	: m_dps_data=atoi(&SetValue[5]); break;
//				case TV_CMD_DISP_AUTO		: m_dpa_data=atoi(&SetValue[5]); break;
//				case TV_CMD_DISP_4_3		: m_dp_strech_data=atoi(&SetValue[5]); break;

				//Schedule
				case TV_CMD_RTC_YEAR		: m_rtc_year=atoi(&SetValue[5])+2000; break;
				case TV_CMD_RTC_MONTH		: m_rtc_month=atoi(&SetValue[5]); 	break;
				case TV_CMD_RTC_DAY			: m_rtc_date=atoi(&SetValue[5]); 	break;
				case TV_CMD_RTC_HOUR		: m_rtc_hour=atoi(&SetValue[5]); 	break;
				case TV_CMD_RTC_MINUTE		: m_rtc_min=atoi(&SetValue[5]); 	break;
				
				default						: Set_to_ACK=0;break;	
			}	
			if(Set_to_ACK==1)
				SendTVACK();
				
		}			
		if(SetValue[3]=='r'){
			//Set command to Update Data
			switch (SetValue[4]){
				case TV_GET_TOUCH_MODE		: m_touch_mode_data=atoi(&SetValue[5]);break;
			    case TV_GET_PWR_SAVE_LOW	: // Set the power saving mode 
											  m_pw_save_mode=atoi(&SetValue[5]); break;
				case TV_GET_POWER			: m_pwr_data=atoi(&SetValue[5]); break;
				//Picture&Sound
				case TV_GET_LAN_GET_MODE 	: m_src_mode_data=atoi(&SetValue[5]); break;
				case TV_GET_SRC				: m_src_data=atoi(&SetValue[5]); break;
				case TV_GET_PIC_MODE		: m_pic_data=atoi(&SetValue[5]); break;
				case TV_GET_BACKLIGHT		: m_bkl_data=atoi(&SetValue[5]); break;
				case TV_GET_CONTRAST		: m_ct_data=atoi(&SetValue[5]);  break;
				case TV_GET_BRIGHTNESS		: m_br_data=atoi(&SetValue[5]);  break;
				case TV_GET_SHARPNESS		: m_sr_data=atoi(&SetValue[5]);  m_sr_data -=50; break;
				case TV_GET_COLOR_TEMP		: m_cot_data=atoi(&SetValue[5]); break;
//				case TV_GET_NR_AUTO_CLEAN	: m_nra_data=atoi(&SetValue[5]); break;
//				case TV_GET_NR_MPEG			: m_nrm_data=atoi(&SetValue[5]); break;
//				case TV_GET_NR_DNR			: m_nrd_data=atoi(&SetValue[5]); break;
				case TV_GET_HUE				: m_hue_data=atoi(&SetValue[5]); m_hue_data -=50; break;
				case TV_GET_SATURATION		: m_sat_data=atoi(&SetValue[5]); break;
				case TV_GET_SOUND_MODE		: m_sm_data=atoi(&SetValue[5]);  break;
				case TV_GET_TREBLE			: m_tre_data=atoi(&SetValue[5]); m_tre_data -=50; break;
				case TV_GET_BASS			: m_bas_data=atoi(&SetValue[5]); m_bas_data -=50; break;
				case TV_GET_BALANCE			: m_bal_data=atoi(&SetValue[5]); m_bal_data -=50; break;
				case TV_GET_VOLUME			: m_vol_data=atoi(&SetValue[5]); break;
				case TV_GET_MUTE			: m_mut_data=atoi(&SetValue[5]); break;
//				case TV_GET_AUDIO_DISTORTION: m_aud_data=atoi(&SetValue[5]); break;
//				case TV_GET_AUDIO_LEVEL_OFS	: m_alo_data=atoi(&SetValue[5]); m_alo_data -=11; break;
				case TV_GET_HDMI_PC_AUDIO	: m_hpa_data=atoi(&SetValue[5]); break;

				//Screen
				case TV_GET_H_MONITOR		: m_hms_data=atoi(&SetValue[5]); break;
				case TV_GET_V_MONITOR		: m_vms_data=atoi(&SetValue[5]); break;
				case TV_GET_H_POSITION		: m_hps_data=atoi(&SetValue[5]); break;
				case TV_GET_V_POSITION		: m_vps_data=atoi(&SetValue[5]); break;
				case TV_GET_FRAME_COMP		: m_frc_data=atoi(&SetValue[5]); break;
				case TV_GET_DISP_WALL_LED	: m_led_data=atoi(&SetValue[5]); break;
				case TV_GET_DISP_WALL_PW_ON	: m_pod_data=atoi(&SetValue[5]); break;
				case TV_GET_CONTRAST_BOOSTER: m_ctb_data=atoi(&SetValue[5]); break;
				case TV_GET_BLUE_SCREEN		: m_bls_data=atoi(&SetValue[5]); break;
//				case TV_GET_3D_COLOR		: m_m3d_data=atoi(&SetValue[5]); break;
				case TV_GET_ADAPTIVE_CONT	: m_act_data=atoi(&SetValue[5]); break;
				case TV_GET_ASPECT			: m_asp_data=atoi(&SetValue[5]); break;				
				case TV_GET_ASPECT_MODE     : ctemp=atoi(&SetValue[5]); 
											  if(ctemp==0x00) m_asp_mode =0; else m_asp_mode =1;
											  printf("Get Aspect Mode=%d\n",(u16_t)m_asp_mode);
											  break;
				//Setting
				case TV_GET_MONITOR_ID		: m_mid_data=atoi(&SetValue[5]); break;
				case TV_GET_POWER_SAVE		: m_pws_data=atoi(&SetValue[5]); break;
				case TV_GET_AMBIENT_LT_SNS	: m_lts_data=atoi(&SetValue[5]); break;
				case TV_GET_LANGUAGE		: m_lag_data=atoi(&SetValue[5]); break;
				case TV_GET_IR_OUT			: m_iro_data=atoi(&SetValue[5]); break;
				case TV_GET_AUTO_POWER_DOWN	: m_apd_data=atoi(&SetValue[5]); break;
//				case TV_GET_PWR_SAVEING_ID	: m_psi_data=atoi(&SetValue[5]); break;
//				case TV_GET_PANEL_LOCK		: m_plk_data=atoi(&SetValue[5]); break;
				case TV_GET_IMAGE_RETENTION	: m_imr_data=atoi(&SetValue[5]); break;
				case TV_GET_OSD_INFO_BOX	: m_osd_data=atoi(&SetValue[5]); break;
				case TV_GET_TOUCH_FEATURE	: m_tch_data=atoi(&SetValue[5]); break;
				case TV_GET_CLOCK_DISP		: m_cdp_data=atoi(&SetValue[5]); break;
//				case TV_GET_OSD_ROTATION	: m_osr_data=atoi(&SetValue[5]); break;
//				case TV_GET_DISP_SETTINGS	: m_dps_data=atoi(&SetValue[5]); break;
//				case TV_GET_DISP_AUTO		: m_dpa_data=atoi(&SetValue[5]); break;
//				case TV_GET_DISP_4_3		: m_dp_strech_data=atoi(&SetValue[5]); break;

				//Schedule
				case TV_GET_RTC_YEAR		: m_rtc_year=atoi(&SetValue[5])+2000; break;
				case TV_GET_RTC_MONTH		: m_rtc_month=atoi(&SetValue[5]); 	break;
				case TV_GET_RTC_DAY			: m_rtc_date=atoi(&SetValue[5]); 	break;
				case TV_GET_RTC_HOUR		: m_rtc_hour=atoi(&SetValue[5]); 	break;
				case TV_GET_RTC_MINUTE		: m_rtc_min=atoi(&SetValue[5]); 	break;
				
				default						: Set_to_ACK=0;break;	
			}	
		}	
	}
}




u8_t ssi_telnet(u8_t varid, data_value *vp)
{
	u16_t select_chk;
#ifdef MODULE_NET_UART

	vp->value.string ="";
    vp->type = string;
    switch (varid)
    {
    	//Special Condition Control for Some Items
//    	case  CGI_TELNET_VGA:
//			    if ((m_pwr_data==0)||(m_sync_data==0)||(m_src_data==4))
//					vp->value.string ="disabled"; 
//				break;
	 	case CGI_TELNET_STB:
				if (m_pwr_data==0)
					vp->value.string ="disabled"; 
				break;
		case CGI_TELNET_SYNC:
				if (m_sync_data==0)
					vp->value.string ="disabled"; 
				break;
		//Normal Tag Process		
		case CGI_TELNET_SRC_1:
		case CGI_TELNET_SRC_2:
		case CGI_TELNET_SRC_3:
		case CGI_TELNET_SRC_4:
		case CGI_TELNET_SRC_5:
		case CGI_TELNET_SRC_6:
		case CGI_TELNET_SRC_7:	
				if((m_src_mode_data==1)||((m_src_mode_data==0)&&(varid!=CGI_TELNET_SRC_4))) {
			 		select_chk= varid-CGI_TELNET_SRC_1;
			  		if(select_chk==m_src_data) { 
			  			vp->value.string ="Selected"; 
			  		}	
				}
				else
					vp->value.string ="hidden"; 
			  	break;

		case CGI_TELNET_PIC_1:
		case CGI_TELNET_PIC_2:
		case CGI_TELNET_PIC_3: 
		case CGI_TELNET_PIC_4: 
				select_chk= varid-CGI_TELNET_PIC_1;
				if(select_chk==m_pic_data) { 
					vp->value.string ="Selected"; 
				}	
				break;

		case CGI_TELNET_COT_1:
		case CGI_TELNET_COT_2:
		case CGI_TELNET_COT_3:
		case CGI_TELNET_COT_4:
			select_chk= varid-CGI_TELNET_COT_1;
			if(select_chk==m_cot_data) { 
				vp->value.string ="Selected"; 
			}	
			break;


//		case CGI_TELNET_NRA_1:
//		case CGI_TELNET_NRA_2:
//			select_chk= varid-CGI_TELNET_NRA_1;
//			if(select_chk==m_nra_data) { 
//				vp->value.string ="Selected"; 
//			}	
//			break;
			
//		case CGI_TELNET_NRM_1:
//		case CGI_TELNET_NRM_2:
//		case CGI_TELNET_NRM_3:
//		case CGI_TELNET_NRM_4:
//			select_chk= varid-CGI_TELNET_NRM_1;
//			if(select_chk==m_nrm_data) { 
//				vp->value.string ="Selected"; 
//			}	
//			break;
			
//		case CGI_TELNET_NRD_1:
//		case CGI_TELNET_NRD_2:
//		case CGI_TELNET_NRD_3:
//		case CGI_TELNET_NRD_4:
//			select_chk= varid-CGI_TELNET_NRD_1;
//			if(select_chk==m_nrd_data) { 
//				vp->value.string ="Selected"; 
//			}	
//			break;
				
		case CGI_TELNET_BKL:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_bkl_data;
				break;
		
		case CGI_TELNET_CT:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_ct_data;
				break;

		case CGI_TELNET_BR:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_br_data;
				break;

//		case CGI_TELNET_CR:	
//				vp->type=digits_3_int;
//				vp->value.digits_3_int=m_cr_data;
//				break;

		case CGI_TELNET_SR:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_sr_data;
				break;

//		case CGI_TELNET_TINT:	
//				vp->type=digits_3_int;
//				vp->value.digits_3_int=m_tint_data;
//				break;

		case CGI_TELNET_HUE:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_hue_data;
				break;

		case CGI_TELNET_SAT:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_sat_data;
				break;

		
		case CGI_TELNET_CTB_1:
		case CGI_TELNET_CTB_2:
		case CGI_TELNET_CTB_3:
		case CGI_TELNET_CTB_4:
			select_chk= varid-CGI_TELNET_CTB_1;
			if(select_chk==m_ctb_data) { 
				vp->value.string ="Selected"; 
			}	
			break;

//		case CGI_TELNET_M3D_1:
// 		case CGI_TELNET_M3D_2:
//			select_chk= varid-CGI_TELNET_M3D_1;
//			if(select_chk==m_m3d_data) { 
//				vp->value.string ="Selected"; 
//			}	
//			break;

		case CGI_TELNET_ACT_1:
 		case CGI_TELNET_ACT_2:
			select_chk= varid-CGI_TELNET_ACT_1;
			if(select_chk==m_act_data) { 
				vp->value.string ="Selected"; 
			}	
			break;

		case CGI_TELNET_SM_1:
		case CGI_TELNET_SM_2:
		case CGI_TELNET_SM_3:
		case CGI_TELNET_SM_4:
			select_chk= varid-CGI_TELNET_SM_1;
			if(select_chk==m_sm_data) { 
				vp->value.string ="Selected"; 
			}	
			break;

		case CGI_TELNET_TRE:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_tre_data;
				break;

		case CGI_TELNET_BAS:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_bas_data;
				break;

		case CGI_TELNET_BAL:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_bal_data;
				break;
			
		case CGI_TELNET_MUT_1:
		case CGI_TELNET_MUT_2:
			select_chk= varid-CGI_TELNET_MUT_1;
			if(select_chk==m_mut_data) { 
				vp->value.string ="Selected"; 
			}	
			break;
/*	
		case CGI_TELNET_AUD_1:
		case CGI_TELNET_AUD_2:
			select_chk= varid-CGI_TELNET_AUD_1;
			if(select_chk==m_aud_data) { 
				vp->value.string ="Selected"; 
			}	
			break;

		case CGI_TELNET_HPA_1:
		case CGI_TELNET_HPA_2:
			select_chk= varid-CGI_TELNET_HPA_1;
			if(select_chk==m_hpa_data) { 
				vp->value.string ="Selected"; 
			}	
			break;
*/

    	case  CGI_TELNET_HDMI_AUDIO:
			    if (m_src_data>3)
					vp->value.string ="disabled"; 
				break;



		case CGI_TELNET_HMA_1:
		case CGI_TELNET_HMA_2:
			select_chk= varid-CGI_TELNET_HMA_1;
			if(select_chk==m_hpa_data) { 
				vp->value.string ="Selected"; 
			}	
			break;


		case CGI_TELNET_VOL:	
				vp->type=digits_3_int;
				vp->value.digits_3_int=m_vol_data;
				break;

//		case CGI_TELNET_ALO:	
//				vp->type=digits_3_int;
//				vp->value.digits_3_int=m_alo_data;
//				break;

				
		default:
			 	 break;
			   
    }
#else //MODULE_NET_UART
int temp=(int)varid;
temp=(int)vp;
#endif //MODULE_NET_UART
    return OK;
}




u8_t cgi_telnet(u8_t cnt, void *vptr)
{
	u8_t en_re_rf=0, count=0;
	eSCALER_ST eCMDStatus=eScaler_RS232_Ready;
	req_data *vp = vptr;
	int i,wdata;
	//NetUartTelnetClose();
	for(i=0;i<cnt;i++)
	{
		if(!strcmp(vp[i].value, "s_ip"))
		{ NetUARTData.pTelnetData->pEEPROM_Telnet->ip_demo=0; }
		if(!strcmp(vp[i].value, "s_demo"))
		{ NetUARTData.pTelnetData->pEEPROM_Telnet->ip_demo=1; }
	}
    for (i=0; i < cnt; i++, vp++)
    {       
    		if(!strcmp(vp->item, "bkl_val")){
				 m_bkl_data=atoi(vp->value);
    		}	
    		if(!strcmp(vp->item, "bkl_act")){
			
				if((*vp->value)=='1') 	m_bkl_data++;
				if((*vp->value)=='0') 	m_bkl_data--;
				eCMDStatus=SendTVSetCommand('s',TV_CMD_BACKLIGHT,(m_bkl_data));
    		}	

    		if(!strcmp(vp->item, "ct_val")){
				 m_ct_data=atoi(vp->value);
    		}				
    		if(!strcmp(vp->item, "ct_act")){
				if((*vp->value)=='1') 	m_ct_data++;
				if((*vp->value)=='0') 	m_ct_data--;
				eCMDStatus=SendTVSetCommand('s',TV_CMD_CONTRAST,(m_ct_data));
    		}	


    		if(!strcmp(vp->item, "br_val")){
				 m_br_data=atoi(vp->value);
    		}							
    		if(!strcmp(vp->item, "br_act")){
				if((*vp->value)=='1') 	m_br_data++;
				if((*vp->value)=='0') 	m_br_data--;
				eCMDStatus=SendTVSetCommand('s',TV_CMD_BRIGHTNESS,(m_br_data));
    		}	
		
 			
			if(!strcmp(vp->item, "sr_val")){
				 m_sr_data=atoi(vp->value);
			}											
    		if(!strcmp(vp->item, "sr_act")){
				if((*vp->value)=='1') 	m_sr_data++;
				if((*vp->value)=='0') 	m_sr_data--;
				eCMDStatus=SendTVSetCommand('s',TV_CMD_SHARPNESS,(m_sr_data+50));
    		}	

			if(!strcmp(vp->item, "hue_val")){
				 m_hue_data=atoi(vp->value);
			}											 
    		if(!strcmp(vp->item, "hue_act")){
				if((*vp->value)=='1') 	m_hue_data++;
				if((*vp->value)=='0') 	m_hue_data--;
				eCMDStatus=SendTVSetCommand('s',TV_CMD_HUE,(m_hue_data+50));
    		}	

    		if(!strcmp(vp->item, "ctb_sel")){
 				m_ctb_data= atoi(vp->value);
				eCMDStatus=SendTVSetCommand('s',TV_CMD_CONTRAST_BOOSTER,(m_ctb_data));
    		}	


//    		if(!strcmp(vp->item, "m3d_sel")){
// 				m_m3d_data= atoi(vp->value);
//				eCMDStatus=SendTVSetCommand('s',TV_CMD_3D_COLOR,(m_m3d_data));
//    		}	


    		if(!strcmp(vp->item, "abc_sel")){
 				m_act_data= atoi(vp->value);
				eCMDStatus=SendTVSetCommand('s',TV_CMD_ADAPTIVE_CONT,(m_act_data));
    		}	

			if(!strcmp(vp->item, "sat_val")){
				 m_sat_data=atoi(vp->value);
			}											 			
    		if(!strcmp(vp->item, "sat_act")){
				if((*vp->value)=='1') 	m_sat_data++;
				if((*vp->value)=='0') 	m_sat_data--;
				eCMDStatus=SendTVSetCommand('s',TV_CMD_SATURATION,(m_sat_data));
    		}	


			if(!strcmp(vp->item, "tre_val")){
				 m_tre_data=atoi(vp->value);
			}											 			
    		if(!strcmp(vp->item, "tre_act")){
				if((*vp->value)=='1') 	m_tre_data++;
				if((*vp->value)=='0') 	m_tre_data--;
				eCMDStatus=SendTVSetCommand('s',TV_CMD_TREBLE,(m_tre_data+50));
    		}	

			if(!strcmp(vp->item, "bas_val")){
				 m_bas_data=atoi(vp->value);
			}											 			
    		if(!strcmp(vp->item, "bas_act")){
				if((*vp->value)=='1') 	m_bas_data++;
				if((*vp->value)=='0') 	m_bas_data--;
				eCMDStatus=SendTVSetCommand('s',TV_CMD_BASS,(m_bas_data+50));
    		}	

			if(!strcmp(vp->item, "bal_val")){
				 m_bal_data=atoi(vp->value);
			}											 			
    		if(!strcmp(vp->item, "bal_act")){
				if((*vp->value)=='1') 	m_bal_data++;
				if((*vp->value)=='0') 	m_bal_data--;
				eCMDStatus=SendTVSetCommand('s',TV_CMD_BALANCE,(m_bal_data+50));
    		}	

    		if(!strcmp(vp->item, "vol_val")){
				 m_vol_data=atoi(vp->value);
    		}							
    		if(!strcmp(vp->item, "vol_act")){
				if((*vp->value)=='1') 	m_vol_data++;
				if((*vp->value)=='0') 	m_vol_data--;
				eCMDStatus=SendTVSetCommand('s',TV_CMD_VOLUME,(m_vol_data));
    		}	

//    		if(!strcmp(vp->item, "alo_act")){
//				if((*vp->value)=='1') 	m_alo_data++; else m_alo_data--;
//				eCMDStatus=SendTVSetCommand('s',TV_CMD_AUDIO_LEVEL_OFS,(m_alo_data+11));
//    		}	

    		if(!strcmp(vp->item, "sm_sel")){
				m_sm_data= atoi(vp->value);
				eCMDStatus=SendTVSetCommand('s',TV_CMD_SOUND_MODE,(m_sm_data));
				if(eCMDStatus!=eScaler_RS232_No_Service)
					eCMDStatus=eScaler_Mode_Change;
					
    		}	


    		if(!strcmp(vp->item, "mut_sel")){
				m_mut_data= atoi(vp->value);				
				eCMDStatus=SendTVSetCommand('s',TV_CMD_MUTE,(m_mut_data));
    		}	

//    		if(!strcmp(vp->item, "aud_sel")){
//				m_aud_data= atoi(vp->value);
//				eCMDStatus=SendTVSetCommand('s',TV_CMD_AUDIO_DISTORTION,(m_aud_data));
//    		}	


			
    		if(!strcmp(vp->item, "src_sel")){
				m_src_data= atoi(vp->value);				
				eCMDStatus=SendTVSetCommand('s',TV_CMD_SRC,(m_src_data));
				if(eCMDStatus!=eScaler_RS232_No_Service)
					eCMDStatus=eScaler_Mode_Change;
    		}	

    		if(!strcmp(vp->item, "pic_sel")){
				printf("pic_sel\n");
				m_pic_data= atoi(vp->value);
				eCMDStatus=SendTVSetCommand('s',TV_CMD_PIC_MODE,(m_pic_data));				
				if(eCMDStatus!=eScaler_RS232_No_Service)
					eCMDStatus=eScaler_Mode_Change;
    		}	

    		if(!strcmp(vp->item, "cot_sel")){
				m_cot_data= atoi(vp->value);
				eCMDStatus=SendTVSetCommand('s',TV_CMD_COLOR_TEMP,(m_cot_data));
    		}	
			
//    		if(!strcmp(vp->item, "nra_sel")){
//				m_nra_data= atoi(vp->value);
//				eCMDStatus=SendTVSetCommand('s',TV_CMD_NR_AUTO_CLEAN,(m_nra_data));
//    		}	
			
//    		if(!strcmp(vp->item, "nrm_sel")){
//				m_nrm_data= atoi(vp->value);
//				eCMDStatus=SendTVSetCommand('s',TV_CMD_NR_MPEG,(m_nrm_data));
//    		}	
			
//    		if(!strcmp(vp->item, "nrd_sel")){
//				m_nrd_data= atoi(vp->value);
//				eCMDStatus=SendTVSetCommand('s',TV_CMD_NR_DNR,(m_nrd_data));
//    		}	
			if(!strcmp(vp->item, "pwr_act")){
				wdata= atoi(vp->value);
				m_pwr_data=(u8_t)wdata;
				if(m_pwr_data==0){
					m_eScaler_State=eScaler_Standy;
				    m_sync_data=0;
				}	
				else
					m_eScaler_State=eScaler_PowerOn;
				
				eCMDStatus=SendTVSetCommand('s',TV_CMD_POWER,(u8_t)wdata);
    		}	

			
			if(!strcmp(vp->item, "pic_rst_act")){
				wdata= atoi(vp->value);
				eCMDStatus=SendTVSetCommand('s',TV_CMD_PIC_RESET,0);
				SendGetAllPictureSettingCommand();				
				if(eCMDStatus!=eScaler_RS232_No_Service)
					eCMDStatus=eScaler_Mode_Change;
    		}	


			if(!strcmp(vp->item, "hma_sel")){
				m_hpa_data= atoi(vp->value);
				eCMDStatus=SendTVSetCommand('s',TV_CMD_HDMI_PC_AUDIO,(m_hpa_data));
			}	


			
			if(!strcmp(vp->item, "sud_rst_act")){
				wdata= atoi(vp->value);
				eCMDStatus=SendTVSetCommand('s',TV_CMD_SOUND_RESET,0);
				SendGetAllSoundSettingCommand();
				if(eCMDStatus!=eScaler_RS232_No_Service)
					eCMDStatus=eScaler_Mode_Change;
    		}	
			

    }
	//peter disable the flollowing statment ,it will cause the UART RX/TX Buffer reset
	//NetUartInit();
    
	if(eCMDStatus==eScaler_RS232_No_Service){
		
		hs->errormsg = "RS232 No Service";
		errmsgflag = ERRORMSG_WAIT5;
		httpd_init_file(fs_open("NoService.htm",&hs->op_index));    
	}	
	else if(eCMDStatus==eScaler_Mode_Change){
		hs->errormsg = "Change the Setting";
		errmsgflag = ERRORMSG_WAIT3;
		httpd_init_file(fs_open("NoService.htm",&hs->op_index));
	}	
	else{	
		//SaveModuleInfo2EEPROM(TelnetData.pEEPROM_Telnet,sizeof(STelnetInfo));   
    	httpd_init_file(fs_open("telnet.htm",&hs->op_index));
	}
    //peter change the responce way enable above statement and disable the next three statments
	//	hs->errormsg = "RESET to effect";
	//	errmsgflag = ERRORMSG_SETOK;
	//	httpd_init_file(fs_open("errormsg.htm",&hs->op_index));    
	
    return OK;
}
#endif


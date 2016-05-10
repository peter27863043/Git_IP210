
//-----------------------------
//  Define the  TV Set Command Code
// -----------------------------
#define TV_CMD_SYNC				0x20
#define TV_CMD_POWER 			0x21
#define TV_CMD_SRC				0x22
#define TV_CMD_CONTRAST			0x23
#define TV_CMD_BRIGHTNESS   	0x24
#define TV_CMD_SHARPNESS		0x25
#define TV_CMD_PIC_RESET		0x26
#define TV_CMD_HUE				0x27
#define TV_CMD_SATURATION		0x28
#define TV_CMD_NR_AUTO_CLEAN	0x29
#define TV_CMD_NR_MPEG			0x2a
#define TV_CMD_NR_DNR			0x2b
#define TV_CMD_BACKLIGHT		0x2c
#define TV_CMD_COLOUR			0x2d
#define TV_CMD_TINT				0x2e
#define TV_CMD_CONTRAST_BOOSTER 0x2f
#define TV_CMD_ASPECT_MODE		0x30
#define TV_CMD_ASPECT			0x31
#define TV_CMD_LANGUAGE			0x32
#define TV_CMD_SOUND_MODE		0x33
//#define TV_CMD_AUDIO_DISTORTION 0x34
#define TV_CMD_LAN_GET_MODE		0x34
#define TV_CMD_VOLUME			0x35
#define TV_CMD_MUTE				0x36
#define TV_CMD_TREBLE			0x37
#define TV_CMD_BASS				0x38
#define TV_CMD_BALANCE			0x39
#define TV_CMD_AUDIO_LEVEL_OFS	0x3a
#define TV_CMD_SOUND_RESET		0x3b
#define TV_CMD_HDMI_PC_AUDIO	0x3c
#define TV_CMD_MONITOR_ID		0x3d
#define TV_CMD_3D_COLOR			0x3e
#define TV_CMD_ACTIVE_BKL		0x3f
#define TV_CMD_BLUE_SCREEN		0x40
#define TV_CMD_IR_OUT			0x41
#define TV_CMD_IR_CTRL			0x42
#define TV_CMD_BUTTON_IR_CTRL	0x43
#define TV_CMD_AUTO_POWER_DOWN	0x44
#define TV_CMD_BUTTON_CTRL		0x45
#define TV_CMD_PANEL_LOCK		0x46
#define TV_CMD_IMAGE_RETENTION	0x47
#define TV_CMD_FREEZE			0x48
#define TV_CMD_PWR_SAVEING_ID	0x49
#define TV_CMD_OSD_INFO_BOX		0x5b
#define TV_CMD_ALL_RESET		0x7e
#define TV_CMD_PIC_MODE			0x81
#define TV_CMD_ADAPTIVE_CONT 	0x85
#define TV_CMD_COLOR_TEMP		0x86
#define TV_CMD_SPEAKER			0x89
#define TV_CMD_AMBIENT_LT_SNS	0x94
#define TV_CMD_OVER_SCAN		0x97
#define TV_CMD_RTC_YEAR			0x98
#define TV_CMD_RTC_MONTH		0x99
#define TV_CMD_RTC_DAY			0x9a
#define TV_CMD_RTC_HOUR			0x9b
#define TV_CMD_RTC_MINUTE		0x9c
#define TV_CMD_TOUCH_FEATURE	0x9d
#define TV_CMD_CLOCK_DISP		0x9e
#define TV_CMD_OSD_ROTATION		0x9f
#define TV_CMD_H_MONITOR		0xa4
#define TV_CMD_V_MONITOR		0xa5
#define TV_CMD_H_POSITION		0xa6
#define TV_CMD_V_POSITION		0xa7
#define TV_CMD_FRAME_COMP		0xa8
#define TV_CMD_POWER_SAVE		0xa9
#define TV_CMD_AUTO_ADJUST		0xaa
#define TV_CMD_AUTO_EXCUTE		0xab
#define TV_CMD_DISP_WALL_LED	0xae
#define TV_CMD_DISP_WALL_PW_ON	0xaf
#define TV_CMD_NO_SIG_PW_DW		0xc2
#define TV_CMD_DISP_SETTINGS	0xc3
#define TV_CMD_DISP_AUTO		0xc4
#define TV_CMD_DISP_4_3			0xc5
#define TV_CMD_TEMP_ALERT		0xdb
#define TV_CMD_TIMER_ON_OFF		0xe0
#define TV_CMD_SMTP_TIME		0xe3
#define TV_CMD_SEND_MAIL		0xe2
#define TV_CMD_NETWORK_SETTING  0xe3


//-----------------------------
//  Define the  TV Get Command Code
// -----------------------------
#define TV_GET_READY_TST 		0x00
#define TV_GET_MODEL_INFO 		0x20
#define TV_GET_TREBLE 			TV_CMD_TREBLE
#define TV_GET_BASS 			TV_CMD_BASS
#define TV_GET_BALANCE 			TV_CMD_BALANCE
#define TV_GET_AUDIO_DISTORTION 0x3a    
#define TV_GET_AUDIO_LEVEL_OFS  0x3b
#define TV_GET_HDMI_PC_AUDIO    TV_CMD_HDMI_PC_AUDIO
#define TV_GET_COLOUR    		0x41
#define TV_GET_TINT				0x42
#define TV_GET_CONTRAST_BOOSTER 0x43
#define TV_GET_CINEMA_MODE      0x44
#define TV_GET_OSD_INFO_BOX		0x5d
#define TV_GET_NR_AUTO_CLEAN	0x5e
#define TV_GET_NR_MPEG			0x5f
#define TV_GET_NR_DNR			0x60
#define TV_GET_CONTRAST			0x61
#define TV_GET_BRIGHTNESS		0x62
#define TV_GET_SHARPNESS		0x63
#define TV_GET_BACKLIGHT		0x64
#define TV_GET_BACKLIGHT		0x64
#define TV_GET_SOUND_MODE		0x65
#define TV_GET_VOLUME			0x66
#define TV_GET_MUTE				0x67
#define TV_GET_IR_CTRL			0x68
#define TV_GET_BUTTON_IR_CTRL	0x69
#define TV_GET_SRC				0x6a
#define TV_GET_AUTO_POWER_DOWN	0x6b
#define TV_GET_POWER			0x6c
#define TV_GET_PWR_SAVEING_ID	0x6d
#define TV_GET_IMAGE_RETENTION	0x72
#define TV_GET_BUTTON_CTRL		0x73
#define TV_GET_MONITOR_ID		0x75
#define TV_GET_OPERATION_TIME	0x76
#define TV_GET_ASPECT			0x77
#define TV_GET_LANGUAGE			0x78
#define TV_GET_ASPECT_MODE		0x79
#define TV_GET_LAN_GET_MODE		0x80
#define TV_GET_TOUCH_FEATURE	0x9e
#define TV_GET_3D_COLOR			0x9f
#define TV_GET_ACTIVE_BKL		0xa0
#define TV_GET_BLUE_SCREEN		0xa1
#define TV_GET_IR_OUT			0xa2
#define TV_GET_PANEL_LOCK		0xa3
#define TV_GET_DISP_WALL_LED	TV_CMD_DISP_WALL_LED
#define TV_GET_DISP_WALL_PW_ON  TV_CMD_DISP_WALL_PW_ON
#define TV_GET_PIC_MODE			0xb1
#define TV_GET_HUE				0xb2
#define TV_GET_SATURATION		0xb3
#define TV_GET_ADAPTIVE_CONT	0x85
#define TV_GET_COLOR_TEMP		0xb6
#define TV_GET_SPEAKER			0xb9
#define TV_GET_AMBIENT_LT_SNS   0xc4
#define TV_GET_OVER_SCAN		0xc7
#define TV_GET_RTC_YEAR			0xc8
#define TV_GET_RTC_MONTH		0xc9
#define TV_GET_RTC_DAY			0xca
#define TV_GET_RTC_HOUR			0xcb
#define TV_GET_RTC_MINUTE		0xcc
#define TV_GET_CLOCK_DISP		0xce

#define TV_GET_OSD_ROTATION		0xcf
#define TV_GET_H_MONITOR		0xd4
#define TV_GET_V_MONITOR		0xd5
#define TV_GET_H_POSITION		0xd6
#define TV_GET_V_POSITION		0xd7
#define TV_GET_FRAME_COMP		0xd8
#define TV_GET_POWER_SAVE		0xd9
#define TV_GET_AUTO_ADJUST		0xda
#define TV_GET_DISP_SETTINGS	0xdd
#define TV_GET_DISP_AUTO		0xde
#define TV_GET_DISP_4_3			0xdf
#define TV_GET_TIMER_ON_OFF		TV_CMD_TIMER_ON_OFF
#define TV_GET_SMTP_TIME        TV_CMD_SMTP_TIME
#define TV_GET_NETWORK_SETTING	0xe3

typedef enum{
	eScaler_Init,
	eScaler_Init_check,	
	eScaler_Init_Data,
	eScaler_Init_Timer,
	eScaler_Init_Info,
	eScaler_RS232_No_Service,
	eScaler_RS232_Ready,
	eScaler_Standy,
	eScaler_PowerOn,
	eScaler_Wait_Ack,
	eScaler_Mode_Change,
	eScaler_Mode_Reset,
} eSCALER_ST;


//---------------------------
//   TV Command Service Routine
// --------------------------


void TVStateInitial(void);


eSCALER_ST GetTVState(void);


eSCALER_ST SendTVSetCommand(u8_t cType, u8_t SetCmd, u8_t SetValue);

eSCALER_ST SendTVSetTimerCommand(u8_t TimerNo, u8_t *SetValue);




/******************************************************************************
*
*   Name:
*
*   Description:    
*
*   Copyright:      (c) 2005-2050    IC Plus Corp.
*                   All rights reserved.  By Joe
*
*******************************************************************************/
typedef struct _SCLT
{
	u8_t number_count;
	char Name[16];
}SCLT;
//-----------------------------------------
#ifdef MODULE_NET_UART
	#ifdef MODULE_CLI_UDP
		#define Command_NUM			43//41
	#else
		#define Command_NUM			49//37
	#endif
#else
	#define Command_NUM				16//14
#endif

#define ATSET				0
#define EXIT				1
#define R_CODE				2
#define LOADDEFAULT			3
#define T_CLIEND			4
#define F_CONTROL			5
#define RESET				6
#define ECHO				7
#define U_MANE				8
#define P_WORD				9
#define IP					10
#define SUBNET				11
#define GATEWAY				12
#define DNS					13
#define DHCP				14
#define N_NAME				15
#define TELNET				16
#define R_TELNET			17
#define D_PORT				18
#define C_PORT				19
#define SERVER_IP			20
#define TIMEOUT				21
#define UDP_S				22
#define L_PORT				23
#define R_IP				24
#define R_PORT				25
#define UART_MODE			26
#define CBAUDRATE			27
#define CHARA				28
#define CPARITY				29
#define STOP				30
#define ATD_T				31
#define DIS_CON				32
#define T_TIMEOUT			33
#define D_CHARA1			34
#define D_CHARA2			35
#define D_TIME				36
#define D_DROP				37
#define AT					38
#define ATA					39
#define S_TIMEOUT			40
#define ATINFO				41
#define ATFRESH				42

#ifdef MODULE_NET_UART
extern u8_t conn_flag;
#ifdef MODULE_DNS
extern u8_t udp_dns;
extern u8_t udp_get_dns();
#endif
#endif

void CLI_Timer();
void CLI_init();
void scan_cmd();
void command_type();
void help();
void command_setting();
void command_info();
void re_rorn();
void ATD_loop();
unsigned int getkey2(void);
extern void NetworkConfig();
extern void Restore_Switch();
extern void	Restore_Web();
extern void server_ip();
extern u8_t EEPROMLoadDefault(void);
extern void Delay(u16_t data delay);
extern u8_t code pr_v[5];
extern char code pr_n[5][6];
extern u8_t code sb_v[2];
extern char code sb_n[2][9];
extern u8_t code fc_v[2];
extern char code fc_n[2][9];
extern u8_t force_reset_countdown;
extern void ip210_isr_ctrl(u8_t status);
extern u8_t CLI_flag;
extern void hex_to_asc(u8_t hex, s8_t * buf);
extern u16_t asc_to_hex(char asc[]);
extern u8_t listen_wait;
extern u8_t code car[14];
extern u8_t code car1[5];
extern u8_t end[4];
extern u8_t code car[14];
extern u8_t code car1[5];
extern u8_t listen_back;
extern u8_t back_2_CLI;
extern u8_t ip_info;
extern u8_t code ans[13];
extern u8_t code ans1[5];
extern u32_t s_con_timer;


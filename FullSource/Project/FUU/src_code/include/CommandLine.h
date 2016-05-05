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



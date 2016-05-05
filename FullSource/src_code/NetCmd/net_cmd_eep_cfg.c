#include "module.h"
#ifdef MODULE_NET_CMD_EEP_CFG
#include "net_cmd.h"
#include "net_cmd_eep_cfg.h"
#include "eepdef.h"
net_cmd_eep_cfg_pri_t net_cmd_eep_cfg_pri;
net_cmd_eep_cfg_t* p_net_cmd_eep_cfg;
#define EEPROM_TMP_BUF TCP_TxBuf
#define NETCMD_EEP_CFG_HEADER_LEN (sizeof(SNetCmdPkt)-1+sizeof(net_cmd_eep_cfg_t)-1)
//------------------------------------------------------------------------------
void got_download_eep_req()
{
	u16_t remain_len;
	LocalCmdTxPrefix();
	NetCmdData.NetCmdPkt->CmdOP=WOWNLOAD_EEP_ACK;
	p_net_cmd_eep_cfg=(net_cmd_eep_cfg_t*)NetCmdData.NetCmdPkt->CmdData;	
	p_net_cmd_eep_cfg->project_num=PROJECT_NUM;
	p_net_cmd_eep_cfg->eep_version=EEPROM_VER;
	p_net_cmd_eep_cfg->total_len=sizeof(uEE_Info);
	p_net_cmd_eep_cfg->len=0;
	p_net_cmd_eep_cfg->offset=0;
	
	while(p_net_cmd_eep_cfg->offset<p_net_cmd_eep_cfg->total_len)
	{
		remain_len=p_net_cmd_eep_cfg->total_len-p_net_cmd_eep_cfg->offset;
		p_net_cmd_eep_cfg->len=(remain_len<SIZE_PER_EEP_PKT)?remain_len:SIZE_PER_EEP_PKT;
		
		for(i=0;i<p_net_cmd_eep_cfg->len;i++)
		{			
			EEPROM_Read_Byte(p_net_cmd_eep_cfg->offset+i,p_net_cmd_eep_cfg->eep_data+i);
		}
		EthernetSend(&uip_buf_real,p_net_cmd_eep_cfg->len+NETCMD_EEP_CFG_HEADER_LEN);
		wait_packet_sent();
		//p_net_cmd_eep_cfg->offset+=p_net_cmd_eep_cfg->total_len;
		p_net_cmd_eep_cfg->offset+=p_net_cmd_eep_cfg->len;  //joe for 080631
	}
}
//------------------------------------------------------------------------------
void got_upload_eep_req()
{
	p_net_cmd_eep_cfg=(net_cmd_eep_cfg_t*)NetCmdData.NetCmdPkt->CmdData;
	if(p_net_cmd_eep_cfg->offset==0)//it is the first upload packet
	{
		IP210_Update_Timer_Counter();
		net_cmd_eep_cfg_pri.start_time=timercounter;
		net_cmd_eep_cfg_pri.total_len=p_net_cmd_eep_cfg->total_len;
		net_cmd_eep_cfg_pri.offset=0;
		ModuleInfo->TelnetInfo->op_mode=NU_OP_MODE_DISABLE;		
	}
	
	if(net_cmd_eep_cfg_pri.offset==p_net_cmd_eep_cfg->offset)
	{
		memcpy(EEPROM_TMP_BUF+net_cmd_eep_cfg_pri.offset,p_net_cmd_eep_cfg->eep_data,p_net_cmd_eep_cfg->len);
		net_cmd_eep_cfg_pri.offset+=p_net_cmd_eep_cfg->len;
	}
	
	if(net_cmd_eep_cfg_pri.offset==net_cmd_eep_cfg_pri.total_len)
	{
		u16_t i;
		for(i=0;i<net_cmd_eep_cfg_pri.total_len;i++)
		{
			EEPROM_Write_Byte(i,EEPROM_TMP_BUF[i]);
		}
		send_upload_eep_ack();
		wait_packet_sent();
		force_reset_countdown=10;
	}
}
//------------------------------------------------------------------------------
void init_eep_cfg()
{
	net_cmd_eep_cfg_pri.start_time=0;
}
//------------------------------------------------------------------------------
void timer_eep_cfg()
{
	if(net_cmd_eep_cfg_pri.start_time)
	{
		if(Delta_Time(net_cmd_eep_cfg_pri.start_time)>EEP_CFG_REBOOT_TIMEOUT)
		force_reset_countdown=1;
	}
}
//------------------------------------------------------------------------------
void send_upload_eep_ack()
{
	LocalCmdTxPrefix();
	NetCmdData.NetCmdPkt->CmdOP=UPLOAD_EEP_ACK;
	EthernetSend(&uip_buf_real,sizeof(SNetCmdPkt)-1);
}
//------------------------------------------------------------------------------
void got_load_def_eep_req()
{
	EEPROMLoadDefault();
	
	LocalCmdTxPrefix();
	NetCmdData.NetCmdPkt->CmdOP=LOAD_DEF_EEP_ACK;
	EthernetSend(&uip_buf_real,sizeof(SNetCmdPkt)-1);
	wait_packet_sent();
	force_reset_countdown=1;
}
#endif //MODULE_NET_CMD_EEP_CFG

#include "module.h"
#ifdef NET_CMD_EEP_CFG_H
#ifdef MODULE_NET_CMD_EEP_CFG
#define SIZE_PER_EEP_PKT 1200
//30 sec
#define EEP_CFG_REBOOT_TIMEOUT 3000
typedef struct _net_cmd_eep_cfg_t
{
	u32_t project_num;
	u32_t eep_version;
	u16_t total_len;
	u16_t len;
	u16_t offset;
	u8_t  eep_data[1];
}net_cmd_eep_cfg_t;

typedef struct _net_cmd_eep_cfg_pri_t
{
	u32_t start_time;
	u16_t total_len;
	u16_t offset;
}net_cmd_eep_cfg_pri_t;

void got_download_eep_req();

void got_upload_eep_req();
void send_upload_eep_ack();

void got_load_def_eep_req();
void send_load_def_eep_ack();

void timer_eep_cfg();
#endif //MODULE_NET_CMD_EEP_CFG
#endif//NET_CMD_EEP_CFG_H
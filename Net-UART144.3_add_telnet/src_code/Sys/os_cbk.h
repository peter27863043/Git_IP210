extern u8_t need_load_default;
extern u8_t force_reset_countdown;
extern void FUU_Run(void);
extern u8_t firm_flag;
extern u8_t boot_flag;
extern void rfc_loop();
extern u8_t force_reset_countdown;
extern void gateway_mac_req();
extern u16_t gateway_count;
extern u8_t gateway_mac[6];

void os_loop();
void os_loop_100ms();
void LoadDefault();
void CheckPktSend();
void os_initial();
void Check_Link();
void Check_FirmwareUpdate();
void fiber_cover();
void CPU_REBOOT();
void Gateway_timer();
void Check_Speed();

#define GATEWAY_RESET_TIME	600



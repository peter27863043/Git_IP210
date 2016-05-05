#define KS_WDT_WAIT_CYCLE                 7//if WatchDog Timer is enabled, the larger KS_WDT_WAIT_CYCLE, the longer time allowing CPU to hang before WDT Reset
#define KS_WDT_ENABLE                     1

void WatchDogTimerEnable(void);
void WatchDogTimerDisable(void);
void WatchDogTimerReset(void);
void WatchDogTimerInit(void);
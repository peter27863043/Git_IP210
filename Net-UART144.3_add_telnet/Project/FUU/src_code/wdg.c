#include "type_def.h"
#include "IP210reg.h"
#include "wdg.h"
u8_t    wdt_Setting=KS_WDT_ENABLE;
u8_t    wdtwcyc;
/******************************************************************************
*
*  Function:    WatchDogTimerEnable()
*
*  Description: start WDT counting
*
*  Parameters:  None
*
*  Returns:     None
*
*******************************************************************************/
void WatchDogTimerEnable(void)
{
    if(wdt_Setting==KS_WDT_ENABLE)
	{
        WDTEN=1;		//enable Watch Dog Timer
	}
	else
	{
        WDTEN=0;		//Disable Watch Dog Timer
	}
}

/******************************************************************************
*
*  Function:    WatchDogTimerDisable()
*
*  Description: stop WDT from counting
*
*  Parameters:  None
*
*  Returns:     None
*
*******************************************************************************/
void WatchDogTimerDisable(void)
{
    WDTEN=0;		//Disable Watch Dog Timer
}
/******************************************************************************
*
*  Function:    WatchDogTimerReset()
*
*  Description: issue a WDT reset in case cpu is reset due to WDT overflow
*
*  Parameters:  None
*
*  Returns:     None
*
*******************************************************************************/
void WatchDogTimerReset(void)
{
    WDTRST=1;//reset WatchDog Timer
    WDTCLR=1; WDTCLR=0;//reset independent WatchDog Timer
}
/******************************************************************************
*
*  Function:    WatchDogTimerInit()
*
*  Description: select WDT counter width and count operation interval
*
*  Parameters:  KSetting_Info.wdtwcyc
*
*  Returns:     None
*
*******************************************************************************/
void WatchDogTimerInit(void)
{
    //select timer width
    CKCON|=0xc0;	//26bits wait time (about 1.3sec if WDTWCYC is 0)

    //determine timer count interval by SFR:WDTWCYC
    WDTWCYC=KS_WDT_WAIT_CYCLE;
}
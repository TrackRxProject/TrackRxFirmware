// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_ints.h"
#include "debug.h"
#include "interrupt.h"
#include "timer.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"

// TI-RTOS includes
#if defined(USE_TIRTOS) || defined(USE_FREERTOS) || defined(SL_PLATFORM_MULTI_THREADED)
#include <stdlib.h>
#include "osi.h"
#endif

#include "timer_if.h"
#include "trackrxfirmware.h"
#include "sk6812miniLED_driver.h"

int wait;
void ledTimerIRQ(void)
{
	Timer_IF_InterruptClear(TIMERA0_BASE);
	Timer_IF_DeInit(TIMERA0_BASE, TIMER_A);
	wait = 0;
}

/****************************** PUBLIC API ***********************************/
void startLEDwait_timer()
{
	Timer_IF_Init(PRCM_TIMERA0, TIMERA0_BASE, TIMER_CFG_PERIODIC, TIMER_A, 0);
	Timer_IF_IntSetup(TIMERA0_BASE, TIMER_A, ledTimerIRQ);
	/* Start the timer, in milliseconds */
	Timer_IF_Start(TIMERA0_BASE, TIMER_A, 10000);
	wait = 1;
}

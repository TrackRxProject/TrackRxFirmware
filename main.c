//*****************************************************************************
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

// The above Copyright applies to only the section labeled TI

// Standard includes
#include <stdio.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "hw_apps_rcm.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "gpio.h"
#include "utils.h"

// Common interface includes
#include "gpio_if.h"
#include "pinmux.h"

#include "trackrxfirmware.h"

#define APPLICATION_VERSION     "1.1.1"

//*****************************************************************************
//                TI Copyrighted Code -- Start
//*****************************************************************************
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

static void BoardInit(void);
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
    //
    // Set vector table base
    //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif

    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}
//*****************************************************************************
//                TI Copyrighted Code -- End
//*****************************************************************************

/******************* Dosing State Machine Definitions *************************/
#define NOTIFY 			0
#define QUIET_WAIT 		1
#define DOSE_ACCESS 	2
#define LOG				3
#define SET_HIBERNATE	4
#define HIBERNATE		5

void sleepUntilNextDose();
void dosingStateMachine();
/******************************************************************************/
/******************* Prescribing State Machine Definitions ********************/
void prescribeStateMachine();
/******************************************************************************/
/*********************** Registering Bottle Definitions ***********************/
void registerBottle();
/******************************************************************************/
void LEDSleepyBlinkyRoutine();

void registerBottle()
{
	/* TODO: 	wait until set up is initiated,
	 * 			transfer credentials,
	 * 			mark bottle as registered on flash & server
	 */
}

void dosingStateMachine()
{
	int doseState = NOTIFY;
	while (doseState != HIBERNATE)
	{
		if(doseState == NOTIFY)
		{
			/* TODO: Notification code */
		} else if (doseState == QUIET_WAIT)
		{
			/* TODO: If not opened in NOTIFY, kill notifications and wait quietly*/
		} else if (doseState == DOSE_ACCESS)
		{
			/* TODO: Allow patient access to the dose */
		} else if (doseState == LOG)
		{
			/* TODO: Log the dose being taken */
		} else if (doseState == SET_HIBERNATE)
		{
			/* TODO: Set the next dosing window */
		}
	}
	sleepUntilNextDose();
}

void sleepUntilNextDose()
{
	float hours = 0.0083333; //TODO: Get this from the server or something
	unsigned long long ticks = 3600*3278*hours;
	PRCMHibernateWakeupSourceEnable(PRCM_HIB_SLOW_CLK_CTR);
	PRCMHibernateIntervalSet(ticks);
	PRCMHibernateEnter();
}

void LEDSleepyBlinkyRoutine()
{
    //
    // Toggle the lines initially to turn off the LEDs.
    // The values driven are as required by the LEDs on the LP.
    //
    GPIO_IF_LedOff(MCU_ALL_LED_IND);
    //while(1)
    //{
        //
        // Alternately toggle hi-low each of the GPIOs
        // to switch the corresponding LED on/off.
        //
        MAP_UtilsDelay(8000000);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        MAP_UtilsDelay(8000000);
        GPIO_IF_LedOff(MCU_RED_LED_GPIO);
        MAP_UtilsDelay(8000000);
        GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
        MAP_UtilsDelay(8000000);
        GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
        MAP_UtilsDelay(8000000);
        GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
        MAP_UtilsDelay(8000000);
        GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
        //httpDemo();
        sleepUntilNextDose();
    //}

}

int main()
{
    BoardInit();

    // Power on the corresponding GPIO port B for 9,10,11.
    // Set up the GPIO lines to mode 0 (GPIO)
    //httpDemo();
    PinMuxConfig();
    GPIO_IF_LedConfigure(LED1|LED2|LED3);
    GPIO_IF_LedOff(MCU_ALL_LED_IND);

    LEDSleepyBlinkyRoutine();

    if(0/*TODO: If woken by GPIO interrupt*/)
    {
    	/* Pharmacists programming stuff here */
    	prescribeStateMachine();
    } else if(0/*TODO: If REGISTERED_BOTTLE flag is not set in flash*/)
    {
    	registerBottle();
    } else
    {
    	/* Notify, Administer, Log, and Hibernate */
    	dosingStateMachine();
    }

    return 0;
}



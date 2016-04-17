/*
 * Name: pwm.c
 * Author: Jacob R. Stevens
 * Date: 2/21/2016
 *
 * Contains the internal methods and public API methods
 * used to drive the motor components of the pill bottle
 * via PWM
 */

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_apps_rcm.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "rom.h"
#include "rom_map.h"
#include "timer.h"
#include "utils.h"
#include "prcm.h"

#include "pinmux.h"
#include "pwm.h"

void updateDutyCycle_pwm(unsigned long ulBase, unsigned long ulTimer,
                     unsigned char ucLevel)
{
    //
    // Match value is updated to reflect the new dutycycle settings
    //
    MAP_TimerMatchSet(ulBase,ulTimer,(ucLevel*DUTYCYCLE_GRANULARITY));
}

//****************************************************************************
//
//! Setup the timer in PWM mode
//!
//! \param ulBase is the base address of the timer to be configured
//! \param ulTimer is the timer to be setup (TIMER_A or  TIMER_B)
//! \param ulConfig is the timer configuration setting
//! \param ucInvert is to select the inversion of the output
//!
//! This function
//!    1. The specified timer is setup to operate as PWM
//!
//! \return None.
//
//****************************************************************************
void setupTimerPWMMode(unsigned long ulBase, unsigned long ulTimer,
                       unsigned long ulConfig, unsigned char ucInvert)
{
    //
    // Set GPT - Configured Timer in PWM mode.
    //
    MAP_TimerConfigure(ulBase,ulConfig);
    MAP_TimerPrescaleSet(ulBase,ulTimer,0);

    //
    // Inverting the timer output if required
    //
    MAP_TimerControlLevel(ulBase,ulTimer,ucInvert);

    //
    // Load value set to ~0.5 ms time period
    //
    MAP_TimerLoadSet(ulBase,ulTimer,TIMER_INTERVAL_RELOAD);

    //
    // Match value set so as to output level 0
    //
    MAP_TimerMatchSet(ulBase,ulTimer,TIMER_INTERVAL_RELOAD);

}

void enablePWMModules_pwm()
{
    setupTimerPWMMode(TIMERA3_BASE, TIMER_B,
            (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM), 1);

    MAP_TimerEnable(TIMERA3_BASE,TIMER_A);
}

void disablePWMModules_pwm()
{
    MAP_TimerDisable(TIMERA3_BASE, TIMER_A);
    MAP_PRCMPeripheralClkDisable(PRCM_TIMERA3, PRCM_RUN_MODE_CLK);
}
/**************** EXTERNALLY CALLED METHODS **********************************/
void startNotify_pwm()
{
	int level = 255;
	for (; level < 256; level++)
	{
		updateDutyCycle_pwm(TIMERA3_BASE, TIMER_A, level);
    	UtilsDelay(80000);
    }
    for (; level >= 150; level--)
    {
    	updateDutyCycle_pwm(TIMERA3_BASE, TIMER_A, level);
    	UtilsDelay(80000);
    }
}

void stopNotify_pwm()
{
	updateDutyCycle_pwm(TIMERA3_BASE, TIMER_A, 0);
}

void openBottle_pwm()
{
	/*TODO: Implement me */
}

/*****************************************************************************/

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

#include "device.h"

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_hib1p2.h"
#include "hw_common_reg.h"
#include "timer.h"
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
#include "common.h"
#include "utils_if.h"
#include "timer_if.h"

#include "trackrxfirmware.h"
#include "sk6812miniLED_driver.h"
#include "flash.h"
#include "pwm.h"
#include "ultrasonic.h"
#include "http.h"

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

int stopMotor = 0;

void notify()
{
	int level;
	for (level = 150; level < 256; level++)
	{
		//updateDutyCycle_pwm(TIMERA3_BASE, TIMER_A, level);
		showColor_led(255, level, 0);
    }
    for (level = 255; level >= 150; level--)
    {
    	//updateDutyCycle_pwm(TIMERA3_BASE, TIMER_A, level);
    	showColor_led(255, level, 0);
    }
}

void registerBottle()
{
	/* TODO: 	wait until set up is initiated,
	 * 			transfer credentials,
	 */
	int interval = getIntervalAndActivate_http();
	sl_Stop(0xFF);
	sl_Start(NULL,NULL,NULL);
	int ret = writeSSID_flash("Abraham Linksys", 15); //TODO: Get this from ASK
	ret = writeInterval_flash(interval);
	unsigned char activationFlag = 1;
	ret = writeActivationFlag_flash(&activationFlag);
	unsigned char zero = 0;
	ret = initAdherence_flash();
	ret = writeHistoryLength_flash(&zero);
	ret = writeMissingDose_flash(&zero);
	sl_Stop(0xFF);
	//sleepUntilNextDose(0.0083333); //TODO: use value from flash instead
}

void dispense()
{
	GPIOIntTypeSet(GPIOA0_BASE,GPIO_PIN_0,GPIO_RISING_EDGE); // GPIO_10
	GPIOIntRegister(GPIOA0_BASE,gpioISR);
	GPIOIntClear(GPIOA0_BASE,GPIO_PIN_0);
	//TODO: Turn on motor
	//TODO: Delay
	GPIOIntEnable(GPIOA0_BASE, GPIO_PIN_0);
	//while(!stopMotor);
	//stopMotor = 0;
}

void giveDose(unsigned char missingDose)
{
	int ret;
	unsigned char length;
	if (!missingDose)
	{
		enablePWMModules_pwm();
		startLEDwait_timer();
		while(wait) { notify();}
		clearNotification_led();
		stopNotify_pwm();
		disablePWMModules_pwm();
		int authorized = !stopMotor; //getPatientAuthorization_ultrasonic(); //blocking call
		if(authorized)
			openBottle_pwm();
		else {
			sl_Start(NULL,NULL,NULL);
			missingDose = 1;
			readMissingDose_flash(&missingDose);
			// a dose was missed before this dose became currently missing
			if(missingDose)
			{
				writeAdherence_flash(0);
				length = 0;
				readHistoryLength_flash(&length);
				if (length == ADHERENCE_LENGTH) {
					unsigned char adherence[ADHERENCE_LENGTH];
					readAdherenceHistory_flash(adherence);
					unsigned char zero = 0;
					writeHistoryLength_flash(&zero);
					sl_Stop(0xFF);
					putAdherence_http(adherence, ADHERENCE_LENGTH);
					sl_Stop(0xFF);
				} else
				sl_Stop(0xFF);
			} else {
				missingDose = 1;
				ret = writeMissingDose_flash(&missingDose);
				sl_Stop(0xFF);
			}
			stopMotor = 0; //TODO DELETE ME
			return;
		}
		stopMotor = 0; //TODO DELETE ME
		sl_Start(NULL,NULL,NULL);
		//if there was a missing dose pending but the dose was
		//just dispensed, clear the pending missing dose
		if (missingDose)
		{
			missingDose = 0;
			writeMissingDose_flash(&missingDose);
		}
		// missingDose will now hold if a missing dose was pending
		ret = readMissingDose_flash(&missingDose);
		length = 0;
		if(missingDose)
		{
			unsigned char zero = 0;
			ret = writeAdherence_flash(zero);
			//Dose has been recorded as a miss, so clear flag
			writeMissingDose_flash(&zero);
			readHistoryLength_flash(&length);
			if (length == ADHERENCE_LENGTH) {
				unsigned char adherence[ADHERENCE_LENGTH];
				readAdherenceHistory_flash(adherence);
				unsigned char zero = 0;
				writeHistoryLength_flash(&zero);
				sl_Stop(0xFF);
				putAdherence_http(adherence, ADHERENCE_LENGTH);
				sl_Stop(0xFF);
				sl_Start(NULL,NULL,NULL);
			}
		}
	} else {
		// clear the missing flag before logging dose and hibernating
		sl_Start(NULL,NULL,NULL);
		int missingFlag = 0;
		writeMissingDose_flash(&missingFlag);
	}
	ret = writeAdherence_flash(1);
	ret = readHistoryLength_flash(&length);
	if (length == ADHERENCE_LENGTH) {
		unsigned char adherence[ADHERENCE_LENGTH];
		readAdherenceHistory_flash(adherence);
		unsigned char zero = 0;
		writeHistoryLength_flash(&zero);
		sl_Stop(0xFF);
		putAdherence_http(adherence, ADHERENCE_LENGTH);
		sl_Stop(0xFF);
	} else
		sl_Stop(0xFF);

	//TODO: read this from flash
	//sleepUntilNextDose(0.0083333);
}

void prescribeStateMachine()
{
	getPharmAuthorization_ultrasonic();
	sl_Start(NULL,NULL,NULL);
	unsigned char zero = 0;
	int ret = writeActivationFlag_flash(&zero);
	ret = writeHistoryLength_flash(&zero);
	/* TODO: Write UUID into flash */
	sl_Stop(NULL);
}

void sleepUntilNextDose(float hours)
{
	unsigned long long ticks = 117964800*hours;//3600*3278*hours;
	// Zero out the slow counter
	PRCMHIBRegWrite(HIB3P3_BASE + 0x0000001C, 0);
	PRCMHIBRegWrite(HIB3P3_BASE + 0x00000020, 0);
	PRCMHibernateIntervalSet(ticks);
	PRCMHibernateWakeUpGPIOSelect(PRCM_HIB_GPIO17, PRCM_HIB_FALL_EDGE);
	PRCMHibernateWakeupSourceEnable(PRCM_HIB_SLOW_CLK_CTR | PRCM_HIB_GPIO17);
	PRCMHibernateEnter();
}

int main()
{
	unsigned long long current, alarm, alarmLSW, alarmMSW;
	current = PRCMSlowClkCtrGet();
	alarmLSW = PRCMHIBRegRead(HIB3P3_BASE + 0x0000001C);
	alarmMSW = PRCMHIBRegRead(HIB3P3_BASE + 0x00000020);
	alarm = (alarmMSW) << 32;
	alarm |= alarmLSW;
	int gpioWakeup;
start:
	gpioWakeup = 0;//(alarm > current);

	BoardInit();
    PinMuxConfig();
    dispense();

    sl_Start(NULL,NULL,NULL);
    unsigned char registered_bottle = 0;
    int ret = readActivationFlag_flash(&registered_bottle);
    /* Read WiFi Credentials from Flash */
    readSSID_flash(SSID_NAME);
    unsigned char length;
    readSSIDLen_flash(&length);
    SSID_NAME[length] = '\0';
    sl_Stop(0xFF);
    if(gpioWakeup)
    {
    	sl_Start(NULL, NULL, NULL);
    	unsigned char missingDoseFlag = 0;
    	ret = readMissingDose_flash(&missingDoseFlag);
    	sl_Stop(NULL);
    	if(missingDoseFlag)
    		giveDose(missingDoseFlag);
    	else
    		prescribeStateMachine();
    } else if(!registered_bottle)
    {
    	registerBottle();
    	goto start;
    } else
    {
    	/* Notify, Administer, Log, and Hibernate */
    	giveDose(0);
    	goto start;
    }

    return 0;
}

void gpioISR()
{
	unsigned long ulPinState = GPIOIntStatus(GPIOA0_BASE, 1);
	if(ulPinState & GPIO_PIN_0)
		stopMotor = 1;
	GPIOIntClear(GPIOA0_BASE, GPIO_PIN_0);
}





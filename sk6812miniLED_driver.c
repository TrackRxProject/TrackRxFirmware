#include "pinmux.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "rom.h"
#include "rom_map.h"
#include "gpio.h"
#include "prcm.h"
#include "utils.h"

#include "gpio_if.h"
#include "sk6812miniLED_driver.h"

#define T1H 900 //ns
#define T1L 600 //ns
#define T0H 400 //ns
#define T0L 900 //ns
#define LATCH 6000 //ns
#define NS_PER_SEC (1000000000L)
#define CYCLES_PER_SEC (80000000L)
#define NS_PER_CYCLE (NS_PER_SEC / CYCLES_PER_SEC)
#define NS_TO_CYCLES(n) ( (n) / NS_PER_CYCLE)

void sendBit(unsigned char bit)
{
	if(!bit)
	{

		GPIOPinWrite(GPIOA2_BASE, 0x40,64);
		UtilsDelay(6);
		GPIOPinWrite(GPIOA2_BASE, 0x40, 0);
		UtilsDelay(4);
	} else
	{
		//send a 1
		GPIOPinWrite(GPIOA2_BASE, 0x40,64);
		GPIOPinWrite(GPIOA2_BASE, 0x40, 0);
		UtilsDelay(6);
	}
}

static void sendByte(unsigned char byte)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		sendBit(byte & 0x80);
		byte <<=1;
	}
}

void setPixel(unsigned char r, unsigned char g, unsigned char b)
{
	sendByte(g);
	sendByte(r);
	sendByte(b);
}

void latchPixels()
{
	MAP_UtilsDelay(560);
}

void showColor(unsigned char r, unsigned char g, unsigned char b)
{
	int i;
	for(i = 0; i < PIXELS; i+=(PIXELS/60))
	{
		int p = 0;
		while(p++<=i)
			setPixel(r,g,b);
		while(p++<=PIXELS)
			setPixel(0,0,0);
		latchPixels();
	}
}

/* Following methods will be used by other files as an API */

void clearNotification_led()
{
	showColor(255,255,255);
}

void setNotification_led()
{
	showColor(255,0,0);
}

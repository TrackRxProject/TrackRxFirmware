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

/* Following methods will be used by other files as an API */
void ledSetup()
{
	MAP_PinTypeGPIO(PIN_50, PIN_MODE_0, false);
	MAP_GPIODirModeSet(GPIOA0_BASE, 0x1, GPIO_DIR_MODE_OUT);

}

void setPixel(unsigned char r, unsigned char g, unsigned char b)
{
	sendByte(g);
	sendByte(r);
	sendByte(b);
}

void latchPixel()
{
	MAP_UtilsDelay(560);
}

void testLEDs()
{
	//ledSetup();

	/*while(1) {
		sendBit(0);
		sendBit(1);
	}*/

	while(1) {
	int pixels = 96;
	int i;
	for (i = 0; i<pixels; i+=(pixels/60))
	{
		unsigned int p = 0;
		while(p++<=i)
			setPixel(255,0,0);

		while(p++<=pixels)
			setPixel(0,0,0);

		latchPixel();

	}
	for (i = 0; i<pixels; i+=(pixels/60))
	{
		unsigned int p = 0;
		while(p++<=i)
			setPixel(0,255,0);

		while(p++<=pixels)
			setPixel(0,0,0);

		latchPixel();

	}
	}
}

void clearNotification_led()
{
/* TODO: IMPLEMENT ME */
}

void setNotification_led()
{
	/*TODO: IMPLEMENT ME */
}

/*
 * trackrxfirmware.c
 *
 *  Created on: Feb 8, 2016
 *      Author: Jake
 *
 *      This file contains utility functions used throughout the firmware files
 */
#include <math.h>
#include "trackrxfirmware.h"

static int pow10(int power)
{
	int retVal = 1;
	for(; power > 0; power--)
		retVal *= 10;
	return retVal;
}

unsigned char * intToCharArray (int integer, int length, unsigned char * charArray)
{
	int i;
	for(i = 0; i < length; i++)
	{
		int temp = integer % 10;
		charArray[i] = temp;//integer % 10;
		integer /= 10;
	}
	return charArray;
}

int charArrayToInt(unsigned char * charArray, int length)
{
	int integer = 0;
	length--;
	for(; length >= 0; length--)
	{
		int temp = charArray[length];
		temp *= pow10(length);
		integer += temp;//charArray[length] * pow10(length);
	}
	return integer;
}





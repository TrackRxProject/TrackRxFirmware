/*
 * trackrxfirmware.h
 *
 *  Created on: Feb 4, 2016
 *      Author: Jake
 */

#ifndef TRACKRXFIRMWARE_H_
#define TRACKRXFIRMWARE_H_

#define UUID_LENGTH 36

int httpDemo();
unsigned char * intToCharArray (int integer, int length, unsigned char * charArray);
int charArrayToInt(unsigned char * charArray, int length);

#endif /* TRACKRXFIRMWARE_H_ */

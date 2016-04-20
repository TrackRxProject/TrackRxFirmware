/*
 * trackrxfirmware.h
 *
 *  Created on: Feb 4, 2016
 *      Author: Jake
 */

#ifndef TRACKRXFIRMWARE_H_
#define TRACKRXFIRMWARE_H_

#define UUID_LENGTH 36
#define ADHERENCE_LENGTH 4
void sleepUntilNextDose(float hours);
void giveDose(unsigned char missingDose);
void notify();
/******************************************************************************/
/******************* Prescribing State Machine Definitions ********************/
void prescribeStateMachine();
/******************************************************************************/
/*********************** Registering Bottle Definitions ***********************/
void registerBottle();
/******************************************************************************/
int httpDemo();
unsigned char * intToCharArray (int integer, int length, unsigned char * charArray);
int charArrayToInt(unsigned char * charArray, int length);
void gpioISR();
void dispense();

extern int wait;
extern signed char SSID_NAME[32];


#endif /* TRACKRXFIRMWARE_H_ */

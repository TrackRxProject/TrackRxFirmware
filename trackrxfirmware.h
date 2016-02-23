/*
 * trackrxfirmware.h
 *
 *  Created on: Feb 4, 2016
 *      Author: Jake
 */

#ifndef TRACKRXFIRMWARE_H_
#define TRACKRXFIRMWARE_H_

#define UUID_LENGTH 36
/******************* Dosing State Machine Definitions *************************/
#define NOTIFY 			0
#define QUIET_WAIT 		1
#define DOSE_ACCESS 	2
#define LOG				3
#define SET_HIBERNATE	4
#define HIBERNATE		5

void sleepUntilNextDose(float hours);
void dosingStateMachine();
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


#endif /* TRACKRXFIRMWARE_H_ */

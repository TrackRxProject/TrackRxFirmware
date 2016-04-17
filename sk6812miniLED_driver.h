/*
 * sk6812miniLED_driver.h
 *
 *  Created on: Feb 15, 2016
 *      Author: Jake
 */

#ifndef SK6812MINILED_DRIVER_H_
#define SK6812MINILED_DRIVER_H_

#define PIXELS 96*3 //96*number of leds
void showColor_led(unsigned char r, unsigned char g, unsigned char b);
void clearNotification_led();
void setNotification_led();


#endif /* SK6812MINILED_DRIVER_H_ */

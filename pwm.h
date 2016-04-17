/*
 * pwm.h
 *
 *  Created on: Feb 21, 2016
 *      Author: Jake
 */

#ifndef PWM_H_
#define PWM_H_

#define TIMER_INTERVAL_RELOAD   40035 /* =(255*157) */
#define DUTYCYCLE_GRANULARITY   157

void updateDutyCycle_pwm(unsigned long ulBase, unsigned long ulTimer,
                     unsigned char ucLevel);
void setupTimerPWMMode(unsigned long ulBase, unsigned long ulTimer,
                       unsigned long ulConfig, unsigned char ucInvert);
void disablePWMModules_pwm();
void enablePWMModules_pwm();

void openBottle_pwm();
void startNotify_pwm();
void stopNotify_pwm();

#endif /* PWM_H_ */

 /******************************************************************************
 *
 * Module: pwm
 *
 * File Name: pwm.h
 *
 * Description: Source file for the pwm driver
 *
 * Author: Marwan Medhat
 *
 *******************************************************************************/

#ifndef PWM_H_
#define PWM_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define MAX_TIMER_COUNT 255
#define MAX_DUTY_CYCLE 100

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * start the PWM timer
 */
void PWM_Timer0_Start(uint8 duty_cycle);

#endif /* PWM_H_ */

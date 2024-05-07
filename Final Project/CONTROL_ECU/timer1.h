 /******************************************************************************
 *
 * Module: Timer1
 *
 * File Name: timer1.h
 *
 * Description: Header file for the Timer1 driver
 *
 * Author: Marwan Medhat
 *
 *******************************************************************************/
#ifndef TIMER1_H_
#define TIMER1_H_

#include "std_types.h"
/*******************************************************************************
 *                      Types Declaration                                      *
 *******************************************************************************/
typedef enum
{
	T1_1 = 1, T1_8,T1_64,T1_256,T1_1024
}TIMER1_Prescaler;

typedef enum
{
	OVERFLOW,COMPARE
}TIMER1_Mode;

typedef struct{
	uint16 initial_value;
	uint16 compare_value;
	TIMER1_Prescaler prescaler;
	TIMER1_Mode mode;
}TIMER1_ConfigType;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * initialize the timer
 */
void TIMER1_init(const TIMER1_ConfigType * Config_Ptr);
/*
 * Description :
 * Stop and clear the timer settings
 */
void TIMER1_deinit(void);
/*
 * Description :
 * Sets the call back function which will be executed every interrupt
 */
void TIMER1_setCallBack(void(*a_ptr)(void));
#endif /* TIMER1_H_ */

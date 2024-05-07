 /******************************************************************************
 *
 * Module: pwm
 *
 * File Name: pwm.c
 *
 * Description: Source file for the pwm driver
 *
 * Author: Marwan Medhat
 *
 *******************************************************************************/

#include <avr/io.h>
#include "common_macros.h"
#include "pwm.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * set WGM01 and WGM00 for FAST PWM mode
 * COM01 = 1 COM00 = 0 for non inverting PWM mode
 * CS02 = 0 CS01 = 1 CS00 = 0 to set F_CPU/8 prescaler
 * set PB3 as output pin the OC0 pin
 */
void PWM_Timer0_Start(uint8 duty_cycle){
	SET_BIT(DDRB,3);
	TCCR0 |= (1<<WGM01) | (1<<WGM00) | (1<<COM01) | (1<<CS01);
	TCNT0 = 0;
	if( duty_cycle > MAX_DUTY_CYCLE || duty_cycle < 0)
	{
		/* do nothing */
	}
	else
	{
	OCR0 = (uint8) ( ( (uint16) MAX_TIMER_COUNT * duty_cycle ) / ( MAX_DUTY_CYCLE ) );
	}
}

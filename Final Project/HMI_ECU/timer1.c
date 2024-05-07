 /******************************************************************************
 *
 * Module: Timer1
 *
 * File Name: timer1.c
 *
 * Description: Source file for the Timer1 driver
 *
 * Author: Marwan Medhat
 *
 *******************************************************************************/
#include "timer1.h"
#include "avr/io.h"
#include <avr/interrupt.h>
/*******************************************************************************
 *                      Global Variables                                       *
 *******************************************************************************/
static volatile void (*g_callBackPtr)(void) = NULL_PTR;
/*******************************************************************************
 *                                ISR's                                        *
 *******************************************************************************/
ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR) /* check if there is a call back function */
	{
		(*g_callBackPtr)(); /* call back function will be executed */
	}
}

ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR) /* check if there is a call back function */
	{
		(*g_callBackPtr)(); /* call back function will be executed */
	}
}
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * initialize the timer
 */
void TIMER1_init(const TIMER1_ConfigType * Config_Ptr)
{
	TCNT1 = Config_Ptr->initial_value; /* sets the initial counter register according to the value in the ConfigType structure */
	TCCR1A |= (1<<FOC1A) | (1<<FOC1B); /* enable for non-pwm mode */
	TCCR1B = ( TCCR1B & 0xF7 ) | ( ( Config_Ptr->mode & 0x01 ) << WGM12 ); /* will set the bits responsible for the timer mode */
	TCCR1B = ( TCCR1B & 0xF8 ) | ( Config_Ptr->prescaler & 0x07 ) ; /* will set the bits responsible for the prescaler */

	if ( Config_Ptr->mode == COMPARE)
	{
		OCR1A = Config_Ptr->compare_value; /* sets the compare value if compare mode is set */
		TIMSK= (1<<OCIE1A); /* enable the compare mode interrupt */
	}
	else
	{
		TIMSK = (1<<TOIE1); /* enable the overflow mode interrupt */
	}
}
/*
 * Description :
 * Stop and clear the timer settings
 */
void TIMER1_deinit(void)
{
	/* Clear All Timer1 Registers */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 0;

	/* Disable the compare and overflow interrupt */
	TIMSK &= ~(1<<OCIE1A) & ~(1<<TOIE1);

	/* Reset the global pointer value */
	g_callBackPtr = NULL_PTR;
}
/*
 * Description :
 * Sets the call back function which will be executed every interrupt
 */
void TIMER1_setCallBack(void(*a_ptr)(void))
{
	g_callBackPtr = a_ptr;
}

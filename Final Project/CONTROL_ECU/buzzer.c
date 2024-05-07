 /******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the Buzzer driver
 *
 * Author: Marwan Medhat
 *
 *******************************************************************************/
#include <avr/io.h>
#include "gpio.h"
#include "buzzer.h"
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * initialize the buzzer
 */
void BUZZER_init(void)
{
	GPIO_setupPinDirection(BUZZER_PORT_ID,BUZZER_PIN_ID,PIN_OUTPUT); /* set the buzzer pin as output pin */
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID, LOGIC_LOW); /* enable the buzzer output pin */
}
/*
 * Description :
 * Turns on the buzzer
 */
void BUZZER_on(void)
{
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID, LOGIC_HIGH); /* enable the buzzer output pin */
}
/*
 * Description :
 * Turns off the buzzer
 */
void BUZZER_off(void)
{
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID, LOGIC_LOW); /* disable the buzzer output pin */
}

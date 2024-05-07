 /******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the Buzzer driver
 *
 * Author: Marwan Medhat
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define BUZZER_PORT_ID 				PORTC_ID
#define BUZZER_PIN_ID				PIN7_ID
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * initialize the buzzer
 */
void BUZZER_init(void);
/*
 * Description :
 * Turns on the buzzer
 */
void BUZZER_on(void);
/*
 * Description :
 * Turns off the buzzer
 */
void BUZZER_off(void);

#endif /* BUZZER_H_ */

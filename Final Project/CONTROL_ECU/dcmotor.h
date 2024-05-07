 /******************************************************************************
 *
 * Module: DcMotor
 *
 * File Name: dcmotor.h
 *
 * Description: Header file for the DcMotor driver
 *
 * Author: Marwan Medhat
 *
 *******************************************************************************/

#ifndef DCMOTOR_H_
#define DCMOTOR_H_

#include "std_types.h"
#include "pwm.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define DC_MOTOR_PORT_ID				PORTB_ID
#define DC_MOTOR_INPUT_1_PIN_ID			PIN1_ID
#define DC_MOTOR_INPUT_2_PIN_ID			PIN2_ID

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum
{
	STOP,CW,ACW
}DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void DcMotor_init(void); /* initialize DcMotor */
void DcMotor_Rotate(DcMotor_State state, uint8 speed); /* Adjust the direction and speed of the motor */

#endif /* DCMOTOR_H_ */

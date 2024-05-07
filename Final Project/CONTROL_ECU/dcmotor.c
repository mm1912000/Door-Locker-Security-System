 /******************************************************************************
 *
 * Module: DcMotor
 *
 * File Name: dcmotor.c
 *
 * Description: Source file for the DcMotor driver
 *
 * Author: Marwan Medhat
 *
 *******************************************************************************/
#include "dcmotor.h"
#include "gpio.h"


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Set input 1, input 2, and enable pins direction for the motor according to their values in the header file.
 */
void DcMotor_init(void){
	GPIO_setupPinDirection(DC_MOTOR_PORT_ID, DC_MOTOR_INPUT_1_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_PORT_ID, DC_MOTOR_INPUT_2_PIN_ID, PIN_OUTPUT);
}

/*
 * Description :
 * Set input 1, input 2, according to the desired state
 * both inputs are low if state is off
 * input 1 is low and input 2 is high if CW
 * input 1 is high and input 2 is low if ACW
 */
void DcMotor_Rotate(DcMotor_State state, uint8 speed)
{
	PWM_Timer0_Start(speed);
	if ( state == STOP )
	{
		GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_INPUT_1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_INPUT_2_PIN_ID, LOGIC_LOW);
	}
	else if ( state == CW )
	{
		GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_INPUT_1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_INPUT_2_PIN_ID, LOGIC_HIGH);
	}
	else if ( state == ACW )
	{
		GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_INPUT_1_PIN_ID, LOGIC_HIGH);
		GPIO_writePin(DC_MOTOR_PORT_ID, DC_MOTOR_INPUT_2_PIN_ID, LOGIC_LOW);
	}
}


 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Marwan Medhat
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

/*******************************************************************************
 *                      Types Declaration                                      *
 *******************************************************************************/
 /* Description :
 * In the main function a structure of UART_ConfigType should be defined using the following types declaration
 * 1- choose the baudrate
 * 2- set the partiy mode to either disabled, even, or odd
 * 3- set the stop bit to either one or two bits
 * 4- set the databits to either 5,6,7,8, or 9 bits
 * 5- pass this structure to the UART init function.
 */
typedef enum
{
	DISABLED,EVEN = 2,ODD
}UART_ParityMode;

typedef enum
{
	ONE_BIT,TWO_BIT
}UART_StopBit;

typedef enum
{
	FIVE_BIT,SIX_BIT,SEVEN_BIT,EIGHT_BIT,NINE_BIT = 7
}UART_BitData;

typedef uint32 UART_BaudRate;

typedef struct
{
	UART_BaudRate baudrate;
	UART_ParityMode parity;
	UART_StopBit stopbit;
	UART_BitData datasize;
}UART_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits through the UART_ConfigType structure.
 * 2- Pass the structure to the init function
 * 2. Enable the UART.
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */

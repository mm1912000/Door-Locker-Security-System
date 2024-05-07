 /******************************************************************************
 *
 * Module: HMI_ECU
 *
 * Description: Source file for the HMI_ECU
 *
 * Author: Marwan Medhat
 *
 *******************************************************************************/
#include "lcd.h"
#include "keypad.h"
#include <util/delay.h>
#include "uart.h"
#include "timer1.h"
#include <avr/io.h>
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define KEYPAD_MAX_VALUE 9
#define PASSWORD_MATCH 0x20
#define PASSWORD_N_MATCH 0x30
#define PW_SIZE 5
#define UNLOCK_DOOR 0x50
#define CHANGE_PASSWORD 0x60
/*******************************************************************************
 *                      PreProcessor Macros                                    *
 *******************************************************************************/
#define HECU_UNLOCKING_DOOR_STRING \
	{\
		LCD_sendCommand(LCD_CLEAR_COMMAND);\
		LCD_displayString("Unlocking Door");\
	}
#define HECU_LOCKING_DOOR_STRING \
	{\
		LCD_sendCommand(LCD_CLEAR_COMMAND);\
		LCD_displayString("Locking Door");\
	}
#define HECU_PLZ_RE_ENTER_PASS_STRING \
	{\
		LCD_sendCommand(LCD_CURSOR_ON);\
		LCD_displayStringRowColumn(0,0,"Plz re-enter the");\
		LCD_displayStringRowColumn(1,0,"same pass:");\
		LCD_moveCursor(1,11);\
	}
#define HECU_PLZ_ENTER_PASS_STRING \
	{\
		LCD_sendCommand(LCD_CLEAR_COMMAND);\
		LCD_sendCommand(LCD_CURSOR_ON);\
		LCD_displayStringRowColumn(0,0,"Plz enter pass :");\
		LCD_moveCursor(1,0);\
	}
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void HECU_typePassword(uint8 * password);
void HECU_mainOptions(void);
void HECU_timer1CallBack(void);
/*******************************************************************************
 *                      Global Variables                                       *
 *******************************************************************************/
uint8 HECU_password[PW_SIZE], HECU_passwordConfirm[PW_SIZE];
static volatile uint8 timer_counter, HECU_checkPassword, HECU_keyInput;
/*******************************************************************************
 *                           Main Function                                     *
 *******************************************************************************/
int main()
{
	UART_ConfigType uartConfig = { 19200 , EVEN , ONE_BIT, EIGHT_BIT }; /* UART configurations  19200 baud rate */

	LCD_init(); /* initialize LCD */
	UART_init(&uartConfig); /* initialize UART */
	SREG |= (1<<7); /* global interrupt enable i-bit */

	while(1) /* this while loop if password is wrong will keep looping until user creates a password */
	{
		HECU_PLZ_ENTER_PASS_STRING;
		HECU_typePassword(HECU_password); /* will send the CONTROL ECU the first password */
		HECU_PLZ_RE_ENTER_PASS_STRING;
		HECU_typePassword(HECU_passwordConfirm); /* will send the CONTROL ECU the confirmation password */
		HECU_checkPassword = UART_recieveByte(); /* will receive from the control ecu if password is matched or not */
		if(HECU_checkPassword == PASSWORD_MATCH )
		{
			HECU_mainOptions(); /* if password match will display the main options */
		}
		else
		{
			/* if password doesnt match the system will ask the user to try again */
		}

	}

}
/*
 * Description :
 * takes password from the user and sends it to another device
 */
void HECU_typePassword(uint8 *password)
{
	uint8 counter = 0, i; /* a variable to make sure no more than 5 digits are entered for the password */
	while(1) /* this while loop is for password input  we will break by pressing = sign on the keypad which means we have entered the password */
	{
		HECU_keyInput = KEYPAD_getPressedKey();
		if(HECU_keyInput <= KEYPAD_MAX_VALUE && counter < PW_SIZE ) /* this if condition will disable any input from the keypad other than 0 to 9 */
		{
			LCD_displayCharacter('*'); /* will display password characters as asterisk */
			_delay_ms(500); /* this delay will prevent multiple inputs from the keypad only one at a time */
			password[counter] = HECU_keyInput; /* will store the keypad input in the password array */
			counter++; /* after the keypad input is stored will increment the counter to store the next digit in the password */
		}
		if(HECU_keyInput == '=') /* Equal sign is used as a confirm button to send the password to the CONTROL ECU */
		{
			for ( i = 0 ; i < PW_SIZE; i++)
			{
				UART_sendByte(password[i]); /* will send the password digit by digit to the CONTROL ECU */
			}
			return; /* once the password is sent the function task is done */
		}

	}
}
/*
 * Description :
 * will display the Main options and basically includes the whole system after creating the password.
 */
void HECU_mainOptions(void)
{
	uint8 pw_wrong = 0, i; /* variable to count how many pw entries are wrong */
	TIMER1_ConfigType timer1Config = { 0 , 58594 , T1_1024, COMPARE }; /* timer1 configurations for 7.5 seconds (58594 ticks)*/

	/* this while loop will keep displaying the main options until the user chooses either + or - and will ask the user to enter the password
	 * if password is correct in the + option -> will display strings on LCD and then return
	 * if password is correct in the - option -> will return to the first menu which asks the user to enter a password 2 times to create a new one
	 * if password is incorrect in both -> will ask the user to enter the password for 3 times if the 3 times are incorrect the HECU will freeze for 1 minute
	   and then return to main options again
	 */
	while(1)
	{
		LCD_sendCommand(LCD_CLEAR_COMMAND);
		LCD_displayString("+ = Open Door");
		LCD_displayStringRowColumn(1,0,"- = ChangePass");
		HECU_keyInput = KEYPAD_getPressedKey();
		if(HECU_keyInput == '+') /* + sign will choose the open door menu */
		{
			while(1)
			{
				UART_sendByte(UNLOCK_DOOR); /* send UNLOCK DOOR definition to let the CONTROL ECU knows which option the user have chosen */
				HECU_PLZ_ENTER_PASS_STRING;
				HECU_typePassword(HECU_passwordConfirm); /* ask the user to send the password one more time */
				HECU_checkPassword = UART_recieveByte(); /* wait for the CONTROL ECU to confirm if the password is correct from the eeprom */
				if (HECU_checkPassword == PASSWORD_MATCH)
				{
					TIMER1_setCallBack(HECU_timer1CallBack); /* initialize this function as call back function */
					TIMER1_init(&timer1Config); /* start the timer */
					HECU_UNLOCKING_DOOR_STRING; /* display door is unlocking on the LCD */
					while(1)
					{
						if (timer_counter == 2) /* after 15 seconds */
						{
							LCD_sendCommand(LCD_CLEAR_COMMAND);
							LCD_displayString("Door is on Hold.");
							_delay_ms(3000); /* Door is on Hold will be displayed for 3 seconds */
							TCNT1 = 0; /* reset starting timer count */
							HECU_LOCKING_DOOR_STRING;
							timer_counter++; /* increment the counter to prevent it from accessing this if condition again */
						}
						else if (timer_counter == 5) /* after another 15 seconds ( its 5 not 4 because we incremented the timer counter in last if condition ) */
						{
							TIMER1_deinit(); /* stop and clear timer */
							timer_counter = 0; /* reset timer counter */
							return;
						}
					}
				}
				else if (HECU_checkPassword == PASSWORD_N_MATCH ) /* if password is not matched */
				{
					pw_wrong++; /* count how many times the password was entered incorrectly */
					if (pw_wrong == 3) /* if pw entered 3 times wrong */
					{
						LCD_sendCommand(LCD_CLEAR_COMMAND);
						LCD_displayString("ERROR!!!");
						for ( i = 0 ; i < 60 ; i++) /* for loop for 60 times */
						{
							_delay_ms(1000); /* 1 second delay for 60 times to freeze the system for 1 minute */
						}
						pw_wrong = 0; /* reset the counter */
						break; /* this break will return us to main menu option again */
					}
					else
					{
						/* we will loop to enter the pw one more time */
					}
				}
			}
		}
		if ( HECU_keyInput == '-')
		{
			while(1)
			{
				UART_sendByte(CHANGE_PASSWORD);
				HECU_PLZ_ENTER_PASS_STRING;
				HECU_typePassword(HECU_passwordConfirm);
				HECU_checkPassword = UART_recieveByte();
				if (HECU_checkPassword == PASSWORD_MATCH)
				{
					pw_wrong = 0; /* reset the number of times the pw was entered wrong because we wont reach this if condition if pw is correct */
					return; /* if the password is correct we will return to step 1 to change the password */
				}
				else if (HECU_checkPassword == PASSWORD_N_MATCH )
				{
					pw_wrong++; /* count how many times the password was entered incorrectly */
					if (pw_wrong == 3) /* if pw entered 3 times wrong */
					{
						LCD_sendCommand(LCD_CLEAR_COMMAND);
						LCD_displayString("ERROR!!!");
						for ( i=0 ; i<60 ; i++) /* 1 second delay for 60 times to freeze the system for 1 minute */
						{
							_delay_ms(1000);
						}
						pw_wrong = 0; /* reset the counter */
						break; /* this break will return us to the main menu one more time */
					}
					else
					{
						/* we will loop to enter the pw one more time */
					}
				}
			}
		}
	}
}
/*
 * Description :
 * the CallBackfunction which will be called in the ISR every 7.5 seconds
 */
void HECU_timer1CallBack(void)
{
	timer_counter++;
}

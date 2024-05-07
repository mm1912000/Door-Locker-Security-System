 /******************************************************************************
 *
 * Module: CONTROL_ECU
 *
 * Description: Source file for the CONTROL_ECU
 *
 * Author: Marwan Medhat
 *
 *******************************************************************************/
#include "uart.h"
#include "external_eeprom.h"
#include "twi.h"
#include "dcmotor.h"
#include "timer1.h"
#include <util/delay.h>
#include <avr/io.h>
#include "buzzer.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PASSWORD_MATCH 0x20
#define PASSWORD_N_MATCH 0x30
#define CECU_TWI_BIT_RATE 400000 /* kb/s */
#define CECU_EEPROM_ADDRESS 10
#define CECU_EEPROM_MEM_ADDRESS 0x0191
#define PW_SIZE 5
#define UNLOCK_DOOR 0x50
#define CHANGE_PASSWORD 0x60
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
uint8 CECU_charactersMatch(uint8 *firstPassword, uint8 *secondPassword);
void CECU_storePassword(uint8 * password);
void CECU_timer1CallBack(void);
/*******************************************************************************
 *                      Global Variables                                       *
 *******************************************************************************/
static volatile uint8 timer_counter = 0;
/*******************************************************************************
 *                           Main Function                                     *
 *******************************************************************************/
int main() {
	UART_ConfigType uartConfig = { 19200, EVEN, ONE_BIT, EIGHT_BIT }; /* UART configurations  19200 baud rate */
	TWI_ConfigType twiConfig = { CECU_TWI_BIT_RATE, CECU_EEPROM_ADDRESS, ONE }; /* I2C configurations  */
	TIMER1_ConfigType timer1Config = { 0 , 58594 , T1_1024, COMPARE }; /* timer1 configurations for 7.5 seconds (58594 ticks)*/
	uint8 CECU_password[PW_SIZE] = {0} ; /* will initialize the password array to five zeros to avoid garbage values */
	uint8 CECU_passwordConfirm[PW_SIZE] = {0};  /* will initialize the password confirmation array to five zeros to avoid garbage values */
	uint8 CECU_readPassword[PW_SIZE] = {0}; /* will initialize the read password array to five zeros to avoid garbage values */
	uint8 CECU_readMenu; /* variable to know if the user have chosen which option from the main options */
	uint8 pw_counter = 0, pw_wrong = 0; /* pw_counter to check if password matches and pw_wrong to count how many times pw was wrong */
	uint8 i;
	UART_init(&uartConfig); /* Initialize UART registers */
	TWI_init(&twiConfig); /* Initialize I2C registers */
	DcMotor_init(); /* Initialize DcMotor registers */
	SREG |= (1<<7); /* global interrupt enable i-bit */

	while (1)
	{
		CECU_storePassword(CECU_password);
		CECU_storePassword(CECU_passwordConfirm);
		pw_counter = CECU_charactersMatch(CECU_password, CECU_passwordConfirm);
		if (pw_counter == 5) /* 5 means password array matches the passwordConfirm array */
		{
			UART_sendByte(PASSWORD_MATCH); /* CECU will let the HECU know that the passwords are matched */
			for (i = 0; i < PW_SIZE; i++)
			{
				EEPROM_writeByte(CECU_EEPROM_MEM_ADDRESS + i, CECU_password[i]); /* Save the password in a specific address in the EEPROM */
				_delay_ms(10); /* delay to give the memory time to save the data in the corresponding address */
			}
			while(1)
			{
				CECU_readMenu = UART_recieveByte(); /* the HECU will send the option which the user have chosen */
				CECU_storePassword(CECU_passwordConfirm);
				for (i = 0; i < PW_SIZE; i++)
				{
					EEPROM_readByte(CECU_EEPROM_MEM_ADDRESS + i , &CECU_readPassword[i]); /* read the saved password from the EEPROM and save it inside readPassword array */
					CECU_password[i] = CECU_readPassword[i]; /* save the readPassword inside the password array */
					_delay_ms(10);/* delay to give the memory time to read the data from the corresponding address */
				}
				pw_counter = 0; /* reset pw counter to check for the password for the second time */
				pw_counter = CECU_charactersMatch(CECU_passwordConfirm, CECU_password);
				if (pw_counter == 5 && CECU_readMenu == UNLOCK_DOOR) /* if pw match and + option was chosen */
				{
					UART_sendByte(PASSWORD_MATCH); /* let the HECU knows password is matched */
					TIMER1_setCallBack(&CECU_timer1CallBack); /* set the call back function for the timer1 */
					TIMER1_init(&timer1Config); /* initialize the timer registers */
					DcMotor_Rotate(CW,100); /* rotate DcMotor in clockwise direction with maximum speed */
					while(1)
					{
						if (timer_counter == 2) /* after 15 seconds */
						{
							DcMotor_Rotate(STOP,0);
							_delay_ms(3000); /* stop the dcMotor  for 3 seconds */
							TCNT1 = 0; /* reset starting timer count */
							DcMotor_Rotate(ACW,100);  /* rotate DcMotor in anti - clockwise direction with maximum speed */
							timer_counter++; /* increment the counter to prevent it from accessing this if condition again */
						}
						else if (timer_counter == 5) /* after another 15 seconds ( its 5 not 4 because we incremented the timer counter in last if condition ) */
						{
							TIMER1_deinit(); /* stop and clear timer */
							timer_counter = 0;  /* reset timer counter */
							DcMotor_Rotate(STOP,0); /* stop the dcMotor */
							pw_counter=0; /* reset pw counter for future comparison between passwords */
							break; /* task is done now we need to get out of the loop */
						}
					}
					break; /* get out of the second loop to return to the menu where the user enters the password for the first time */
				}
				else if (pw_counter == 5 && CECU_readMenu == CHANGE_PASSWORD)
				{
					UART_sendByte(PASSWORD_MATCH); /* if password match and - option was chosen CECU will send to HECU pw is match */
					pw_counter=0; /* resets pw_counter for future password comparison */
					pw_wrong = 0; /* resets the pw_wrong as the pw was entered correctly so no need to store previous wrong trials */
					break; /* task is don we need to get out of the loop to the menu where the user enters the password for the first time */
				}
				else
				{
					pw_wrong++; /* we will be in this else condition if pw_counter not equal to 5 which means pw is wrong so increment this variable for every wrong trial*/
					UART_sendByte(PASSWORD_N_MATCH); /* CECU will send that pw is not matched */
					pw_counter=0; /* reset pw_counter to check for the password in the next trial */
					if( pw_wrong == 3) /* if pw is entered 3 times wrong */
					{
						BUZZER_on(); /* turn on the buzzer */
						for ( i=0 ; i<60 ; i++)
						{
							_delay_ms(1000); /* freeze the system for 1 minute */
						}
						BUZZER_off(); /* turn off the buzzer */
						pw_wrong = 0; /* reset the pw_wrong to let the user try again after 1 minute*/
						/* no break as we will be stuck in this loop unless the pw is entered correctly */
					}
				}
			}
		}
		else /* this else if the user entered the confirmation password wrong */
		{
			UART_sendByte(PASSWORD_N_MATCH); /* CECU will let the HECU pw is wrong */
			pw_counter=0; /* reset pw_counter to 0 to compare the password in the next trial */
		}

	}

}
/*
 * Description :
 * receive password array from HECU and store it inside an array
 */
void CECU_storePassword(uint8 * password)
{
	uint8 i;
	for ( i = 0 ; i < PW_SIZE ; i++)
	{
		password[i] = UART_recieveByte();
	}
}
/*
 * Description :
 * check the number of indexes that matches in 2 password arrays
 */
uint8 CECU_charactersMatch(uint8 *firstPassword, uint8 *secondPassword)
{
	uint8 i, counter = 0;
	for ( i = 0 ; i < PW_SIZE ; i++)
	{
		if (firstPassword[i] == secondPassword[i]) /* if the indexes are equal it means password match */
		{
			counter++; /* if the indexes match then increment pw counter to know how many indexes match */
		}
	}
	return counter;
}
/*
 * Description :
 * the CallBackfunction which will be called in the ISR every 7.5 seconds
 */
void CECU_timer1CallBack(void)
{
	timer_counter++;
}

/******************************************************************************
 *
 * Module: Application Layer
 *
 * File Name: main.c
 *
 * Description: Source file for the main application for Human-Machine Interface
 *
 * Author: Yousouf Soliman
 *
 *******************************************************************************/

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "HAL/lcd.h" /*Includes LCD module and related functions*/
#include "HAL/keypad.h" /*Includes KEYPAD module and related functions*/
#include "MCAL/uart.h" /*Includes UART module and related functions*/
#include "MCAL/timer.h" /*Includes TIMER1/Timer0-PWM module and related functions*/
#include "UTIL/communication_commands.h" /*Includes all communication agreements between Control ECU and HMI ECU*/
#include <util/delay.h> /* For the delay functions */
#include <avr/io.h> /* To enable and disable interrupts*/

/*******************************************************************************
 *                               Definitions                                   *
 *******************************************************************************/
#define Interrupts_Enable() (SREG |= (1<<7))
#define Interrupts_Disable() (SREG &= ~(1<<7))
/* Macro to Enable and Disable interrupts using I-bit in S-Reg*/

#define WRONG_PASS_ATTEMPTS 3
/* Password input allowed attempts
 * User can input the password incorrectly (AFTER setting it)
 * for this predetermined amount of time
 * */

/*******************************************************************************
 *                      Global Variables Declarations                          *
 *******************************************************************************/
uint8 g_setSystemPassFlag = 1; /* Flag to indicate that the user is setting the system password */
uint8 password[PASSWORD_LENGTH]; /* Array to store the password input from user in */
uint8 password_verification[PASSWORD_LENGTH]; /* Array to store the password verification input from user in */





/*******************************************************************************
 *                         Function Callback                                   *
 *******************************************************************************/
void CountThreeSeconds(void){
	/* 1 ISR = 3 seconds pass */
	count3Seconds = 0;
	/* mask flag to indicate that the count has finished */
	Timer1_DeInit();
	/* DeInit the timer for next use */
}

void CountFifteenSeconds(void){
	/* 1 ISR = 3 seconds pass */
	static uint8 counter = 0;
	counter++;
	TCNT1 = TIMER1_OVF_INITIAL_VALUE_FOR_3_SECONDS;
	/* with each ISR, we shall reset the TCNT1 register the correct value */
	if(counter == 5)
	{
		/* 5 ISRs = 15 seconds */
	count15Seconds = 0;
	/* mask flag to indicate that the count has finished */
	Timer1_DeInit();
	/* DeInit the timer for next use */
	counter = 0;
	/* mask count for next use */
	}
}

void CountSixtySeconds(void){
	/* 1 ISR = 3 seconds pass */
	static uint8 counter = 0;
	counter++;
	TCNT1 = TIMER1_OVF_INITIAL_VALUE_FOR_3_SECONDS;
	/* with each ISR, we shall reset the TCNT1 register the correct value */
	if(counter == 20)
	{
		/* 20 ISRs = 60 seconds */
	count60Seconds = 0;
	/* mask flag to indicate that the count has finished */
	Timer1_DeInit();
	/* DeInit the timer for next use */
	counter = 0;
	/* mask count for next use */
	}
}

/*******************************************************************************
 *                          Function Definitions                               *
 *******************************************************************************/

/*
 * Function Description:
 * Function used to send the password input by user to Control ECU to check it via UART module
 * Inputs: void
 * Returns: void
 * */

void HMI_sendPasswords(void) {
	uint8 CONTROL_readyFlag;
	/* Flag for synchronizing between HMI ECU and Control ECU */

	UART_sendByte(HMI_ECU_READY);
	/* Tell Control ECU that we are ready to send the command */

	if (g_setSystemPassFlag == 1) {
		UART_sendByte(SET_PASSWORD);
		/* If we are setting the system's password, we will send SET_PASSWORD command to the Control ECU */
	} else {
		UART_sendByte(CHECK_PASSWORD);
		/* If we taking password input from user for authentication,
		 *  we will send CHECK_PASSWORD command to the Control ECU */
	}
	do {
		CONTROL_readyFlag = UART_receiveByte();
		/* Wait until Control_ECU is ready to receive the data */
	} while (CONTROL_readyFlag != CONTROL_ECU_READY);

	UART_sendData(password, PASSWORD_LENGTH);
	/* Send the required string to Control_ECU through UART */

	/* Send the password verification if we are setting system password */
	if (g_setSystemPassFlag == 1) {
		CONTROL_readyFlag = 0;
		do {
			CONTROL_readyFlag = UART_receiveByte();
			/* Wait until Control_ECU is ready to receive the string */
		} while (CONTROL_readyFlag != CONTROL_ECU_READY);
		/* Send the required string to Control_ECU through UART */
		UART_sendData(password_verification, PASSWORD_LENGTH);
		g_setSystemPassFlag = 0;
		/* Mask the set system password flag so */
	}
}

/*
 * Function Description:
 * Function used to accept the user's password entry
 * Inputs: void
 * Returns: void
 * */
void HMI_passwordInput(void) {
	uint8 loop_counter = 0;
	uint8 key = 0;
	/* initializing loop counter used in all for loops
	 * initializing key to act as buffer input for our keypad
	 *  */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Plz Enter Pass:");
	LCD_moveCursor(1, 0); /* Move the cursor to the second row */
	/* display the desired message on LCD */

	for (loop_counter = 0; loop_counter < PASSWORD_LENGTH + 1; loop_counter++) {
			key = KEYPAD_getPressedKey();
		if ((key <= 9) && (key >= 0)) {
			/* Get the pressed key number, if any switch pressed for more than 500 ms it will considered more than one press */
			password[loop_counter] = key;
			LCD_displayCharacter('*');/* display '*' with each pressed key */
		}
		/* Accept user input and display a '*' with each number */
		if (loop_counter == PASSWORD_LENGTH) {
			if (key == '=') {
				break;
			}
			/* If user click Enter button "=", the we break out of the input loop */
			else {
				HMI_passwordInput();
			}
			/* if they don't hit enter after they are done, re-call the function*/
		}
		_delay_ms(500); /* Press time */
	}

	/* If we are setting the system password, then repeat the same steps but with password verification this time */
	if (g_setSystemPassFlag == 1) {
		LCD_clearScreen();

		LCD_displayStringRowColumn(0, 0, "Plz Enter The");
		LCD_displayStringRowColumn(1, 0, "Same Pass:");
		LCD_moveCursor(1, 10); /* Move the cursor to the second row */

		for (loop_counter = 0; loop_counter < PASSWORD_LENGTH + 1; loop_counter++) {
			key = KEYPAD_getPressedKey();
			if ((key <= 9) && (key >= 0)) {
				LCD_displayCharacter('*'); /* display '*' with each pressed key */
				/* Get the pressed key number, if any switch pressed for more than 500 ms it will considered more than one press */
				password_verification[loop_counter] = key;
			}
			if (loop_counter == PASSWORD_LENGTH) {
				if (key == '=') {
					break;
				} else {
					HMI_passwordInput();
				}
			}
			_delay_ms(500); /* Press time */
		}
	}

	LCD_clearScreen();
	HMI_sendPasswords();
	/* After accepting all inputs, clear the screen and call the send pass
	 * function to send the passwords to Control ECU */
}

/*
 * Function Description:
 * Function used to initiate the alarm protocol:
 * 1- Start the timer with required configurations
 * 2- Display the phrase "ERROR" for the required amount of time
 * 3- Disabling any keypad input
 * 4- Send the ALARM command to the Control ECU
 * Inputs: void
 * Returns: void
 * */

void alarmProtocol(void) {
	Timer1_ConfigType Timer1_Config;
	Timer1_Config.initial_value =
	TIMER1_OVF_INITIAL_VALUE_FOR_3_SECONDS;
	Timer1_Config.compare_value = 0;
	Timer1_Config.prescalar = Prescalar_1024;
	Timer1_Config.mode = Timer1_NormalMode;
	/* Configure timer:
	 * 1- Normal Mode (Overflow Mode)
	 * 2- Prescalar set to 1024
	 * 3- Count three seconds
	 * 4- F_CPU = 1Mhz -> F_Timer = 1Khz -> Time for 1 tick = 1ms Ticks for 3s = 3000 tick
	 * Timer initial value = 65535 - 300 =  62535 */
	LCD_clearScreen();
	LCD_displayString("ERROR");
	/* Display "ERROR" */
	KEYPAD_disable();
	/* Disable input from user */
	UART_sendByte(HMI_ECU_READY);
	UART_sendByte(ALARM);
	/* Send the ALARM command to Control ECU */
	Timer1_Init(&Timer1_Config);
	Timer1_setCall(CountSixtySeconds);
	count60Seconds = 1;
	/* Start timer and Count 60 Seconds */
	while (count60Seconds)
		;
	KEYPAD_enable();
	/* Re-enable the keypad to accept input again from user */
}

/*
 * Function Description:
 * Function used to initiate the Door Unlock protocol:
 * 1- Start the timer with required configurations
 * 2- Display the phrase "Door is Unlocking" for the required amount of time
 * 3- Send the UNLOCK_DOOR command to the Control ECU
 * 4- Display the phrase "Door is Locking" for the required amount of time
 * Inputs: void
 * Returns: void
 * */
void doorUnlockProtocol(void) {
	Timer1_ConfigType Timer1_Config;
	Timer1_Config.initial_value =
	TIMER1_OVF_INITIAL_VALUE_FOR_3_SECONDS;
	Timer1_Config.compare_value = 0;
	Timer1_Config.prescalar = Prescalar_1024;
	Timer1_Config.mode = Timer1_NormalMode;
	/* Configure timer:
	/* Configure timer:
	 * 1- Normal Mode (Overflow Mode)
	 * 2- Prescalar set to 1024
	 * 3- Count three seconds
	 * 4- F_CPU = 1Mhz -> F_Timer = 1Khz -> Time for 1 tick = 1ms Ticks for 3s = 3000 tick
	 * Timer initial value = 65535 - 300 =  62535 */

	LCD_clearScreen();
	LCD_displayString("Door is Unlocking");
	/* Display "Door is Unlocking" */
	UART_sendByte(HMI_ECU_READY);
	UART_sendByte(UNLOCK_DOOR);
	/* Send the UNLOCK_DOOR command to Control ECU */
	Timer1_Init(&Timer1_Config);
	Timer1_setCall(CountFifteenSeconds);
	count15Seconds = 1;
	while (count15Seconds)
		;
	/* Start timer and Count 15 Seconds */
	LCD_clearScreen();
	/* Display Nothing while door is open */
	Timer1_Init(&Timer1_Config);
	Timer1_setCall(CountThreeSeconds);
	count3Seconds = 1;
	while (count3Seconds)
		;
	/* Start timer and Count 3 Seconds */
	LCD_displayString("Door is Locking");
	/* Display "Door is Locking" */
	Timer1_Init(&Timer1_Config);
	Timer1_setCall(CountFifteenSeconds);
	count15Seconds = 1;
	while (count15Seconds)
		;
	/* Start timer and Count 15 Seconds */
}

/*
 * Function Description:
 * Main function:
 * Responsible for initiating all modules, enabling interrupts, and configuring UART
 * */
int main(void) {
	uint8 loop_counter = 0;
	uint8 userChoice;
	uint8 passMatchFlag;
	uint8 ready;
	/* Variables used in main logic */
	Interrupts_Enable();
	LCD_init();
	KEYPAD_enable();
	/* Initialize LCD and enable keypad input */

	UART_ConfigType UART_Config;
	UART_Config.baud_rate = BaudRate_9600;
	UART_Config.bit_data = BitData_8;
	UART_Config.parity = Parity_Even;
	UART_Config.stop_bit = StopBit_1;
	UART_init(&UART_Config);
	/* Initialize the UART driver with Baud-rate = 9600 bits/sec, 8_bit data, Even parity and One stop-bit */

	do {
		g_setSystemPassFlag = 1;
		HMI_passwordInput();
		passMatchFlag = UART_receiveByte();

	} while (passMatchFlag != PASSWORDS_MATCHED);
	/* Set the system password until the input password and its verification are matched */

	for (;;) {
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "+ : Open Door");
		LCD_displayStringRowColumn(1, 0, "- : Change Pass");
		/* Main screen with main options */
		userChoice = KEYPAD_getPressedKey();
		/* get user's choice */

		if (userChoice == '+') {
			for (loop_counter = 0; loop_counter < WRONG_PASS_ATTEMPTS;
					/* Loop to only allow user 3 password attempts */
					loop_counter++) {
				HMI_passwordInput();
				/* get the password from user until they input it correctly or they run out of attempts */
				do {
					ready = UART_receiveByte();
				} while (ready != CONTROL_ECU_READY);
				/* wait until Control ECU is ready to receive command */

				passMatchFlag = 0;
				do {
					passMatchFlag = UART_receiveByte();
				} while (passMatchFlag != PASSWORDS_MATCHED
						&& passMatchFlag != PASSWORDS_UNMATCHED);
				/* Receive the password status */
				if (passMatchFlag == PASSWORDS_MATCHED) {
					break;
				}
				/* if passwords are matched, break from the for loop */
			}
			if (passMatchFlag == PASSWORDS_MATCHED) {
				doorUnlockProtocol();
			}
			/* and then call the unlock protocol */
			else {
				alarmProtocol();
			}
			/* if user runs out of attempts, call the alarm protocol */

		} else if (userChoice == '-') {
			passMatchFlag = 0;
			loop_counter = WRONG_PASS_ATTEMPTS;
			do {
				g_setSystemPassFlag = 1;
				HMI_passwordInput();
				passMatchFlag = UART_receiveByte();
				loop_counter--;
			} while ((passMatchFlag != PASSWORDS_MATCHED) && loop_counter);
			/* get the new passwords from user and allow them only three attempts */
			/* ALTERNATIE LOGIC TO THE ONE USED ABOVE */
			if (passMatchFlag != PASSWORDS_MATCHED) {
				alarmProtocol();
			}



			/* if user runs out of attempts, call the alarm protocol */
		}
	}
}


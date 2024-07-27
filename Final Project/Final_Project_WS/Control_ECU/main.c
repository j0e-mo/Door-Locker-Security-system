/******************************************************************************
 *
 * Module: Application Layer
 *
 * File Name: main.c
 *
 * Description: Source file for the Control ECU
 *
 * Author: Yousouf Soliman
 *
 *******************************************************************************/


/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "HAL/buzzer.h" /*Includes BUZZER module and related functions*/
#include "HAL/external_eeprom.h" /*Includes External EEPROM module and related functions*/
#include "HAL/motor.h" /*Includes MOTOR module and related functions*/
#include "MCAL/twi.h" /*Includes TWI module and related functions*/
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

#define PASSWORD_ADDRESS 0x0111

/*******************************************************************************
 *                      Global Variables Declarations                          *
 *******************************************************************************/
uint8 password[PASSWORD_LENGTH]; /* Array to store the password input from user in */
uint8 password_verification[PASSWORD_LENGTH]; /* Array to store the password verification input from user in */



/*******************************************************************************
 *                           Function Callback                                 *
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

/* Function Description:
 * Set the system password for first time entry or changing password
 * */
void setSystemPassword(void) {
	uint8 loop_counter = 0;
	uint8 passwordsUnmatchedFlag = 0;
	/* variable initialization */
	/* Signal to HMI ECU that we are ready to receive password */
	UART_sendByte(CONTROL_ECU_READY);
	UART_receiveData(password, PASSWORD_LENGTH);
	/* Receive password from HMI ECU */
	UART_sendByte(CONTROL_ECU_READY);
	/* Signal to HMI ECU that we are ready to receive password verification */
	UART_receiveData(password_verification, PASSWORD_LENGTH);
	/* Receive password verification from HMI ECU */

	for (loop_counter = 0; loop_counter < PASSWORD_LENGTH; loop_counter++) {
		if (password[loop_counter] != password_verification[loop_counter]) {
			passwordsUnmatchedFlag = 1;
			UART_sendByte(PASSWORDS_UNMATCHED);
			break;
		}
	}
	/* if any digits are unmatched (between pass and pass verify) we set the passwordsUnmatchedFlag and break from loop */

	if (passwordsUnmatchedFlag == 0) {
		UART_sendByte(PASSWORDS_MATCHED);
		/* If all digits matched, we signal to HMI ECU that the passwords matched */
		for (loop_counter = 0; loop_counter < PASSWORD_LENGTH; loop_counter++) {
			EEPROM_writeByte(PASSWORD_ADDRESS + loop_counter,
					password[loop_counter]);
			_delay_ms(10);
		}
	}
	/* If all digits are matched then passwords matched, start saving it in the EEPROM */
}

/* Function description:
 * Compare password to the saved password
 * */
void passwordVerify(void) {
	uint8 loop_counter = 0;
	uint8 passwordsUnmatchedFlag = 0;
	uint8 passwordCheck;
	/* variable initialization */
	/* Signal to HMI ECU that we are ready to receive password */
	UART_sendByte(CONTROL_ECU_READY);
	UART_receiveData(password, PASSWORD_LENGTH);
	/* Receive password from HMI ECU */
	for (loop_counter = 0; loop_counter < PASSWORD_LENGTH; loop_counter++) {
		EEPROM_readByte(PASSWORD_ADDRESS + loop_counter, &passwordCheck);
		/* fetch the saved password from the EEPROM */
		if (password[loop_counter] != passwordCheck) {
			passwordsUnmatchedFlag = 1;
			UART_sendByte(CONTROL_ECU_READY);
			UART_sendByte(PASSWORDS_UNMATCHED);
			break;
		}
		_delay_ms(15);
	}
	/* if any digits are unmatched (between pass and the saved pass)
	 *  we set the passwordsUnmatchedFlag and break from loop */


	if (passwordsUnmatchedFlag == 0) {
		UART_sendByte(CONTROL_ECU_READY);
		UART_sendByte(PASSWORDS_MATCHED);
	}
	/* If all digits matched, we signal to HMI ECU that the passwords matched */
}

/* Function Description:
 * Unlock the door for 15s
 * Hold the door open for 3s
 * Close the door for 15s
 * */
void unlockDoor(void) {

	Timer1_ConfigType Timer1_Config;
	Timer1_Config.initial_value = TIMER1_OVF_INITIAL_VALUE_FOR_3_SECONDS;
	Timer1_Config.compare_value = 0;
	Timer1_Config.prescalar = Prescalar_1024;
	Timer1_Config.mode = Timer1_NormalMode;
	/* Configure timer:
	 * 1- Normal Mode (Overflow Mode)
	 * 2- Prescalar set to 1024
	 * 3- Count three seconds
	 * 4- F_CPU = 8Mhz -> F_Timer = 8Khz -> Time for 1 tick = 0.000125s Ticks for 3s = 24000 tick
	 * Timer initial value = 65535 - 24000 =  41535 */

	DcMotor_Rotate(CW, 100);
	/* Unlock the door using motor */
	Timer1_Init(&Timer1_Config);
	Timer1_setCall(CountFifteenSeconds);
	count15Seconds = 1;
	while (count15Seconds)
		;
	/* Start timer and Count 15 Seconds */

	DcMotor_Rotate(STOP, 0);
	/* Hold the door open */
	Timer1_Init(&Timer1_Config);
	Timer1_setCall(CountThreeSeconds);
	count3Seconds = 1;
	while (count3Seconds)
		;
	/* Start timer and Count 3 Seconds */

	DcMotor_Rotate(A_CW, 100);
	/* Lock the door using motor */
	Timer1_Init(&Timer1_Config);
	Timer1_setCall(CountFifteenSeconds);
	count15Seconds = 1;
	while (count15Seconds)
		;
	/* Start timer and Count 15 Seconds */
	DcMotor_Rotate(STOP, 0);
	/* Close the door shut */
}

/* Function description:
 * Initiate the alam protocol:
 * turn on the buzzer for 1 minute
 * */
void alarm(void) {
	Timer1_ConfigType Timer1_Config;
	Timer1_Config.initial_value = TIMER1_OVF_INITIAL_VALUE_FOR_3_SECONDS;
	Timer1_Config.compare_value = 0;
	Timer1_Config.prescalar = Prescalar_1024;
	Timer1_Config.mode = Timer1_NormalMode;
	/* Configure timer:
	 * 1- Normal Mode (Overflow Mode)
	 * 2- Prescalar set to 1024
	 * 3- Count three seconds
	 * 4- F_CPU = 8Mhz -> F_Timer = 8Khz -> Time for 1 tick = 0.000125s Ticks for 3s = 24000 tick
	 * Timer initial value = 65535 - 24000 =  41535 */
	Buzzer_on();
	/* Turn the buzzer on */
	Timer1_Init(&Timer1_Config);
	Timer1_setCall(CountSixtySeconds);
	count60Seconds = 1;
	while (count60Seconds)
		;
	/* Start timer and Count 60 Seconds */
	Buzzer_off();
	/* Turn the buzzer off */
}

/*
 * Function Description:
 * Main function:
 * Responsible for initiating all modules, enabling interrupts, and configuring UART
 * */
int main(void) {
	uint8 command;
	UART_ConfigType UART_Config;
	UART_Config.baud_rate = BaudRate_9600;
	UART_Config.bit_data = BitData_8;
	UART_Config.parity = Parity_Even;
	UART_Config.stop_bit = StopBit_1;
	UART_init(&UART_Config);
	/* Initialize the UART driver with Baud-rate = 9600 bits/sec, 8_bit data, Even parity and One stop-bit */
	TWI_ConfigType TWI_Config = { 0x10, 400000 };
	TWI_init(&TWI_Config);
	/* Initialize the TWI driver with slave address 10 and 400kbps data rate  */
	Buzzer_init();
	DcMotor_init();
	/* Initialize the buzzer module and motor module */
	Interrupts_Enable();
	/* Enable interrupts */

	for (;;) {
		uint8 HMI_readyFlag;
		do {
			HMI_readyFlag = UART_receiveByte();
		} while (HMI_readyFlag != HMI_ECU_READY);
		/* Stay here until the theHMI ECU is ready to send us the command */
		command = UART_receiveByte();
		/* Receive the command */
		switch (command) {
		/* Switch on the command and act accordingly */
		case SET_PASSWORD:
			setSystemPassword();
			break;
		case CHECK_PASSWORD:
			passwordVerify();
			break;
		case UNLOCK_DOOR:
			unlockDoor();
			break;
		case ALARM:
			alarm();
			break;
		}
	}
}


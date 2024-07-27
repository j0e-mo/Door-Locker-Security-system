/******************************************************************************
 *
 * Module: MOTOR
 *
 * File Name: motor.c
 *
 * Description: Source file for the AVR MOTOR driver
 *
 * Author: Yousouf Soliman
 *
 *******************************************************************************/
#include "motor.h"

#include "../MCAL/gpio.h"
#include "../MCAL/timer.h"


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * Initializes the motor module with connected h-bridge
 */
void DcMotor_init(void) {
	//Configuring output pins.
	GPIO_setupPinDirection(DcMotor_PORT, DcMotor_PIN, PIN_OUTPUT);
	GPIO_setupPinDirection(DcMotor_PORT, (DcMotor_PIN + 1), PIN_OUTPUT);
	//Initializing motor pins to LOGIC_LOW (Stop motors from spinning).
	GPIO_writePin(DcMotor_PORT, DcMotor_PIN, LOGIC_LOW);
	GPIO_writePin(DcMotor_PORT, (DcMotor_PIN + 1), LOGIC_LOW);
}


/*
 * Description :
 * Changes the direction the motor is moving towards
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed){

	switch (state) {
	case STOP:
		GPIO_writePin(DcMotor_PORT, DcMotor_PIN, LOGIC_LOW);
		GPIO_writePin(DcMotor_PORT, (DcMotor_PIN + 1), LOGIC_LOW);
		break;
	case CW:
		GPIO_writePin(DcMotor_PORT, DcMotor_PIN, LOGIC_HIGH);
		GPIO_writePin(DcMotor_PORT, (DcMotor_PIN + 1), LOGIC_LOW);
		break;
	case A_CW:
		GPIO_writePin(DcMotor_PORT, DcMotor_PIN, LOGIC_LOW);
		GPIO_writePin(DcMotor_PORT, (DcMotor_PIN + 1), LOGIC_HIGH);
		break;
	}
	/*call the timer start function to provide the correct speed for the motor*/
	PWM_Timer0_Start(255 * ((float) speed / 100.0));
}


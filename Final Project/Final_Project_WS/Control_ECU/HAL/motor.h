 /******************************************************************************
 *
 * Module: MOTOR
 *
 * File Name: motor.h
 *
 * Description: Header file for the AVR MOTOR driver
 *
 * Author: Yousouf Soliman
 *
 *******************************************************************************/
#ifndef MOTOR_H_
#define MOTOR_H_

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#include "../UTIL/std_types.h"
#define DcMotor_PORT PORTC_ID
#define DcMotor_PIN PIN2_ID
/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum{
	STOP,CW,A_CW
}DcMotor_State;
/*type definition for rotation directions of the Dc Motor*/
/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description :
 * Initializes the motor module with connected h-bridge
 */
void DcMotor_init(void);

/*
 * Description :
 * Changes the direction the motor is moving towards
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);

#endif /* MOTOR_H_ */

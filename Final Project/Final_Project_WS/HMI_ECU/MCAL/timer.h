/******************************************************************************
 *
 * Module: TIMER
 *
 * File Name: timer.h
 *
 * Description: Header file for the AVR Timer driver
 *
 * Author: Yousouf Soliman
 *
 *******************************************************************************/
#ifndef TIMER_H_
#define TIMER_H_

#include "../UTIL/std_types.h"
#include "gpio.h"

/*******************************************************************************
 *                            Definitions                                      *
 *******************************************************************************/
/*#define COMPARE_OUTPUT_A*/
/*#define COMPARE_OUTPUT_B*/
#define COMPARE_OUTPUT_PORT_ID PORTD_ID
#define OC1A PIN5_ID
#define OC1B PIN4_ID
#define TIMER1_OVF_INITIAL_VALUE_FOR_3_SECONDS 62535
/*******************************************************************************
 *                         External Variables                                  *
 *******************************************************************************/
extern volatile uint8 count15Seconds;
extern volatile uint8 count3Seconds;
extern volatile uint8 count60Seconds;
/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum{
	Prescalar_OFF, Prescalar_noPrescalar, Prescalar_8, Prescalar_64, Prescalar_256, Prescalar_1024, Prescalar_EXT_FALLING, Prescalar_EXT_RISING
}Timer1_Prescalar;

typedef enum{
	Timer1_NormalMode, Timer1_CompareToggle, Timer1_CompareClear, Timer1_CompareSet
}Timer1_ModeSelect;

typedef struct{
	uint16 initial_value;
	uint16 compare_value;
	Timer1_Prescalar prescalar;
	Timer1_ModeSelect mode;
}Timer1_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description : Function to initialize the Timer0 driver
 * 	1. Set the required duty cycle.
 * 	2. Set the required clock.
 * 	2. Set PWM mode.
 * 	3. Initialize Timer0 Registers.
 */
void PWM_Timer0_Start(uint8 duty_cycle);

/*
 * Description : Function to initialize the Timer1 driver
 * 	1. Set the required clock.
 * 	2. Set the required mode.
 * 	3. Initialize Timer1 Registers.
 * 	4. Enable Interrupts for the according mode.
 */
void Timer1_Init(const Timer1_ConfigType *Config_Ptr);

/*
 * Description: Function to disable the Timer1
 */
void Timer1_Deinit(void);

/*
 * Description: Function to set the Call Back function address.
 */
void Timer1_setCall(void(*a_ptr)(void));


#endif /* MCAL_TIMER_H_ */

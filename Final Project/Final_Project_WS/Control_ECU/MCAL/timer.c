/******************************************************************************
 *
 * Module: TIMER
 *
 * File Name: timer.c
 *
 * Description: Source file for the AVR Timer driver
 *
 * Author: Yousouf Soliman
 *
 *******************************************************************************/

#include "timer.h"
#include <avr/interrupt.h>/* For Timer1 ISR */

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the addresses of the each call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;
volatile uint8 count15Seconds = 0;
volatile uint8 count3Seconds = 0;
volatile uint8 count60Seconds = 0;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
#ifdef COMPARE_OUTPUT_A
ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
#endif
#ifdef COMPARE_OUTPUT_B
ISR(TIMER1_COMPB_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
#endif
ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : Function to initialize the Timer0 driver
 * 	1. Set the required duty cycle.
 * 	2. Set the required clock.
 * 	2. Set PWM mode.
 * 	3. Initialize Timer0 Registers.
 */
void PWM_Timer0_Start(uint8 duty_cycle) {
	TCNT0 = 0; /*Set Timer Initial value */

	OCR0 = duty_cycle; /* Set Compare Value */

	DDRB = DDRB | (1 << PB3); /*set PB3/OC0 as output pin --> pin where the PWM signal is generated from MC.*/

	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	TCCR0 = (1 << WGM00) | (1 << WGM01) | (1 << COM01) | (1 << CS01);
}

/*
 * Description : Function to initialize the Timer1 driver
 * 	1. Set the required clock.
 * 	2. Set the required mode.
 * 	3. Initialize Timer1 Registers.
 * 	4. Enable Interrupts for the according mode.
 */
void Timer1_Init(const Timer1_ConfigType *Config_Ptr) {
	TCCR1B = (TCCR1B & 0xF8) | ((Config_Ptr->prescalar) & 0x7);
	/* Insert the required prescalar value*/
	TCNT1 = Config_Ptr->initial_value;
	/* Set the initial value*/
	TCCR1A = (TCCR1A & 0xF3) | (1 << FOC1A) | (1 << FOC1B);
	/* Set FOC1A and FOC1B for non-PWM mode*/
	if (Config_Ptr->mode == Timer1_NormalMode) {
		TIMSK = (TIMSK & 0xC3) | (1 << TOIE1);
		/* Enable Overflow Interrupts*/
	} else {
		TCCR1B = (TCCR1B & 0xE7) | (1 << WGM12);
		/* Enable CTC Mode*/
#ifdef COMPARE_OUTPUT_A
		TCCR1A = (TCCR1A & 0x0F) | (((Config_Ptr->mode) & 0x03) << COM1A0);
		/* Insert the specified Compare Mode to work with OC1A*/
		TIMSK = (TIMSK & 0xC3) | (1 << OCIE1A);
		/* Enable Output Compare Match A Interrupts*/
		OCR1A = Config_Ptr->compare_value;
		/* Initialize output compare A register*/
		GPIO_setupPinDirection(COMPARE_OUTPUT_PORT_ID, OC1A, PIN_OUTPUT);
		/* Set OC1A as output pin*/
#endif
#ifdef COMPARE_OUTPUT_B
		TCCR1A = (TCCR1A & 0x0F) | (((Config_Ptr->mode) & 0x03) << COM1B0);
		/* Insert the specified Compare Mode to work with OC1A*/
		TIMSK = (TIMSK & 0xC3) | (1 << OCIE1B);
		/* Enable Output Compare Match A Interrupts*/
		OCR1B = Config_Ptr -> compare_value;
		/* Initialize output compare A register*/
		GPIO_setupPinDirection(COMPARE_OUTPUT_PORT_ID, OC1B, PIN_OUTPUT);
		/* Set OC1A as output pin*/
#endif
	}
}

/*
 * Description: Function to disable the Timer1
 */
void Timer1_DeInit(void) {
	TCCR1A = 0;
	TCCR1B = 0;
	OCR1A = 0;
	OCR1B = 0;
	TCNT1 = 0;
	/* De-initialize all Timer1 Registers*/

	TIMSK &= ~0x3C;
	/* Disable Timer1 interrupt */

	g_callBackPtr = NULL_PTR;
	/* Reset the global pointer value */
}

/*
 * Description: Function to set the Call Back function address.
 */
void Timer1_setCall(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;


}


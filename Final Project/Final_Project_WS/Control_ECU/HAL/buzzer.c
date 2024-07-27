/******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the buzzer driver
 *
 * Author: Yousouf Soliman
 *
 *******************************************************************************/
#include "buzzer.h"
#include "../UTIL/std_types.h"
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description:
 * 1. Initializes the pin connected to the buzzer
 * 2. Turns buzzer OFF as default
 * */
void Buzzer_init(void) {
	GPIO_setupPinDirection(BUZZER_PORT_ID, BUZZER_PIN_ID, PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW);
}
/*
 * Description:
 * Turns Buzzer ON*/
void Buzzer_on(void) {
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_HIGH);
}
/*
 * Description:
 * Turns Buzzer OFF*/
void Buzzer_off(void) {
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW);
}

/******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the buzzer driver
 *
 * Author: Yousouf Soliman
 *
 *******************************************************************************/
#ifndef BUZZER_H_
#define BUZZER_H_

#include "../MCAL/gpio.h"

/*******************************************************************************
 *                            Definitions                                      *
 *******************************************************************************/
#define BUZZER_PORT_ID PORTA_ID
#define BUZZER_PIN_ID PIN0_ID

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum {
	Buzzer_OFF, Buzzer_ON
} Buzzer_state;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description:
 * 1. Initializes the pin connected to the buzzer
 * 2. Turns buzzer OFF as default
 * */
void Buzzer_init(void);
/*
 * Description:
 * Turns Buzzer ON*/
void Buzzer_on(void);
/*
 * Description:
 * Turns Buzzer OFF*/
void Buzzer_off(void);

#endif /* BUZZER_H_ */

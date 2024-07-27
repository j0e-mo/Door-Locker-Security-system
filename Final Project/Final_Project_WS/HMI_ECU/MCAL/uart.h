/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Yousouf Soliman
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "../UTIL/std_types.h"


/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum {
	BitData_5, BitData_6, BitData_7, BitData_8, BitData_9 = 7
} UART_BitData;

typedef enum {
	Parity_Disabled, Parity_Even = 2, Parity_Odd
} UART_Parity;

typedef enum {
	StopBit_1, StopBit_2
} UART_StopBit;

typedef enum {
	BaudRate_2400 = 2400,
	BaudRate_4800 = 4800,
	BaudRate_9600 = 9600,
	BaudRate_14400 = 14400,
	BaudRate_19200 = 19200,
	BaudRate_28800 = 28800,
	BaudRate_38400 = 38400,
	BaudRate_57600 = 57600,
	BaudRate_76800 = 76800,
	BaudRate_115200 = 115200,
	BaudRate_230400 = 230400,
	BaudRate_250K = 250000,
	BaudRate_500K = 500000,
	BaudRate_1M = 1000000
} UART_BaudRate;

typedef struct {
	UART_BitData bit_data;
	UART_Parity parity;
	UART_StopBit stop_bit;
	UART_BaudRate baud_rate;
} UART_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType *Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_receiveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

/*
 * Description :
 * Send the required data according to its size through UART to the other UART device.
 */
void UART_sendData(uint8 *Data, uint8 size);

/*
 * Description :
 * Receive the required data according to its size through UART from the other UART device.
 */
void UART_receiveData(uint8 *Data, uint8 size);
#endif /* UART_H_ */

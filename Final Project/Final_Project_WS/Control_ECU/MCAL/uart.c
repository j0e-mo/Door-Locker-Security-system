/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author: Yousouf Soliman
 *
 *******************************************************************************/

#include "uart.h"
#include "avr/io.h" /* To use the UART Registers */
#include "../UTIL/common_macros.h" /* To use the macros like SET_BIT */

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType *Config_Ptr) {
	uint16 ubrr_value = 0;

	/* U2X = 1 for double transmission speed
	 * MPCM for Multi-processor Communication Mode */
	UCSRA = (1 << U2X) | (1 << MPCM);

	/************************** UCSRB Description **************************
	 * RXCIE = 0 Disable USART RX Complete Interrupt Enable
	 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
	 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 * UCSZ2 = Insert the required BitData mode
	 ***********************************************************************/
	UCSRB = (1 << RXEN) | (1 << TXEN)
			| (((UCSRB & 0xFB) | ((Config_Ptr->bit_data) & 0x4) << UCSZ2));

	/************************** UCSRC Description **************************
	 * URSEL   = 1 The URSEL must be one when writing the UCSRC
	 * UMSEL   = 0 Asynchronous Operation
	 * UPM1:   Insert the required parity
	 * USBS    Insert the required stop bits
	 * UCSZ1:  Insert the required bit data mode
	 * UCPOL   = 0 Used with the Synchronous operation only
	 ***********************************************************************/
	UCSRC = (1 << URSEL)
			| (((UCSRC & 0xCF) | ((Config_Ptr->parity) & 0x3) << UPM0))
			| ((UCSRC & 0xF7) | (((Config_Ptr->stop_bit) & 0x1) << USBS))
			| ((UCSRC & 0xF9) | (((Config_Ptr->bit_data) & 0x3) << UCSZ0));

	/* Calculate the UBRR register value */
	ubrr_value = (uint16) (((F_CPU / (Config_Ptr->baud_rate * 8UL))) - 1);

	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRH = ubrr_value >> 8;
	UBRRL = ubrr_value;
}

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data) {
	/*
	 * UDRE flag is set when the Tx buffer (UDR) is empty and ready for
	 * transmitting a new byte so wait until this flag is set to one
	 */
	while (BIT_IS_CLEAR(UCSRA, UDRE)) {
	}

	/*
	 * Put the required data in the UDR register and it also clear the UDRE flag as
	 * the UDR register is not empty now
	 */
	UDR = data;

	/************************* Another Method *************************
	 UDR = data;
	 while(BIT_IS_CLEAR(UCSRA,TXC)){} // Wait until the transmission is complete TXC = 1
	 SET_BIT(UCSRA,TXC); // Clear the TXC flag
	 *******************************************************************/
}

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_receiveByte(void) {
	/* RXC flag is set when the UART receive data so wait until this flag is set to one */
	while (BIT_IS_CLEAR(UCSRA, RXC)) {
	}

	/*
	 * Read the received data from the Rx buffer (UDR)
	 * The RXC flag will be cleared after read the data
	 */
	return UDR;
}

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str) {
	uint8 i = 0;

	/* Send the whole string */
	while (Str[i] != '\0') {
		UART_sendByte(Str[i]);
		i++;
	}
}

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str) {
	uint8 i = 0;

	/* Receive the first byte */
	Str[i] = UART_receiveByte();

	/* Receive the whole string until the '#' */
	while (Str[i] != '#') {
		i++;
		Str[i] = UART_receiveByte();
	}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	Str[i] = '\0';
}

void UART_sendData(uint8 *Data, uint8 size) {
	uint8 i = 0;
	for (i = 0; i < size; i++) {
		UART_sendByte(Data[i]);
	}
}

void UART_receiveData(uint8 *Data, uint8 size) {
	uint8 i = 0;
	Data[i] = UART_receiveByte();
	for (i = 1; i < size; i++) {
		Data[i] = UART_receiveByte();
	}
}

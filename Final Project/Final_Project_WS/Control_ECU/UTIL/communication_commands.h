 /******************************************************************************
 *
 * Module: Common - Platform Types Abstraction
 *
 * File Name: communication commands.h
 *
 * Description: Communication Commands Document
 *
 * Author: Yousouf Soliman
 *
 *******************************************************************************/
#ifndef UTIL_COMMUNICATION_COMMANDS_H_
#define UTIL_COMMUNICATION_COMMANDS_H_

#define PASSWORD_LENGTH 5
#define CONTROL_ECU_READY 0x10
#define HMI_ECU_READY 0x01
#define PASSWORDS_MATCHED 0x0F
#define PASSWORDS_UNMATCHED 0xF0
#define SET_PASSWORD 0x33
#define CHECK_PASSWORD 0x25
#define UNLOCK_DOOR 0xCC
#define ALARM 0x22

#endif /* UTIL_COMMUNICATION_COMMANDS_H_ */

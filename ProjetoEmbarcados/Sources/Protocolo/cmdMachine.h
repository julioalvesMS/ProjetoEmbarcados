/* ***************************************************************** */
/* File name:        stateMachine.h                                  */
/* File description: Header file containing the functions/methods    */
/*                   interfaces for handling the state machine       */
/* Author name:      julioalvesMS & IagoAF                           */
/* Creation date:    12abr2018                                       */
/* Revision date:    21abr2018                                       */
/* ***************************************************************** */

#ifndef SOURCES_CMDMASCHINE_H_
#define SOURCES_CMDMASCHINE_H_

typedef enum
{
    IDLE,

    LED,LED_CLEAR,LED_SET,LED_READ,

	SWITCH,

	BUZZER,BUZZER_TIMER_X00, BUZZER_TIMER_XX0,
} state_machine_type_e;

/* Constants to inform if the input was wrong or not */
#define ERR 1
#define OK  0

/* ***************************************************** */
/* Method name:        stateMachine_stateProgression     */
/* Method description: Change the state machine state    */
/*                     according to the user input       */
/* Input params:       ucDataValue = character read from */
/*                       the user input                  */
/*                     cLedsStates[] = vector containing */
/*                       data on what leds should be on  */
/*                     iBuzzerTimer = reference to a     */
/*                       variable containing the time in */
/*                       ms to play the buzzer           */
/* Output params:      n/a                               */
/* ***************************************************** */
void stateMachine_stateProgression(unsigned char ucDataValue, char cLedsStates[], int* iBuzzerTimer);

#endif /* SOURCES_CMDMASCHINE_H_ */
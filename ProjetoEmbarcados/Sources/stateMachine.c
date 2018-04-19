/* ***************************************************************** */
/* File name:        buzzer_hal.c                                    */
/* File description: File dedicated to the hardware abstraction layer*/
/*                   related buzzer from the peripheral board        */
/* Author name:      dloubach                                        */
/* Creation date:    12jan2016                                       */
/* Revision date:    25fev2016                                       */
/* ***************************************************************** */

#include "stateMachine.h"
#include "serial.h"
#include "ledswi_hal.h"

void stateMachine_readSwitch(char cSwitchID)
{
	char cSwitchState;
	ledswi_initLedSwitch(0, 4);
	if(SWITCH_ON == ledswi_getSwitchStatus(cSwitchID))
		cSwitchState = 'C';
	else
		cSwitchState = 'O';
	serial_sendData(cSwitchState);
}

void stateMachine_stateProgression(unsigned char ucDataValue, char cLedsStates[], int* iBuzzerTimer)
{
	static state_machine_type_e ssmCurrentState = IDLE;
	static int siBuzzerTimer;
	state_machine_type_e smNextState = IDLE;
	char cErr = 0;
	switch(ssmCurrentState)
	{
	    case IDLE:
	        if('L'==ucDataValue)
	        {
	           smNextState = LED;
	        }
	        else if('S'==ucDataValue)
	        {
	        	smNextState = SWITCH;
	        }
	        else if('B'==ucDataValue)
	        {
	        	smNextState = BUZZER;
	        }
	        else cErr = 1;
	    	break;

	   case LED:
			if('S'==ucDataValue)
			{
			   smNextState = LED_SET;
			}
			else if('C'==ucDataValue)
			{
				smNextState = LED_CLEAR;
			}
			else cErr = 1;
			break;

	    case LED_CLEAR:
			if('1'<=ucDataValue && ucDataValue <='4')
			{
				cLedsStates[ucDataValue-'1'] = 0;
				serial_sendAck();
			}
			else cErr = 1;
			break;

	    case LED_SET:
			if('1'<=ucDataValue && ucDataValue <='4')
			{
				cLedsStates[ucDataValue-'1'] = 1;
				serial_sendAck();
			}
			else cErr = 1;
			break;

	    case SWITCH:
			if('1'<=ucDataValue && ucDataValue <='4')
			{
				stateMachine_readSwitch(ucDataValue-'1');
				serial_sendAck();
			}
			else cErr = 1;
			break;

	    case BUZZER:
			if('0'<=ucDataValue && ucDataValue <='9')
			{
				siBuzzerTimer = 100*(ucDataValue-'0');
				smNextState = BUZZER_TIMER_X00;
			}
			else cErr = 1;
			break;

	    case BUZZER_TIMER_X00:
			if('0'<=ucDataValue && ucDataValue <='9')
			{
				siBuzzerTimer = siBuzzerTimer+10*(ucDataValue-'0');
				smNextState = BUZZER_TIMER_XX0;
			}
			else cErr = 1;
			break;

	    case BUZZER_TIMER_XX0:
			if('0'<=ucDataValue && ucDataValue <='9')
			{
				siBuzzerTimer = siBuzzerTimer+(ucDataValue-'0');
				(*iBuzzerTimer) = siBuzzerTimer;
				serial_sendAck();

			}
			else cErr = 1;
			break;
	}
	ssmCurrentState = smNextState;
	if (1 == cErr)
	{
		serial_sendErr();
	}
}


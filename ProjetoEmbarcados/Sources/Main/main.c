/* ********************************************** */
/* File name:         main.c                      */
/* File description:  Main file of the code.      */
/*                    Contains the initialization */
/*                    sequence and the main loop  */
/* Author name:       julioalvesMS & IagoAF       */
/* Creation date:     08mar2018                   */
/* Revision date:     25abr2018                   */
/* ********************************************** */

/* System includes */
#include "Util\util.h"
#include "Util\tc_hal.h"

/* Hardware abstraction layers */
#include "Buzzer\buzzer_hal.h"
#include "Util\mcg_hal.h"
#include "LedSwi\ledswi_hal.h"
#include "Display7Seg\display7seg_hal.h"
#include "LCD\lcd_hal.h"
#include "Cooler\cooler_hal.h"
#include "Cooler\tachometer_hal.h"
#include "Cooler\timer_counter.h"
#include "ADC\adc.h"
#include "ADC\lut_adc_3v3.h"

/* communication */
#include "Serial\serial.h"
#include "Protocolo\cmdMachine.h"


/* defines */
#define CYCLIC_EXECUTIVE_PERIOD         250 * 1000 /* 300000 micro seconds */


/* globals */
volatile unsigned int uiFlagNextPeriod = 0;         /* cyclic executive flag */
extern const unsigned char tabela_temp[256];

/* ****************************************************** */
/* Method name:         showHexNumber                     */
/* Method description:  Display a 2 bytes hex value using */
/*                      all four 7 segments displayes     */
/* Input params:        uiValue: Value to be shown        */
/* Output params:       n/a                               */
/* ****************************************************** */
void showHexNumber(unsigned int uiValue)
{
    /* Extreme left display */
    display7seg_setDisplay(uiValue%16,0,4);
    uiValue = uiValue/16;
    util_genDelay1ms();

    /* Middle left display */
    display7seg_setDisplay(uiValue%16,0,3);
    uiValue = uiValue/16;
    util_genDelay1ms();

    /* Middle right display */
    display7seg_setDisplay(uiValue%16,0,2);
    uiValue = uiValue/16;
    util_genDelay1ms();

    /* Extreme right display */
    display7seg_setDisplay(uiValue%16,0,1);
    util_genDelay1ms();

}


/* ************************************************ */
/* Method name:        playBuzz1ms                  */
/* Method description: Plays the buzzer for 1 ms    */
/* Input params:       n/a                          */
/* Output params:      n/a                          */
/* ************************************************ */
void playBuzz1ms(void)
{
    /* Wave 1 */
    buzzer_setBuzz();
    util_genDelay250us();
    buzzer_clearBuzz();
    util_genDelay250us();

    /* Wave 2 */
    buzzer_setBuzz();
    util_genDelay250us();
    buzzer_clearBuzz();
    util_genDelay250us();
}


/* ************************************************ */
/* Method name:        main_cyclicExecuteIsr        */
/* Method description: cyclic executive interrupt   */
/*                     service routine              */
/* Input params:       n/a                          */
/* Output params:      n/a                          */
/* ************************************************ */
void main_cyclicExecuteIsr(void)
{
    /* set the cyclic executive flag */
    uiFlagNextPeriod = 1;
}


/* ****************************************************** */
/* Method name:         setupPeripherals                  */
/* Method description:  Makes the necessaries setups and  */
/*                      initializations for a proper      */
/*                      preparation of the peripherals    */
/* Input params:        n/a                               */
/* Output params:       n/a                               */
/* ****************************************************** */
void setupPeripherals()
{
    /* Start clock */
    mcg_clockInit();

    /* Prepare the tachometer */
    tachometer_initSensor();

    /* Start serial communication */
    serial_init();

    /* Start leds */
    ledswi_initLedSwitch(0, 4);

    /* Start display7seg */
    /* display7seg_initDisplay(); */

    /* Start the buzzer */
    buzzer_init();

    /* Start the LCD */
    lcd_initLcd();

    /* Prepare the cooler as PWM */
    timer_initTPM1AsPWM();
    timer_cooler_init();

    adc_initADCModule();

    timer_heater_initHeater();

}


/* ****************************************************** */
/* Method name:         setupPeripherals                  */
/* Method description:  Makes the necessaries setups and  */
/*                      initializations for a proper      */
/*                      preparation of the peripherals    */
/* Input params:        n/a                               */
/* Output params:       n/a                               */
/* ****************************************************** */
void enableInterruptions()
{

    /* configure cyclic executive interruption */
    tc_installLptmr0(CYCLIC_EXECUTIVE_PERIOD, main_cyclicExecuteIsr);

    serial_enableIRQ();

}


/* ****************************************************** */
/* Method name:         main                              */
/* Method description:  System main function. Contains    */
/*                      all the code that will actually   */
/*                      run                               */
/* Input params:        n/a                               */
/* Output params:       n/a                               */
/* ****************************************************** */
int main(void)
{

    unsigned char ucDataValue;
    char cLedsStates[4] = {0, 0, 0, 0};
    int iBuzzerTimer = 0;
    int iLCDDisplayNumber = 0;
    int iRawTemperatureData = 0;
    int *piBuzzerTimer = &iBuzzerTimer;
    char cLine1[17] = "T:###@C R:###";
	char cLine2[17] = "C:###@Hz RIP SC";
	cLine1[5] = 223;
	cLine2[5] = 247;

    /* Make all the required inicializations */
    setupPeripherals();

    enableInterruptions();

    for (;;)
    {
    	ucDataValue = serial_bufferReadData();
        if(ucDataValue){
            cmdMachine_stateProgression(ucDataValue, cLedsStates, piBuzzerTimer);
        }

        /* Set the LEDs ON/OFF according to the state vector */
        ledswi_changeAllLeds(cLedsStates);

        /* If needed, play the buzzer */
        if(iBuzzerTimer>0)
        {
            playBuzz1ms();
            iBuzzerTimer--;
        }

        iLCDDisplayNumber = tachometer_readSensor();
        cLine2[4] = (char) (iLCDDisplayNumber % 10) + '0';
        iLCDDisplayNumber = iLCDDisplayNumber/10;
        cLine2[3] = (char) (iLCDDisplayNumber % 10) + '0';
        iLCDDisplayNumber = iLCDDisplayNumber/10;
        cLine2[2] = (char) (iLCDDisplayNumber % 10) + '0';

        iRawTemperatureData = adc_converter();
        iLCDDisplayNumber = tabela_temp[iRawTemperatureData];
        cLine1[4] = (char) (iLCDDisplayNumber % 10) + '0';
        iLCDDisplayNumber = iLCDDisplayNumber/10;
        cLine1[3] = (char) (iLCDDisplayNumber % 10) + '0';
        iLCDDisplayNumber = iLCDDisplayNumber/10;
        cLine1[2] = (char) (iLCDDisplayNumber % 10) + '0';

        iLCDDisplayNumber = iRawTemperatureData;
        cLine1[12] = (char) (iLCDDisplayNumber % 10) + '0';
        iLCDDisplayNumber = iLCDDisplayNumber/10;
        cLine1[11] = (char) (iLCDDisplayNumber % 10) + '0';
        iLCDDisplayNumber = iLCDDisplayNumber/10;
        cLine1[10] = (char) (iLCDDisplayNumber % 10) + '0';

        lcd_writeText(cLine1,cLine2);

        /* WAIT FOR CYCLIC EXECUTIVE PERIOD */
        while(!uiFlagNextPeriod);
        uiFlagNextPeriod = 0;

    } /* Never leave main */
    return 0;
}

/* ***************************************************************** */
/* File name:        serial.c                                        */
/* File description: File dedicated to the abstraction of the serial */
/*                   port communication                              */
/* Author name:      julioalvesMS & IagoAF                           */
/* Creation date:    12abr2018                                       */
/* Revision date:    25abr2018                                       */
/* ***************************************************************** */

#include "serial.h"

#include "KL25Z4\es670_peripheral_board.h"
#include "debugUart.h"
#include "fsl_debug_console.h"
#include "fsl_device_registers.h"


/* ************************************************ */
/* Method name:        serial_init                  */
/* Method description: Initialize the serial port   */
/* Input params:       n/a                          */
/* Output params:      n/a                          */
/* ************************************************ */
void serial_init(void)
{
    /* Start serial communication */
    debugUart_init();

}


/* ************************************************ */
/* Method name:        serial_hasData               */
/* Method description: Informs if there is data in  */
/*                     the buffer that can be read  */
/* Input params:       n/a                          */
/* Output params:      char: 1 if true else 0       */
/* ************************************************ */
char serial_hasData(void)
{
    return UART0_BRD_S1_RDRF(UART0);
}


/* ************************************************ */
/* Method name:        serial_sendAck               */
/* Method description: Send "ACK" through the       */
/*                     serial port                  */
/* Input params:       n/a                          */
/* Output params:      n/a                          */
/* ************************************************ */
void serial_sendAck(void)
{
    PUTCHAR('A');
    PUTCHAR('C');
    PUTCHAR('K');
}


/* ************************************************ */
/* Method name:        serial_sendErr               */
/* Method description: Send "ERR" through the       */
/*                     serial port                  */
/* Input params:       n/a                          */
/* Output params:      n/a                          */
/* ************************************************ */
void serial_sendErr(void)
{
    PUTCHAR('E');
    PUTCHAR('R');
    PUTCHAR('R');
}


/* ************************************************ */
/* Method name:        serial_getChar               */
/* Method description: Reads a character            */
/* Input params:       n/a                          */
/* Output params:      n/a                          */
/* ************************************************ */
unsigned char serial_getChar()
{
	return GETCHAR();
}


/* ************************************************ */
/* Method name:        serial_putChar               */
/* Method description: Sends a character            */
/* Input params:       ucDataToSend: Character to   */
/*                       be sent                    */
/* Output params:      n/a                          */
/* ************************************************ */
void serial_putChar(unsigned char ucDataToSend)
{
	PUTCHAR(ucDataToSend);
}

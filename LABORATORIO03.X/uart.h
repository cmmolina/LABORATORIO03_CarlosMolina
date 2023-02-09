/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UART_H
#define	UART_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
// TODO Insert appropriate #include <>


// Prototipo de función
void UART_RX_config (uint16_t baudrate);
void UART_TX_config (uint16_t baudrate);
void UART_write_char (unsigned char *character);
char UART_read_char ();

#endif	/* XC_HEADER_TEMPLATE_H */

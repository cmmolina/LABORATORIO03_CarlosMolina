/*
 * File:   adc.c
 * Author: carlo
 *
 * Created on 19 de enero de 2023, 11:43 PM
 */

#include "adc.h"
#include <stdint.h>

#define _XTAL_FREQ 8000000

//Variables


//Funciones
void UART_RX_config (uint16_t baudrate){
    SPBRG = 12;                     // Baud rate (8MHz/9600)
    //SBPRGH = 0;
    //BAUDZLbits.BRG16 = 1;
    TXSTAbits.SYNC = 0;             // Baud rate (8MHz/9600)
    RCSTAbits.SPEN = 1;             // Se habilita el módulo UART
    RCSTAbits.CREN = 1;             // *Recepción habilitada
}

void UART_TX_config (uint16_t baudrate){
    TXSTAbits.TXEN = 1;             /* Transmisión habilitada; TXIF se enciende
                                     automaticamente.*/
    PIR1bits.TXIF = 0;              // Apagamos la bandera de transmisión
}

void UART_write_char (unsigned char *character){
    while (*character != '\0'){
        while (TXIF != 1);
        TXREG = *character;
        *character++;
    }
}

char UART_read_char (){
    return RCREG;
}
/*
 * Universidad del Valle de Guatemala
 * Electrónica Digital 2
 * Carlos Mauricio Molina López (#21253)
 * LABORATORIO 03 - SPI Esclavo
 * Created on 02 de febrero de 2023, 00:20 AM
 */

//******************************************************************************
// Palabra de Configuración
//******************************************************************************

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <proc/pic16f887.h>
#include "adc.h"
#include "spi.h"

#define _XTAL_FREQ 2000000

//******************************************************************************
// Variables 
//******************************************************************************
unsigned int contador; 
unsigned int temporal;

float valorADC;
float VOLTAJE1;

//******************************************************************************
// Prototipos de Funciones
//******************************************************************************
void setup(void);
//******************************************************************************
// Interrupción
//******************************************************************************
void __interrupt() isr (void){    
    
    //Interrupción que indica que una Transmisión/Recepción SPI ha tomado lugar
    if (PIR1bits.SSPIF){
        temporal = SSPBUF;  
        __delay_ms(10);
        
        if (temporal == 1){    
            SSPBUF = contador;            // Cargamos el contador a la salida.
        }
        else if (temporal == 0){
            VOLTAJE1 = readADC(0);
            SSPBUF = VOLTAJE1;
        }
        
        PIR1bits.SSPIF = 0;
    }
    
    //Interrupción de Envío
    if (PIR1bits.TXIF){
        PIR1bits.TXIF = 0;
    }
    
    //Interrupción de Recepción
    if (PIR1bits.RCIF){
        PIR1bits.RCIF = 0;
    }
    
    //Interrupción del ADC cuando la lectura termina
    if (PIR1bits.ADIF){
        PIR1bits.ADIF = 0; 
    }
    
    //Interrupción del TMR0
    if (INTCONbits.T0IF){
        //TMR0 = tmr0_value;          // Cargamos 5ms de nuevo al TMR0
        INTCONbits.T0IF = 0;
    }
    
    //Interrupción del Puerto B 
    if (INTCONbits.RBIF){ 
        if (PORTBbits.RB0 == 1){
            /*
            while (PORTBbits.RB0 ==1){
                ;
            }
            */
            contador++;
            PORTD++;
        }
        else if (PORTBbits.RB1 == 1){
            /*
            while (PORTBbits.RB1 == 1){
                ;
            }
            */
            contador--;
            PORTD--;
        }   
        INTCONbits.RBIF = 0;
    }
}

//******************************************************************************
// Código Principal 
//******************************************************************************
void main(void) {
    setup();
    setupADC();
    contador = 0;
    temporal = 0;
    VOLTAJE1 = 0;
    
    //Loop Principal
    while(1){
        ; 
    }
}

//******************************************************************************
//Funciones
//******************************************************************************

void setup(void){
    
    //Configuración de I/O
    ANSEL = 0b00000001;             // RA0 como analógico
    ANSELH = 0; 

            //76543210
    TRISA = 0b11100001;             // RA0 como input, RA5 Slave Select
    TRISB = 0b00000011;             // RB0, RB1 como input
  //TRISC = 0b00000000;             // 
    TRISD = 0b00000000;             //
    TRISE = 0b00000000;             // 
    
    PORTA = 0b00000000; 
    PORTB = 0b00000000; 
  //PORTC = 0b00000100; 
    PORTD = 0b00000000; 
    PORTE = 0b00000000; 
    
    //Configuración del Puerto B 
           //76543210
    IOCB = 0b00000011;              // Pines de Puerto B con Interrupción
    OPTION_REGbits.nRBPU = 0;       // Pull-Up/Pull-Down
    INTCONbits.RBIE = 1;            // Se habilitan las interrupciones del Puerto B
    
    //Configuración de las Interrupciones
    INTCONbits.GIE = 1;             // Se habilitan las interrupciones globales
    INTCONbits.PEIE = 1;            // Se habilitan interrupciones de perifericos
    PIE1bits.SSPIE = 1;             // Se habilita la interrupción del SPI
    PIE1bits.ADIE = 1;              // Se habilita la interrupción del ADC
    INTCONbits.TMR0IE = 0;          // Se inhabilitan las interrupciones del TMR0    

    PIR1bits.SSPIF = 0;             // Flag de SPI en 0
    PIR1bits.ADIF = 0;              // Flag de ADC en 0
    INTCONbits.RBIF = 0;            // Flag de Interrupciones del Puerto B en 0
    INTCONbits.T0IF = 0;            // Flag de TMR0 en 0
    
    //Configuración del Oscilador
    //OSCCONbits.IRCF = 0b100;        // 1MHz
    //OSCCONbits.SCS = 1;             // Oscilador Interno
    
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}
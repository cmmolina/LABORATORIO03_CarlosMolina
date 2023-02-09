/*
 * Universidad del Valle de Guatemala
 * Electrónica Digital 2
 * Carlos Mauricio Molina López (#21253)
 * LABORATORIO 03 - SPI Maestro
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
#include "LCD.h"
#include "spi.h"

#define _XTAL_FREQ 2000000
#define tmr0_value 179

//Bits de Control
#define RS PORTDbits.RD2
#define EN PORTDbits.RD3
#define D4 PORTDbits.RD4
#define D5 PORTDbits.RD5
#define D6 PORTDbits.RD6
#define D7 PORTDbits.RD7

//
#define uC2 PORTCbits.RC2
#define uC3 PORTCbits.RC1

//******************************************************************************
// Variables 
//******************************************************************************

float voltaje1; 
float voltaje2; 

float voltaje1temporal;

unsigned int cont; 

float VOLTAJE1;
float VOLTAJE2; 

char ADC1[4];
char ADC2[4];
char prueba[4];

char hundreds;
char tens;
char ones;
char signo;

uint8_t unidades;
uint8_t decenas;
uint8_t unidades2;
uint8_t decenas2;

uint8_t tabla[16] = {'0',   //0
                     '1',   //1
                     '2',   //2
                     '3',   //3
                     '4',   //4
                     '5',   //5
                     '6',   //6
                     '7',   //7
                     '8',   //8
                     '9',   //9
                     'A',   //10
                     'B',   //11
                     'C',   //12
                     'D',   //13
                     'E',   //14
                     'F'};  //15

//******************************************************************************
// Prototipos de Funciones
//******************************************************************************
void setup(void);
unsigned int map(uint8_t value, int inputmin, int inputmax, int outmin, int outmax);
//******************************************************************************
// Interrupción
//******************************************************************************
void __interrupt() isr (void){    
    
    //Interrupción que indica que una Transmisión/Recepción SPI ha tomado lugar
    if (PIR1bits.SSPIF){
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
        TMR0 = tmr0_value;          // Cargamos 5ms de nuevo al TMR0
        INTCONbits.T0IF = 0;
    }
    
    //Interrupción del Puerto B 
    if (INTCONbits.RBIF){ 
        INTCONbits.RBIF = 0;
    }
}

//******************************************************************************
// Código Principal 
//******************************************************************************
void main(void) {
    setup();
    Lcd_Init();
    Lcd_Clear(); 
    
    while(1){
        
        //Slave Select
        uC2 = 1;                         // Se deshabilita el uC2
        __delay_ms(10);
        uC2 = 0;                         // Se habilita el uC2
        
        SSPBUF = 0;                      // Se envía el valor 0 para obtener el contador.
        
        while(!SSPSTATbits.BF){}          // Esperamos que el SSPBUF esté completo
        
        //SSPCONbits.WCOL
        cont = SSPBUF;            // Se obtiene el valor del contador
        
        __delay_ms(10);
        
        
        //Slave Select
        uC2 = 1;                         // Se deshabilita el uC2
        __delay_ms(10);
        uC2 = 0;                         // Se habilita el uC2
         
        SSPBUF = 1;                      // Se envía el valor 1 para obtener el voltaje.
        
        while(!SSPSTATbits.BF){}          // Esperamos que el valor se haya cargado al SSPBUF
        voltaje1 = SSPBUF;               // Guardamos el contenido del SSPBUF a la variable
         
        
        //Slave Select
        uC2 = 1;
        __delay_ms(10);
        uC3 = 1; 
        __delay_ms(10);
        uC3 = 0; 
        
        SSPBUF = 0; 
   
        while(!SSPSTATbits.BF){}        // Esperamos que el valor se haya cargado al SSPBUF
                
        voltaje2 = SSPBUF;             // Guardamos el contenido del SSPBUF a la variable
       
        
        uC3 = 1; 
       __delay_ms(1);
        
        
       
       //Desplegamos Índices de Voltaje
        Lcd_Set_Cursor(1,2);
        Lcd_Write_String("S1");
        Lcd_Set_Cursor(1,8); 
        Lcd_Write_String("S2");
        Lcd_Set_Cursor(1,13);
        Lcd_Write_String("CON");
        
        
        //Desplegamos S1 (Valor del POT1)
        Lcd_Set_Cursor(2,1);
        VOLTAJE1 = (voltaje1*5)/255;
        sprintf(ADC1,"%.2f", VOLTAJE1);
        Lcd_Write_String(ADC1);
        Lcd_Write_String("V");
        
        
        //Desplegamos CONT
        Lcd_Set_Cursor(2,13);
        
        hundreds = (cont/100);
        tens = (cont/10)%10;
        ones = cont%10; 
        
        Lcd_Write_Char(hundreds+48);
        Lcd_Write_Char(tens+48);
        Lcd_Write_Char(ones+48);
        
        
        //Desplegamos S2 (Valor del POT2)
        Lcd_Set_Cursor(2,7);
        VOLTAJE2 = (voltaje2*5)/255;
        sprintf(ADC2,"%.2f", VOLTAJE2);
        Lcd_Write_String(ADC2);
        Lcd_Write_String("V");
        
        
       __delay_ms(1);
    }
    return;
}

//******************************************************************************
//Funciones
//******************************************************************************

void setup(void){
    
    //Configuración de I/O 
    ANSEL = 0b00000000;             // 
    ANSELH = 0; 

            //76543210
    TRISA = 0b00000000;             // 
    TRISB = 0b00000000;             // 
    //TRISC = 0b00000000;           // 
    TRISD = 0b00000000;             //
    TRISE = 0b00000000;             // 
    
    PORTA = 0b00000000; 
    PORTB = 0b00000000; 
    //PORTC = 0b00000100; 
    PORTD = 0b00000000; 
    PORTE = 0b00000000;
    
    TRISCbits.TRISC2 = 0;
    PORTCbits.RC2 = 0;
    TRISCbits.TRISC1 = 0; 
    PORTCbits.RC1 = 0;
    
    //Configuración de las Interrupciones
    INTCONbits.GIE = 0;             // Se habilitan las interrupciones globales
    INTCONbits.PEIE = 0;            // Se habilitan interrupciones de perifericos
    PIE1bits.SSPIE = 0;             // Se habilita la interrupción del SPI
    PIE1bits.ADIE = 0;              // Se habilita la interrupción del ADC
    INTCONbits.TMR0IE = 0;          // Se inhabilitan las interrupciones del TMR0    

    PIR1bits.SSPIF = 0;             // Flag de SPI en 0
    PIR1bits.ADIF = 0;              // Flag de ADC en 0
    INTCONbits.RBIF = 0;            // Flag de Interrupciones del Puerto B en 0
    INTCONbits.T0IF = 0;            // Flag de TMR0 en 0
    
    //Configuración del Oscilador
    OSCCONbits.IRCF = 0b101;        // 2MHz
    OSCCONbits.SCS = 1;             // Oscilador Interno
    
    SSPCONbits.WCOL = 0;
    
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}

//******************************************************************************
// Función de Conversión 
//******************************************************************************
unsigned int map(uint8_t value, int inputmin, 
                  int inputmax, int outmin, int outmax){
    return ((value - inputmin)*(outmax-outmin)) / ((inputmax-inputmin)+outmin);
}
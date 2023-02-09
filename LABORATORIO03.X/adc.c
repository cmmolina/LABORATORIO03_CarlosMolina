/*
 * File:   adc.c
 * Author: carlo
 *
 * Created on 19 de enero de 2023, 11:43 PM
 */

#include "adc.h"
#include <stdint.h>

#define _XTAL_FREQ 8000000

float ADC_Voltaje;

//******************************************************************************
// Función de Lectura de ADC
//******************************************************************************
int readADC(int canal){    
    //Canal AN0
    if (canal == 0){
        //--Lectura Canal AN0
        ADCON0bits.CHS = 0b0000;    // Usamos el AN0
         __delay_us(100);
        ADCON0bits.GO = 1;          //Iniciamos la conversión del ADC
        
        while (ADCON0bits.GO == 1){
        ;
        }
        PIR1bits.ADIF=0;
        ADC_Voltaje = ADRESH;
        __delay_us(100);
        
    }
    
    //Canal AN1
    else if (canal == 1){
        //--Lectura Canal AN1
        ADCON0bits.CHS = 0b0001;    
         __delay_us(100);
        ADCON0bits.GO = 1;          //Iniciamos la conversión del ADC
        
        while (ADCON0bits.GO == 1){
            ;
        }
        PIR1bits.ADIF=0;
        ADC_Voltaje = ADRESH;
        __delay_us(100);
    } 
    
    return ADC_Voltaje;
}


void setupADC(void){
    //Módulo de ADC
    ADCON0bits.ADCS = 0b01;         // Fosc/8
    
    ADCON1bits.VCFG1 = 0;           // Voltaje de Referencia + - VSS
    ADCON1bits.VCFG0 = 0;           // Voltaje de Referencia - - VDD
    
    //Formato de Resultado 
    ADCON1bits.ADFM = 0;            // Justificado a la Izquierda
    
    //Canal
    ADCON0bits.CHS = 0b0000;        // Canal AN0 (Para empezar)
    
    ADCON0bits.ADON = 1;            // Se habilita el ADC
    
    PIR1bits.ADIF = 0;              // Apagamos la bandera
    
    //Delay (Ejemplo)
    __delay_us(100);
    
}

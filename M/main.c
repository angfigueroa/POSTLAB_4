/* 
 * File:   main.c
 * Author: ANGELA
 *
 * Created on 3 de agosto de 2023, 16:20
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <stdint.h>
#include <pic16f887.h>
#include "I2C.h"
#include "DS3231.h"
#include "LCD.h"
#include <xc.h>
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
#define I2C_100KHZ 100000

unsigned char valor_ADC; //Guarda el valor del ADC
char buffer[20]; //Buffer para guardar la cadena de la LCD
int8_t selector = 0; //Selecciona el modo de configuracion
//Variables para guardar los datos del sensor
uint8_t dia;
uint8_t mes;
uint8_t ano;
uint8_t dow;
uint8_t horas;
uint8_t minutos;
uint8_t segundos;


//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
void CLK_CONFIG(void);

//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    while(1){
        
        //Recepcion de valores con I2C
        I2C_Master_Start();
        I2C_Master_Write(0x51);
        valor_ADC = I2C_Master_Read(I2C_100KHZ);
        I2C_Master_Stop();
        __delay_us(10);
        
        if(PORTAbits.RA0 == 1)
        {
            while(PORTAbits.RA0 == 1);
            __delay_ms(20);
            Lcd_Clear();
            selector++;
        }    
        CLK_CONFIG();  
         
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
//Puertos digitales

void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    
    //CONFIG de salidas
    TRISB = 0;
    TRISD = 0;
    
    //Reinicio de puertos
    PORTB = 0;
    PORTD = 0;
    PORTA = 0;
    
    //Frecuencia de trabajo a 8MHz
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    
    //Reloj interno
    OSCCONbits.SCS = 1;
    
    //Iniciamos la LCD
    Lcd_Init();
    
    //Config del I2C para el maestro
    I2C_Master_Init(I2C_100KHZ);  
   
}

void CLK_CONFIG(void)
{
    switch(selector)
        {
        case 0: 
            DS3231_Get_Date((uint8_t)&dia, (uint8_t)&mes, (uint8_t)&ano, (uint8_t)&dow);
            DS3231_Get_Time((uint8_t)&horas, (uint8_t)&minutos, (uint8_t)&segundos);
            
            //Escribir el valor del adc en el display
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("ADC: ");
            sprintf(buffer, "%u ", valor_ADC);
            Lcd_Set_Cursor(2,1);
            Lcd_Write_String(buffer);
            
            //Escribe la fecha y hora en la LCD
            sprintf(buffer, "%02u/%02u/20%02u", dia,mes,ano);
            Lcd_Set_Cursor(1,6);
            Lcd_Write_String(buffer);
            sprintf(buffer,"%02u:%02u:20%02u", horas,minutos,segundos);
            Lcd_Set_Cursor(2,8);
            Lcd_Write_String(buffer);
            __delay_ms(200);
            break;
        case 1:
            //Mensaje del selector
            Lcd_Set_Cursor(1,1);
            sprintf(buffer, "Dia:%02u", dia);
            Lcd_Write_String(buffer);
            
            //Push button de incremento
            if (PORTAbits.RA1 == 1)
            {
                while(PORTAbits.RA1 == 1);
                __delay_ms(20);
                dia++;
                if(dia > 31){
                    dia=31;
                }
            }
            //Push button de decremento
            if (PORTAbits.RA2 == 1)
            {
                while(PORTAbits.RA2 == 1);
                __delay_ms(20);
                dia--;
                if(dia < 1){
                    dia= 1;
                }
            }
            break;
        case 2: //Selector del mes
            
            //Mensaje del selector
            Lcd_Set_Cursor(1,1);
            sprintf(buffer, "Mes:%02u", mes);
            Lcd_Write_String(buffer);
            
            //Push button de incremento
            if (PORTAbits.RA1 == 1)
            {
                while(PORTAbits.RA1 == 1);
                __delay_ms(20);
                mes++;
                if(mes > 12){
                    mes = 12;
                }
            }
            //Push button de decremento
            if (PORTAbits.RA2 == 1)
            {
                while(PORTAbits.RA2 == 1);
                __delay_ms(20);
                mes--;
                if(mes < 1){
                    mes = 1;
                }
            }
            break;
        case 3: //Selector del ano
            
            //Mensaje del selector
            Lcd_Set_Cursor(1,1);
            sprintf(buffer, "Ano:%02u", ano);
            Lcd_Write_String(buffer);    
            
            //Push button de incremento
            if (PORTAbits.RA1 == 1)
            {
                while(PORTAbits.RA1 == 1);
                __delay_ms(20);
                ano++;
                if(ano > 99){
                    ano = 99;
                }
            }
            //Push button de decremento
            if (PORTAbits.RA2 == 1)
            {
                while(PORTAbits.RA2 == 1);
                __delay_ms(20);
                ano--;
                if(ano == -1){
                    ano = 0;
                }
            }
            break;
            
        case 4: //Selector del dia de la semana
            
            //Mensaje del selector
            Lcd_Set_Cursor(1,1);
            sprintf(buffer, "Dia semana:%u", dow);
            Lcd_Write_String(buffer);
            Lcd_Set_Cursor(2,1);
            sprintf(buffer, "%s   ", dw[dow]);
            Lcd_Write_String(buffer);
            
            //Push button de incremento
            if (PORTAbits.RA1 == 1)
            {
                while(PORTAbits.RA1 == 1);
                __delay_ms(20);
                dow++;
                if(dow > 6){
                    dow = 6;
                }
            }
            //Push button de decremento
            if (PORTAbits.RA2 == 1)
            {
                while(PORTAbits.RA2 == 1);
                __delay_ms(20);
                dow--;
                if(dow == -1){
                    dow = 0;
                }
            }
            break;
        case 5: //Selector de hora
            
             
            //Mensaje del selector
            Lcd_Set_Cursor(1,1);
            sprintf(buffer, "Hora:%02u", horas);
            Lcd_Write_String(buffer);
            
            //Push button de incremento
            if (PORTAbits.RA1 == 1)
            {
                while(PORTAbits.RA1 == 1);
                __delay_ms(20);
                horas++;
                if(horas > 23){
                    horas = 23;
                }
            }
            //Push button de decremento
            if (PORTAbits.RA2 == 1)
            {
                while(PORTAbits.RA2 == 1);
                __delay_ms(20);
                horas--;
                if(horas == -1){
                    horas = 0;
                }
            }
            break;
        case 6: //Selector de minutos
            
            //Mensaje del selector
            Lcd_Set_Cursor(1,1);
            sprintf(buffer, "minutosuto:%02u", minutos);
            Lcd_Write_String(buffer);
            
            //Push button de incremento
            if (PORTAbits.RA1 == 1)
            {
                while(PORTAbits.RA1 == 1);
                __delay_ms(20);
                minutos++;
                if(minutos > 59){
                    minutos = 59;
                }
            }
            //Push button de decremento
            if (PORTAbits.RA2 == 1)
            {
                while(PORTAbits.RA2 == 1);
                __delay_ms(20);
                minutos--;
                if(minutos == -1){
                    minutos = 0;
                }
            }
            break;
        case 7: //Selector de segundos
            
            //Mensaje del selector
            Lcd_Set_Cursor(1,1);
            sprintf(buffer, "Segundo:%02u", segundos);
            Lcd_Write_String(buffer);
            
            //Push button de incremento
            if (PORTAbits.RA1 == 1)
            {
                while(PORTAbits.RA1 == 1);
                __delay_ms(20);
                segundos++;
                if(segundos > 59){
                    segundos = 59;
                }
            }
            //Push button de decremento
            if (PORTAbits.RA2 == 1)
            {
                while(PORTAbits.RA2 == 1);
                __delay_ms(20);
                segundos--;
                if(segundos == -1){
                    segundos = 0;
                }
            }
            break;   
            
        default: 
            dia = (uint8_t)dia;
            mes = (uint8_t)mes;
            ano = (uint8_t)ano;
            horas = (uint8_t)horas;
            minutos = (uint8_t)minutos;
            segundos = (uint8_t)segundos;
            DS3231_Set_Date_Time(dia,mes,ano,dow,horas,minutos,segundos);
            selector = 0;
            break;
        
            
    }
}

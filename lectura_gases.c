#define uC18f4620 
#include <18F4620.h>
#device adc=10
#fuses HS, NOFCMEN, NOIESO, PUT, NOBROWNOUT, NOWDT
#fuses NOPBADEN, NOMCLR, STVREN, NOLVP, NODEBUG
#use delay(clock=16000000)
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

//LCD
#define LCD_RS_PIN      PIN_D0                                   
#define LCD_RW_PIN      PIN_D1   
#define LCD_ENABLE_PIN  PIN_D2 
#define LCD_DATA4       PIN_D4                                    
#define LCD_DATA5       PIN_D5                                    
#define LCD_DATA6       PIN_D6                                    
#define LCD_DATA7       PIN_D7        
#include <LCD.C>


int leerADC_ok = 0, banderaButano = 0, banderaHidrogeno = 0;
int16 leerADC = 0, ppmHidrogeno = 0, ppmButano = 0;

#int_ad
void isr_adc(){
    leerADC = read_adc(ADC_READ_ONLY);
    leerADC_ok = 1;
}

void main(){
   set_tris_a(0xFF);
   setup_oscillator(OSC_16MHZ);
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_adc_ports(AN0_TO_AN1);
   enable_interrupts(INT_AD);
   enable_interrupts(GLOBAL);
   
   lcd_init();
   
   banderaHidrogeno = 1;
   set_adc_channel(0);
   delay_us(10);
   leerADC = read_adc();
   read_adc(ADC_START_ONLY);
   
   while(1){ 
       if(banderaHidrogeno == 1 && leerADC_ok == 1){
           banderaHidrogeno = 0;
           banderaButano = 1;
           leerADC_ok = 0;
           set_adc_channel(1);
           delay_us(10);
           read_adc(ADC_START_ONLY);
           ppmHidrogeno = leerADC;
       }
       if(banderaButano == 1 && leerADC_ok == 1){
           banderaHidrogeno = 1;
           banderaButano = 0;
           leerADC_ok = 0;
           set_adc_channel(0);
           delay_us(10);
           read_adc(ADC_START_ONLY);
           ppmButano = leerADC;
       }
       lcd_gotoxy(1,1);
       printf(lcd_putc,"Hidrogeno: %lu", ppmHidrogeno);
       lcd_gotoxy(1,2);
       printf(lcd_putc,"Butano: %lu", ppmButano);
       delay_ms(500);
   }
}




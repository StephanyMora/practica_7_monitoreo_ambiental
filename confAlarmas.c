#define uC18f4620 
#include <18F4620.h>
#device adc=10
#fuses HS, NOFCMEN, NOIESO, PUT, NOBROWNOUT, NOWDT
#fuses NOPBADEN, NOMCLR, STVREN, NOLVP, NODEBUG
#use delay(clock=16000000)
#include <stdio.h>
#include <stdlib.h>

//LCD
#define LCD_RS_PIN      PIN_D0                                   
#define LCD_RW_PIN      PIN_D1   
#define LCD_ENABLE_PIN  PIN_D2 
#define LCD_DATA4       PIN_D4                                    
#define LCD_DATA5       PIN_D5                                    
#define LCD_DATA6       PIN_D6                                    
#define LCD_DATA7       PIN_D7        
#include <LCD.C>

//BOTONES DE MENU
#define ARRIBA PIN_B4
#define ABAJO PIN_B5
#define ADELANTE PIN_B6
#define ATRAS PIN_B7

#define testigoTemperatura PIN_B0
#define testigoHumedad PIN_B1
#define testigoButano PIN_B2
#define testigoHidrogeno PIN_B3

#BIT datoDHT = 0xF82.4             //pin de dato de entrada del DHT11 portc
#BIT trisDHT = 0xF94.4  //bit 0 del tris b para ponerlo como entrada o salida

void alarmas();

void mostrarLCD();

short timeOut;
void conexionDHT();
short conexionOK();
unsigned int leerDHT();
void mostrarDHT();
void mostrarGases();

int leerADC_ok = 0, banderaButano = 0, banderaHidrogeno = 0;
int8 enteroTemperatura = 0, decimalTemperatura = 0, enteroRH = 0, decimalRH = 0, checksum = 0, lectura = 0;
int16 leerADC = 0, ppmHidrogeno = 0, ppmButano = 0, temperatura = 0, humedad = 0;
int8 initTemperatura = 50, initHumedad = 80, initHidrogeno = 500, initButano = 500;

#int_ad
void isr_adc(){
    leerADC = read_adc(ADC_READ_ONLY);
    leerADC_ok = 1;
}

void main(){
   setup_oscillator(OSC_16MHZ);
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_adc_ports(AN0_TO_AN1);
   enable_interrupts(INT_AD);
   enable_interrupts(GLOBAL);
   set_tris_a(0xFF);
   set_tris_b(0xF0);
   set_tris_d(0x00);  
   
   banderaHidrogeno = 1;
   set_adc_channel(0);
   delay_us(10);
   leerADC = read_adc();
   read_adc(ADC_START_ONLY);
   
   lcd_init();
               
   while(1){
      
      conexionDHT();
      if(conexionOK()){          
         mostrarDHT();
         mostrarGases();
      }
      
   }
}

void conexionDHT(){
   trisDHT = 0;  //configuracion del pin C4 como salida
   datoDHT = 0;       //se encia un 0 al sensor
   delay_ms(18);
   datoDHT = 1;
   delay_us(30);
   trisDHT = 1;  //configuracion de pin C4 como entrada
}
short conexionOK(){
   delay_us(40);
   if(!datoDHT){                     // Read and test if connection pin is low
      delay_us(80);
      if(datoDHT){                    // Read and test if connection pin is high
         delay_us(50);
         return 1;}
   }
}
unsigned int leerDHT(){
   unsigned int i, k, data = 0;     // k is used to count 1 bit reading duration
   if(timeOut)
      break;
   for(i = 0; i < 8; i++){
      k = 0;
      while(!datoDHT){                          // Wait until pin goes high
         k++;
         if (k > 100) {
            timeOut = 1; 
            break;
         }
         delay_us(1);
      }
      disable_interrupts(GLOBAL);
      delay_us(30);
      if(!datoDHT)
         bit_clear(data, (7 - i));               // Clear bit (7 - i)
      else{
         bit_set(data, (7 - i));                 // Set bit (7 - i)
         while(datoDHT){                         // Wait until pin goes low
            k++;
            if (k > 100) {timeOut = 1; break;}
               delay_us(1);}
      }
      enable_interrupts(GLOBAL);
   }
   return data;
}
void mostrarDHT(){
   enteroRH = leerDHT();
   decimalRH = leerDHT();
   enteroTemperatura = leerDHT();
   decimalTemperatura = leerDHT();
   checksum = leerDHT();
   if(checksum == (enteroRH + decimalRH + enteroTemperatura + decimalTemperatura)){
      temperatura = enteroTemperatura + decimalTemperatura;
      humedad = enteroRH + decimalRH;
      alarmas();
      lcd_gotoxy(1,1);
      printf(lcd_putc, "Temperatura: %lu", temperatura);
      lcd_gotoxy(1,2);
      printf(lcd_putc, "Humedad: %lu", humedad);
      delay_ms(1000);
      printf(lcd_putc, "\f");
   }
}

void alarmas(){
   if(temperatura >= initTemperatura){
      output_high(testigoTemperatura);
   }
   else{
      output_low(testigoTemperatura);
   }
   if(humedad >= initHumedad){
      output_high(testigoHumedad);
   }
   else{
      output_low(testigoHumedad);
   }
   if(ppmHidrogeno >= initHidrogeno){
      output_high(testigoHidrogeno);
   }
   else{
      output_low(testigoHidrogeno);
   }
   if(ppmButano >= initButano){
      output_high(testigoButano);
   }
   else{
      output_low(testigoButano);
   }
}

void mostrarGases(){
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
   alarmas();
   lcd_gotoxy(1,1);
   printf(lcd_putc,"Hidrogeno: %lu", ppmHidrogeno);
   lcd_gotoxy(1,2);
   printf(lcd_putc,"Butano: %lu", ppmButano);
   delay_ms(1000);
   printf(lcd_putc, "\f");
}



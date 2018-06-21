#define uC18f4620 
#include <18F4620.h>
#fuses HS, NOFCMEN, NOIESO, PUT, NOBROWNOUT, NOWDT
#fuses NOPBADEN, NOMCLR, STVREN, NOLVP, NODEBUG
#use delay(clock=16000000)
#include <stdio.h>
#include <stdlib.h>
#define datosDHT PIN_C4

#BIT setTrisC = 0xF94.4

#define LCD_RS_PIN      PIN_D0                                   
#define LCD_RW_PIN      PIN_D1   
#define LCD_ENABLE_PIN  PIN_D2 
#define LCD_DATA4       PIN_D4                                    
#define LCD_DATA5       PIN_D5                                    
#define LCD_DATA6       PIN_D6                                    
#define LCD_DATA7       PIN_D7        
#include <LCD.C>

//se declaran variables globales
int8 enteroTemperatura = 0, decimalTemperatura = 0, enteroRH = 0, decimalRH = 0, checksum = 0, lectura = 0;

int establecerConexion(){
   setTrisC = 0;
   output_low(datosDHT);
   delay_ms(18);
   output_high(datosDHT);
   delay_us(30);
   setTrisC = 1;
   lectura = 0;
   delay_us(40);
   if(input(datosDHT) == 0){
      delay_us(80);
      if(input(datosDHT) == 1){
         delay_us(40);
         lectura = 1;
         
      }
   }
   return lectura;
}

unsigned int8 leerDHT(){
   unsigned int8 i, k, datosLeidos = 0;
   disable_interrupts(GLOBAL);   
   for(i = 0; i < 8; i++){
      k = 0;
      while(!input(datosDHT)){
         k++;
         if(k > 100){
            break;
         }
         delay_us(1);
      }
      delay_us(30);
      if(!input(datosDHT)){
         bit_clear(datosLeidos, (7 - i));
      }
      else{
         bit_set(datosLeidos, (7 - i));
         while(input(datosDHT)){
            k++;
            if(k > 100){
               break;
            }
            delay_us(1);
         }
      }
   }
   enable_interrupts(GLOBAL);
   return datosLeidos;
}
void main (void){
   setup_oscillator(OSC_16MHZ);
   set_tris_d(0x00);   
   lcd_init();
   while(1){
      if(establecerConexion()){
         enteroRH = leerDHT();
         decimalRH = leerDHT();
         enteroTemperatura = leerDHT();
         decimalTemperatura = leerDHT();
         checksum = leerDHT();
         if(checksum == (enteroRH + decimalRH + enteroTemperatura + decimalTemperatura)){
             lcd_gotoxy(1,1);
             printf(lcd_putc,"conexion ok");
         }
      }
   }    
} 


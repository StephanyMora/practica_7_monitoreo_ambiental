#define uC18f4620 
#include <18F4620.h>
#device adc = 10
#fuses HS, NOFCMEN, NOIESO, PUT, NOBROWNOUT, NOWDT
#fuses NOPBADEN, NOMCLR, STVREN, NOLVP, NODEBUG
#use delay(clock=16000000)

#BIT setTrisC = 0xF94.4


#define alertaTemperatura PIN_B0
#define alertaHumedad PIN_B1
#define alertaHidrogeno PIN_B2
#define alertaButano PIN_B3
#define adelante PIN_B4
#define atras PIN_B5
#define arriba PIN_B6
#define abajo PIN_B7
//#define hidrogeno PIN_A0
//#define butano PIN_A1
#define datosDHT PIN_C4


unsigned int conteoArriba = 0, conteoAbajo = 0, conteoAdelante = 0, conteoAtras = 0;
int8 enteroTemperatura = 0, decimalTemperatura = 0, enteroRH = 0, decimalRH = 0, checksum = 0, lectura = 0;
int voltajeHidrogeno = 0, voltajeButano = 0, ppmHidrogeno = 0, ppmButano = 0;

#INT_RB
void ISR_pushButtn(void){
    if(input(adelante)){
        conteoAdelante = 1;
    }
    else if(input(atras)){
        conteoAtras = 1;
    }
    else if(input(arriba)){
        conteoArriba = 1;
    }
    else if(input(abajo)){
        conteoAbajo = 1;
    }
}

void limpiar_banderas(void){
    conteoAdelante = 0;
    conteoAtras = 0;
    conteoArriba = 0;
    conteoAbajo = 0;
}

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

int calcular_ppmHidrogeno(int lecturaHidrogeno){
    int calculoPPM = 0;
    calculoPPM = ((lecturaHidrogeno*1000)/3.3);
    return calculoPPM;
}

int calcular_ppmButano(int lecturaButano){
    int calculoPPM = 0;
    //calculoPPM = ((lecturaButano*1000)/3.3);
    return calculoPPM;
}

void main (void){
    setup_oscillator(OSC_16MHZ);
    enable_interrupts(INT_RB);
    enable_interrupts(GLOBAL);
    set_tris_d(0x00);
    set_tris_b(0xFF);
    set_tris_c(0b00001100);
     
    setup_adc(ADC_CLOCK_DIV_32);  
    setup_adc_ports(AN0_TO_AN1); 
   
    //menu
    if(conteoAdelante == 1){
            limpiar_banderas();
            
        }
        else if(conteoAtras == 1){
            limpiar_banderas();
            
        }
        else if(conteoArriba == 1){
            limpiar_banderas();
            
        }
        else if(conteoAbajo == 1){
            limpiar_banderas();
        }
    
    while(1){
        //leer butano e hidrogeno
        set_adc_channel(0); 
        delay_ms(20);
        voltajeHidrogeno = read_adc(); 
        delay_ms(20);
        ppmHidrogeno = calcular_ppmHidrogeno(voltajeHidrogeno);
        set_adc_channel(1); 
        delay_ms(20);
        voltajeButano = read_adc(); 
        delay_ms(20);
        ppmButano = calcular_ppmButano(voltajeButano);
        
        voltajeButano = 0, voltajeHidrogeno = 0;
       
        //lectura de humedad y temperatura
        if(establecerConexion()){
            enteroRH = leerDHT();
            decimalRH = leerDHT();
            enteroTemperatura = leerDHT();
            decimalTemperatura = leerDHT();
            checksum = leerDHT();
            if(checksum == (enteroRH + decimalRH + enteroTemperatura + decimalTemperatura)){
            }
        }  
    }
}

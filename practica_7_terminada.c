#define uC18f4620 
#include <18F4620.h>
#device adc=10
#fuses HS, NOFCMEN, NOIESO, PUT, NOBROWNOUT, NOWDT
#fuses NOPBADEN, NOMCLR, STVREN, NOLVP, NODEBUG
#use delay(clock=16000000)



#define LCD_RS_PIN      PIN_D0                                   
#define LCD_RW_PIN      PIN_D1   
#define LCD_ENABLE_PIN  PIN_D2 
#define LCD_DATA4       PIN_D4                                    
#define LCD_DATA5       PIN_D5                                    
#define LCD_DATA6       PIN_D6                                    
#define LCD_DATA7       PIN_D7        
#include <LCD.C>

#define ARRIBA PIN_B4
#define ABAJO PIN_B5
#define ADELANTE PIN_B6
#define ATRAS PIN_B7
#define testigoTemperatura PIN_B0
#define testigoHumedad PIN_B1
#define testigoButano PIN_B2
#define testigoHidrogeno PIN_B3
#BIT datoDHT = 0xF82.4             
#BIT trisDHT = 0xF94.4  
#define TX_232        PIN_C6
#define RX_232        PIN_C7
#use RS232(BAUD=9600, XMIT=TX_232, RCV=RX_232, stream = uart, BITS=8, PARITY=N, STOP=1)
#use fast_io(c)

void menu();
void saludo();
void menuDatos();
void menuConfiguracion();
void subMenuAlarmas();
void cambioDeEstado();
void configuracionSerial();
void configuracionLCD();
void configuracionTemperatura();
void configuracionHumedad();
void configuracionHidrogeno();
void configuracionButano();
void mensajeOK();
void alarmas();
void mostrarLCD();
short timeOut;
void conexionDHT();
short conexionOK();
unsigned int leerDHT();
void mostrarDHT();
void mostrarGases();
void menuSerial();
void refreshSerial_LCD();
void saludoSerial();

int banderaMenu = 0, banderaCambioEstado = 0;
int apuntador = 1, apuntadorMenuConf = 1, apuntadorMenuAlarmas = 1;
int contadorAdelante = 0;
int enteroTemperatura = 0, decimalTemperatura = 0, enteroRH = 0, decimalRH = 0, checksum = 0;
int initTemperatura = 40, initHumedad = 45, initHidrogeno = 500, initButano = 500;
int initSerial = 400, aux = 0, initLCD = 10, refreshSerial = 0, refreshLCD = 0;
int leerADC_ok = 0, banderaButano = 0, banderaHidrogeno = 0;
int16 leerADC = 0, ppmHidrogeno = 0, ppmButano = 0, temperatura = 0, humedad = 0;

#int_timer0
void isr_timer0(){
    refreshSerial++;
    refreshLCD++;
}

#int_ad
void isr_adc(){
    leerADC = read_adc(ADC_READ_ONLY);
    leerADC_ok = 1;
}

#int_RB
void isr_RB(){
   if(input(ARRIBA) == 1){
      banderaMenu = 1;  
   }
   if(input(ABAJO) == 1){
      banderaMenu = 2;
   }
   if(input(ADELANTE)){
      banderaMenu = 3;
   }	
   if(input(ATRAS)){
   	  banderaMenu = 4;
   }
   banderaCambioEstado = 1;
}

void main(){
   setup_oscillator(OSC_16MHZ);
   setup_adc(ADC_CLOCK_INTERNAL);
   enable_interrupts(INT_RB);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_128|RTCC_8_BIT);
   enable_interrupts(INT_TIMER0);
   enable_interrupts(INT_AD);
   setup_adc_ports(AN0_TO_AN1);
   enable_interrupts(GLOBAL);
   set_tris_a(0xFF);
   set_tris_b(0xF0);
   set_tris_d(0x00);   
   
   lcd_init();
   
   banderaHidrogeno = 1;
   set_adc_channel(0);
   delay_us(10);
   leerADC = read_adc();
   read_adc(ADC_START_ONLY);
   saludoSerial();
   saludo();
   while(1){
       conexionDHT();
       if(conexionOK()){  
          mostrarDHT();
          mostrarGases();
       }
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
       refreshSerial_LCD();
       menu();
       cambioDeEstado();
   }    
} 

void cambioDeEstado(){
    if(banderaCambioEstado == 1){
        banderaCambioEstado = 0;
        switch(banderaMenu){
            case 1:
                printf(lcd_putc,"\f");
                apuntador--;
                if(apuntador <= 1){
                    apuntador = 1;
                }
                if(contadorAdelante == 1){
                    apuntadorMenuConf--;
                    if(apuntadorMenuConf <= 1){
                        apuntadorMenuConf = 1;
                    }
                }
                if(contadorAdelante == 2){
                    switch(apuntadorMenuConf){
                        case 1:
                            if(aux < 500){
                                aux++;
                            }
                            else{
                                aux = 500;
                            }
                            break;
                        case 2:
                            if(aux < 200){
                                aux++;
                            }
                            else{
                                aux = 200;
                            }
                            break;
                    }
                }
                if(contadorAdelante == 2 && apuntadorMenuConf == 3){
                    apuntadorMenuAlarmas--;
                    if(apuntadorMenuAlarmas <= 1){
                        apuntadorMenuAlarmas = 1;
                    }
                }
                if(contadorAdelante == 3){
                    switch(apuntadorMenuAlarmas){
                        case 1:
                            if(aux < 50){
                                aux++;
                            }
                            else{
                                aux = 50;
                            }
                            break;
                        case 2:
                            if(aux < 95){
                                aux++;
                            }
                            else{
                                aux = 95;
                            }
                            break;
                        case 3:
                            if(aux < 1000){
                                aux++;     
                            }
                            else{
                                aux = 1000;
                            }
                            break;
                        case 4:
                            if(aux < 1000){
                                aux++;
                            }
                            else{
                                aux = 1000;
                            }
                            break;
                    }
                }
                break;
            
            case 2:
                printf(lcd_putc,"\f");
                apuntador++;
                if(apuntador >= 4){
                    apuntador = 4;
                }
                if(contadorAdelante == 1){
                    apuntadorMenuConf++;
                    if(apuntadorMenuConf >= 3){
                        apuntadorMenuConf = 3;
                    }
                }
                if(contadorAdelante == 2){
                    switch(apuntadorMenuConf){
                        case 1:
                            if(aux > 10){
                                aux--;
                            }
                            else{
                                aux = 10;
                            }
                            break;
                        case 2:
                            if(aux > 20){
                                aux--;
                            }else
                            {
                                aux = 20;
                            }
                            break;
                    }
                }
                if(contadorAdelante == 2 && apuntadorMenuConf == 3){
                    if(apuntadorMenuAlarmas < 4){
                        apuntadorMenuAlarmas++;
                    }
                    else{
                        apuntadorMenuAlarmas = 4;
                    }
                }
                if(contadorAdelante == 3){
                    switch(apuntadorMenuAlarmas){
                        case 1:
                            if(aux > 1){
                                aux--; 
                            }
                            else{
                                aux = 1;
                            }
                            break;
                        case 2:
                            if(aux > 20){
                                aux--;
                            }
                            else{
                                aux = 20;
                            }
                            break;
                        case 3:
                            if(aux > 100){
                                aux--;
                            }
                            else{
                                aux = 100;
                            }
                            break;
                        case 4:
                            if(aux > 100){
                                aux--;
                            }
                            else{
                                aux = 100;
                            }
                            break;
                    }
                }
                break;
            
            case 3:
                printf(lcd_putc,"\f");
                contadorAdelante++;
                if(contadorAdelante == 1){
                    menuConfiguracion();
                }
                if(contadorAdelante == 3){
                    switch(apuntadorMenuConf){
                        case 1:
                            initSerial = aux;
                            mensajeOK();
                            contadorAdelante = 1;
                            aux = 0;
                            break;
                        case 2:
                            initLCD = aux;
                            mensajeOK();
                            contadorAdelante = 1;
                            aux = 0;
                            break;   
                    }
                }
                if(contadorAdelante == 4){
                    switch(apuntadorMenuAlarmas){
                        case 1:
                            initTemperatura = aux;
                            mensajeOK();
                            contadorAdelante = 2;
                            aux = 0;
                            break;
                        case 2:
                            initHumedad = aux;
                            mensajeOK();
                            contadorAdelante = 2;
                            aux = 0;
                            break;
                        case 3:
                            initHidrogeno = aux;
                            mensajeOK();
                            contadorAdelante = 2;
                            aux = 0;
                            break;
                        case 4:
                            initButano = aux;
                            mensajeOK();
                            contadorAdelante = 2;
                            aux = 0;
                            break;
                    }
                }
                break;
            
            case 4:
                printf(lcd_putc,"\f");
                if(contadorAdelante <= 0){
                    menuDatos();
                }
                else{
                    contadorAdelante--;
                    aux = 0;
                }
                break;
        }
    }
    banderaCambioEstado = 0;
    banderaMenu = 0;
}

void saludo(){
   lcd_gotoxy(4,1);
   printf(lcd_putc, "Bienvenido");
   delay_ms(1000);
   printf(lcd_putc, "\f");
   delay_ms(200);
   for(int i = 1; i <= 16; i++){
      lcd_gotoxy(6,1);
      printf(lcd_putc, "U de C");
      lcd_gotoxy(i,2);
      printf(lcd_putc, "Ing. Telematica");
      delay_ms(200);
      printf(lcd_putc, "\f");
   }
}

void menuDatos(){
    printf(lcd_putc, "\f");
    switch(apuntador){
            case 1:
                lcd_gotoxy(1,1);
                printf(lcd_putc,"->Temp: %lu", temperatura);
                lcd_gotoxy(1,2);
                printf(lcd_putc, "Humedad: %lu", humedad);
            break;
            case 2:
                lcd_gotoxy(1,1);
                printf(lcd_putc,"Temp: %lu", temperatura);
                lcd_gotoxy(1,2);
                printf(lcd_putc, "->Humedad: %lu", humedad);
            break;
            case 3:
                lcd_gotoxy(1,1);
                printf(lcd_putc,"Humedad: %lu", humedad);
                lcd_gotoxy(1,2);
                printf(lcd_putc, "->Hidrogeno: %lu", ppmHidrogeno);
            break;
            case 4:
                lcd_gotoxy(1,1);
                printf(lcd_putc,"Hidrogeno: %lu", ppmHidrogeno);
                lcd_gotoxy(1,2);
                printf(lcd_putc, "->Butano: %lu", ppmButano);
            break;
        }
}

void menu(){
    if(contadorAdelante == 0){
        menuDatos();
    }
    else if(contadorAdelante == 1){
        menuConfiguracion();
    }
    else if(contadorAdelante == 2){
        switch(apuntadorMenuConf){
            case 1:
                configuracionSerial();
            break;
            
            case 2:
                configuracionLCD();
            break;
            
            case 3:
                subMenuAlarmas();
            break;
        }
    }
    else if(contadorAdelante == 3){
        switch(apuntadorMenuAlarmas){
            case 1:
                configuracionTemperatura();
            break;
            
            case 2:
                configuracionHumedad();
            break;
            
            case 3:
                configuracionHidrogeno();
            break;
            
            case 4:
                configuracionButano();
                break;
        }
    }
    
}

void menuConfiguracion(){
   switch(apuntadorMenuConf){
         case 1:
            lcd_gotoxy(1,1);
			printf(lcd_putc,"->Conf. Serial");
            lcd_gotoxy(1,2);
		    printf(lcd_putc, "Conf. LCD");
            break;
               
         case 2:
            lcd_gotoxy(1,1);
            printf(lcd_putc,"Conf. Serial");
			lcd_gotoxy(1,2);
            printf(lcd_putc, "->Conf. LCD");
            break;
            
         case 3: 
            lcd_gotoxy(1,1);
			printf(lcd_putc,"Conf. LCD");
			lcd_gotoxy(1,2);
			printf(lcd_putc, "->Conf. Alarmas");
            break;
   }    
}

void subMenuAlarmas(){
    switch(apuntadorMenuAlarmas){
         case 1:
            lcd_gotoxy(1,1);
			printf(lcd_putc,"->Alarma Temp.");
            lcd_gotoxy(1,2);
		    printf(lcd_putc, "Alarma Humed.");
            break;
               
         case 2:
            lcd_gotoxy(1,1);
            printf(lcd_putc,"Alarma Temp.");
			lcd_gotoxy(1,2);
            printf(lcd_putc, "->Alarma Humed.");
            break;
            
         case 3: 
            lcd_gotoxy(1,1);
			printf(lcd_putc,"Alarma Humed.");
			lcd_gotoxy(1,2);
			printf(lcd_putc, "->Alarma Hidrog.");
            break;
            
         case 4: 
            lcd_gotoxy(1,1);
			printf(lcd_putc,"Alarma Hidrog.");
			lcd_gotoxy(1,2);
			printf(lcd_putc, "->Alarma Butano");
            break;
   }
}

void configuracionSerial(){
    lcd_gotoxy(4,1);
    printf(lcd_putc, "Conf. Serial");
    lcd_gotoxy(1,2);
    printf(lcd_putc, "Tiempo %i", aux);
}

void configuracionLCD(){
    lcd_gotoxy(4,1);
    printf(lcd_putc, "Conf. LCD");
    lcd_gotoxy(1,2);
    printf(lcd_putc, "Tiempo %i", aux);
}

void configuracionTemperatura(){
    lcd_gotoxy(1,1);
    printf(lcd_putc, "Alarma Temperatura");
    lcd_gotoxy(3,2);
    printf(lcd_putc, "Tiempo %i", aux);
}

void configuracionHumedad(){
    lcd_gotoxy(1,1);
    printf(lcd_putc, "Alarma Humedad");
    lcd_gotoxy(3,2);
    printf(lcd_putc, "Tiempo %i", aux);
}

void configuracionHidrogeno(){
    lcd_gotoxy(1,1);
    printf(lcd_putc, "Alarma Hidrog.");
    lcd_gotoxy(3,2);
    printf(lcd_putc, "Tiempo %i", aux);
}

void configuracionButano(){
    lcd_gotoxy(1,1);
    printf(lcd_putc, "Alarma Butano");
    lcd_gotoxy(3,2);
    printf(lcd_putc, "Tiempo %i", aux);
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
}

void saludoSerial(){
    printf("            Bienvenido          \r");
    printf("--------------------------------\r\r");
    printf("            Iniciando           \r");    
    for(int i = 0; i < 4; i++){
        printf("               ...              \r");
        delay_ms(1000);
    }
    printf("\r\r");
}

void menuSerial(){
    printf("--------------------------------\r");
    printf("|    Universidad de Colima     |\r");
    printf("|   Ingenieria en Telematica   |\r");
    printf("--------------------------------\r");
    printf("    Temperatura (°C): %ld\r", temperatura);
    printf("    Humedad     (HR): %ld\r", humedad);
    printf("    Hidrogeno  (ppm): %ld\r", ppmHidrogeno);
    printf("    Butano     (ppm): %ld\r", ppmButano);
    printf("\r\r\r");
}

void mensajeOK(){
    printf(lcd_putc,"\f");
    lcd_gotoxy(3,1);
    printf(lcd_putc,"Configuracion");
    lcd_gotoxy(4,2);
    printf(lcd_putc,"exitosa!");
    delay_ms(700);
    printf(lcd_putc,"\f");
}

void refreshSerial_LCD(){
    if(refreshSerial == initSerial){
        menuSerial();
        refreshSerial= 0;
    }
    else if(refreshLCD == initLCD){
        if(contadorAdelante == 0){
            menuDatos();
        }
        refreshLCD = 0;
    }
}
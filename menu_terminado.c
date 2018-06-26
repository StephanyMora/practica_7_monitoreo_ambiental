#define uC18f4620 
#include <18F4620.h>
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

//BAANDERAS
int8 banderaMenu = 0, banderaSaludo = 0, banderaCambioEstado = 0;
int apuntador = 1, apuntadorMenuConf = 1, apuntadorMenuAlarmas = 1;
int8 banderaSubMenuAlarma = 0, banderaMenuDatos = 0;
int8 contadorAdelante = 0;

int initSerial = 10, aux = 0, initLCD = 10;
int alarmaTemperatura = 10, alarmaHumedad = 20, alarmaHidrogeno = 100, alarmaButano = 100;

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
   enable_interrupts(INT_RB);
   enable_interrupts(GLOBAL);
   set_tris_a(0xFF);
   set_tris_b(0xF0);
   set_tris_d(0x00);   
   
   lcd_init();
   saludo();
   while(1){
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
                            aux++;
                            if(aux >= 500){
                                aux = 500;
                            }
                            break;
                        case 2:
                            aux++;
                            if(aux >= 200){
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
                            aux++;
                            if(aux >= 50){
                                aux = 50;
                            }
                            break;
                        case 2:
                            aux++;
                            if(aux >= 95){
                                aux = 95;
                            }
                            break;
                        case 3:
                            aux++;
                            if(aux >= 1000){
                                aux = 1000;
                            }
                            break;
                        case 4:
                            aux++;
                            if(aux >= 1000){
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
                            aux--;
                            if(aux <= 10){
                                aux = 10;
                            }
                            break;
                        case 2:
                            aux--;
                            if(aux <= 10){
                                aux = 10;
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
                            aux--;
                            if(aux <= 1){
                                aux = 1;
                            }
                            break;
                        case 2:
                            aux--;
                            if(aux <= 20){
                                aux = 20;
                            }
                            break;
                        case 3:
                            aux--;
                            if(aux <= 100){
                                aux = 100;
                            }
                            break;
                        case 4:
                            aux--;
                            if(aux <= 100){
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
                            alarmaTemperatura = aux;
                            mensajeOK();
                            contadorAdelante = 2;
                            aux = 0;
                            break;
                        case 2:
                            alarmaHumedad = aux;
                            mensajeOK();
                            contadorAdelante = 2;
                            aux = 0;
                            break;
                        case 3:
                            alarmaHidrogeno = aux;
                            mensajeOK();
                            contadorAdelante = 2;
                            aux = 0;
                            break;
                        case 4:
                            alarmaButano = aux;
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
   delay_ms(500);
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
    switch(apuntador){
            case 1:
                lcd_gotoxy(1,1);
                printf(lcd_putc,"->Temperatura: ");
                lcd_gotoxy(1,2);
                printf(lcd_putc, "Humedad: ");
            break;
            case 2:
                lcd_gotoxy(1,1);
                printf(lcd_putc,"Temperatura:");
                lcd_gotoxy(1,2);
                printf(lcd_putc, "->Humedad:");
            break;
            case 3:
                lcd_gotoxy(1,1);
                printf(lcd_putc,"Humedad:");
                lcd_gotoxy(1,2);
                printf(lcd_putc, "->Hidrogeno:");
            break;
            case 4:
                lcd_gotoxy(1,1);
                printf(lcd_putc,"Hidrogeno:");
                lcd_gotoxy(1,2);
                printf(lcd_putc, "->Butano:");
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

void mensajeOK(){
    printf(lcd_putc,"\f");
    lcd_gotoxy(3,1);
    printf(lcd_putc,"Configuracion");
    lcd_gotoxy(4,2);
    printf(lcd_putc,"exitosa!");
    delay_ms(700);
    printf(lcd_putc,"\f");
}
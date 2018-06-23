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


void saludo();
void menuDatos();
void menuConfiguracion();
void subMenuAlarma();


int8 seleccionPrimerMenu = 1;
int8 seleccionSubMenu = 1;
int8 seleccionMenuDatos = 1;
int8 banderaArriba = 0, banderaAbajo = 0, banderaAdelante = 0, banderaAtras = 0;
int8 banderaSubMenuAlarma = 0, banderaMenuDatos = 1;
int8 valorSerial = 100, valorLCD = 100, alarmaTemperatura = 1, alarmaHumedad = 20;
int8 alarmaHidrogeno = 100, alarmaButano = 100;
int8 aux = 0;
int8 contadorAdelante = 0, contadorArriba = 0;

#int_RB
void isr_RB(){
   if(input(ADELANTE)){
      banderaAdelante = 1;	
   }	
   if(input(ABAJO)){
   	   banderaAbajo = 1;
   }
   if(input(ARRIBA)){
   	   banderaArriba = 1;
   }
   if(input(ATRAS)){
   	   banderaAtras = 1;
   }
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

int esArriba(){
	if(banderaArriba == 1){
		banderaArriba = 0;
		if(banderaMenuDatos == 1){
			seleccionMenuDatos--;
			if(seleccionMenuDatos == 1){
				seleccionMenuDatos = 4;
			}
		}
		else if(contadorAdelante == 0){
			seleccionPrimerMenu--;
			if(seleccionPrimerMenu == 1){
				seleccionPrimerMenu = 4;
			}
		}
		else if(contadorAdelante == 1){
			switch(seleccionPrimerMenu){
				case 1:
					aux++;
					if(aux > 50){
						aux = 1;
					}
                    break;
				case 2:
					aux++;
					if(aux > 20){
						aux = 1;
					}
                    break;
            }   
		}
		else if(contadorAdelante == 1 && banderaSubMenuAlarma == 1){
			seleccionSubMenu--;
			if(seleccionSubMenu == 1){
				seleccionSubMenu = 4;
			}
		}
		else if(contadorAdelante == 2 && banderaSubMenuAlarma == 1){
			switch(seleccionSubMenu){
				case 1:
					aux++;
					if(aux > 50){
						aux = 1;
					}
                    break;
				case 2: 
					aux++;
					if(aux > 95){
						aux = 20;
					}
                    break;
				case 3:
					aux++;
					if(aux > 1000){
						aux = 100;
					}
                    break;
				case 4:
					aux++;
					if(aux > 1000){
						aux = 100;
					}
                    break;
			}
		}    
    }
    return  contadorArriba--;
}

int esAbajo(){
	if(banderaAbajo == 1){
		banderaAbajo = 0;
		if(banderaMenuDatos == 1){
			seleccionMenuDatos++;
			if(seleccionMenuDatos == 4){
				seleccionMenuDatos = 1;
			}
		}
		else if(contadorAdelante == 0){
			seleccionPrimerMenu++;
			if(seleccionPrimerMenu == 4){
				seleccionPrimerMenu = 1;
			}
		}
		else if(contadorAdelante == 1){
			switch(seleccionPrimerMenu){
				case 1:
					aux--;
					if(aux < 1){
						aux = 50;
					}
				case 2:
					aux--;
					if(aux < 1){
						aux = 20;
					}
				}
			}
		}
		else if(contadorAdelante == 1 && banderaSubMenuAlarma == 1){
			seleccionSubMenu++;
			if(seleccionSubMenu == 4){
				seleccionSubMenu = 1;
			}
		}
		else if(contadorAdelante == 2 && banderaSubMenuAlarma == 1){
			switch(seleccionSubMenu){
				case 1:
					aux--;
					if(aux < 1){
						aux = 50;
					}
                    break;
				case 2: 
					aux--;
					if(aux < 20){
						aux = 95;
					}
                    break;
				case 3:
					aux--;
					if(aux < 100){
						aux = 1000;
					}
                    break;
				case 4:
					aux--;
					if(aux > 100){
						aux = 1000;
					}
                    break;
			}
    }
    return contadorArriba++;
}

int esAdelante(){
	if(banderaAdelante == 1){
        banderaMenuDatos = 0;
		banderaAdelante = 0;
		if(contadorAdelante == 0){
			menuConfiguracion();
		}
		else if(contadorAdelante == 1){
			switch(seleccionPrimerMenu){
				case 1:
					printf(lcd_putc, "Ing. Telematica");
                    break;
				case 2:
					printf(lcd_putc, "Ing. Telematica");
                    break;
				case 3:
					printf(lcd_putc, "Ing. Telematica");
                    break;
			}
		} 	
		else if(contadorAdelante == 2 && banderaSubMenuAlarma == 1){
			switch(seleccionSubMenu){
				case 1:
					printf(lcd_putc, "Ing. Telematica");
                    break;
				case 2:
					printf(lcd_putc, "Ing. Telematica");
                    break;
				case 3:
					printf(lcd_putc, "Ing. Telematica");
                    break;
				case 4:
					printf(lcd_putc, "Ing. Telematica");
                    break;
			}
		}
		else if(contadorAdelante == 3){
			switch(seleccionSubMenu){
				case 1:
					alarmaTemperatura = aux;
                    break;
				case 2: 
					alarmaHumedad = aux;
                    break;
				case 3:
					alarmaHidrogeno = aux;
                    break;
				case 4:
					alarmaButano = aux;
                    break;
			}
		}
		else if(contadorAdelante == 2 && banderaSubMenuAlarma == 0){
			switch(seleccionPrimerMenu){
				case 1:
					valorSerial = aux;
                    break;
				case 2: 
					valorLCD = aux;
                    break;
			}
		}
    }
    return contadorAdelante++;
}

int esAtras(){
	if(banderaAtras == 1){
		banderaAtras = 0;
		if(contadorAdelante == 1){
			banderaMenuDatos = 1;
		} 
		else if(contadorAdelante == 2){
			menuConfiguracion();
		}
		else if(contadorAdelante == 3){
			subMenuAlarma();
		}	
    }
    return contadorAdelante--;
}

void menuDatos(){
    if(banderaMenuDatos == 1){
        if(seleccionMenuDatos == 1){
            lcd_gotoxy(1,1);
        	printf(lcd_putc,"->Temperatura:");
        	lcd_gotoxy(1,2);
        	printf(lcd_putc, "Humedad:");		
        }
        else if(seleccionMenuDatos == 2){
            lcd_gotoxy(1,1);
            printf(lcd_putc,"Temperatura:");
            lcd_gotoxy(1,2);
        	printf(lcd_putc, "->Humedad:");
        }
        else if(seleccionMenuDatos == 3){
        	lcd_gotoxy(1,1);
        	printf(lcd_putc,"Humedad:");
        	lcd_gotoxy(1,2);
        	printf(lcd_putc, "->Hidrogeno:");
        }
        else if(seleccionMenuDatos == 4){
        	lcd_gotoxy(1,1);
        	printf(lcd_putc,"Hidrogeno:");
        	lcd_gotoxy(1,2);
            printf(lcd_putc, "->Butano:");
        }
    }	
    banderaMenuDatos = 0;
}

void menuConfiguracion(){
	if(contadorAdelante == 0 && banderaMenuDatos == 0){
		if(seleccionPrimerMenu == 1){
			lcd_gotoxy(1,1);
			printf(lcd_putc,"->Conf. Serial");
			lcd_gotoxy(1,2);
			printf(lcd_putc, "Conf. LCD");
		}
		if(seleccionPrimerMenu == 2){
			lcd_gotoxy(1,1);
			printf(lcd_putc,"Conf. Serial");
			lcd_gotoxy(1,2);
			printf(lcd_putc, "->Conf. LCD");
		}
		if(seleccionPrimerMenu == 3){
			lcd_gotoxy(1,1);
			printf(lcd_putc,"Conf. LCD");
			lcd_gotoxy(1,2);
			printf(lcd_putc, "->Conf. Alarmas");
		}
	}
}

void subMenuAlarma(){
	if(seleccionSubMenu == 1){
		lcd_gotoxy(1,1);
		printf(lcd_putc,"->Alarma Temperatura");
		lcd_gotoxy(1,2);
        printf(lcd_putc, "Alarma Humedad");
		if(seleccionSubMenu == 2){
			lcd_gotoxy(1,1);
			printf(lcd_putc,"Alarma Temperatura");
			lcd_gotoxy(1,2);
			printf(lcd_putc, "->Alarma Humedad");
		}
		if(seleccionSubMenu == 3){
			lcd_gotoxy(1,1);
			printf(lcd_putc,"Alarma Humedad");
			lcd_gotoxy(1,2);
			printf(lcd_putc, "->Alarma Hidrogeno");
		}
		if(seleccionSubMenu == 4){
			lcd_gotoxy(1,1);
			printf(lcd_putc,"Alarma Hidrogeno");
			lcd_gotoxy(1,2);
			printf(lcd_putc, "->Alarma Butano");
		}
    }
}   

void main (void){
   setup_oscillator(OSC_16MHZ);
   enable_interrupts(INT_RB);
   enable_interrupts(GLOBAL);
   set_tris_a(0xFF);
   set_tris_b(0xF0);
   set_tris_d(0x00);   
   lcd_init();
   saludo();
   while(1){
       menuDatos();
       esAdelante();
       esAtras();
       esArriba();
       esAbajo();
   }    
} 
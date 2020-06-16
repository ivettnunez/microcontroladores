/*
 * sensor.c
 *
 * Created: 6/11/2020 8:29:24 PM
 * Authors: Marco Mancha - Ivett Nunez - Mayka Arizbeth    
 */

//Configure display
#asm
    .equ __lcd_port=0x08
    .equ __lcd_EN=1
    .equ __lcd_RS=0
    .equ __lcd_D4=2
    .equ __lcd_D5=3
    .equ __lcd_D6=4
    .equ __lcd_D7=5
#endasm


//Configure keypad
#asm
    .equ __keypad_port=0x0B
    .equ __keypad_R1=0
    .equ __keypad_R2=1
    .equ __keypad_R3=2
    .equ __keypad_R4=3
    .equ __keypad_C1=6
    .equ __keypad_C2=5
    .equ __keypad_C3=4
#endasm


#include <stdio.h>
#include <io.h>
#include <delay.h>
#include <display.h>
#include <eeprom.h>

void SetupKeypad()
{
    #asm   
        SBI __keypad_port-1,__keypad_R1 ;Renglón 1 de salida 
        SBI __keypad_port-1,__keypad_R2 ;Renglón 2 de salida
        SBI __keypad_port-1,__keypad_R3 ;Renglón 3 de salida
        SBI __keypad_port-1,__keypad_R4 ;Renglón 4 de salida  
        SBI __keypad_port,__keypad_C1   ;Pull-up en Columna 1 
        SBI __keypad_port,__keypad_C2   ;Pull-up en Columna 2
        SBI __keypad_port,__keypad_C3   ;Pull-up en Columna 3
    #endasm
}

#pragma warn-
char LeeTeclado()
{
    #asm    
    Inicio:
        SBIS __keypad_port-2,__keypad_C1 
        RJMP BarridoC1  
        SBIS __keypad_port-2,__keypad_C2 
        RJMP BarridoC2
        SBIS __keypad_port-2,__keypad_C3 
        RJMP BarridoC3
        CLR R30    ;no hay tecla presionada
        RJMP Fin
     BarridoC1:
        LDI R30,'1'
        SBI __keypad_port,__keypad_R2   ;R2=1  
        SBI __keypad_port,__keypad_R3   ;R3=1
        SBI __keypad_port,__keypad_R4   ;R4=1  
        NOP
        SBIS __keypad_port-2,__keypad_C1
        RJMP Fin    
        LDI R30,'4'
        SBI __keypad_port,__keypad_R1   ;R1=1  
        CBI __keypad_port,__keypad_R2   ;R2=0
        NOP
        SBIS __keypad_port-2,__keypad_C1
        RJMP Fin   
        LDI R30,'7'
        SBI __keypad_port,__keypad_R2   ;R2=1  
        CBI __keypad_port,__keypad_R3   ;R3=0    
        NOP
        SBIS __keypad_port-2,__keypad_C1
        
        RJMP Fin  
        LDI R30,'*'
        SBI __keypad_port,__keypad_R3   ;R3=1  
        CBI __keypad_port,__keypad_R4   ;R4=0
        NOP
        SBIS __keypad_port-2,__keypad_C1
        RJMP Fin     
        CLR R30
        RJMP Fin 
     BarridoC2:
        LDI R30,'2'
        SBI __keypad_port,__keypad_R2   ;R2=1  
        SBI __keypad_port,__keypad_R3   ;R3=1
        SBI __keypad_port,__keypad_R4   ;R4=1 
        NOP
        SBIS __keypad_port-2,__keypad_C2
        RJMP Fin    
        LDI R30,'5'
        SBI __keypad_port,__keypad_R1   ;R1=1  
        CBI __keypad_port,__keypad_R2   ;R2=0
        NOP
        SBIS __keypad_port-2,__keypad_C2
        RJMP Fin   
        LDI R30,'8'
        SBI __keypad_port,__keypad_R2   ;R2=1  
        CBI __keypad_port,__keypad_R3   ;R3=0
        NOP
        SBIS __keypad_port-2,__keypad_C2
        RJMP Fin  
        LDI R30,'0'
        SBI __keypad_port,__keypad_R3   ;R3=1  
        CBI __keypad_port,__keypad_R4   ;R4=0 
        NOP
        SBIS __keypad_port-2,__keypad_C2
        RJMP Fin     
        CLR R30   
        
     BarridoC3:
        LDI R30,'3'
        SBI __keypad_port,__keypad_R2   ;R2=1  
        SBI __keypad_port,__keypad_R3   ;R3=1
        SBI __keypad_port,__keypad_R4   ;R4=1
        NOP
        SBIS __keypad_port-2,__keypad_C3
        RJMP Fin    
        LDI R30,'6'
        SBI __keypad_port,__keypad_R1   ;R1=1  
        CBI __keypad_port,__keypad_R2   ;R2=0
        NOP
        SBIS __keypad_port-2,__keypad_C3
        RJMP Fin   
        LDI R30,'9'
        SBI __keypad_port,__keypad_R2   ;R2=1  
        CBI __keypad_port,__keypad_R3   ;R3=0
        NOP
        SBIS __keypad_port-2,__keypad_C3
        RJMP Fin  
        LDI R30,'#'
        SBI __keypad_port,__keypad_R3   ;R3=1  
        CBI __keypad_port,__keypad_R4   ;R4=0 
        NOP
        SBIS __keypad_port-2,__keypad_C3
        RJMP Fin     
        CLR R30
     Fin:
        CBI __keypad_port,__keypad_R1
        CBI __keypad_port,__keypad_R2
        CBI __keypad_port,__keypad_R3
        CBI __keypad_port,__keypad_R4   
              
    #endasm  
}
#pragma warn+

typedef unsigned short uint8_t;

unsigned int distancia;
unsigned int estado;
char tecla;
unsigned int i = 0;
unsigned int estatus = 1;
// 1 - Alarma Activada
// 2 - Alarma Desactivada
// 3 - Cambiar clave

char read_key[4];
char write_key[4];
uint8_t setting;

void trigger_s()
{
    PORTB.0 = 1;  //Enviar señal
    delay_us(10); //Despues del delay, trigger envia pulsos de 40KHz y echo comienza a medir
    PORTB.0 = 0;  //Apagar señal
}

void echo_s()
{
    distancia = 0;
    while(PINB.1 == 0); //Esperar activacion de echo
    
    do
      {
        delay_us(54);
        #asm("NOP")
        distancia++; //Sumar un cm de distancia por cada 54 us de echo
      }
    while ((PINB.1 == 1) && (distancia<400));        
}

void reiniciaClave()
{
   unsigned char i;
   for (i=0;i<4;i++)
   {  
      read_key[i]='0';
   }    
}

int comparaClave(){
   unsigned char i;
   unsigned int resultado = 1;
   for (i=0;i<4;i++)
   { 
     if(read_key[i] != write_key[i]) 
        resultado = 0;
   }  
   return resultado;
}

void esperaTeclaNoImprime()
{
    //Espera a presionar tecla
    do{             
        tecla=LeeTeclado(); 
    }while(tecla==0);  
    write_key[i] = tecla;   
    //Espera a soltar tecla
    do{                    
        tecla=LeeTeclado();  
    }while(tecla!=0); 
}

void esperaTeclaImprime()
{
    //Espera a presionar tecla
    do{                  
        tecla=LeeTeclado();
         
    }while(tecla==0);  
    write_key[i] = tecla;
    CharLCD(tecla);
    //Espera a soltar tecla
    do{      
        tecla=LeeTeclado();  
    }while(tecla!=0);         
}


void esperaTeclaImprimeConSensor()
{
    //Espera a presionar tecla
    do{                  
        tecla=LeeTeclado();
        //Activar sensor
        trigger_s();
        echo_s();
        //Si detecta algo a 20 cm o menos, activa sonido y led
        if(distancia <= 20 || estado == 1)
        {
            PORTB.2 = 1;
            PORTD.7 = 1;
            estado = 1;                        
        }      
         
    }while(tecla==0);  
    write_key[i] = tecla;
    CharLCD(tecla);
    //Espera a soltar tecla
    do{      
        tecla=LeeTeclado();  
    }while(tecla!=0);         
}

void main(void)
{    
    // Micro a 8MHz    
    CLKPR=0x80;
    CLKPR=0;
    
    SetupLCD();
    SetupKeypad();
    
    PORTB.1 = 1; //PC1 entrada pull-up para echo
    DDRB.0 = 1;  //PC0 salida para trigger
    DDRD.7 = 1;  //PD7 salida para bocina
    DDRB.2 = 1;  //PB2 salida para led
                                             
    // Lectura inicial eeprom y configuracion si aplica
    eeprom_read_block((const void*)read_key , (eeprom void*)45, 4);  
    setting = eeprom_read_byte((uint8_t*)45);  
    if(setting == 0xff) {
        reiniciaClave();   
    }
    
    while (1)
        {
                        
            switch(estatus)
            {      
                // Estatus inicial, checar sensor
                case 1:                                            
                    MoveCursor(1,0);
                    StringLCD("ALARMA ACTIVA"); 
                    MoveCursor(0,1);    
                    StringLCD("CLAVE: ");
                    MoveCursor(7+i,1);     
                    
                    esperaTeclaImprimeConSensor();                      
                    i++;
                    
                    //Si ya ingreso clave de 4 digitos
                    if(i == 4){
                        if(comparaClave() == 1){  
                            estado = 0;
                            estatus = 2;
                        }     
                        EraseLCD();
                        i = 0;
                    }
                
                    break; 
                
                //Estatus de alarma desactivada, se puede volver a activar la alarma
                // o se puede cambiar la clave
                case 2:
                
                    //Desactivar alarma porque ingreso contraseña correcta
                    PORTB.2 = 0;
                    PORTD.7 = 0;
                    estado = 0;
                        
                    MoveCursor(5,0);
                    StringLCD("ALARMA"); 
                    MoveCursor(2,1);
                    StringLCD("DESACTIVADA");   
                            
                    esperaTeclaNoImprime();
                            
                    if(write_key[i] == '*'){ 
                        // Estatus para activar alarma 
                        i = 0;
                        estatus = 1;
                        EraseLCD();
                        //Delay para que el usuario corra
                        MoveCursor(0,0);
                        StringLCD("ALARMA ACTIVADA");
                        delay_ms(3000);
                        EraseLCD();
                        
                    }else if(write_key[i] == '#'){     
                        // Estatus para cambiar clave
                        i = 0;
                        estatus = 3;   
                        EraseLCD();
                    }
                    break;
                        
                // Estatus para cambiar clave
                case 3:   
                    MoveCursor(1,0);
                    StringLCD("CAMBIAR CLAVE");    
                    MoveCursor(0,1);    
                    StringLCD("CLAVE: ");
                    MoveCursor(7+i,1);     
                    
                    esperaTeclaImprime();  
                   
                    i++;
                    
                    // Si ya ingreso clave de 4 digitos
                    if(i == 4){     
                        // Escribir y leer valor de eeprom y cambiar a estatus
                        // de alarma activa
                        eeprom_write_block(write_key,(eeprom void*)45,sizeof(write_key));    
                        eeprom_read_block(read_key,(eeprom void*)45,sizeof(read_key)); 
                        estatus = 1;       
                        i = 0;   
                        EraseLCD();
                        //Delay para que el usuario corra    
                        delay_ms(3000);
                    }
                    break; 
            }
                      

        }
}

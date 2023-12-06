   #include <18F4550.h>
   #device ADC=10
   #include <usb_bootloader.h>
   #fuses HS,NOWDT,NOPROTECT,NOLVP
   #define cuenta 15536
   #include <time.h>
   #include <stdlib.h>
   #use Delay(Clock=20M)
   
   #use rs232(baud=9600,xmit=PIN_C6,rcv=PIN_C7)
   #include <lcd.c>
   //------------- Declaración de funciones -------------------
   void rota(int ciclos);
   void semaforo(int ciclosdos);
   void menuderotas(void);
   void tempvolt(void);
   void invernadero(void);
   void controlmanualinv(void);
   //--------------ROTAS INT RB------------------------------
   //------------------Declaración de variables-------------
   int *ptr,aux=0,contador=0,flag=0,seg=-1,min=-1,hr=-1;
   char opcion='7';
   char tecla='0';
   //-----------------Interrupciones------------------------
   
   //-----------------Interrupcion teclado------------------
   #INT_RB        //Interrupt por el Puerto B
   void intext_isr() {
      
          OUTPUT_LOW(PIN_B2);  //Fila1
          OUTPUT_HIGH(PIN_B3);  //Fila2
          OUTPUT_HIGH(PIN_B4);  //fila3
         
         if(INPUT(PIN_B5)== 0){
             tecla= '1';
         }
         if(INPUT(PIN_B6)== 0){
             tecla= '2';
         }
         if(INPUT(PIN_B7)== 0){
             tecla= '3';
         }
   
         OUTPUT_HIGH(PIN_B2);  //Fila1
         OUTPUT_LOW(PIN_B3);  //Fila2
         OUTPUT_HIGH(PIN_B4);  //fila3
         if(INPUT(PIN_B5)== 0){
             tecla= '4';
         }
         if(INPUT(PIN_B6)== 0){
             tecla= '5';
         }
         if(INPUT(PIN_B7)== 0){
             tecla= '6';
         }
   
         OUTPUT_HIGH(PIN_B2);  //Fila1
         OUTPUT_HIGH(PIN_B3);  //Fila2
         OUTPUT_LOW(PIN_B4);  //fila3
         if(INPUT(PIN_B5)== 0){
             tecla= '7';
         }
         if(INPUT(PIN_B6)== 0){
             tecla= '8';
         }
         if(INPUT(PIN_B7)== 0){
             tecla= '9';
         }
   
     printf("Tecla= %c\r\n",tecla);
     delay_ms( 1000);  
     aux= INPUT_b(); // Limpiamos la bandera RBIF
      OUTPUT_LOW(PIN_B2);  //Fila1
      OUTPUT_LOW(PIN_B3);  //Fila2
      OUTPUT_LOW(PIN_B4);  //fila3
   //opcion=getch();
   
   }
   
   //-----------------INTERRUPCION RELOJ--------------------
   #INT_RTCC    //INT-Timer0                                                                
   void clock_isr() {
     set_timer0(cuenta);
     contador=contador+1;                        
     if(contador>4){ 
           contador=0;
           seg++;
           if(seg==60){
              seg=0;
              min++;
              if(min==60){
                 min=0;
                 hr++;
                 if(hr==24){
                    hr=0;
                 }
              }           
      }
         lcd_init();
         //delay_ms( 500 );
         lcd_gotoxy(2,2);
           printf(lcd_putc,"\fReloj %d :%d :%d",hr, min, seg);      
      }
   }
   //---------------------AJUSTE RELOJ----------------------
   #INT_EXT
   void intext_isr2() {
    hr++;
    if(hr==24){
         hr=0;
      }
   }
   #INT_EXT1        
   void intext_isr3() {
    min++;
    if(min>60){
         min=0;
           hr++;
      }
   }
   //-------------------------------------------------------
   void main(void) {
   //lcd_init();
   set_tris_d(0x00);
   set_tris_b(0x0F0);
   set_tris_c(0x0F0);       
   PORT_B_PULLUPS(TRUE);
   ENABLE_INTERRUPTS(GLOBAL | INT_RB);    
   SETUP_TIMER_0(RTCC_INTERNAL | RTCC_DIV_16);
   //ENABLE_INTERRUPTS(GLOBAL | INT_TIMER0 );
   SET_TIMER0(cuenta);
   EXT_INT_EDGE(H_TO_L );
   ENABLE_INTERRUPTS(INT_EXT);
   ENABLE_INTERRUPTS(INT_EXT1);      
   OUTPUT_LOW(PIN_B2);  //Fila1
   OUTPUT_LOW(PIN_B3);  //Fila2
   OUTPUT_LOW(PIN_B4);  //fila3
   //--------TEMP/VOLT---------------------
   setup_adc(ADC_CLOCK_DIV_8);
   setup_adc_ports(AN0_TO_AN1);  //Configura los puertosa AN0 Y AN1 COMO A/D
   setup_adc(ADC_CLOCK_INTERNAL);
   delay_ms(100);
   printf("\r\nINSTRUMENTRACION.\r");
   printf("\r\n1-Opcion 1 (Invernadero)\r");
   printf("\r\n2.-Opcion 2 (Control manual invernadero)\r");
   printf("\r\n3.-Opcion 3 (Reloj)");
   printf("\r\nElige la opcion que decees "); 
      while(1){    
         if(kbhit()!=0){
         printf("\r\nINSTRUMENTACION\r");
         printf("\r\n1.-Opcion 1 (Invernadero)\r");
         printf("\r\n2.-Opcion 2 (Control manual invernadero)\r");
         printf("\r\n3.-Opcion 3 (Reloj)");
         printf("\r\nElige la opcion que decees ");
   //      delay_ms(1000);
         tecla=getch();    
          }
          switch(tecla){
         case'1': //---------------------------
         DISABLE_INTERRUPTS(INT_TIMER0 );
         ptr=&invernadero;
         invernadero();
         delay_ms( 200 );
         break;
         case'2': //------------------------------
         DISABLE_INTERRUPTS(INT_TIMER0 );
         ptr=&controlmanualinv;
         controlmanualinv();
         delay_ms( 200 );
         break;
         case'3': //------------------------
         //lcd_init();
         ENABLE_INTERRUPTS(INT_TIMER0 );delay_ms( 500 );
         break;
         }
      }
   }
   //----------------------Invernadero automatico-------------------
   void invernadero(){
   long int longdato;
   float x; //Valor de temperatura
   float y; //Valor de humedad
   float valor;
   delay_ms(100);
   lcd_init();
          output_low(PIN_A3); //VENTILADOR
         output_low(PIN_A4); //CALEFACTOR
            output_low(PIN_A5); //BOMBA
         //delay_ms(500);
          set_adc_channel(0);
         delay_ms(10);             
          longdato= read_adc();
         delay_us(20);       
          x= ((((float)longdato*5.0)/1023))*100-4;
          //x=x-2;
         //delay_ms(200);
         printf("Grados= %.1f\r",x);
          lcd_gotoxy(1,1);
         printf(lcd_putc,"Grados= %.1f\r",x);
         set_adc_channel(1);
         delay_ms(10);
         valor=read_adc();
         delay_us(20);
         y=read_adc();
          delay_us(20);
         y=(-0.17*valor)+171.4;
         y=y-15;
         lcd_gotoxy(1,2);
         printf("Humedad= %.1f\r",y);
         printf(lcd_putc,"Humedad= %.1f\r",y);
         delay_ms(500);
          while(y<50){
         output_high(PIN_A5);//BOMBA
          output_low(PIN_A3);//VENTILADOR 
          output_low(PIN_A4);//CALEFACCION
         //output_low(PIN_C2);//BOMBA
          set_adc_channel(0);
         delay_ms(10);             
          longdato= read_adc();
         delay_us(20);       
          x= ((((float)longdato*5.0)/1023))*100;  
         //delay_ms(200);
         printf("Grados= %.1f\r",x);
          lcd_gotoxy(1,1);
         printf(lcd_putc,"Grados= %.1f\r",x);
         set_adc_channel(1);
         delay_ms(10);
         valor=read_adc();
         delay_us(20);
         y=read_adc();
          delay_us(20);
         y=(-0.17*valor)+171.4;
         y=y-15;
         lcd_gotoxy(1,2);
         printf("Humedad= %.1f\r",y);
         printf(lcd_putc,"Humedad= %.1f\r",y);
         delay_ms(500);   
         }
         while(x>26){
         output_high(PIN_A3);//VENTILADOR 
          output_low(PIN_A4);//CALEFACCION
         output_low(PIN_A5);//BOMBA
          set_adc_channel(0);
         delay_ms(10);             
          longdato= read_adc();
         delay_us(20);       
          x= ((((float)longdato*5.0)/1023))*100;  
         //delay_ms(200);
         printf("Grados= %.1f\r",x);
          lcd_gotoxy(1,1);
         printf(lcd_putc,"Grados= %.1f\r",x);
         set_adc_channel(1);
         delay_ms(10);
         valor=read_adc();
         delay_us(20);
         y=read_adc();
          delay_us(20);
         y=(-0.17*valor)+171.4;
         y=y-15;
         lcd_gotoxy(1,2);
         printf("Humedad= %.1f\r",y);
         printf(lcd_putc,"Humedad= %.1f\r",y);
         delay_ms(500);
         }
         while(x<16){
         output_high(PIN_A4);//CALEFACCION 
          output_low(PIN_A3);//VENTILADOR
         output_low(PIN_A5);//BOMBA
          set_adc_channel(0);
         delay_ms(10);             
          longdato= read_adc();
         delay_us(20);       
          x= ((((float)longdato*5.0)/1023))*100;  
         //delay_ms(200);
         printf("Grados= %.1f\r",x);
          lcd_gotoxy(1,1);
         printf(lcd_putc,"Grados= %.1f\r",x);
         set_adc_channel(1);
         delay_ms(10);
         valor=read_adc();
         delay_us(20);
         y=read_adc();
          delay_us(20);
         y=(-0.17*valor)+171.4;
         y=y-15;
         lcd_gotoxy(1,2);
         printf("Humedad= %.1f\r",y);
         printf(lcd_putc,"Humedad= %.1f\r",y);
         delay_ms(500);
   
         }
   }
   //----------------Invernadero manual
   void controlmanualinv(){
   while(1){    
      if(kbhit()!=0){
      printf("\n\rCONTROL MANUAL INVERNADERO");
      printf("\n\r 1) Ventilador");  
      printf("\n\r 2) Calefactor");
      printf("\n\r 3) Bomba");
      printf("\n\r 4) Ventilador/Bomba");
      printf("\n\r 5) Calefactor/Bomba");
      printf("\n\r 6) Apagar todo");
      printf("\n\r 8) Salir");
      printf("\n\n\r Que deseas activar? ");
       tecla=getch();    
          }
      switch(tecla){
      case'1':output_high(PIN_A3);//VENTILADOR
      output_low(PIN_A4);
      output_low(PIN_A5);
      break;
      case'2':output_high(PIN_A4);//CALEFACTOR
      output_low(PIN_A3);
      output_low(PIN_A5);
      break;
      case'3':output_high(PIN_A5);//BOMBA
      output_low(PIN_A3);
      output_low(PIN_A4);
      break;
      case'4':output_high(PIN_A3);output_high(PIN_A5);//VENTILADOR/BOMBA
      output_low(PIN_A5);
      break;
      case'5':output_high(PIN_A4);output_high(PIN_A5);//CALEFACTOR/BOMBA
      output_low(PIN_A3);
      break;
      case'6':output_low(PIN_A3);output_low(PIN_A4);output_low(PIN_A5);break;//APAGA TODO
      case'8':return;break;//SALIR DEL SUBMENU
      }
   }
   }

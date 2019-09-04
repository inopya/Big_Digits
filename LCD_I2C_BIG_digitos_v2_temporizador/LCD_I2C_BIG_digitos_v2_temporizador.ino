
/*
  #       _\|/_   A ver..., ¿que tenemos por aqui?
  #       (O-O)        
  # ---oOO-(_)-OOo---------------------------------
   
   
  ##########################################################
  # ****************************************************** #
  # *            DOMOTICA PARA AFICIONADOS               * #
  # *               BIG DIGITS LCD 16x2                  * #
  # *          Autor:  Eulogio López Cayuela             * #
  # *                                                    * #
  # *       Versión v2.0      Fecha: 10/09/2015          * #
  # ****************************************************** #
  ##########################################################
*/

#define __VERSION__ "BIG DIGITS LCD 16x2  v2.0"
#define __LCD_VERSION__ "BIG DIGITS v2.0"

/*

 ===== NOTAS DE LA VERSION =====
 
  1.- BIG DIGITS LCD 16x2  v1.0. Solo las funciones para la representacion de digitos grandes.
  
  2.- Implementado como un ejemplo de uso de la funcion para representar 
      digitos grandes en el LCD 16x2 y añadido parpadeo de los puntos del segundero.
      No contabiliza tiempo real. Se basa en un contador apoyado en la funcion millis()

    
    Este Sketch usa 5812 bytes, el (18%) del espacio de ROM
    Las variables Globales usan 516 bytes, el (25%)  del espacio de RAM
 
      
  CONEXIONES:

 =======================
  ARDUINO     LCD
 =======================

   A4   --->   SDA  
   A5   --->   SCL
   GND  --->   GND
   5v   --->   Vcc
   
*/ 



/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        IMPORTACION DE LIBRERIAS 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

#include <Wire.h>               //libreria para comunicaciones I2C
#include <LiquidCrystal_I2C.h>  // LiquidCrystal library
//#include <avr/pgmspace.h>       //Manejo de variables almacenadas en Memoria de Programa
                                  //sin uso


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        SECCION DE DECLARACION DE CONSTANTES  Y  VARIABLES GLOBALES
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//------------------------------------------------------
//algunas definiciones personales para mi comodidad al escribir codigo
//------------------------------------------------------
#define AND    &&
#define OR     ||
#define NOT     !
#define ANDbit  &
#define ORbit   |
#define XORbit  ^
#define NOTbit  ~
//Versiones actuales de Arduino ya contemplan 'and', 'or'...  pero arrastro la costumbre de cuando no era asi


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        SECCION DE DECLARACION DE CONSTANTES  Y  VARIABLES GLOBALES
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//------------------------------------------------------
//Otras definiciones para pines y variables
//------------------------------------------------------
#define PIN_LED_OnBoard       13   // Led on Board
#define LCD_AZUL_ADDR       0x27   // Direccion I2C de nuestro LCD color azul
#define LCD_VERDE_ADDR      0x3F   // Direccion I2C de nuestro LCD color verde

// defino los nuevos caracteres
//const PROGMEM 

/* piezas para construir los digitos Grandes */
byte bloques[5][8] = {{B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000},
                      {B11111, B11111, B00000, B00000, B00000, B00000, B00000, B00000},
                      {B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111},
                      {B11111, B11111, B00000, B00000, B00000, B00000, B11111, B11111},
                      {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}};

/* descripcion de piezas necesarias para construir cada digito */                      
byte numeros[10][6] = {{4,1,4,4,2,4},  //0
                       {1,4,0,2,4,2},  //1
                       {3,3,4,4,2,2},  //2
                       {1,3,4,2,2,4},  //3
                       {4,2,4,0,0,4},  //4
                       {4,3,3,2,2,4},  //5
                       {4,3,3,4,2,4},  //6
                       {1,1,4,0,4,0},  //7
                       {4,3,4,4,2,4},  //8
                       {4,3,4,2,2,4}}; //9 


float momento_actualizar_reloj = millis();
float momento_parpadeo = 0;
boolean FLAG_actualizar_reloj = false;
boolean FLAG_parpadeo_puntos = false;

/*
  "contador" almacena el valor inicial del temporizador , CERO o un numero positivo
  la bandera "FLAG_cuenta_Atras" indica como comporta dicho temporizador.
  > false = un reloj (tiempos crecientes)
  > true = una cuenta atras
*/

int contador = 120;                  //almacena la cuenta de nuestro 'falso' temporizador
boolean FLAG_cuenta_Atras = true;    //podemos activarla e iniciar el contador con un valor mayor que cero
                                     //para de esa forma tener una cuenta atras

#define INTERVALO_TEMPORIZADOR 1000  //1000 = 1 segundo, 100 = 1 decima de segundo


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//        SECCION DE DECLARACION OBJETOS
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//Creamos el objeto 'lcd' como una instancia del tipo "LiquidCrystal_I2C"
LiquidCrystal_I2C lcd(LCD_AZUL_ADDR, 16, 2); //libreria nueva




//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//         FUNCION DE CONFIGURACION
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 

void setup() 
{
  randomSeed(analogRead(0));
  Serial.begin(9600);  //Se inicia el puerto serie para depuracion
  Serial.println(F(__VERSION__));
  
  lcd.begin();    //Inicializar lcd  libreria nueva
  
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  
  
  //Mensaje inicial para demostrar que el LCD esta bien conectado y comunicado
  lcd.clear();         //Reset del display 
  lcd.backlight();     //Activamos la retroiluminacion
  
  lcd.createChar(0, bloques[0]);  
  lcd.createChar(1, bloques[1]);
  lcd.createChar(2, bloques[2]);
  lcd.createChar(3, bloques[3]);
  lcd.createChar(4, bloques[4]);

  lcd.clear();
  lcd.print(F(__LCD_VERSION__));
  delay(3500);
  lcd.clear();
}



//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 
//***************************************************************************************************
//  BUCLE PRINCIPAL DEL PROGRAMA   (SISTEMA VEGETATIVO)
//***************************************************************************************************
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 

void loop() 
{
  float ahora = millis();
  
  if(ahora >= momento_actualizar_reloj){
    momento_actualizar_reloj = ahora + INTERVALO_TEMPORIZADOR;
    if(FLAG_cuenta_Atras == true){
      contador--;
      if(contador<0){
        //decision a tomar cuando acabe la cuenta a tras
        lcd.clear();
        lcd.print(F("TIEMPO AGOTADO"));
        while(true); 
      }
    }
    else{
      contador++;
    }
    FLAG_actualizar_reloj = true;
  }
 
   
  /* actualizar la informacion del reloj/contyador/temporizador */
  if(FLAG_actualizar_reloj == true){
    int cifra = 0;
    cifra = contador%60; //obtengo los segundos   
    bigNumero(cifra%10, 13); //mostrar 1 cifra de los segundos
    bigNumero((cifra-(cifra%10))/10, 9);  //mostrar 2 cifra de los segundos
    cifra = contador-(cifra);//minutos completos (dados en segundos)
    bigNumero((cifra/60)%10, 5); //mostrar 1 cifra de los minutos 
    bigNumero(((cifra/60)-((cifra/60)%10))/10, 1);  //mostrar 2 cifra de los minutos    
    FLAG_actualizar_reloj = false;
  }

  /* actualizar el estado de los ":" separadores de minutos : segundos */
  if(ahora >= momento_parpadeo){
    momento_parpadeo = ahora + int(INTERVALO_TEMPORIZADOR/2.0);
    FLAG_parpadeo_puntos = NOT FLAG_parpadeo_puntos;
    print_dos_puntos(FLAG_parpadeo_puntos);    
  }
}


/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
   ###################################################################################################### 
        BLOQUE DE FUNCIONES: LECTURAS DE SENSORES, COMUNICACION SERIE, CONTROL LCD...
   ###################################################################################################### 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

/*mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    EXTRAS PARA EL LCD
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm*/

//========================================================
// FUNCION PARA CREAR UN DIGITO GRANDE
//========================================================

void bigNumero(byte i, byte posicion)
{
  lcd.setCursor(posicion, 0);
  lcd.write(numeros[i][0]); lcd.write(numeros[i][1]); lcd.write(numeros[i][2]);
  lcd.setCursor(posicion, 1);
  lcd.write(numeros[i][3]); lcd.write(numeros[i][4]); lcd.write(numeros[i][5]);   
  return;
}

void print_dos_puntos(boolean FLAG_parpadeo)
{
  if(FLAG_parpadeo==true){
    lcd.setCursor(8, 0);
    lcd.write(165);
    lcd.setCursor(8, 1);  
    lcd.write(165);
  }
  else{
    lcd.setCursor(8, 0);
    lcd.print(" ");
    lcd.setCursor(8, 1);  
    lcd.print(" ");      
  }  
}



//*******************************************************
//                    FIN DE PROGRAMA
//*******************************************************

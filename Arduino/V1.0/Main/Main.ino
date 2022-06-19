/* Librerias */
#include <SoftwareSerial.h>    
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

/* Instancias */
LiquidCrystal_I2C lcd(0x3F,16,2);        //Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas
SoftwareSerial BT(4,5);                  // Definimos los pines RX y TX del Arduino conectados al Bluetooth
Servo servo_1;                           // Servos
Servo servo_2;

/* Definiciones*/
#define S1_home 6                        // Posicion inicial servomotor
#define S1_max 90
#define S2_home 0
#define S2_max 90


/* Variables*/
char menu, angle;
int encoder = 0;


void setup() {
  // Inicializar el Bluetooth
  BT.begin(9600);    

  // Inicializar lectura del encoder
  attachInterrupt(0, ISR_1, CHANGE);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  

  // Inicializar el servos
  servo_1.attach(9);                  // Pines de los servomotores
  servo_2.attach(10);
  servo_1.write(S1_home);             // Posiciones iniciales
  servo_2.write(S2_home);
  
  // Inicializar el LCD
  lcd.init();
  lcd.backlight();            //Encender la luz de fondo.
  lcd.print("Status: Waiting ");     // Escribimos el Mensaje en el LCD
  lcd.setCursor(0, 1); 
}



void loop() {
  while(!BT.available()){;}
  menu = BT.read();

  switch(menu){
    case 'a':                 // Home
      //BT.write(menu);
      servo_1.write(S1_home);
      servo_2.write(S2_home);
      lcd.setCursor(0, 1);
      lcd.print("State: Home     ");
      break;
    case 'b':                 // Escribir servo 1
      //BT.write(menu);
      writeServo_1();
      break;
    case 'c':                 // Escribir servo 2
      //BT.write(menu);
      writeServo_2();
      break;
    case 'd':                 // leer encoder
      //BT.write(menu);
      delay(20);
      BT.write(encoder);
      encoder = 0;
      break;
    case 'e':
      lcd.setCursor(0, 0);
      lcd.print("Status: Training"); 
      break;
    case 'f':
      lcd.setCursor(0, 0);
      lcd.print("Status: Ready   "); 
      break;
    case 'g':
      lcd.setCursor(7, 1);
      lcd.print("[");

      while(!BT.available()){;}
      lcd.print(BT.read());
      lcd.print(",");
      while(!BT.available()){;};
      lcd.print(BT.read());
      lcd.print("]   ");
      break;
    default:
      break;
  }

  delay(200);   
  
  
}

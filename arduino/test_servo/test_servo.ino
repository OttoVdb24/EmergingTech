#include <Ramp.h>
#include <Servo.h>

rampInt servoRamp_1;   // Servo ramp object aanmaken
rampInt servoRamp_2;
Servo Servo_1;          //Aanmaken van het servo object
Servo Servo_2;

int servo1_In = 0;
int servo1_Uit= 180*0.75;

int servo2_In = 0;
int servo2_Uit= 90*0.75;

bool servoRampActive_1 = false;       // True wanneer de Ramp bezig is en de servo dus in beweging is. Laag tijdens het wachten.
bool servoRampActive_2 = false;

bool flankDetectieBools[2];           
/* Verzameling van bytes om flank detecties mee te doen op inputs. 
De laatste state van de input wordt geschreven naar deze bool om hiermee te vergelijken*/



void setup() {

  Serial.begin(9600);
  Servo_1.attach(9);  // attaches the servo on pin 9 to the Servo object
  Servo_2.attach(10);  // attaches the servo on pin 9 to the Servo object

  servoRamp_1.setGrain(1);
  servoRamp_2.setGrain(1);

  servoRamp_1.go(servo1_In, 0, LINEAR);   // initialiseer op startpositie
  servoRamp_2.go(servo1_In, 0, LINEAR);  // initialiseer op startpositie
  
  Servo_1.write(servoRamp_1.update());
  Servo_2.write(servoRamp_2.update());

  pinMode(13,OUTPUT);
}


void loop() {

  bool startKnop = positieveFlankDetectie(digitalRead(2),flankDetectieBools[0]);   // Leest pin 2 in met een positieve flank detetctie.


/* Wannneer er op de knop wordt gedrukt zal de functie geset worden, 
 en zal de ramp functie over een gegeven tijd volgens een gegeven functie naar de gewenste positie bewegen
 Deze positie zal wisselen telkens de vorige beweging is afgerond en er opnieuw op de knop wordt geduuwd.*/

  if (startKnop || servoRampActive_1){
    servoRampActive_1 = servoRampFunctie(Servo_1,servoRamp_1, servoRampActive_1, servo1_In, servo1_Uit); 
  }

  if (startKnop || servoRampActive_2){
    servoRampActive_2 = servoRampFunctie(Servo_2,servoRamp_2, servoRampActive_2, servo2_In, servo2_Uit); 
  }
}



bool positieveFlankDetectie(bool Input, bool &flankDetectieByte){

  if (Input && !flankDetectieByte) {
    flankDetectieByte = true;
    return true;
  }
  else if (flankDetectieByte && Input){
    flankDetectieByte=true;
    return false;
  }
  else if (!Input){
    flankDetectieByte=false;
    return false;
  }
}

bool servoRampFunctie(Servo &myservo, rampInt &servoRamp, bool servoRampActive, int inPos, int uitPos) {
    if (servoRamp.isFinished() && servoRampActive) {              // Triggert wanneer de beweging klaar is en zal de 
    digitalWrite(13, LOW);
    return false;  // Functie doen stoppen.
  }

  // Initialisatie van de motor beweging. Runt wanneer de ramp nog niet runnign is.
  // De code kijkt naar de huidige positie van servo en zal dan naar de andere positie gaan.

  if (!servoRamp.isRunning()){
    if (servoRamp.getValue() == inPos) { 
        servoRamp.go(uitPos, 3000, QUADRATIC_IN);
    }
    else{  
        servoRamp.go(inPos, 3000, QUADRATIC_IN );
    }}


  // De postitie van de servi veranderen wanneer de Ramp actief is. + controle led doen branden. 
  if (servoRamp.isRunning()) {      
    myservo.write(servoRamp.update());   // De volgende waarde uit de ramp opvragen en deze in de servo write scrijven.
    digitalWrite(13, HIGH);             
    return true;                          
  }

}
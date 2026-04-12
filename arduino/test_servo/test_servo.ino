#include <Ramp.h>
#include <Servo.h>

rampInt servoRamp;   // Servo ramp object aanmaken
Servo myservo;  //Aanmaken van het servo object

int inPos = 0;       // de positie wanneer de arm ingeklapd is.
int uitPos = 120;   // de positite wanneer de arm uitgestrekt is
int servoPos;        // globale variabele, de effectieve hoek van de servo.

bool servoRampActive = false;

void servoRampFunctie(){

  if (!servoRampActive && servoPos==inPos){
  servoRampActive=true;
  servoRamp.go(uitPos,10000,QUINTIC_INOUT);
  }

  if (servoRampActive){
  servoPos = servoRamp.update();
  myservo.write(servoPos);  
  }

  if (servoRamp.isFinished()){
    servoRampActive=false;
  }
}



void setup() {

  Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the Servo object
  myservo.write(inPos);
  servoRamp.setGrain(1);

}


void loop() {

  servoRampFunctie();

 }

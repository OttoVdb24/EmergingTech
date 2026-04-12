#include <Ramp.h>
#include <Servo.h>

rampInt servoRamp;   // Servo ramp object aanmaken
Servo myservo;  //Aanmaken van het servo object

int inPos = 0;       // de positie wanneer de arm ingeklapd is.
int uitPos = 120;   // de positite wanneer de arm uitgestrekt is
int servoPos;        // globale variabele, de effectieve hoek van de servo.

bool servoRampActive = false;

bool flankDetectieBytes[2];

void servoRampFunctie(){

  if (!servoRampActive && servoPos==inPos){
  servoRampActive=true;
  servoRamp.go(uitPos,1000,QUINTIC_OUT);
  }
  else if (!servoRampActive && servoPos==uitPos){
  servoRampActive=true;
  servoRamp.go(inPos,1000,QUINTIC_IN);
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

  bool startKnop = positieveFlankDetectie(digitalRead(2),flankDetectieBytes[0]);
  if (startKnop || servoRampActive){
    servoRampFunctie();
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

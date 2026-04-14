#include <Ramp.h>
#include <Servo.h>

rampInt servoRamp_1;   // Servo ramp object aanmaken
rampInt servoRamp_2;
Servo Servo_1;  //Aanmaken van het servo object
Servo Servo_2;


bool servoRampActive_1 = false;
bool servoRampActive_2 = false;

bool flankDetectieBytes[2];














void setup() {

  Serial.begin(9600);
  Servo_1.attach(9);  // attaches the servo on pin 9 to the Servo object
  Servo_2.attach(10);  // attaches the servo on pin 9 to the Servo object


  servoRamp_1.setGrain(1);
  servoRamp_2.setGrain(1);

  servoRamp_1.go(0, 0, LINEAR);   // initialiseer op startpositie
  servoRamp_2.go(90, 0, LINEAR);  // initialiseer op startpositie
  Servo_1.write(servoRamp_1.update());
  Servo_2.write(servoRamp_2.update());

  pinMode(13,OUTPUT);
}


void loop() {

  bool startKnop = positieveFlankDetectie(digitalRead(2),flankDetectieBytes[0]);
  if (startKnop || servoRampActive_1){
    servoRampActive_1 = servoRampFunctie(Servo_1,servoRamp_1, servoRampActive_1, 0, 90); 
  }
  if (startKnop || servoRampActive_2){
    servoRampActive_2 = servoRampFunctie(Servo_2,servoRamp_2, servoRampActive_2, 90,0); 
    Serial.println("2,Actief");
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

  if (servoRamp.isFinished() && servoRampActive) {
    servoRampActive = false;
    digitalWrite(13, LOW);
    return false;  // Functie doen stoppen.
  }

  if (!servoRamp.isRunning() && servoRamp.getValue() == inPos) { // Start van de actie motorbeweging , checkt of die uit of in moet bewegen
    servoRampActive = true;
    servoRamp.go(uitPos, 3000, QUADRATIC_IN);
  }
  else if (!servoRamp.isRunning() && servoRamp.getValue() == uitPos) {
    servoRampActive = true;
    servoRamp.go(inPos, 3000, QUADRATIC_IN );
  }

  if (servoRamp.isRunning()) {      // De postitie van de servi veranderen wanneer de Ramp actief is. + controle led doen branden.
    myservo.write(servoRamp.update());
    digitalWrite(13, HIGH);
    return true;
  }

}
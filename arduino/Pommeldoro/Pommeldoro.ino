bool flankDetectie [2];              // Array om al de flankdetectie bits in op te slaan.
const int STARTKNOP = 2;
bool startKnop;
const int RESETKNOP = 3;
bool resetKnop;

// Alles voor de timer functie
bool timerActive = false;             // Boolean die toont wanneer de timer actief is.

long startTimer = 0;                 // die start moment van de timer zal capteren
long pauzeTimerDuur = (10)*1000;     // Pauze tijd instellen in ms (ingeven in seconden, vermenigvuldigen met 1000 om naar ms te gaan)
long blokTimerDuur = (10)*1000;      // Blok(studeer) tijd instellen  in s -> ms 
unsigned long currTime = 0;
bool timerKlaar = false;
long timerValue;

int aantalHerhalingen = 5;
int herhalingTeller = 0;

int state = 0;
bool bewegingKlaar;


// Alles voor het servo gedeelte
#include <Ramp.h>
#include <Servo.h>

rampInt servoRamp_1;   // Servo ramp object aanmaken
rampInt servoRamp_2;
Servo Servo_1;  //Aanmaken van het servo object
Servo Servo_2;

bool servoRampActive_1 = false;
bool servoRampActive_2 = false;
float servoOffset = 0.75;
float servo1Uit = 90 * servoOffset;
float servo1In = 0;
float servo1Neutr = 45*servoOffset;

float servo2Uit = 180*0.75-servo1Uit;
float servo2In = 180*0.75-servo1In;
float servo2Neutr = 180*0.75- servo1Neutr; 

void inputHandeling(){
  
  startKnop = positieveFlankDetectie(digitalRead(STARTKNOP),flankDetectie[0]);
  resetKnop = positieveFlankDetectie(digitalRead(RESETKNOP),flankDetectie[1]);
  currTime = millis();
}

void servoSetup(){
  Servo_1.attach(9);  // attaches the servo on pin 9 to the Servo object
  Servo_2.attach(10);  // attaches the servo on pin 9 to the Servo object

  servoRamp_1.setGrain(1);
  servoRamp_2.setGrain(1);

  servoRamp_1.go(servo1In, 0, LINEAR);   // initialiseer op startpositie
  servoRamp_2.go(servo2In, 0, LINEAR);  // initialiseer op startpositie
  
  Servo_1.write(servoRamp_1.update());
  Servo_2.write(servoRamp_2.update());
}



void setup() {

Serial.begin(9600);
pinMode(13,OUTPUT);
servoSetup();

}

void loop() {
inputHandeling();
digitalWrite(13,LOW);


  switch (state){
    case 0:
      if (!bewegingKlaar){
     bewegingKlaar =servoRampFunctie(Servo_1,servoRamp_1,servoRampActive_1,servo1Neutr);
      servoRampFunctie(Servo_2,servoRamp_2,servoRampActive_2,servo2Neutr);
      }

      Serial.println("Wacht status: ...");

      if (startKnop){       //Overgangsvoorwaarde
        state  ++;
        bewegingKlaar = false;

      }

    break;

    case 1: // Servo beweging Links kantelen
        if (!bewegingKlaar){
     bewegingKlaar =servoRampFunctie(Servo_1,servoRamp_1,servoRampActive_1,servo1In);
           servoRampFunctie(Servo_2,servoRamp_2,servoRampActive_2,servo2Uit);

      }
    else{
      state++;
      bewegingKlaar=false;
    }
    break;

    case 2: //BlokTimer
      // Herhaalde acties

      timerKlaar = timerFunctie(resetKnop,timerActive,startTimer,blokTimerDuur,currTime,timerValue);

      Serial.print("Blokken!: ...");
      Serial.println((blokTimerDuur-timerValue)/1000);


      if (timerKlaar){  //Overgangsvoorwaarde
        state++;       //Eenmalige actie
      }


    break;

    case 3: // Servo's bewegen rechts kantelen
    if (!bewegingKlaar){
     bewegingKlaar =servoRampFunctie(Servo_1,servoRamp_1,servoRampActive_1,servo1Uit);
           servoRampFunctie(Servo_2,servoRamp_2,servoRampActive_2,servo2In);

      }
    else{
      state++;
      bewegingKlaar =false;
    }
    
    break;

    case 4: //PauzeTimer
      timerKlaar = timerFunctie(resetKnop,timerActive,startTimer,pauzeTimerDuur,currTime, timerValue);
      Serial.print("Pauze!: ...");
      Serial.println((pauzeTimerDuur-timerValue)/1000);


      if (timerKlaar && herhalingTeller<(aantalHerhalingen-1)){  //Overgangsvoorwaarde, Bij een aantal herhalingen onder de max zal hij terug starten met het blokken, anders naar wacht.
        state =1;                                             // Terug naar blok state
        herhalingTeller++;                                    // Een extra rondje tellen
      }
      else if (timerKlaar) {  
      state=0;                                                // Terug naar wacht state
      aantalHerhalingen=0;                                    // Aantal herhaling resetten om volgende keer opnieuw van 0 te beginnen.
      }

    break;




  

  
  
  
  }




  if (resetKnop){                                             // Wanneer de resetknop wordt geduuwd zal de loop stoppen en naar wachtstate resetten.
        state=0;                                               // Terug naar wacht state
      aantalHerhalingen=0;                                    // Aantal herhaling resetten om volgende keer opnieuw van 0 te beginnen.
  }

}



bool timerFunctie ( bool reset, bool &actief, long &startTimer, long timerDuur, long currTime, long &timerValue ){

  if (reset){                 // reset van de timer (actief laag plaatsen)
    actief = false;
    return false;                  // meteen uit de timer functie
  }

  if (!actief){       // Timer opstart, actief hoogplaatsen, startTimer zetten
    actief = true;
    startTimer = currTime;
  }
  
  if (actief){                    // Timer loop
    digitalWrite(13,HIGH);
    timerValue = currTime-startTimer;
    if (timerValue> timerDuur){
      actief = false;
      Serial.println("TimerKlaar");
      return true;

    }
    else{
      return false;
    }
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

bool servoRampFunctie(Servo &myservo, rampInt &servoRamp, bool &servoRampActive, int uitPos) {

  if (servoRamp.isFinished() && servoRampActive) {
    servoRampActive = false;
    digitalWrite(13, LOW);
    return true;  // Functie doen stoppen.
  }


  if (!servoRamp.isRunning()) { // Start van de actie motorbeweging , checkt of die uit of in moet bewegen
    servoRampActive = true;
    servoRamp.go(uitPos, 3000, QUADRATIC_IN);
  }
    // De postitie van de servi veranderen wanneer de Ramp actief is. + controle led doen branden.
    myservo.write(servoRamp.update());
    digitalWrite(13, HIGH);
    return false;



}


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

int aantalHerhalingen = 5;        // Aantal herhalingen de pommeldoro zal doen vooraleer hij terug naar wacht zal gaan
int herhalingTeller = 0;          // Counter om aantal herhalingen bij te houden

int state = 0;                    // Switch state 

bool bewegingKlaar;               // bit die hoog wordt geplaatst wanneer de servobeweging klaar is. 


// Alles voor het servo gedeelte
#include <Ramp.h>
#include <Servo.h>

rampInt servoRamp_1;              // Servo ramp object aanmaken
rampInt servoRamp_2;
Servo Servo_1;                    //Aanmaken van het servo object
Servo Servo_2;

bool servoRampActive_1 = false;       // bit om aan te geven dat de ramp actief is. (om eerste cyclus te bepalen)
bool servoRampActive_2 = false;
float servoOffset = 0.75;             // Hoek offset om ervoor te zorgen dat 180° effectief 180° draait.

float servo1Uit = 90 * servoOffset;    // Hoeken waar de servo naartoe zal rampen
float servo1In = 0;
float servo1Neutr = 45*servoOffset;

float servo2Uit = 90*0.75-servo1Uit;   
float servo2In = 180*0.75-servo1In;
float servo2Neutr = 45*0.75- servo1Neutr; 

/*
Functie om alle inputs in de arduino in te lezen. Waaronder de knoppen met positieve flank detectie en de huidige tijd in miliseconden vast leggen aan het begin van de loop.*/
void inputHandeling(){                  
  
  startKnop = positieveFlankDetectie(digitalRead(STARTKNOP),flankDetectie[0]);
  resetKnop = positieveFlankDetectie(digitalRead(RESETKNOP),flankDetectie[1]);
  currTime = millis();

}

/*
Alle setup van de servo's. Servo koppelen aan hun respectievelijke PWM pin. De ramp objecten fijn instellen en dan de servo naar hun in positie sturen door een korte ramp van 0sec.*/
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
// Alle inputs binnen trekken  
inputHandeling();
// Status lamp laag zetten aan het begin van de loop.
digitalWrite(13,LOW);

  // De single state code. Telkens een bepaalde status zal actief zijn
  switch (state){
    case 0: // Wacht status

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
     bewegingKlaar =servoRampFunctie(Servo_1,servoRamp_1,servoRampActive_1,servo1In);   // Servo 1 aansturen. 
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

bool servoRampFunctie(Servo &myservo, rampInt &servoRamp, bool &servoRampActive, int Pos) {

  if (servoRamp.isFinished() && servoRampActive) {  // Servo beweging is klaar want finished maar de rampActive staat nog hoog, dus eerste loop na klaar.
    servoRampActive = false;
    digitalWrite(13, LOW);
    return true;                             // Functie doen stoppen.
  }


  if (!servoRamp.isRunning()) {                   // Start van de actie motorbeweging , start een ramp naar de gevraagde Pos.
    servoRampActive = true;
    servoRamp.go(Pos, 3000, QUADRATIC_IN);
  }

    // De postitie van de servi veranderen wanneer de Ramp actief is. + controle led doen branden.
    myservo.write(servoRamp.update());
    digitalWrite(13, HIGH);
    return false;

}


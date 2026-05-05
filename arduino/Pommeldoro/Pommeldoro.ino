#include <Ramp.h>
#include <Servo.h>
#include <ArduinoBLE.h>


// Inputs____________________________________________________________________________________
const int STARTKNOP = 2;
bool flankStartKnop = false;    // boolean die vorige waarde van de startknop onthoud
bool startKnop;                 // boolean die na de flank detectie de waarde van de startknop zal krijgen (telkens 1 loop hoog)
const int RESETKNOP = 3;
bool flankResetKnop = false;
bool resetKnop;

// Alles voor de timer functie__________________________________________________________________
bool timerActive = false;             // Boolean die toont wanneer de timer actief is.
long startTimer = 0;                 // die start moment van de timer zal capteren
long pauzeTimerDuur = (5)*1000;     // Pauze tijd instellen in ms (ingeven in seconden, vermenigvuldigen met 1000 om naar ms te gaan)
long blokTimerDuur = (10)*1000;      // Blok(studeer) tijd instellen  in s -> ms 
unsigned long currTime = 0;
bool timerKlaar = false;
long timerValue;

int aantalHerhalingen = 2;        // Aantal herhalingen de pommeldoro zal doen vooraleer hij terug naar wacht zal gaan
int herhalingTeller = 0;          // Counter om aantal herhalingen bij te houden

int state = 0;                    // Switch state 
bool bewegingKlaar;               // bit die hoog wordt geplaatst wanneer de servobeweging klaar is. 

// SERVO's_______________________________________________________________________________________
rampInt servoRamp_1;              // Servo ramp object aanmaken
rampInt servoRamp_2;
Servo Servo_1;                    //Aanmaken van het servo object
Servo Servo_2;

bool servoRampActive_1 = false;       // bit om aan te geven dat de ramp actief is. (om eerste cyclus te bepalen)
bool servoRampActive_2 = false;
float servoOffset = 0.75;             // Hoek offset om ervoor te zorgen dat 180° effectief 180° draait.

float servo1Uit = 90* servoOffset;    // Hoeken waar de servo naartoe zal rampen
float servo1In = 0;
float servo1Neutr = 45*servoOffset;

float servo2Uit = 90*servoOffset;   
float servo2In = 180*servoOffset;
float servo2Neutr = 135*servoOffset; 

// Bluetooth__________________________________________________________________
BLEService sensorService("12345678-1234-1234-1234-123456789012");
BLEFloatCharacteristic sensorChar(  "12345678-1234-1234-1234-123456789013",  BLERead | BLENotify);




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

/* Setup van de Bluetooth Low energy code*/
void BLE_setup(){
    if (!BLE.begin()) {
    Serial.println("BLE starten mislukt!");
    while (1);      // Laat de code stoppen wanneer de BLE niet aanwezig is of niet werkt
  }

  BLE.setLocalName("Arduino-Pommeldoro");
  BLE.setAdvertisedService(sensorService);
  sensorService.addCharacteristic(sensorChar);
  BLE.addService(sensorService);

  sensorChar.writeValue(0.0f);
  BLE.advertise();
  Serial.println("BLE actief, wachten op verbinding...");
}



void setup() {
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  BLE_setup();
  servoSetup();

}


void loop() {

  inputHandeling();   // Alle inputs binnen trekken  
  digitalWrite(13,LOW); // Status lamp laag zetten aan het begin van de loop.


  switch (state){      // De single state code. Telkens een bepaalde case zal actief zijn

    case 0: // Wacht status

      if (!bewegingKlaar){
        bewegingKlaar =servoRampFunctie(Servo_1,servoRamp_1,servoRampActive_1,servo1Neutr);
                    servoRampFunctie(Servo_2,servoRamp_2,servoRampActive_2,servo2Neutr);
      }

      Serial.println("Wacht status: ...");

      if (startKnop){       //Overgangsvoorwaarde, induwen van de startknop
        state  ++;          //Naar volgende case gaan.
        bewegingKlaar = false;    //Beweging resetten.
      }
    break;

    case 1: // Servo beweging Links kantelen

      if (!bewegingKlaar){
        bewegingKlaar = servoRampFunctie(Servo_1,servoRamp_1,servoRampActive_1,servo1In);   // Servo 1 aansturen. 
                        servoRampFunctie(Servo_2,servoRamp_2,servoRampActive_2,servo2Uit);

      }
      else  {         // Overgangsvoorwaarde, einde van de beweging. 
        state++;            // Naar volgende case
        bewegingKlaar=false;
      }
    break;

    case 2: //Timer 1: Studeren 

      timerKlaar = timerFunctie(resetKnop,timerActive,startTimer,blokTimerDuur,currTime,timerValue);

      Serial.print("Blokken!: ...");
      Serial.println((blokTimerDuur-timerValue)/1000);  // Reseterende tijd printen

      if (timerKlaar){  //Overgangsvoorwaarde, timer klaar
        state++;       //Naar volgende case
      }
    break;

    case 3: // Servo's bewegen rechts kantelen
    if (!bewegingKlaar){
     bewegingKlaar = servoRampFunctie(Servo_1,servoRamp_1,servoRampActive_1,servo1Uit);
                     servoRampFunctie(Servo_2,servoRamp_2,servoRampActive_2,servo2In);
      }
    else{         //Overgangsvoorwaarde, Bewging klaar
      state++;
      bewegingKlaar =false;
    }
    
    break;

    case 4: //Timer 2: Pauze

      timerKlaar = timerFunctie(resetKnop,timerActive,startTimer,pauzeTimerDuur,currTime, timerValue); // Timer functie, geeft boolean terug die hoog is wanneer de timer afgelopen is.
      Serial.print("Pauze!: ...");
      Serial.println((pauzeTimerDuur-timerValue)/1000);

      if (timerKlaar && herhalingTeller<(aantalHerhalingen-1)){  //Overgangsvoorwaarde 1, Bij een aantal herhalingen onder de max zal hij terug starten met het blokken, anders naar wacht.
        state =1;                                                   // Terug naar blok state
        herhalingTeller++;                                          // Een extra rondje tellen
      }
      else if (timerKlaar) {                                     //Overgangsvoorwaarde 2, Timer klaar en aantal herhalingen overschreden, terig naar wacht status.
      state=0;                                                      // Terug naar wacht state
      herhalingTeller=0;                                            // Aantal herhaling resetten om volgende keer opnieuw van 0 te beginnen.
      }
    break;
  }
  // Reset voorwaarde.
  if (resetKnop){                                             // Wanneer de resetknop wordt geduuwd zal de loop stoppen en naar wachtstate resetten.
        state=0;                                               // Terug naar wacht state
      herhalingTeller=0;                                       // Aantal herhaling resetten om volgende keer opnieuw van 0 te beginnen.
  }
  BLE_send(state);

}


/*
Functie om alle inputs in de arduino in te lezen. Waaronder de knoppen met positieve flank detectie en de huidige tijd in miliseconden vast leggen aan het begin van de loop.*/
void inputHandeling(){                  
  
  startKnop = positieveFlankDetectie(digitalRead(STARTKNOP),flankStartKnop);
  resetKnop = positieveFlankDetectie(digitalRead(RESETKNOP),flankResetKnop);
  currTime = millis();

}


bool timerFunctie(bool reset, bool &actief, long &startTimer, long timerDuur, long currTime, long &timerValue) {

  if (reset) {           // Reset, timer stoppen en false returnen
    actief = false;
    return false;
  }

  if (!actief) {         // Eerste aanroep: timer opstarten
    actief = true;
    startTimer = currTime;
  }

  // Timer actief
  timerValue = currTime - startTimer;       //Verstreken tijd berekenen
  digitalWrite(13, (currTime/1000)%2);

  if (timerValue >= timerDuur) {   // Timer klaar
    actief = false;
    Serial.println("Timer klaar");
    return true;
  }

  return false;          // Timer nog bezig
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

void BLE_send(int value) {
  BLE.poll();  // BLE events verwerken

  BLEDevice central = BLE.central();
  if (central && central.connected()) {
    sensorChar.writeValue((float)value);
  }
}


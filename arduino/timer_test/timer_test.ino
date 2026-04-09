bool flankDetectie [2];              // Array om al de flankdetectie bits in op te slaan.
const int STARTKNOP = 2;
const int RESETKNOP = 3;


// Alles voor de timer functie
bool timerActive = false;             // Boolean die toont wanneer de timer actief is.

long startTimer = 0;                 // die start moment van de timer zal capteren
long pauzeTimerDuur = (10)*1000;     // Pauze tijd instellen in ms (ingeven in seconden, vermenigvuldigen met 1000 om naar ms te gaan)
long blokTimerDuur = (20)*1000;      // Blok(studeer) tijd instellen  in s -> ms 
long currTime = 0;




void setup() {
Serial.begin(9600);
pinMode(13,OUTPUT);


}

void loop() {
bool startKnop = positieveFlankDetectie(digitalRead(STARTKNOP),flankDetectie[0]);
bool resetKnop = positieveFlankDetectie(digitalRead(RESETKNOP),flankDetectie[1]);
unsigned long currTime = millis();

bool timerKlaar = timerFunctie(startKnop,resetKnop,timerActive,startTimer,pauzeTimerDuur,currTime);

if (timerActive){
  digitalWrite(13,HIGH);
}
else{
  digitalWrite(13,LOW);
}

}



bool timerFunctie ( bool trigger, bool reset, bool &actief, long &startTimer, long timerDuur, long currTime ){

  if (reset){                 // reset van de timer (actief laag plaatsen)
    actief = false;
    return false;                  // meteen uit de timer functie
  }

  if (trigger and !actief){       // Timer opstart, actief hoogplaatsen, startTimer zetten
    actief = true;
    startTimer = currTime;
  }
  
  if (actief){
    unsigned long timerValue = currTime-startTimer;
    if (timerValue> timerDuur){
      actief = false;
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



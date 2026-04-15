bool flankDetectie [2];              // Array om al de flankdetectie bits in op te slaan.
const int STARTKNOP = 2;
bool startKnop;
const int RESETKNOP = 3;
bool resetKnop;

// Alles voor de timer functie
bool timerActive = false;             // Boolean die toont wanneer de timer actief is.

long startTimer = 0;                 // die start moment van de timer zal capteren
long pauzeTimerDuur = (3)*1000;     // Pauze tijd instellen in ms (ingeven in seconden, vermenigvuldigen met 1000 om naar ms te gaan)
long blokTimerDuur = (2)*1000;      // Blok(studeer) tijd instellen  in s -> ms 
unsigned long currTime = 0;
bool timerKlaar = false;
long timerValue;

int aantalHerhalingen = 5;
int herhalingTeller = 0;

int state = 0;

void inputHandeling(){
  
  startKnop = positieveFlankDetectie(digitalRead(STARTKNOP),flankDetectie[0]);
  resetKnop = positieveFlankDetectie(digitalRead(RESETKNOP),flankDetectie[1]);
  currTime = millis();
}



void setup() {

Serial.begin(9600);
pinMode(13,OUTPUT);


}

void loop() {
inputHandeling();
digitalWrite(13,LOW);


  switch (state){
    case 0:
      Serial.println("Wacht status: ...");

      if (startKnop){       //Overgangsvoorwaarde
        state = 1;
      }

    break;

    case 1: //BlokTimer
      // Herhaalde acties
      timerKlaar = timerFunctie(resetKnop,timerActive,startTimer,blokTimerDuur,currTime,timerValue);

      Serial.print("Blokken!: ...");
      Serial.println((blokTimerDuur-timerValue)/1000);


      if (timerKlaar){  //Overgangsvoorwaarde
        state =3;       //Eenmalige actie

      }


    break;

    case 3: //PauzeTimer
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



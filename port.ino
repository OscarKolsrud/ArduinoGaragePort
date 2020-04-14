//Variabler for inputs (knapper)
const int pinNodStopp = 7;
const int pinStoppOpp = 8;
const int pinStoppNed = 5;
const int pinStartOpp = 2;
const int pinStartNed = 4;

//Variabler for knapp status
int oppState;
int nedState;
int limitOppState;
int limitNedState;
int nodState;

//Variabler for LED lys outputs
const int red_light_pin= 3;
const int green_light_pin = 6;
const int blue_light_pin = 9;

//Variabler for hvilke pinner motorene er koblet på (Må støtte PWM, aka ~ ved siden av tallet til den på arduinoen)
const int motor1 = 10;
const int motor2 = 11;

//Variabler for å "huske" hva den gjør
bool opp = false;
bool ned = false;
bool nodstopp = false;

//Variabel som brukes til "debounce" for å hindre at et trykk på knapp registreres som flere
unsigned long debounceTime = 0;

//Variabel som lagrer debounce forsinkelsen
unsigned long debounceDelay = 150;

//Denne kode delen kjøres bare når den starter opp og brukes gjerne til å konfigurere hva in/outputs skal gjøre
void setup()
{
  //Start serial med 9600bps
  Serial.begin(9600);

  //Definer pinsene til RGB LED
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);

  //Definer at knappene skal være input
  pinMode(pinNodStopp, INPUT);
  pinMode(pinStoppOpp, INPUT);
  pinMode(pinStoppNed, INPUT);
  pinMode(pinStartOpp, INPUT);
  pinMode(pinStartNed, INPUT);
}

//Denne koden kjører om og om igjen
void loop()
{
  //Les statusen til knappene og lagre i en variabel
  oppState = digitalRead(pinStartOpp);
  nedState = digitalRead(pinStartNed);
  limitOppState = digitalRead(pinStoppOpp);
  limitNedState = digitalRead(pinStoppNed);
  nodState = digitalRead(pinNodStopp);

  if ((oppState && nedState && limitOppState && limitNedState) == LOW && !nodstopp) {
    RGB_color(0, 0, 0); // Slå av lyset om ingen knapper er trykket, og nødstopp ikke er aktiv
  }

  //Alt innenfor denne "if" blokken debounces og beskyttes dermed mot gjentatte knappetrykk
  if ((millis() - debounceTime) > debounceDelay) {
    //Bruk statusen til nødstopp knappen, og lagre i en variabel om den er trykket eller ikke
    if (nodState == HIGH && nodstopp == true) {
      debounceTime = millis();
      Serial.print("FALSE");
      Serial.println();
      RGB_color(0, 0, 0); // av
      nodstopp = false;
    } else if (nodState == HIGH && nodstopp == false) {
      debounceTime = millis();
      Serial.print("TRUE");
      Serial.println();
      RGB_color(255, 0, 0); // rødt lys
      nodstopp = true;
    }
  }

  //kode som kun kan kjøre om nødstopp ikke er aktivert
  if (!nodstopp) {
    //Denne blokken håndterer kjøring av porten ned, indikert med "Gul" LED (Kan ikke kjøre om den kjører opp, eller om limit NED aktivert)
    if(nedState == HIGH && oppState == LOW && limitNedState == LOW) {
      Serial.print("NED");
      Serial.println();
      RGB_color(255, 255, 0);

      //Kjør motoren, bruker analogWrite på en PWM port for å kunne hastighetsstyre med pulsmodulering (endring av volt)
      analogWrite(motor2, 0);
      analogWrite(motor1, 255);
    }

    //Denne blokken håndterer kjøring av porten opp, indikert med "Grønn" LED (Kan ikke kjøre om den kjører ned, eller om limit opp aktivert)
    if(nedState == LOW && oppState == HIGH && limitOppState == LOW) {
      Serial.print("OPP");
      Serial.println();
      RGB_color(255, 0, 255);

      //Kjør motoren, bruker analogWrite på en PWM port for å kunne hastighetsstyre med pulsmodulering (endring av volt)
      analogWrite(motor1, 0);
      analogWrite(motor2, 255);
    }
  }

  //Denne blokken brukes kun når porten er i "hvile" posisjon ved en av endene, aka trykker en av limit switchene
  if ((limitNedState || limitOppState) == HIGH) {
    RGB_color(0, 255, 0); //Sett LED til grønn/lime farge
  }

}

//Denne funksjonen brukes til RGB LED for å sette fargen med RGB koder (Verdier fra 0-255)
void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
{
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}

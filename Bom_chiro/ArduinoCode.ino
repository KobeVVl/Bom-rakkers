#define DEBUG 1
const int OK = 200;
#define GameModesAmount 2
#define LCD "lcd"

/* ------------------- INCLUDE ------------------- */
#include <LiquidCrystal.h>
#include <Preset.h>

/* ------------------- DATA ------------------- */
/* code keyboard */
const int codeValues[] = {90, 175, 234, 320, 411, 507, 616, 696, 838, 929, 1013};
const int codeKeys[] = {OK, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

/* bananenklemmen waardes */
const double banana1[] = {4.3, 101, 4.2, 102, 4.1, 103, 3.1, 104, 105, 110, 3.2, 106, 4.0, 107, 3.8, 108, 112, 3.7, 109, 113, 114, 115, 3.5, 111};
const double banana2[] = {4.3, 101, 4.2, 102, 3.8, 103, 104, 108, 3.6, 105, 106, 109, 4.0, 107, 3.5, 110, 2.7, 111, 114, 3.1, 112, 2.6, 113, 3.2, 115};
const double banana3[] = {4.1, 103, 4.0, 107, 3.8, 104, 105, 3.7, 106, 3.6, 101, 114, 3.5, 110, 111, 3.3, 102, 3.2, 113, 115, 2.7, 108, 109, 2.5, 112};
const double banana4[] = {4.0, 107, 3.8, 103, 104, 110, 111, 3.7, 109, 114, 3.6, 101, 105, 3.5, 108, 3.2, 113, 3.1, 112, 2.7, 106, 2.6, 115, 2.5, 102};

/* draden waardes */
const int wireValues[] = {930, 839, 696, 512, 320, 179};

/* ------------------- PINS ------------------- */
#define bananaPin1 A0
#define bananaPin2 A1
#define bananaPin3 A2
#define bananaPin4 A3
#define wirePin A4
#define codePin A5
#define piezoPin 2
#define ledPin1 3
#define ledPin2 4
#define ledPin3 5
#define rs 6
#define en 7 
#define d4 8 
#define d5 9 
#define d6 10 
#define d7 11

/* ------------------- CONSTANTS ------------------- */
const int maxInterval = 5000;
const int minInterval = 150;
const int fastBeepTime = 5; // De laaste x seconden beept de bom met minInterval er tussen (snel)

const unsigned int beepPitch = 3000;
const unsigned long beepLength = 75;

/* ------------------- SETUP ------------------- */
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
void setup() {
    if (DEBUG) Serial.begin(9600);
    lcd.begin(16,2);
    pinMode(ledPin1, OUTPUT);
    pinMode(ledPin2, OUTPUT);
    pinMode(ledPin3, OUTPUT);
    if (DEBUG) {Serial.println("---------- SETUP DONE ----------");}
}

/* ------------------- LOOP ------------------- */
void loop(){
    int gameMode;    
    
    lcd.clear();
    lcd.print("Select game mode");
    if (DEBUG) {Serial.println("---------- SELECT GAMEMODE ----------");}
    
    gameMode = codeNumber();
    while(gameMode > GameModesAmount || gameMode <= 0){
        gameMode = codeNumber(); 
    }

    int time = getNumberInt(2, "Time (min)"); //Waits for time to be entered

    bool succeeded = runGame(gameMode, time);
    lcd.clear();
    if (succeeded){lcd.print("BOM ONTMANTELD");}
    else {lcd.print("BOM ONTPLOFT");}
    waitForNumber();
}

/* ------------------- HELP FUNCTIONS ------------------- */
int waitForNumber() {
    while(codeNumber() == -1) {
    }
    return codeNumber();
}

/* Geeft nummer terug voor gegeven waarde of -1 als het geen nummer is */
int codeNumber() {
  int value = analogRead(codePin);
  for (int i = 0; i < sizeof(codeValues) / sizeof(codeValues[0]); i++) {
    if (value < codeValues[i] + 20 && value > codeValues[i] - 20) return codeKeys[i];
  }
  return -1;
}

int getNumberInt(int length) {return getNumberInt(length, "");}

int getNumberInt(int length, String display) {return getNumberString(length, display).toInt();}

String getNumberString(int length) {return getNumberString(length, "");}

String getNumberString(int length, String display) {
    lcd.clear();
    lcd.print(display);
    lcd.setCursor(1,0);
    bool okay = false;
    String string = "";
    int waarde;
    while (!okay) {
      waarde = codeNumber();
      if (waarde != -1 && waarde != OK) {
        if (string == "" || string.length() == length) {
          string = String(waarde);
          lcd.print("                ");
        }
        else {
          if (DEBUG) Serial.println(waarde);
          string = string + String(waarde);
        }
        lcd.print(string);
        wait(waarde);
      }
      else if (waarde == OK && string != "") {
        okay = true;
        wait(waarde);
      }
    }
    if (DEBUG) {
      Serial.print("Waarde: ");
      Serial.println(string.toInt());
    }
    return string;
}

void wait(int number) {
  while (codeNumber() == number) {}
}

void printRepeat(char character, unsigned int count, String device)
{
  while (count-- > 0)
  {
    if (device == SERIAL && DEBUG == 1)
      Serial.print(character);
    else if (device == LCD)
      lcd.print(character);
  }
}

void plant(int pressPlant) {
  bool planted = false;
  int buttonState = LOW;
  int prevState = LOW;
  unsigned long startTime;
  while (!planted)
  {
    int number = codeNumber();
    if (number == OK)
    {
      buttonState = HIGH;
    }
    else
    {
      buttonState = LOW;
    }
    if (buttonState == HIGH && prevState == LOW)
    { // Als de knop ingedrukt wordt (en dus nog niet ingedrukt was)
      startTime = millis(); // Sla de tijd op wanneer de knop ingedrukt wordt
      if (DEBUG) {Serial.println("Pressed in button");}
      prevState = HIGH; // Onthoud dat de knop ingedrukt is
    }
    else if (buttonState == HIGH && prevState == HIGH)
    { // Als de knop nog steeds ingedrukt is
      unsigned long currentTime = millis();
      int count = (currentTime - startTime) / (pressPlant * 1000 / 32);
      if (count < 16) {
        lcd.setCursor(0, 0);
      }
      else {
        lcd.setCursor(0, 1);
      }
      printRepeat((char)255, count % 16 + 1, LCD);
      if (currentTime - startTime >= pressPlant * 1000) {
        planted = true;
      } // Als de knop voor `pressPlant` seconden is ingedrukt (verschil tussen tijdstip dat de knop werd ingedruk met tijdstip op dit moment) dan is programma klaar (done).
    }
    else if (buttonState == LOW && prevState == HIGH)
    { // Als knop losgelaten wordt (en dus ingedrukt was)
      if (DEBUG) {Serial.println("Released button");}
      prevState = LOW;
      lcd.clear();
    }
  }
}

unsigned int increaseInterval(int totalTime, int currentTime)
{
  int eersteDeel = (totalTime * 3) / 5;
  int tweedeDeel = totalTime - fastBeepTime;
  if (currentTime < eersteDeel)
  {
    return maxInterval;
  }
  else if (currentTime < tweedeDeel)
  {
    return maxInterval - ((maxInterval - minInterval) / (tweedeDeel - eersteDeel)) * (currentTime - eersteDeel);
  }
  else if (currentTime < totalTime)
  {
    return minInterval;
  }
}

void beep()
{
  tone(piezoPin, beepPitch, beepLength);
}

/* ------------------- BOMB STATE FUNCTIONS ------------------- */
int getWire(){
  return getWireSimple();
}

int getWireSimple() {
  int value = analogRead(wirePin);
  for (int i = 0; i < sizeof(wireValues) / sizeof(wireValues[0]); i++){
    if (value < wireValues[i] + 20 && value > wireValues[i] - 20)
      return i + 1;
  }
  return -1;
}

int combinatieBanana() {
  int combinatie[20];
  memset(combinatie, 0, sizeof(combinatie));
  int index = 0;
  int waarschijnlijk = 0; // reset en alles op nul voor elke keer als je terug opnieuw checked
  int biggest = 0;

  bananaPin(banana1, sizeof(banana1) / sizeof(banana1[0]), bananaPin1, combinatie, index);
  bananaPin(banana2, sizeof(banana2) / sizeof(banana2[0]), bananaPin2, combinatie, index); // checkt elke pin en zet de combinatie die mogelijk is bij die waarde in de array "combinatie"
  bananaPin(banana3, sizeof(banana3) / sizeof(banana3[0]), bananaPin3, combinatie, index);
  bananaPin(banana4, sizeof(banana4) / sizeof(banana4[0]), bananaPin4, combinatie, index);

  if (DEBUG) {
    Serial.print("Combinatie: ");
    for (int k = 0; k < sizeof(combinatie) / sizeof(combinatie[0]); k++)
    {
      Serial.print(combinatie[k]);
    }
    Serial.println();
  }
  for (int j = 0; j < 20; j++)
  { // leest welke dat meest waarschijnlijk voorkomt in "combinatie"
    int count = 0;
    for (int i = 0; i < 20; i++)
    {
      if (combinatie[j] == 0)
      {
        break;
      }
      if (combinatie[j] == combinatie[i])
      {
        // Serial.print("Count: ");
        // Serial.println(count);
        count++;
      }
    }
    if (biggest < count)
    {
      biggest = count;
      waarschijnlijk = combinatie[j];
      if (DEBUG) {
        Serial.print("Biggest: ");
        Serial.println(biggest);
        Serial.print("Waarschijnlijk: ");
        Serial.println(waarschijnlijk);
      }
    }
  }
  return waarschijnlijk - 100;
}

void bananaPin(const double *banana, int length, int pin, int *combinatie, int index)
{
  double value = analogRead(pin);
  value = value * 5 / 1024; // in voltage!
  if (DEBUG == 1)
    Serial.println(value);
  int j;
  for (int i = 0; i < length; i++)
  {
    if (value < 100 && value < banana[i] + 0.05 && value > banana[i] - 0.05)
    {            // binnen een marge van 0.05V checken of overeenkomt
      j = i + 1; // zie naar volgend element
      while (banana[j] > 100)
      {                                // als dat hoger is als 1000 en dus geen spanning, maar combinatieNr is
        combinatie[index] = banana[j]; // voeg je deze toe aan de mogelijke combinaties
        index++;
        j++;
      }
      break; // als je de juiste waarde al gevonden hebt, break je de for loop, moet je niet verder zoeken!
    }
  }
}

int punishment(int time) {
  tone(piezoPin, 500, 100);
  return time;
}

/* ------------------- GAMEMODE PRESETS ------------------- */
Preset getPreset(int gameMode){
    if (gameMode == 2) {
        int lengteCode = getNumberInt(1, "Lengte code:");
        lcd.clear();
        lcd.print("Code van " + String(lengteCode));
        lcd.setCursor(1,0);
        lcd.print("cijfers ingesteld");
        delay(1000);
        String code = getNumberString(lengteCode, "Code:");
        lcd.clear();
        lcd.print("Code " + code);
        lcd.setCursor(1,0);
        lcd.print("ingesteld");
        delay(1000);
        int amQuestions = getNumberInt(1, "#Vragen:");
        lcd.clear();
        lcd.print(amQuestions + " vragen");
        lcd.setCursor(1,0);
        lcd.print("ingesteld");
        delay(1000);
        int plantTime = getNumberInt(2, "Tijd planten sec");
        lcd.clear();
        lcd.print(amQuestions + " vragen");
        lcd.setCursor(1,0);
        lcd.print("ingesteld");
        delay(1000);
        Preset preset("",-1,-1, amQuestions, plantTime);
        return preset;
    }
    else if (gameMode == 1) {
        int lengteCode = getNumberInt(1, "Lengte code");
        lcd.clear();
        lcd.print("Code van " + String(lengteCode));
        lcd.setCursor(1,0);
        lcd.print("cijfers ingesteld");
        delay(1000);
        String code = getNumberString(lengteCode, "Code:");
        lcd.clear();
        lcd.print("Code " + code);
        lcd.setCursor(1,0);
        lcd.print("ingesteld");
        delay(1000);
        int bananaPreset = getNumberInt(1, "banana");
        lcd.clear();
        lcd.print("Bananpreset " + String(bananaPreset));
        lcd.setCursor(1,0);
        lcd.print("ingesteld");
        delay(1000);
        int wire = getNumberInt(1, "wire");
        lcd.clear();
        lcd.print("Draad " + String(wire));
        lcd.setCursor(1,0);
        lcd.print("ingesteld");
        delay(1000);
        Preset preset(code, bananaPreset, wire);
        return preset;
    }
    else {
        Preset preset("",-1,-1);
        return preset; 
    }
}

/* ------------------- RUN GAME FUNCTIONS ------------------- */
void waitForPlantCode(String code) {
    bool plantedCodeDone = false;
    String plantedCode = "";
    lcd.clear();
    lcd.print("Geef code in");
    lcd.setCursor(1,0);
    while(!plantedCodeDone){
      int nr = codeNumber();
      if (nr != -1 && nr != OK)
      {
        wait(nr);
        if (DEBUG) {
          Serial.print("Nummer: ");
          Serial.println(nr);
        }
        plantedCode += String(nr);
        lcd.print(plantedCode);
        if (plantedCode.length() == code.length())
        {
          if (plantedCode == code)
          {
            plantedCodeDone = true;
            digitalWrite(ledPin1, HIGH);
            if (DEBUG) Serial.println("Code completed");
          }
          else
          {
            tone(piezoPin, 500, 500);
            plantedCode = "";
            lcd.print("                ");
          }
        }
      }
    }
}

/* ------------------- RUN GAME ------------------- */
bool runGame(int gameMode, int time) {
    Preset preset = getPreset(gameMode);

    Question questions[preset.getAmQuestions()];
    for (int i=0; i<preset.getAmQuestions(); i++){
      questions[i] = Question(MEDIUM);
    }

    waitForPlantCode(preset.getCode());
    plant(preset.getPlantTime());

    bool codeDone = false;
    bool wireConnectDone = false;
    bool colorWireDone = false;
    bool questionsDone = (preset.getAmQuestions() == 0);

    String ingCode; //Houdt de ingegeven code bij
    int colorWire; //Houdt de uitgetrokken draad bij
    String answer = "";

    int questionIndex = 0;
    int prevQuestionIndex = -1;

    int counter = 0;
    int prevCounter = 0;

    unsigned int interval = maxInterval;
    unsigned long startProgramma = millis();
    unsigned long startCycle = millis();
    while ((!codeDone || !wireConnectDone || !colorWireDone || questionsDone) && (millis() - startProgramma < time * 60 * 1000L))
  {
    if (millis() - startCycle > interval)
    {
      beep();
      startCycle = millis();
    }
    counter = (millis() - startProgramma) / 1000;
    if (counter != prevCounter)
    {
      prevCounter = counter;
      interval = increaseInterval(time*60, counter);
      lcd.clear();
      lcd.print(time*60 - counter);
    }
    if (!codeDone)
    {
      int number = codeNumber();
      if (number != -1 && number != OK)
      {
        wait(number);
        if (DEBUG) {
          Serial.print("Nummer: ");
          Serial.println(number);
        }
        ingCode += String(number);
        if (ingCode.length() == preset.getCode().length())
        {
          if (ingCode == preset.getCode())
          {
            codeDone = true;
            digitalWrite(ledPin1, HIGH);
            if (DEBUG) Serial.println("Code completed");
          }
          else
          {
            tone(piezoPin, 500, 500);
            ingCode = "";
          }
        }
      }
    }
    if (!wireConnectDone)
    {
      int number = codeNumber();
      if (number == OK)
      {
        wait(number);
        int wirePres = combinatieBanana();
        if (DEBUG)
        {
          Serial.print("Banana combinatie: ");
          Serial.println(wirePres);
        }
        if (wirePres == preset.getBananaPreset())
        {
          wireConnectDone = true;
          digitalWrite(ledPin2, HIGH);
          if (DEBUG) Serial.println("Banana wires zijn juist verbonden");
        }
      }
    }
    if (!colorWireDone)
    {
      colorWire = getWire();
      if (colorWire != -1)
      {
        if (colorWire == preset.getWire())
        {
          colorWireDone = true;
          digitalWrite(ledPin3, HIGH);
          if (DEBUG == 1)
            Serial.println("Right wire pulled out");
        }
        // else
        // {
        //   wiresOut[amWireOut] = colorWire;
        //   amWireOut++;
        // }
      }
    }
    if (!questionsDone) {
      if (questionIndex != prevQuestionIndex){
        lcd.clear();
        lcd.print(questions[questionIndex].getQuestion());
        prevQuestionIndex = questionIndex;
      }
      int answerPart = codeNumber();
      if (answerPart != -1 && answerPart != OK) {
        wait(answerPart);
        answer += String(answerPart);
      }
      else if (answerPart == OK) {
        if (questions[questionIndex].checkAnswer(answer.toInt())){
          questionIndex++;
          if (questionIndex == preset.getAmQuestions()) questionsDone = true;
        }
      }
    }
  }
  if (millis() - startProgramma < time * 60 * 1000L) {
    return true;
  }
  else {
    return false;
  }
}
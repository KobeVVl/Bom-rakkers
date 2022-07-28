//TODO: LAMPJES IN JUISTE VOLGORDE
//      FOUT MOET STRAF OPLEVEREN (DRAAD EN BANANAS)

#define DEBUG false
const int OK = 200;
#define GameModesAmount 2
#define LCD "lcd"

/* ------------------- INCLUDE ------------------- */
#include <LiquidCrystal.h>
#include <Preset.h>

/* ------------------- DATA ------------------- */
/* code keyboard */
const int codeValues[] = {414, 511, 696, 839, 928, 234, 1013, 90, 180, 612, 316};
const int codeKeys[] = {OK, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};


/* bananenklemmen waardes */
const double banana1[] = {4.3, 101, 4.2, 102, 4.1, 103, 3.1, 104, 105, 110, 3.2, 106, 4.0, 107, 3.8, 108, 112, 3.7, 109, 113, 114, 115, 3.5, 111};
const double banana2[] = {4.3, 101, 4.2, 102, 3.8, 103, 104, 108, 3.6, 105, 106, 109, 4.0, 107, 3.5, 110, 2.7, 111, 114, 3.1, 112, 2.6, 113, 3.2, 115};
const double banana3[] = {4.1, 103, 4.0, 107, 3.8, 104, 105, 3.7, 106, 3.6, 101, 114, 3.5, 110, 111, 3.3, 102, 3.2, 113, 115, 2.7, 108, 109, 2.5, 112};
const double banana4[] = {4.0, 107, 3.8, 103, 104, 110, 111, 3.7, 109, 114, 3.6, 101, 105, 3.5, 108, 3.2, 113, 3.1, 112, 2.7, 106, 2.6, 115, 2.5, 102};

/* draden waardes */
const int wireValues[] = {930, 839, 696, 512, 320, 179};
const String wireKeys[] = {"blauw", "groen", "rood", "oranje", "geel", "paars"};
 
/* ------------------- PINS ------------------- */
#define bananaPin1 A5
#define bananaPin2 A4
#define bananaPin3 A3
#define bananaPin4 A2
#define wirePin A1  
#define codePin A0  //wit
#define piezoPin 2 //wit
#define ledPin1 3  //paars
#define ledPin2 4  //blauw
#define ledPin3 5  //groen
#define rs 6 //wit
#define en 7 //paars
#define d4 8 //blauw
#define d5 9 //groen
#define d6 10 //geel
#define d7 11  //oranje

/* ------------------- CONSTANTS ------------------- */
const int maxInterval = 5000;
const int minInterval = 150;
const int fastBeepTime = 5; // De laaste x seconden beept de bom met minInterval er tussen (snel)

const unsigned int beepPitch = 3000;
const unsigned long beepLength = 75;

/* ------------------- GLOBAL ------------------- */
Preset previousPreset("0000", -1, -1, 3, 15);
int previousTime = 60;
bool reusePrevious = false;

/* ------------------- SETUP ------------------- */
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
void setup() {
    if (DEBUG) Serial.begin(115200);
    lcd.begin(16,2);
    pinMode(ledPin1, OUTPUT);
    pinMode(ledPin2, OUTPUT);
    pinMode(ledPin3, OUTPUT);
    if (DEBUG) {Serial.println("---------- SETUP DONE ----------");}
}

/* ------------------- LOOP ------------------- */
void loop(){
    int gameMode;    
    
    if (reusePrevious) {
      bool succeeded = runGame(-1, previousTime, true);
    }

    lcd.clear();
    lcd.print("Select gamemode");
    if (DEBUG) {Serial.println("---------- SELECT GAMEMODE ----------");}
    
    gameMode = codeNumber();
    while(gameMode > GameModesAmount || gameMode <= 0){
        gameMode = codeNumber(); 
    }
    if (DEBUG) {Serial.print("Gamemode "); Serial.print(gameMode); Serial.println(" selected");}
    lcd.clear();
    lcd.print("Gamemode " + String(gameMode));
    lcd.setCursor(0,1);
    lcd.print("ingesteld");
    delay(2000);

    int tijd = getNumberInt(4, "Sec tot ontpl."); //Waits for time to be entered
    previousTime = tijd;
    if (DEBUG) {
      Serial.print("Maximale tijd is ");
      Serial.print(tijd);
      Serial.println(" seconden");
    }
    lcd.clear();
    lcd.print(String(tijd) + " seconden");
    lcd.setCursor(0,1);
    lcd.print("tot bom ontploft");
    delay(2000);

    bool succeeded = runGame(gameMode, tijd, false);
    lcd.clear();
    if (succeeded){lcd.print("BOM ONTMANTELD"); }
    else {lcd.print("BOM ONTPLOFT"); tone(piezoPin, beepPitch, 2000);}
    
    resetSystem();

    wait(waitForNumber()); //WACHT TOT ER EEN KNOP INGEDRUKT WORDT OM DOOR TE GAAN

    lcd.clear();
    lcd.print("Hergebruik inst?");
    lcd.setCursor(0,1);
    lcd.print("0 = NEE; 1 = JA");
    int reuse = codeNumber();
    while(reuse > 1 || gameMode < 0){
        reuse = codeNumber(); 
    }
    reusePrevious = (reuse == 0)? false: true;

}

/* ------------------- HELP FUNCTIONS ------------------- */
int waitForNumber() {
    while(codeNumber() == -1) {
    }
    return codeNumber();
}

/* Geeft nummer terug voor gegeven waarde of -1 als het geen nummer is */
int codeNumber() {
  int waarde = -1;
  while(analogRead(codePin) == 0){}

  bool test = false;
  int value;
  while(!test){
    value = analogRead(codePin);
    delay(10);
    if(analogRead(codePin) == value){
      test = true;
    }
  }
  for (int i = 0; i < sizeof(codeValues) / sizeof(codeValues[0]); i++) {
    if (value < codeValues[i] + 10 && value > codeValues[i] - 10){
         waarde = codeKeys[i];
         delay(150);
    }
  }
  return waarde;
}

int getNumberInt(int lengte) {return getNumberInt(lengte, "");}

int getNumberInt(int lengte, String disp) {return getNumberString(lengte, disp).toInt();}

String getNumberString(int lengte) {return getNumberString(lengte, "");}

String getNumberString(int lengte, String disp) {
    lcd.clear();
    lcd.print(disp);
    lcd.setCursor(0,1);
    bool okay = false;
    String string = "";
    int waarde;
    while (!okay) {
      while(analogRead(codePin) != 0){}
      waarde = codeNumber();
      if (waarde != -1 && waarde != OK) {
        if (string == "" || string.length() == lengte) {
          string = String(waarde);
          lcd.setCursor(0,1);
          lcd.print("                ");
        }
        else {
          if (DEBUG) Serial.println(waarde);
          string = string + String(waarde);
        }
        lcd.setCursor(0,1);
        lcd.print(string);
        wait(waarde);
      }
      else if (waarde == OK && string != "") {
        okay = true;
        wait(waarde);
      }
    }
    if (DEBUG) {
      Serial.print(disp==""?"Waarde:":disp);
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

void victoryBlink(){
  setGreenLight(true, true, true);
  delay(500);
  setGreenLight(false, false, false);
  delay(500);
  setGreenLight(true, true, true);
  delay(500);
  setGreenLight(false, false, false);
  delay(500);
  setGreenLight(true, true, true);
  delay(500);
  setGreenLight(false, false, false);
  delay(500);
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
    if (gameMode == 2) {//PLANT AND DEFUSE MET VRAGEN
        int lengteCode = 0;
        while (lengteCode == 0) {
          lengteCode = getNumberInt(1, "Lengte code:");
          if (lengteCode == 0) {
            lcd.clear();
            lcd.print("Lengte mag niet");
            lcd.setCursor(0,1);
            lcd.print("0 zijn");
            delay(1500);
          }
        }
        lcd.clear();
        lcd.print("Code van " + String(lengteCode));
        lcd.setCursor(0,1);
        lcd.print("cijfers ingesteld");
        delay(1000);
        String code = getNumberString(lengteCode, "Code:");
        lcd.clear();
        lcd.print("Code " + code);
        lcd.setCursor(0,1);
        lcd.print("ingesteld");
        delay(1000);
        int amQuestions = getNumberInt(1, "#Vragen:");
        lcd.clear();
        lcd.print(String(amQuestions) + " vragen");
        lcd.setCursor(0,1);
        lcd.print("ingesteld");
        delay(1000);
        int plantTime = getNumberInt(2, "Tijd planten sec");
        lcd.clear();
        lcd.print(String(plantTime) + " sec planten");
        lcd.setCursor(0,1);
        lcd.print("ingesteld");
        delay(1000);
        Preset preset(code,-1,-1, amQuestions, plantTime);
        return preset;
    }
    else if (gameMode == 1) {//DEFUSE (LANG)
        int lengteCode = 0;
        while (lengteCode == 0) {
          lengteCode = getNumberInt(1, "Lengte code:");
          if (lengteCode == 0) {
            lcd.clear();
            lcd.print("Lengte mag niet");
            lcd.setCursor(0,1);
            lcd.print("0 zijn");
            delay(1500);
          }
        }
        lcd.clear();
        lcd.print("Code van " + String(lengteCode));
        lcd.setCursor(0,1);
        lcd.print("cijfers ingesteld");
        delay(1000);
        String code = getNumberString(lengteCode, "Code:");
        lcd.clear();
        lcd.print("Code " + code);
        lcd.setCursor(0,1);
        lcd.print("ingesteld");
        delay(1000);
        int bananaPreset = getNumberInt(1, "banana");
        lcd.clear();
        lcd.print("Bananpreset " + String(bananaPreset));
        lcd.setCursor(0,1);
        lcd.print("ingesteld");
        delay(1000);
        int wire = getNumberInt(1, "wire");
        lcd.clear();
        lcd.print("Draad " + String(wire));
        lcd.setCursor(0,1);
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
    if (DEBUG) {Serial.print("WACHTEN TOT CODE "); Serial.print(code); Serial.println(" IS INGEGEVEN");}
    bool plantedCodeDone = (code == "");
    String plantedCode = "";
    lcd.clear();
    lcd.print("Geef code in");
    lcd.setCursor(0,1);
    while(!plantedCodeDone){
      int nr = codeNumber();
      if (nr != -1 && nr != OK)
      {
        if (DEBUG) {
          Serial.print("Nummer: ");
          Serial.println(nr);
        }
        plantedCode += String(nr);
        lcd.setCursor(0,1);
        lcd.print(plantedCode);
        if (plantedCode.length() == code.length())
        {
          if (plantedCode == code)
          {
            plantedCodeDone = true;
            if (DEBUG) Serial.println("Code completed");
          }
          else
          {
            tone(piezoPin, 500, 500);
            plantedCode = "";
            lcd.setCursor(0,1);
            lcd.print("                ");
          }
        }
        wait(nr);
      }
    }
}

void plant(int pressPlant) {
  bool planted = false;
  int buttonState = LOW;
  int prevState = LOW;
  unsigned long startTime;
  lcd.clear();
  lcd.print("Houd OK " + String(pressPlant) + " sec");
  lcd.setCursor(0,1);
  lcd.print("om te planten");
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
      lcd.clear();
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
      lcd.print("Houd OK " + String(pressPlant) + " sec");
      lcd.setCursor(0,1);
      lcd.print("om te planten");
    }
  }
}

void setGreenLight(bool codeDone, bool wireConnectDone, bool colorWireDone) {
  int lightsOn = 0;
  if (codeDone) lightsOn+=1;
  if (wireConnectDone) lightsOn+=1;
  if (colorWireDone) lightsOn+=1;
  switch (lightsOn) {
    case 0:
      digitalWrite(ledPin1, LOW); digitalWrite(ledPin2, LOW); digitalWrite(ledPin3, LOW);
    case 1:
      digitalWrite(ledPin1, HIGH); digitalWrite(ledPin2, LOW); digitalWrite(ledPin3, LOW);
    case 2:
      digitalWrite(ledPin1, HIGH); digitalWrite(ledPin2, HIGH); digitalWrite(ledPin3, LOW);
    case 3:
      digitalWrite(ledPin1, HIGH); digitalWrite(ledPin2, HIGH); digitalWrite(ledPin3, HIGH);
  } 
}

/* ------------------- RUN GAME ------------------- */
bool runGame(int gameMode, int tijd, bool reusePrevious) {
    int oorsporkelijkeTijd = tijd;
    Preset preset;
    if (reusePrevious) {
      preset = previousPreset;
    }
    else {
      preset = getPreset(gameMode);
    }

    previousPreset = preset;
    
    Question questions[preset.getAmQuestions()];
    for (int i=0; i<preset.getAmQuestions(); i++){
      questions[i] = Question(EASY);
    }

    waitForPlantCode(preset.getCode());
    plant(preset.getPlantTime());
    wait(OK);

    bool codeDone = (gameMode == 2);
    bool wireConnectDone = (preset.getBananaPreset() == -1);
    bool colorWireDone = (preset.getWire() == -1);
    bool questionsDone = (preset.getAmQuestions() == 0);

    String ingCode; //Houdt de ingegeven code bij
    int colorWire; //Houdt de uitgetrokken draad bij
    String answer = "";

    int questionIndex = 0;
    int prevQuestionIndex = -1;

    int counter = 0;
    int prevCounter = 0;

    if (DEBUG){
      Serial.println("------------ GAME PRESET ------------");
      Serial.print("Code: "); Serial.println(preset.getCode());
      Serial.print("Banana: "); Serial.println(preset.getBananaPreset());
      Serial.print("Wire: "); Serial.println(preset.getWire());
      Serial.print("PlantTime: "); Serial.println(preset.getPlantTime());
      Serial.print("#Questions: "); Serial.println(preset.getAmQuestions());
      Serial.print("codeDone: "); Serial.println(codeDone);
      Serial.print("wireConnectDone: "); Serial.println(wireConnectDone);
      Serial.print("colorWireDone: "); Serial.println(colorWireDone);
      Serial.print("questionsDone: "); Serial.println(questionsDone);
      Serial.println("-----------------------------");
    }

    unsigned int interval = maxInterval;
    unsigned long startProgramma = millis();
    unsigned long startCycle = millis();
    while ((!codeDone || !wireConnectDone || !colorWireDone || !questionsDone) && (millis() - startProgramma < tijd * 1000L))
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
      interval = increaseInterval(tijd, counter);
      if (preset.getAmQuestions() <= 0) {
        lcd.clear();
        lcd.print(tijd - counter);
      }
      if (DEBUG){
        Serial.println("-----------------------------");
        Serial.print("codeDone: "); Serial.println(codeDone);
        Serial.print("wireConnectDone: "); Serial.println(wireConnectDone);
        Serial.print("colorWireDone: "); Serial.println(colorWireDone);
        Serial.print("questionsDone: "); Serial.println(questionsDone);
        Serial.println("-----------------------------");
      }
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
            setGreenLight(codeDone, wireConnectDone, colorWireDone);
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
          setGreenLight(codeDone, wireConnectDone, colorWireDone);
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
          setGreenLight(codeDone, wireConnectDone, colorWireDone);
          if (DEBUG)
            Serial.println("Right wire pulled out");
        }
        else
        {
          tijd -= oorsporkelijkeTijd/10;
        }
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
        if (answer.length() == 3) {
          lcd.setCursor(0,1);
          lcd.print("                ");
          answer = String(answerPart);
        }
        else {answer += String(answerPart);}
        lcd.setCursor(0,1);
        lcd.print(answer);
        if (DEBUG){
          Serial.print("Current answer: ");
          Serial.println(answer);
        }
        wait(answerPart);
      }
      else if (answerPart == OK) {
        if (DEBUG){
          Serial.println("Pressed OK to check answer");
          Serial.print("Answer is correct? ");
          Serial.println(questions[questionIndex].checkAnswer(answer.toInt()));
        }
        if (questions[questionIndex].checkAnswer(answer.toInt())){
          questionIndex++;
          if (questionIndex == preset.getAmQuestions()) questionsDone = true;
        }
        else {
          lcd.setCursor(0,1);
          lcd.print("                ");
        }
        answer = "";
        wait(answerPart);
      }
    }
  }
  if (millis() - startProgramma < tijd * 1000L) {
    return true;
  }
  else {
    return false;
  }
}

/* ---------------- RESET SYSTEM ----------------- */
void resetSystem(){
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  lcd.clear();
  noTone(piezoPin);
}

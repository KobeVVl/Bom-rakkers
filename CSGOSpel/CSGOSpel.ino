const int OK = 200;

/* ------------------- INCLUDE ------------------- */
#include <LiquidCrystal.h>
#include <Preset.h>

/* ------------------- DATA ------------------- */
/* code keyboard */
const int codeValues[] = {410, 508, 690, 832, 918, 233, 1005, 90, 180, 608, 314};
const int codeKeys[] = {OK, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

/* ------------------- PINS ------------------- */
#define bananaPin1 A5
#define bananaPin2 A4
#define bananaPin3 A3
#define bananaPin4 A2
#define wirePin A1  
#define codePin A0  //wit
#define piezoPin 2 //wit
#define ledPin1 5  //paars
#define ledPin2 3  //blauw
#define ledPin3 4  //groen
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
    lcd.begin(16,2);
    pinMode(ledPin1, OUTPUT);
    pinMode(ledPin2, OUTPUT);
    pinMode(ledPin3, OUTPUT);
}

/* ------------------- LOOP ------------------- */
void loop(){
    bool succeeded;

    if (reusePrevious) {
      succeeded = CSGOGame(previousTime, true);
    }
    
    else {
      int tijd = getNumberInt(4, "Sec tot ontpl."); //Waits for time to be entered
      previousTime = tijd;
      lcd.clear();
      lcd.print(String(tijd) + " seconden");
      lcd.setCursor(0,1);
      lcd.print("tot bom ontploft");
      delay(2000);

      succeeded = CSGOGame(tijd, false);
    }
    lcd.clear();
    if (succeeded){lcd.print("BOM ONTMANTELD"); victoryBlink();}
    else {lcd.print("BOM ONTPLOFT"); tone(piezoPin, beepPitch, 2000);}
    delay(2000);
    
    resetSystem();
    lcd.print("Druk op een knop");
    lcd.setCursor(0,1);
    lcd.print("om verder te gaan");
    wait(waitForNumber()); //WACHT TOT ER EEN KNOP INGEDRUKT WORDT OM DOOR TE GAAN
    delay(1000);
    lcd.clear();
    lcd.print("Hergebruik inst?");
    lcd.setCursor(0,1);
    lcd.print("0 = NEE; 1 = JA");
    int reuse = codeNumber();
    while(reuse > 1 || reuse < 0){
        reuse = codeNumber(); 
    }
    reusePrevious = (reuse == 0)? false: true;
}

void printRepeat(char character, unsigned int count)
{
  while (count-- > 0)
  {
    lcd.print(character);
  }
}

/* ------------------- NUMPAD FUNCTIONS ------------------- */
int codeNumber() {
  int value1 = analogRead(codePin);
  delay(20);
  int value2 = analogRead(codePin);

  if(value2 > value1 + 20 || value2 < value1 - 20){
    return -1;
  }
  
  for (int i = 0; i < sizeof(codeValues) / sizeof(codeValues[0]); i++) {
    if (value2 < codeValues[i] + 20 && value2 > codeValues[i] - 20){
         return codeKeys[i];
         //delay(150);
    }
  }
  return -1;
}

void wait(int number) {
  while (codeNumber() == number) {}
}

int waitForNumber() {
    while(codeNumber() == -1) {
    }
    return codeNumber();
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
      waarde = codeNumber();
      if (waarde != -1 && waarde != OK) {
        if (string == "" || string.length() == lengte) {
          string = String(waarde);
          lcd.setCursor(0,1);
          lcd.print("                ");
        }
        else {
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
    return string;
}

Preset getPreset(){
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

void waitForPlantCode(String code) {
    bool plantedCodeDone = (code == "");
    String plantedCode = "";
    lcd.clear();
    lcd.print("Geef code in");
    lcd.setCursor(0,1);
    while(!plantedCodeDone){
      int nr = codeNumber();
      if (nr != -1 && nr != OK)
      {
        plantedCode += String(nr);
        lcd.setCursor(0,1);
        lcd.print(plantedCode);
        if (plantedCode.length() == code.length())
        {
          if (plantedCode == code)
          {
            plantedCodeDone = true;
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
      printRepeat((char)255, count % 16 + 1);
      if (currentTime - startTime >= pressPlant * 1000) {
        planted = true;
      } // Als de knop voor `pressPlant` seconden is ingedrukt (verschil tussen tijdstip dat de knop werd ingedruk met tijdstip op dit moment) dan is programma klaar (done).
    }
    else if (buttonState == LOW && prevState == HIGH)
    { // Als knop losgelaten wordt (en dus ingedrukt was)
      prevState = LOW;
      lcd.clear();
      lcd.print("Houd OK " + String(pressPlant) + " sec");
      lcd.setCursor(0,1);
      lcd.print("om te planten");
    }
  }
}

void beep()
{
  tone(piezoPin, beepPitch, beepLength);
}


unsigned int decreaseInterval(int totalTime, int currentTime)
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

void setGreenLight(int lights) {
  switch (lights) {
    case 0:
      digitalWrite(ledPin1, LOW); digitalWrite(ledPin2, LOW); digitalWrite(ledPin3, LOW); break;
    case 1:
      digitalWrite(ledPin1, HIGH); digitalWrite(ledPin2, LOW); digitalWrite(ledPin3, LOW); break;
    case 2:
      digitalWrite(ledPin1, HIGH); digitalWrite(ledPin2, HIGH); digitalWrite(ledPin3, LOW); break;
    case 3:
      digitalWrite(ledPin1, HIGH); digitalWrite(ledPin2, HIGH); digitalWrite(ledPin3, HIGH); break;
  }
}

void victoryBlink(){
  setGreenLight(3);
  delay(500);
  setGreenLight(0);
  delay(500);
  setGreenLight(3);
  delay(500);
  setGreenLight(0);
  delay(500);
  setGreenLight(3);
  delay(500);
  setGreenLight(0);
  delay(500);
}

/* ------------------ RUNGAME ----------------- */
bool CSGOGame(int tijd, bool reusePrevious) {
    int oorsporkelijkeTijd = tijd;
    Preset preset;
    if (reusePrevious) {
      preset = previousPreset;
    }
    else {
      preset = getPreset();
    }

    previousPreset = preset;
    
    Question questions[preset.getAmQuestions()];
    for (int i=0; i<preset.getAmQuestions(); i++){
      questions[i] = Question(EASY);
    }

    waitForPlantCode(preset.getCode());
    plant(preset.getPlantTime());
    wait(OK);

    bool questionsDone = false;

    String answer = "";

    int questionIndex = 0;
    int prevQuestionIndex = -1;

    int counter = 0;
    int prevCounter = 0;

    unsigned int interval = maxInterval;
    unsigned long startProgramma = millis();
    unsigned long startCycle = millis();
    while (!questionsDone && (millis() - startProgramma < tijd * 1000L))
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
      interval = decreaseInterval(tijd, counter);
      if (preset.getAmQuestions() <= 0) {
        lcd.clear();
        lcd.print(tijd - counter);
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
        wait(answerPart);
      }
      else if (answerPart == OK) {
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

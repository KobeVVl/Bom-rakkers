const int OK = 200;

/* ------------------- INCLUDE ------------------- */
#include <LiquidCrystal.h>
#include <Preset.h>

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

/* ------------------- DATA ------------------- */
/* code keyboard */
const int codeValues[] = {414, 511, 696, 839, 928, 234, 1013, 90, 180, 612, 316};
const int codeKeys[] = {OK, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

/* ------------------- CONSTANTS ------------------- */
const int maxInterval = 5000;
const int minInterval = 150;
const int fastBeepTime = 5; // De laaste x seconden beept de bom met minInterval er tussen (snel)

const unsigned int beepPitch = 3000;
const unsigned long beepLength = 75;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
void setup() {
    lcd.begin(16,2);
    pinMode(ledPin1, OUTPUT);
    pinMode(ledPin2, OUTPUT);
    pinMode(ledPin3, OUTPUT);
}

void loop() {
    //int tijd = getNumberInt(4, "Sec tot ontpl."); //Waits for time to be entered
    int tijd = 3600;
    lcd.clear();
    lcd.print(String(tijd) + " seconden");
    lcd.setCursor(0,1);
    lcd.print("tot bom ontploft");
    delay(2000);
    bool succeeded = nachtSpel(tijd);
    lcd.clear();
    if (succeeded){lcd.print("BOM ONTMANTELD"); victoryBlink();}
    else {lcd.print("BOM ONTPLOFT"); tone(piezoPin, beepPitch, 2000);}
    delay(2000);
    resetSystem();
}

bool nachtSpel(int tijd) {
  unsigned long OKinterval = 1500L;
  unsigned long Zerointerval = 5000L;

  int lightsOn = 0;

  bool done = false;

  int counter;
  int prevCounter;

  bool OKPressed = false;
  bool OKwasPressed = false;
  unsigned long startOKPressed;
  bool ZeroPressed = false;
  bool ZerowasPressed = false;
  unsigned long startZeroPressed;

  unsigned long start = millis();
  unsigned long startCycle = start;
  unsigned long interval = maxInterval;
  unsigned long current = start;
  while(current - start < tijd * 1000L && lightsOn < 3) {
    if (current - startCycle > interval)
    {
      tone(piezoPin, beepPitch, beepLength);
      startCycle = current;
    }
    counter = (current - start) / 1000;
    if (counter != prevCounter)
    {
      prevCounter = counter;
      interval = decreaseInterval(tijd, counter);

      if (!OKPressed && !ZeroPressed) {
        lcd.clear();
        lcd.print(tijd - counter);
      }
    }

    int nummer = codeNumber();
    if (nummer == OK) {
      OKPressed = true;
    }
    else {
      OKPressed = false;
    }
    if (nummer == 0) {
      ZeroPressed = true;
    }
    else {
      ZeroPressed = false;
    }
    
    if (nummer == 7) {
      if (tijd > 300) tijd -= 300;
      wait(nummer);
    }
    else if (nummer == 6) {
      tijd += 300;
      wait(nummer);
    }

    if (OKPressed && !OKwasPressed) {
      OKwasPressed = true;
      startOKPressed = current;
    }
    else if (OKPressed && OKwasPressed) {
      int count = (current - startOKPressed) / (OKinterval / 32);
      if (count < 16) {
        lcd.setCursor(0, 0);
      }
      else {
        lcd.setCursor(0, 1);
      }
      printRepeat((char)255, count % 16 + 1);
      if (current - startOKPressed > OKinterval){
        OKwasPressed = false;
        OKPressed = false;
        startOKPressed = current;
        if (lightsOn < 3) lightsOn++;
        setGreenLight(lightsOn);
        wait(OK);
      }
    }
    else if (!ZeroPressed && ZerowasPressed) {
      OKwasPressed = false;
    }

    if (ZeroPressed && !ZerowasPressed) {
      ZerowasPressed = true;
      startZeroPressed = current;
    }
    else if (ZeroPressed && ZerowasPressed) {
      int count = (current - startZeroPressed) / (Zerointerval / 32);
      if (count < 16) {
        lcd.setCursor(0, 0);
      }
      else {
        lcd.setCursor(0, 1);
      }
      printRepeat((char)255, count % 16 + 1);
      if (current - startZeroPressed > Zerointerval){
        ZerowasPressed = false;
        ZeroPressed = false;
        startZeroPressed = current;
        wait(0);
        tijd = (current - start) / 1000 + 3;
      }
    }
    else if (!ZeroPressed && ZerowasPressed) {
      ZerowasPressed = false;
    }
    current = millis();
  }
  if (current - start < tijd * 1000L) {
    return true;
  }
  else {
    return false;
  }
}

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

void setGreenLight(bool codeDone, bool wireConnectDone, bool colorWireDone) {
  int lightsOn = 0;
  if (codeDone) lightsOn+=1;
  if (wireConnectDone) lightsOn+=1;
  if (colorWireDone) lightsOn+=1;
  setGreenLight(lightsOn);
}

void printRepeat(char character, unsigned int count)
{
  while (count-- > 0)
  {
    lcd.print(character);
  }
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

void resetSystem(){
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  lcd.clear();
  noTone(piezoPin);
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

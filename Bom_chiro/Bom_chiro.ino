#define DEBUG 1

#include <Preset.h>
#include <LiquidCrystal.h>

#define SERIAL "serial"
#define LCD "lcd"
#define OK 200
/* ------------------- DATA ------------------- */
/* code keyboard */
const int codeValues[] = {90, 175, 234, 320, 411, 507, 616, 696, 838, 929, 1013};
const int codeKeys[] = {OK, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

/* bananenklemmen waardes */
const double banana1[] = {4.3, 101, 4.2, 102, 4.1, 103, 3.1, 104, 105, 110, 3.2, 106, 4.0, 107, 3.8, 108, 112, 3.7, 109, 113, 114, 115, 3.5, 111};
const double banana2[] = {4.3, 101, 4.2, 102, 3.8, 103, 104, 108, 3.6, 105, 106, 109, 4.0, 107, 3.5, 110, 2.7, 111, 114, 3.1, 112, 2.6, 113, 3.2, 115};
const double banana3[] = {4.1, 103, 4.0, 107, 3.8, 104, 105, 3.7, 106, 3.6, 101, 114, 3.5, 110, 111, 3.3, 102, 3.2, 113, 115, 2.7, 108, 109, 2.5, 112};
const double banana4[] = {4.0, 107, 3.8, 103, 104, 110, 111, 3.7, 109, 114, 3.6, 101, 105, 3.5, 108, 3.2, 113, 3.1, 112, 2.7, 106, 2.6, 115, 2.5, 102};

int combinatie[20];
int index = 0;

/* draden waardes */
const int wireValues[] = {930, 839, 696, 512, 320, 179};
const String wireKeys[] = {"geel", "groen", "blauw", "rood", "paars", "wit"};
const unsigned int wireResistors[] = {1000, 2200, 4700, 10000, 22000, 47000};
const int cstWireResist = 10000;

/* ------------------- PRESETS ------------------- */
const Preset preset1("0000", 1, "geel");
const Preset preset2("1234", 4, "blauw");
Preset presets[] = {preset1, preset2};

/* ------------------- VARIABLES ------------------- */
#define bananaPin1 A0
#define bananaPin2 A1
#define bananaPin3 A2
#define bananaPin4 A3
#define wirePin A4
#define codePin A5
#define piezoPin 8
#define startPin 2

LiquidCrystal lcd(11, 12, 4, 5, 6, 7);

int keuzeWaarden = -1;

const int maxInterval = 5000;
const int minInterval = 150;
const int fastBeepTime = 5; // De laaste x seconden beept de bom met minInterval ertussen (snel)

const int pressDefuse = 20;
const int pressPlant = 10;

void setup()
{
  lcd.begin(16, 2);
  if (DEBUG == 1)
  {
    Serial.begin(9600);
  }
  pinMode(piezoPin, OUTPUT);
  pinMode(startPin, INPUT);
  lcd.print("Bom rakkers");
  delay(2000);
  if (DEBUG == 1)
  {
    Serial.println("-------- SETUP DONE -------");
  }
  // startUp();
  // delay(500);
  // lcd.setCursor(0, 0);
  // lcd.print("Work in progress");
  // lcd.setCursor(0, 1);
  // printRepeat((char)255, 16, LCD);
}

void loop()
{
  man();
  delay(30000);
}

void man()
{
  lcd.setCursor(0, 0);
  lcd.print("KIES GAME MODE");
  delay(2000);
  lcd.clear();
  while (keuzeWaarden == -1)
  {
    lcd.setCursor(0, 0);
    lcd.print("1 = DEFUSE");
    lcd.setCursor(0, 1);
    lcd.print("2 = P & D SWAT");
    keuzeWaarden = codeNumber();
    if (keuzeWaarden != 1 and keuzeWaarden != 2)
    {
      keuzeWaarden = -1;
    }
  }
  if (keuzeWaarden == 1)
  {
    defuse();
  }

  else if (keuzeWaarden == 2)
  {
    plantAndDefuse();
  }
}

void startUp()
{
  pressButtonTime(10, startPin, "planting"); // Wacht tot knop 5 seconden ingedrukt wordt.
  startUpSound();
  //  tone(piezoPin, 750);
  //  delay(1000);
  //  noTone(piezoPin);
}

void pressButtonTime(int pressTime, int buttonPin)
{
  pressButtonTime(pressTime, buttonPin, "default");
}

void pressButtonTime(int pressTime, int buttonPin, String type)
{
  lcd.clear();
  lcd.print("Press button for");
  lcd.setCursor(0, 1);
  lcd.print(pressTime);
  lcd.print(" seconds");
  bool done = false;
  int buttonState = LOW;
  int prevState = LOW;
  unsigned long startTime = millis();

  while (!done)
  {
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH && prevState == LOW)
    { // Als de knop ingedrukt wordt (en dus nog niet ingedrukt was)
      tone(piezoPin, 1046, 100);
      startTime = millis(); // Sla de tijd op wanneer de knop ingedrukt wordt
      if (DEBUG == 1)
      {
        Serial.println("Pressed in button");
      }
      prevState = HIGH; // Onthoud dat de knop ingedrukt is
      if (type != "default")
        lcd.clear();
    }
    else if (buttonState == HIGH && prevState == HIGH)
    { // Als de knop nog steeds ingedrukt is
      unsigned long currentTime = millis();
      int count = (currentTime - startTime) / (pressTime * 1000 / 32);
      if (count < 16)
      {
        lcd.setCursor(0, 0);
      }
      else
      {
        lcd.setCursor(0, 1);
      }
      if (type == "planting")
        printRepeat((char)255, count % 16 + 1, LCD);
      if (currentTime - startTime >= pressTime * 1000)
      {
        done = true;
        if (type == "planting")
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Bomb activated");
        }
      } // Als de knop voor 5 seconden is ingedrukt (verschil tussen tijdstip dat de knop werd ingedruk met tijdstip op dit moment) dan is programma klaar (done).
    }
    else if (buttonState == LOW && prevState == HIGH)
    { // Als knop losgelaten wordt (en dus ingedrukt was)
      if (DEBUG == 1)
        Serial.println("Released button");
      if (type != "default")
      {
        lcd.clear();
        lcd.print("Press button for");
        lcd.setCursor(0, 1);
        lcd.print(pressTime);
        lcd.print(" seconds");
        prevState = LOW; // Onthoud dat de knop niet ingedrukt is
      }
    }
  }
  if (DEBUG == 1)
  {
    Serial.print("Pressed button for ");
    Serial.print(pressTime);
    Serial.println(" seconds.");
  }
}

/* Geeft nummer terug voor gegeven waarde of -1 als het geen nummer is */
int codeNumber()
{
  int value = analogRead(codePin);
  for (int i = 0; i < sizeof(codeValues) / sizeof(codeValues[0]); i++)
  {
    if (value < codeValues[i] + 20 && value > codeValues[i] - 20)
      return codeKeys[i];
  }
  return -1;
}

int getLength(int array[])
{
  return sizeof(array) / sizeof(array[0]);
}

void startUpSound()
{
  tone(piezoPin, 523, 200);
  delay(200);
  tone(piezoPin, 784, 100);
  delay(100);
  tone(piezoPin, 1046, 100);
  delay(100);
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

void printRepeat(char character, unsigned int count)
{
  if (DEBUG == 1)
    printRepeat(character, count, SERIAL);
}

int getTime()
{
  int tijd;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SELECT TIME  min)");
  delay(2000);
  tijd = getNumber(2); // zet de tijd van maximaal 2 cijfers
  lcd.clear();
  lcd.setCursor(0, 0);
  return tijd;
}

int getPreset()
{
  int preset;
  lcd.print("KIES PRESET");
  preset = getNumber(1);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SPEL START IN:");
  for (int timer = 10; timer > 0; timer--)
  {
    lcd.setCursor(0, 0);
    lcd.print("SPEL START IN:");
    lcd.setCursor(7, 1);
    lcd.print(timer);
    delay(1000);
    lcd.clear();
  }
  return preset;
}

void defuse()
{
  wait();
  int tijd = getTime();                 // Geef tijd in in minuten
  Preset preset = presets[getPreset()]; // Geef gewenste preset in
  defuseMain(tijd, preset);
}

void defuseMain(int tijd, Preset preset)
{
  bool codeDone = false;
  bool wireConnectDone = false;
  bool colorWireDone = false;

  int const timeSec = tijd * 60;
  int counter = 0;
  int prevCounter = 0;
  String ingCode;
  String colorWire;
  String wiresOut[sizeof(wireKeys) / sizeof(wireKeys[0]) - 1] = {""};
  int amWireOut = 0;
  unsigned int interval = maxInterval;
  unsigned long startProgramma = millis();
  unsigned long startCycle = millis();
  lcd.clear();
  while ((!codeDone || !wireConnectDone || !colorWireDone) && (millis() - startProgramma < tijd * 60 * 1000L))
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
      interval = increaseInterval(timeSec, counter);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(timeSec - counter);
    }
    if (!codeDone)
    {
      int number = codeNumber();
      if (number != -1 && number != OK)
      {
        wait();
        if (DEBUG == 1)
        {
          Serial.print("Nummer: ");
          Serial.println(number);
        }
        ingCode += String(number);
        if (ingCode.length() == preset.getCode().length())
        {
          if (ingCode == preset.getCode())
          {
            codeDone = true;
            if (DEBUG == 1)
              Serial.println("Code completed");
          }
          else
          {
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
        wait();
        int wirePres = combinatieBanana();
        if (DEBUG == 1)
        {
          Serial.print("Banana combinatie: ");
          Serial.println(wirePres);
        }
        if (wirePres == preset.getBananaPreset())
        {
          wireConnectDone = true;
          if (DEBUG == 1)
            Serial.println("Banana wires zijn juist verbonden");
        }
      }
    }
    if (!colorWireDone)
    {
      colorWire = getWireHard(wiresOut, amWireOut);
      if (colorWire != "")
      {
        if (colorWire == preset.getWire())
        {
          colorWireDone = true;
          if (DEBUG == 1)
            Serial.println("Right wire pulled out");
        }
        else
        {
          wiresOut[amWireOut] = colorWire;
          amWireOut++;
        }
      }
    }
  }
  if (millis() - startProgramma < tijd * 60 * 1000L)
  {
    if (DEBUG == 1)
      Serial.println("Bomb defused");
    lcd.clear();
    lcd.print("YOU WON");
  }
  else
  {
    if (DEBUG == 1)
      Serial.println("Bomb exploded");
    lcd.clear();
    lcd.print("YOU LOST");
  }
}

void beep()
{
  tone(piezoPin, 3000, 75);
}

void plantAndDefuse()
{
  wait();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ENTER CODE (new)");
  delay(2000);
  String code = String(getNumber(4));
  int tijd = getTime();

  bool plantCodeDone = false;
  bool planted = false;

  bool defused = false;
  unsigned long startTime = 0;

  int const timeSec = tijd * 60;
  int counter = 0;
  int prevCounter = 0;
  String ingCode;

  int buttonState = LOW;
  int prevState = LOW;

  unsigned int interval = maxInterval;
  unsigned long startProgramma = millis();
  unsigned long startCycle = millis();
  lcd.clear();
  // PLANT
  while (!plantCodeDone) {
    int number = codeNumber();
      if (number != -1 && number != OK)
      {
        wait();
        if (DEBUG == 1)
        {
          Serial.print("Nummer: ");
          Serial.println(number);
        }
        ingCode += String(number);
        if (ingCode.length() == code.length())
        {
          if (ingCode == code)
          {
            plantCodeDone = true;
            if (DEBUG == 1)
              Serial.println("Code completed");
          }
          else
          {
            ingCode = "";
            lcd.setCursor(0,0);
            lcd.print("Wrong code");
            lcd.setCursor(0,1);
            lcd.print("Try again");
          }
        }
      }
  }
  lcd.setCursor(0,0);
  lcd.print("Entered code");
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
      if (DEBUG == 1)
      {
        Serial.println("Pressed in button");
      }
      prevState = HIGH; // Onthoud dat de knop ingedrukt is
    }
    else if (buttonState == HIGH && prevState == HIGH)
    { // Als de knop nog steeds ingedrukt is
      unsigned long currentTime = millis();
      int count = (currentTime - startTime) / (pressPlant * 1000 / 32);
      if (count < 16)
      {
        lcd.setCursor(0, 0);
      }
      else
      {
        lcd.setCursor(0, 1);
      }
      printRepeat((char)255, count % 16 + 1, LCD);
      if (currentTime - startTime >= pressPlant * 1000)
      {
        planted = true;
      } // Als de knop voor 5 seconden is ingedrukt (verschil tussen tijdstip dat de knop werd ingedruk met tijdstip op dit moment) dan is programma klaar (done).
    }
    else if (buttonState == LOW && prevState == HIGH)
    { // Als knop losgelaten wordt (en dus ingedrukt was)
      if (DEBUG == 1)
        Serial.println("Released button");
      prevState = LOW;
      lcd.clear();
    }
  }
  // DEFUSE
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press OK");
  lcd.print(pressDefuse);
  lcd.print("sec");
  while (!defused && (millis() - startProgramma < tijd * 60 * 1000L))
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
      interval = increaseInterval(timeSec, counter);
      if (prevState != HIGH)
      {
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print(timeSec - counter);
      }
    }

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
      if (DEBUG == 1)
      {
        Serial.println("Pressed in button");
      }
      prevState = HIGH; // Onthoud dat de knop ingedrukt is
    }
    else if (buttonState == HIGH && prevState == HIGH)
    { // Als de knop nog steeds ingedrukt is
      unsigned long currentTime = millis();
      int count = (currentTime - startTime) / (pressDefuse * 1000 / 32);
      if (count < 16)
      {
        lcd.setCursor(0, 0);
      }
      else
      {
        lcd.setCursor(0, 1);
      }
      printRepeat((char)255, count % 16 + 1, LCD);
      if (currentTime - startTime >= pressDefuse * 1000)
      {
        defused = true;
      } // Als de knop voor 5 seconden is ingedrukt (verschil tussen tijdstip dat de knop werd ingedruk met tijdstip op dit moment) dan is programma klaar (done).
    }
    else if (buttonState == LOW && prevState == HIGH)
    { // Als knop losgelaten wordt (en dus ingedrukt was)
      if (DEBUG == 1)
        Serial.println("Released button");
      prevState = LOW;
      lcd.setCursor(0, 0);
      lcd.print("Press OK");
      lcd.print(pressDefuse);
      lcd.print("sec");
    }
    
  }
  if (millis() - startProgramma < tijd * 60 * 1000L)
  {
    if (DEBUG == 1)
      Serial.println("Bomb defused");
    lcd.clear();
    lcd.print("YOU WON");
  }
  else
  {
    if (DEBUG == 1)
      Serial.println("Bomb exploded");
    lcd.clear();
    lcd.print("YOU LOST");
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

void wait()
{
  bool w = false;
  while (w == false)
  {
    if (codeNumber() == -1)
    {
      w = true;
    }
    else
    {
      w = false;
    }
    keuzeWaarden = -1;
  }
}

int getNumber(int length)
{
  bool okay = false;
  String string = "";
  while (!okay)
  {
    keuzeWaarden = codeNumber();
    if (keuzeWaarden != -1 && keuzeWaarden != OK)
    {
      if (string == "")
      {
        string = String(keuzeWaarden);
      }
      else if (string.length() == length)
      {
        string = String(keuzeWaarden);
      }
      else
      {
        if (DEBUG == 1)
          Serial.println(keuzeWaarden);
        string = string + String(keuzeWaarden);
      }
      lcd.setCursor(0, 1);
      lcd.print("  ");
      lcd.setCursor(0, 1);
      lcd.print(string);
      lcd.setCursor(3, 1);
      lcd.print("& PRESS OK");
      wait();
    }
    else if (keuzeWaarden == OK && string != "")
    {
      okay = true;
      wait();
    }
  }
  if (DEBUG == 1)
  {
    Serial.print("Waarde: ");
    Serial.println(string.toInt());
  }
  return string.toInt();
}

int combinatieBanana()
{
  memset(combinatie, 0, sizeof(combinatie));
  index = 0;
  int waarschijnlijk = 0; // reset en alles op nul voor elke keer als je terug opnieuw checked
  int biggest = 0;

  bananaPin(banana1, sizeof(banana1) / sizeof(banana1[0]), bananaPin1);
  bananaPin(banana2, sizeof(banana2) / sizeof(banana2[0]), bananaPin2); // checkt elke pin en zet de combinatie die mogelijk is bij die waarde in de array "combinatie"
  bananaPin(banana3, sizeof(banana3) / sizeof(banana3[0]), bananaPin3);
  bananaPin(banana4, sizeof(banana4) / sizeof(banana4[0]), bananaPin4);

  if (DEBUG == 1)
  {
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
      if (DEBUG == 1)
      {
        Serial.print("Biggest: ");
        Serial.println(biggest);
        Serial.print("Waarschijnlijk: ");
        Serial.println(waarschijnlijk);
      }
    }
  }
  return waarschijnlijk - 100;
}

void bananaPin(const double *banana, int length, int pin)
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

String getWireSimple()
{
  int value = analogRead(wirePin);
  for (int i = 0; i < sizeof(wireValues) / sizeof(wireValues[0]); i++)
  {
    if (value < wireValues[i] + 20 && value > wireValues[i] - 20)
      return wireKeys[i];
  }
  return "";
}

String getWireHard(String *wiresOut, int amWiresOut)
{
  unsigned long resultResist = 0L;
  unsigned int outResistors[amWiresOut];
  getResistorValues(outResistors, wiresOut, amWiresOut);
  for (int i = 0; i < amWiresOut; i++)
  {
    resultResist = resultResist + outResistors[i];
  }
  String remainingWires[sizeof(wireKeys) / sizeof(wireKeys[0]) - amWiresOut];
  getRemainingWires(remainingWires, wiresOut, amWiresOut);
  int remainingValues[sizeof(remainingWires) / sizeof(remainingWires[0])];
  for (int k = 0; k < sizeof(remainingWires) / sizeof(remainingWires[0]); k++)
  {
    unsigned long resistor = resultResist + getResistorValue(remainingWires[k]);
    remainingValues[k] = (1024L * cstWireResist) / (resistor + cstWireResist);
    // if (DEBUG == 1)
    // {Serial.print(remainingWires[k]);
    // Serial.print(": ");
    // Serial.print(resistor);
    // Serial.print(", ");
    // Serial.println(remainingValues[k]);}
  }
  int value = analogRead(wirePin);
  for (int i = 0; i < sizeof(remainingValues) / sizeof(remainingValues[0]); i++)
  {
    if (value < remainingValues[i] + 5 && value > remainingValues[i] - 5)
      return remainingWires[i];
  }
  return "";
}

unsigned int getResistorValue(String wire)
{
  for (int i = 0; i < sizeof(wireKeys) / sizeof(wireKeys[0]); i++)
  {
    if (wire == wireKeys[i])
    {
      return wireResistors[i];
    }
  }
  return -1;
}

void getResistorValues(unsigned int *values, String *wiresOut, int length)
{
  for (int i = 0; i < length; i++)
  {
    String wire = wiresOut[i];
    for (int j = 0; j < (sizeof(wireKeys) / sizeof(wireKeys[0])); j++)
    {
      if (wire == wireKeys[j])
      {
        values[i] = wireResistors[j];
        break;
      }
    }
  }
}

void getRemainingWires(String *remaining, String *wiresOut, int amWiresOut)
{
  int counter = 0;
  for (int i = 0; i < sizeof(wireKeys) / sizeof(wireKeys[0]); i++)
  {
    if (!stringInList(wireKeys[i], wiresOut, amWiresOut))
    {
      remaining[counter] = wireKeys[i];
      counter++;
    }
  }
}

bool stringInList(String string, String *list, int length)
{
  for (int i = 0; i < length; i++)
  {
    if (string == list[i])
    {
      return true;
    }
  }
  return false;
}

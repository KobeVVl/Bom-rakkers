/* ------------------- PINS ------------------- */
#define bananaPin1 A5
#define bananaPin2 A4
#define bananaPin3 A3
#define bananaPin4 A2
#define wirePin A1
#define codePin A0
#define piezoPin 2
#define ledPin1 3
#define ledPin2 4
#define ledPin3 5
#define rest 6
#define en 7
#define dvier 8
#define dvijf 9
#define dzes 10
#define dzeven 11

#include <LiquidCrystal.h>

LiquidCrystal lcd(rest, en, dvier, dvijf, dzes, dzeven);

void setup() {
    Serial.begin(115200);
    pinMode(ledPin1, OUTPUT);
    pinMode(ledPin2, OUTPUT);
    pinMode(ledPin3, OUTPUT);
    lcd.begin(16,2);
}

void loop() {
    lcd.clear();
    Serial.print("bananaPin1: ");
    Serial.println((double)(analogRead(bananaPin1)*5)/1024);
    Serial.print("bananaPin2: ");
    Serial.println((double)(analogRead(bananaPin2)*5)/1024);
    Serial.print("bananaPin3: ");
    Serial.println((double)(analogRead(bananaPin3)*5)/1024);
    Serial.print("bananaPin4: ");
    Serial.println((double)(analogRead(bananaPin4)*5)/1024);
    Serial.print("wirePin: ");
    Serial.println(analogRead(wirePin));
    Serial.print("codePin: ");
    Serial.println(analogRead(codePin));
    Serial.println("-----------------------------");
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
    tone(piezoPin, 3000);
    delay(1000);
    lcd.print("TEST BOM");
    lcd.setCursor(0,1);
    lcd.print("LCD WERKT");
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
    noTone(piezoPin);
    delay(1000);
}

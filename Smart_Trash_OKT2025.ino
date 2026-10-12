#include <Servo.h>
Servo servoOrganik;
Servo servoAnorganik;

const int ledM = 12;
const int ledK = 11;
const int ledH = 10;
const int irPin = 8;
const int metalPin = 7;

enum sistemTempatsampah {
  IDLE,
  CONFIRM_SAMPAH,
  CEK_MATERIAL,
  SORTIR_ANORGANIK,
  SORTIR_ORGANIK,
  RESET
};

sistemTempatsampah currentState = IDLE;
const long confirmTime = 400;
const long sensorTime = 1500;
const long sortirTime = 1500;
const long resetTime = 1000;

unsigned long waktuSebelumnya = 0;
unsigned long waktuSesi = 0;

bool bacaSensorIR() {
static unsigned long lastDebounce = 0;
static bool lastStable = HIGH;
static bool lastReading = HIGH;

bool nilai = (digitalRead(irPin) == LOW);
 if(nilai != lastReading) {
lastDebounce = millis();
lastReading = nilai;
}
if(millis() - lastDebounce >= 20) {
if(nilai != lastStable) {
lastStable = nilai;
}
}
return lastStable;
}

bool bacaSensorMetal() {
static unsigned long lastDebounce = 0;
static bool lastStable = HIGH;
static bool lastReading = HIGH;

bool metal = (digitalRead(metalPin) == LOW);
 if(metal != lastReading) {
lastDebounce = millis();
lastReading = metal;
}
if(millis() - lastDebounce >= 20) {
if(metal != lastStable) {
lastStable = metal;
}
}
return lastStable;
}
void setup() {
  servoOrganik.attach(6);
  servoAnorganik.attach(5);
  pinMode(ledM,OUTPUT);
  pinMode(ledK,OUTPUT);
  pinMode(ledH,OUTPUT);
  pinMode(irPin,INPUT);
  pinMode(metalPin,INPUT);
  servoOrganik.write(0);
  servoAnorganik.write(0);
  Serial.begin(9600);
}

void loop() {
  unsigned long waktuSekarang = millis();
  if(waktuSekarang - waktuSebelumnya >= sensorTime) {
    statepemilahSampah();
    waktuSebelumnya = waktuSekarang;
  }

}

void statepemilahSampah() {
unsigned long waktuState = millis();

bool deteksensorIR = bacaSensorIR();
bool deteksensorMetal = bacaSensorMetal();

switch(currentState) {
  case IDLE:
  if(deteksensorIR) {
    digitalWrite(ledM,HIGH);
    digitalWrite(ledK,LOW);
    digitalWrite(ledH,LOW);
    currentState = CONFIRM_SAMPAH;
    waktuSesi = waktuState;
  }
  break;
  case CONFIRM_SAMPAH:
  if(waktuState - waktuSesi >= confirmTime) {
  if(bacaSensorIR()) {
    currentState = CEK_MATERIAL;
    digitalWrite(ledM,HIGH);
    digitalWrite(ledK,LOW);
    digitalWrite(ledH,LOW);
  } else{
    currentState = IDLE;
  } waktuSesi = waktuState;
  }
  break;
  case CEK_MATERIAL:
  if(bacaSensorMetal()) {
    currentState = SORTIR_ANORGANIK;
    digitalWrite(ledK,HIGH);
    digitalWrite(ledM,LOW);
    digitalWrite(ledH,LOW);
  } else{
    currentState = SORTIR_ORGANIK;
    digitalWrite(ledH,HIGH);
    digitalWrite(ledM,LOW);
    digitalWrite(ledK,LOW);
  } waktuSesi = waktuState;
  break;
  case SORTIR_ANORGANIK:
  if(waktuState - waktuSesi >= sortirTime) {
    servoAnorganik.write(90);
    servoOrganik.write(0);
    currentState = RESET;
    waktuSesi = waktuState;
  }
  break;
  case SORTIR_ORGANIK:
  if(waktuState - waktuSesi >= sortirTime) {
    servoAnorganik.write(0);
    servoOrganik.write(90);
    currentState = RESET;
    waktuSesi = waktuState;
  }
  break;
  case RESET:
  if(waktuState - waktuSesi >= resetTime) {
    servoAnorganik.write(0);
    servoOrganik.write(0);
    digitalWrite(ledM,LOW);
    digitalWrite(ledK,LOW);
    digitalWrite(ledH,LOW);
    currentState = IDLE;
    waktuSesi = waktuState;
  }
break;
}




}

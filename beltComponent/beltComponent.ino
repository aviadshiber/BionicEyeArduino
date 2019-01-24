#include<SoftwareSerial.h>
const int rightPin = A0;
const int leftPin = A2;
const int continuePin = A1;
const int rxPin = 6;
const int txPin = 5;
bool isContinue;
SoftwareSerial hc = SoftwareSerial(rxPin, txPin);

#include <Wire.h>
#include <MechaQMC5883.h>

// SDA=A4, SCL=A5
MechaQMC5883 qmc;

void initVibrationMotors() {
  pinMode(rightPin, OUTPUT);
  pinMode(leftPin, OUTPUT);
  pinMode(continuePin, OUTPUT);
}

void vibAll() {
  for (int POWER = 0; POWER <= 255; POWER++) {
    analogWrite(rightPin, POWER);
    analogWrite(leftPin, POWER);
    analogWrite(leftPin, POWER);
    delay(1);
  }
  delay(250);
  for (int POWER = 255; POWER >= 0; POWER--) {
    analogWrite(rightPin, POWER);
    analogWrite(leftPin, POWER);
    analogWrite(leftPin, POWER);
    delay(1);
  }
}

void vib(int pin) {
  for (int POWER = 0; POWER <= 255; POWER++) {
    analogWrite(pin, POWER);
    delay(1);
  }
  delay(250);
  for (int POWER = 255; POWER >= 0; POWER--) {
    analogWrite(pin, POWER);
    delay(1);
  }
}

void getDiraction() {
  char c;
  if (hc.available()) {
    c = (char)hc.read();
    Serial.println(c);
  }
  else {
    return;
  }
  if (c == '9') {
    vibAll();
  }

  //forward only
  if (c == '3' && isContinue ) {
    vib(continuePin);
    isContinue = false;
  }
  // "hard" right
  if (c == '6') {
    vib(rightPin);
    isContinue = true;
  }
  // "hard" left
  if (c == '0') {
    vib(leftPin);
    isContinue = true;
  }
  // "softer" right
  if (c == '4') {
    vib(continuePin);
    //delay(50);
    vib(rightPin);
    isContinue = true;
  }
  // "soft" right
  if (c == '5') {
    vib(continuePin);
    //delay(200);
    vib(rightPin);
    //delay(200);
    vib(rightPin);
    isContinue = true;
  }
  // "soft" left
  if (c == '1') {
    vib(continuePin);
    //delay(200);
    vib(leftPin);
    //delay(200);
    vib(leftPin);
    isContinue = true;
  }
  // "softer" left
  if (c == '2') {
    vib(continuePin);
    //delay(200);
    vib(leftPin);
    isContinue = true;
  }
}


void setup() {
  initVibrationMotors();
  isContinue = true;
  // put your setup code here, to run once:
  hc.begin(9600); //init bluetooth
  Serial.begin(9600);
  Serial.println("Magnetometer Test"); Serial.println("");
  Wire.begin();
  qmc.init();
  Serial.println("ALL SET!");
  if (hc.isListening()) {
    Serial.println("Bluetooth is listening");
  }
}


void sendAzimuth() {
  /* Get a new sensor event */
  int x, y, z;
  qmc.read(&x, &y, &z);

  float Pi = 3.14159;

  // Calculate the angle of the vector y,x
  float heading = (atan2(y, x) * 180) / Pi;

  // Normalize to 0-360
  if (heading < 0)
  {
    heading = 360 + heading;
  }
  int azimuth = heading;
  String data = "(" + String(azimuth) + ")";
  Serial.print("Compass Heading: ");
  Serial.println(data);
  hc.print(data);
  hc.flush();
  delay(500);
}

void loop() {
  sendAzimuth();
  getDiraction();
  hc.println("");

}
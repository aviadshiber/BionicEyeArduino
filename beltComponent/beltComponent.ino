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
    return; //nothing to do here
  }
  if (c == '9') { //starting navigating
    vibAll();
  }else if (c == '3' && isContinue ) { //forward only
    vib(continuePin);
    isContinue = false;
  }else if (c == '6') {  // "hard" right
    vib(rightPin);
    isContinue = true;
  }else if (c == '0') {  // "hard" left
    vib(leftPin);
    isContinue = true;
  }else if (c == '4') { // "softer" right
    vib(continuePin);
    vib(rightPin);
    isContinue = true;
  }else if (c == '5') { // "soft" right
    vib(continuePin);
    vib(rightPin);
    vib(rightPin);
    isContinue = true;
  }else if (c == '1') { // "soft" left
    vib(continuePin);
    vib(leftPin);
    vib(leftPin);
    isContinue = true;
  }else if (c == '2') {// "softer" left
    vib(continuePin);
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


/*
 Uses code from the Arduino ADXL3xx example (Mellis, Igoe): 
 - http://www.arduino.cc/en/Tutorial/ADXL3xx

*/

#include <SoftwareSerial.h>

// NOTE: depends on Filter v0.6.1 or possibly a later version
#include "Filter.h"

//const int myGroup = 1; // Xbee group
//const int nextGroup = myGroup + 1;
const int nextGroup = 3;

SoftwareSerial xbee(10, 9);
const int groundXbeePin = 2;

Filter fx(40); 
Filter fy(40);
Filter fz(40);

// accelerometer pins
const int powerPin = A0;              // voltage
const int vOutPin = A1;               // unused
const int groundPin = A2;             // ground
const int zPin = A3;                  // z-axis of the accelerometer
const int yPin = A4;                  // y-axis
const int xPin = A5;                  // x-axis

float sensitivity = 2; // higher number increases sensitivity
// FIXME: should make this dynamic, based on stDevSample

void setup() { 
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  
  
  pinMode(groundPin, OUTPUT);
  pinMode(powerPin, OUTPUT);
  pinMode(vOutPin, OUTPUT);
  digitalWrite(groundPin, LOW); 
  digitalWrite(powerPin, HIGH);
  digitalWrite(vOutPin, LOW);

  pinMode(groundXbeePin, OUTPUT);
  digitalWrite(groundXbeePin, LOW);
  delay(10);
  xbee.begin(9600); // connect to Xbee over software serial
  
  fx.attachFilter(throwSensor);
  fy.attachFilter(throwSensor);
  fz.attachFilter(throwSensor);
}

void loop() {
  fx.write(analogRead(xPin));
  fy.write(analogRead(yPin));
  fz.write(analogRead(zPin));

  logCheck();
}

void throwSensor(Filter* f) {
  //Serial.println("in throwSensor");
  int latestVal = f->peek(f->available() - 1);
  int mean = f->mean();
  
  if (abs(latestVal - mean) > (mean / sensitivity)) {
    filterEvent(latestVal);
  }
}

void filterEvent(long value) { 
  digitalWrite(13, HIGH);
  Serial.println("throw detected");
  xbee.println(nextGroup);
}

void logCheck() {
  if ((millis() % 1000) < 5) {
    Serial.print(fx.mean());
    Serial.print("\t");
    Serial.print(fy.mean());
    Serial.print("\t");
    Serial.print(fz.mean());
    Serial.println("\t");
    digitalWrite(13, LOW);
  }
}



/*
 Uses code from the Arduino ADXL3xx example (Mellis, Igoe): 
 - http://www.arduino.cc/en/Tutorial/ADXL3xx

*/

#include <SoftwareSerial.h>

// NOTE: depends on Filter v0.6.1 or possibly a later version
#include "Filter.h"

const int myGroup = 1; // Xbee group
const int nextGroup = myGroup + 1;

SoftwareSerial xbee(2, 3);

Filter fx(40); 
Filter fy(40);
Filter fz(40);

// accelerometer pins
const int groundPin = A2;             // ground
const int powerPin = A0;              // voltage
const int xPin = A5;                  // x-axis of the accelerometer
const int yPin = A4;                  // y-axis
const int zPin = A3;                  // z-axis

float sensitivity = 1.2; // higher number increases sensitivity
// FIXME: should make this dynamic, based on stDevSample

void setup() { 
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  
  xbee.begin(9600); // connect to Xbee over software serial
  
  pinMode(groundPin, OUTPUT);
  pinMode(powerPin, OUTPUT);
  digitalWrite(groundPin, LOW); 
  digitalWrite(powerPin, HIGH);
  
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
  xbee.write(nextGroup);
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


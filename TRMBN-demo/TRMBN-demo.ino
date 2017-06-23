
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Bounce.h>

#define BMP_SCK 16
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10
float initialPressure = 0.0;
Adafruit_BMP280 bmp; // I2C
int channel = 1; // Defines the MIDI channel to send messages on (values from 1-16)
int velocity = 100; // Defines the velocity that the note plays at (values from 0-127)
int note = 70; // b flat on bass clef
int minPressure = 97500;
int maxPressure = 101000;
int velocityBuckets = 100;
int normDivisor = (maxPressure - minPressure)/velocityBuckets;
float pressure = -1.0;
int prevVal = 0;
const int buttonPin = 12;
bool isSustainOn = false;
Bounce pushbutton = Bounce(buttonPin, 10);  // 10 ms debounce
byte previousState = HIGH;         // what state was the button last time
int CC = 7;
bool isNoteOn = false;
bool hasPressureBeenRead = false;

void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));
  pinMode(buttonPin, INPUT_PULLUP);
  if (!bmp.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}

void loop() {
    if (!hasPressureBeenRead) {
      pressure = bmp.readPressure();
      hasPressureBeenRead = true;  
    }
    Serial.print(F("Pressure = "));
    Serial.println(pressure);
    int normP = (pressure - minPressure)/normDivisor;
    Serial.print("normP: ");
    Serial.println(normP);
    Serial.print("isNoteOn: ");
    Serial.println(isNoteOn);
    if (isNoteOn) {
      if (normP > velocityBuckets) {
        usbMIDI.sendControlChange(CC,velocityBuckets,channel);
      } else if (normP > 0) {
        usbMIDI.sendControlChange(CC,normP,channel);
      } else if (normP <= 0) {
        usbMIDI.sendNoteOff(note,0,channel);
        isNoteOn = false;
      }
    } else {
      if (normP > velocityBuckets) {
        usbMIDI.sendNoteOn(note,velocityBuckets,channel);
        isNoteOn = true;
      } else if (normP > 0) {
        usbMIDI.sendNoteOn(note,normP,channel);
        isNoteOn = true;
      }
    }
    

//  if (pushbutton.update()) {
//    if (pushbutton.fallingEdge()) {
//      count = count + 1;
//      countAt = millis();
//    }
//  } else {
//    if (count != countPrinted) {
//      unsigned long nowMillis = millis();
//      if (nowMillis - countAt > 100) {
//        Serial.print("count: ");
//        Serial.println(count);
//        countPrinted = count;
//      }
//    }
//  }
  delay(100);
}


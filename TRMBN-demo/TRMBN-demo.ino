
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Bounce.h>

#define BMP_SCK 16
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10
Adafruit_BMP280 bmp; // I2C
int channel = 1; // Defines the MIDI channel to send messages on (values from 1-16)
int velocity = 100; // Defines the velocity that the note plays at (values from 0-127)
int note = 70; // b flat on bass clef
int CC = 7;

int maxPressure = 101000;
int velocityBuckets = 127;
bool isNoteOn = false;

float normDivisor = 0;
bool isInitPressureRead = false;
float minPressure = 0;
float minPressureDiff = 100;
int pressureRange = 5000;
float normDiv = pressureRange / velocityBuckets;

void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));
  if (!bmp.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}

void loop() {
    if (!isInitPressureRead) {
      isInitPressureRead = true;  
      float initialPressure = bmp.readPressure();
      minPressure = initialPressure + minPressureDiff;
    }
    float pressure = bmp.readPressure();
    int normP = (pressure - minPressure) / normDiv;
    Serial.print(F("Pressure = "));
    Serial.println(pressure);
    Serial.print("normP: ");
    Serial.println(normP);
    Serial.print("isNoteOn: ");
    Serial.println(isNoteOn);
        Serial.print("minPressure: ");
    Serial.println(minPressure);
        Serial.print("normDiv: ");
    Serial.println(normDiv);
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
    delay(50);
}


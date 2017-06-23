
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; // I2C

// MIDI
const int midiChan = 1;
const int velCtrl = 7;
const int modCtrl = 1;
const int midiValMin = 0;
const int midiValMax = 127;
const int note = 70; // b flat on bass clef

// Force Sensor
const float fsrMin = 0;
const float fsrMax = 1000;
const int forcePin = 14;

// Pressure Sensor
const float minPressureDiff = 100;
const float pressureRange = 5000;

// State Variables
bool isInitPressureRead = false;
float minPressure = 0;
float maxPressure = 0;
bool isNoteOn = false;
bool isSustainOn = false;
void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));
  if (!bmp.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}

void loop() {
  float force = map(analogRead(forcePin), fsrMin, fsrMax, midiValMin, midiValMax);
  usbMIDI.sendControlChange(modCtrl,force,midiChan);
  Serial.print("Force = ");
  Serial.println(force);
  if (!isInitPressureRead) {
    isInitPressureRead = true;  
    float initialPressure = bmp.readPressure();
    minPressure = initialPressure + minPressureDiff;
    maxPressure = minPressure + pressureRange;
  }
  float rawPressure = bmp.readPressure();
  float pressure = map(rawPressure, minPressure, maxPressure, midiValMin, midiValMax);
  Serial.print("Raw Pressure = ");
  Serial.println(rawPressure);
  Serial.print("Pressure = ");
  Serial.println(pressure);
  if (isNoteOn) {
    if (pressure > midiValMax) {
      usbMIDI.sendControlChange(velCtrl, midiValMax, midiChan);
    } else if (pressure > 0) {
      usbMIDI.sendControlChange(velCtrl, pressure, midiChan);
    } else if (pressure <= 0 && !isSustainOn) {
      usbMIDI.sendNoteOff(note,0,midiChan);
      isNoteOn = false;
    }
  } else {
    if (pressure > midiValMax) {
      usbMIDI.sendNoteOn(note, midiValMax, midiChan);
      isNoteOn = true;
    } else if (pressure > 0) {
      usbMIDI.sendNoteOn(note, pressure, midiChan);
      isNoteOn = true;
    }
  }
  delay(100);
}


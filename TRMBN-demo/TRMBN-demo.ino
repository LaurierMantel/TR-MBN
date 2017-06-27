
#include <Wire.h>
//#include <SPI.h>
#include <Adafruit_MPL3115A2.h>
#include <Bounce.h>

Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();

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

// Sustain Button
const int buttonPin = 31;
Bounce sustainButton = Bounce(buttonPin, 100);  // 10 ms debounce

// State Variables
bool isInitPressureRead = false;
float minPressure = 0;
float maxPressure = 0;
bool isNoteOn = false;
bool isSustainOn = false;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  if (! baro.begin()) {
    Serial.println("Couldnt find sensor");
    return;
  }
  
}

void loop() {
  float force = map(analogRead(forcePin), fsrMin, fsrMax, midiValMin, midiValMax);
  usbMIDI.sendControlChange(modCtrl,force,midiChan);
  Serial.print("Force = "); Serial.println(force);
  if (!isInitPressureRead) {
    isInitPressureRead = true;  
    float initialPressure = baro.getPressure();
    minPressure = initialPressure + minPressureDiff;
    maxPressure = minPressure + pressureRange;
  }
  float rawPressure = baro.getPressure();
  float pressure = map(rawPressure, minPressure, maxPressure, midiValMin, midiValMax);
  Serial.print("Raw Pressure = "); Serial.println(rawPressure);
  Serial.print("Pressure = "); Serial.println(pressure);
  Serial.print("isSustainOn = "); Serial.println(isSustainOn);
  Serial.print("isNoteOn = "); Serial.println(isNoteOn);
  if (isNoteOn) {
    if (sustainButton.update()) {
      if (sustainButton.fallingEdge()) {
        isSustainOn = !isSustainOn;
      }
    }
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
//  delay(50);
}



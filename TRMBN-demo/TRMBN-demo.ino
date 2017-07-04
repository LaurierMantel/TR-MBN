#include <Adafruit_MPL3115A2.h>
#include <Bounce.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK 9
#define BMP_MISO 10
#define BMP_MOSI 11 
#define BMP_CS 12

// MIDI
const int midiChan = 1;
const int velCtrl = 7;
const int modCtrl = 1;
const int midiValMin = 0;
const int midiValMax = 127;

// Position Sensor
const int positionPin = 33;
const int positionMax = 1023;
const int positionMin = 0;
const int noteMin = 96;
const int noteMax = 108;

// Force Sensor
const int fsrMin = 0;
const int fsrMax = 1000;
const int forcePin = 14;

// Pressure Sensor
Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);
//Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();
const float minPressureDiff = 100;
const float pressureRange = 1000;//2000;//5000;

// Sustain Button
const int buttonPin = 31;
Bounce sustainButton = Bounce(buttonPin, 10);  // 10 ms debounce

// State Variables
bool isInitPressureRead = false;
float minPressure = 0;
float maxPressure = 0;
bool isNoteOn = false;
bool isSustainOn = false;
int prevNote = 0;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  if (!bmp.begin()) {
    Serial.println("Couldnt find BMP280");
    return;
  }
//  if (! baro.begin()) {
//    Serial.println("Couldnt find MPL3115A2");
//    return;
//  }
}

void loop() {
  int rawPosition = analogRead(positionPin);
  int note = map(rawPosition, positionMin, positionMax, noteMin, noteMax);
  Serial.print("Note = "); Serial.println(note);
  Serial.print("RawPosition = "); Serial.println(rawPosition);
  int force = map(analogRead(forcePin), fsrMin, fsrMax, midiValMin, midiValMax);
  usbMIDI.sendControlChange(modCtrl, force, midiChan);
  Serial.print("Force = "); Serial.println(force);
  if (!isInitPressureRead) {
    isInitPressureRead = true;
    float initialPressure = bmp.readPressure();//baro.getPressure();
    minPressure = initialPressure + minPressureDiff;
    maxPressure = minPressure + pressureRange;
  }
//  float rawPressure = baro.getPressure();
  float rawPressure = bmp.readPressure();
  int pressure = map(rawPressure, minPressure, maxPressure, midiValMin, midiValMax);
  Serial.print("Raw Pressure = "); Serial.println(rawPressure);
  Serial.print("Pressure = "); Serial.println(pressure);
  Serial.print("isSustainOn = "); Serial.println(isSustainOn);
  Serial.print("isNoteOn = "); Serial.println(isNoteOn);
  if (isNoteOn) {
    if (note != prevNote) {
      usbMIDI.sendNoteOff(prevNote, 0, midiChan);
      usbMIDI.sendNoteOn(note, midiValMax, midiChan);
    }
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
      usbMIDI.sendNoteOff(note, 0, midiChan);
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
  Serial.println();
  prevNote = note;
  delay(50);
}



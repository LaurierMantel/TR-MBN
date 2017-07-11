//#include <Adafruit_MPL3115A2.h>
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
const int noteMin = 36;
const int noteMax = 48;

// Force Sensor
const int fsrMin = 0;
const int fsrMax = 1000;
const int forcePin = 14;

// Pressure Sensor
Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);
//Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();
const float minPressureDiff = 100;
const float pressureRange = 200;//1000;//2000;//5000;

// Sustain Button
const int buttonPin = 31;
Bounce sustainButton = Bounce(buttonPin, 10);  // 10 ms debounce


// Octave Sensor
//const int midiOctaveLength = 


// Pitch Bend
const int pitchBendMin = 6144;
const int pitchBendDefault = 8192;
const int pitchBendMax = 10240;
const int notePositions[15] = {0, 73, 146, 219, 292, 365, 438, 512, 585, 658, 731, 804, 877, 950, 1023};
const float positionStep = 1023/28.0;
/*
73, 146, 219, 292, 365, 438, 512, 585, 658, 731, 804, 877, 950, 1023
37, 110, 183, 256, 329, 402, 475, 548, 621, 694, 767, 840, 913, 986
*/

// State Variables
bool isInitPressureRead = false;
float minPressure = 0;
float maxPressure = 0;
bool isNoteOn = false;
bool isSustainOn = false;
int prevNote = noteMin;
int prevVel = 0;

// Internal State Variables
int velocity = 0;
int rawPosition = 0;
int note = 0;
int force = 0;
float rawPressure = 0;
int bend = pitchBendDefault;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  if (!bmp.begin()) {
    Serial.println("Couldnt find BMP280");
    while(true){ delay(1000);};
  }
//  if (! baro.begin()) {
//    Serial.println("Couldnt find MPL3115A2");
//    return;
//  }
}

void loop() {
  rawPosition = analogRead(positionPin);

  if (rawPosition < 73) {
    note = noteMin;
    bend = pitchBendDefault;
  } else if (rawPosition >= 73 && rawPosition <= 950) {
    note = map(rawPosition, positionMin, positionMax, noteMin, noteMax);
    int ceilThing = round(ceil(rawPosition/positionStep)*positionStep);
    int floorThing = round(floor(rawPosition/positionStep)*positionStep);
    int mid = 0;
    if (isBendDown(ceilThing)) {
      mid = ceilThing;
    } else {
      mid = floorThing;
    }
    bend = map(rawPosition, round(mid - positionStep), round(mid + positionStep), pitchBendMin, pitchBendMax);
  } else if (rawPosition > 950) {
    note = noteMax;
    bend = pitchBendDefault;
  }

  Serial.print("Note = "); Serial.println(note);
  Serial.print("Bend = "); Serial.println(bend);
  Serial.print("RawPosition = "); Serial.println(rawPosition);
  force = map(analogRead(forcePin), fsrMin, fsrMax, midiValMin, midiValMax);
  if (force > 10) { 
    isSustainOn = true;
  } else {
    isSustainOn = false;
  }
  Serial.print("Force = "); Serial.println(force);
  if (!isInitPressureRead) {
    isInitPressureRead = true;
    float initialPressure = bmp.readPressure();//baro.getPressure();
    minPressure = initialPressure + minPressureDiff;
    maxPressure = minPressure + pressureRange;
  }
  if (sustainButton.update()) {
    if (sustainButton.fallingEdge()) {
      isSustainOn = !isSustainOn;
    }
  }
  if (!isSustainOn) {
    rawPressure = bmp.readPressure();//baro.getPressure();
    velocity = map(rawPressure, minPressure, maxPressure, midiValMin, midiValMax);
  }
  Serial.print("Raw Pressure = "); Serial.println(rawPressure);
  Serial.print("Velocity = "); Serial.println(velocity);
  Serial.print("isSustainOn = "); Serial.println(isSustainOn);
  Serial.print("isNoteOn = "); Serial.println(isNoteOn);
  if (isNoteOn) {
    if (note != prevNote) {
      usbMIDI.sendNoteOff(prevNote, 0, midiChan);
      usbMIDI.sendNoteOn(note, midiValMax, midiChan);
    }
    usbMIDI.sendPitchBend(bend, midiChan);
    if (velocity > midiValMax) {
      Serial.println("test");
      usbMIDI.sendControlChange(velCtrl, midiValMax, midiChan);
    } else if (velocity > 0) {
      usbMIDI.sendControlChange(velCtrl, velocity, midiChan);
    } else if (velocity <= 0 && !isSustainOn) {
      usbMIDI.sendNoteOff(note, 0, midiChan);
      isNoteOn = false;
    }
  } else {
    if (velocity > midiValMax) {
      usbMIDI.sendNoteOn(note, midiValMax, midiChan);
      isNoteOn = true;
    } else if (velocity > 0) {
      usbMIDI.sendNoteOn(note, velocity, midiChan);
      isNoteOn = true;
    }
  }
  Serial.println();
  prevNote = note;
  prevVel = velocity;
  delay(50);
}

bool isBendDown(int pos) {
  for (int i = 0; i < 15; i++) {
    if (notePositions[i] == pos) {
      return true;
    }
  }
  return false;
}



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
int minPressure = 98000;
int maxPressure = 103000;
int velocityBuckets = 100;
int normDivisor = (maxPressure - minPressure)/velocityBuckets;
float pressure = -1.0;
int prevVal = 0;
const int buttonPin = 12;
bool isSustainOn = false;
Bounce pushbutton = Bounce(buttonPin, 10);  // 10 ms debounce
byte previousState = HIGH;         // what state was the button last time

void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));
  pinMode(buttonPin, INPUT_PULLUP);
  if (!bmp.begin()) {  
    Serial.println(F("Could Ânot find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}

void loop() {
    pressure = bmp.readPressure();
    Serial.print(F("Pressure = "));
    Serial.println(pressure);
    int normP = (pressure - minPressure)/normDivisor + 10;
    Serial.print("normP: ");
    Serial.println(normP);
    usbMIDI.sendNoteOff(note,0,channel);
    if (normP > velocityBuckets) {
      usbMIDI.sendNoteOn(note,velocityBuckets,channel);
    } else if (normP > 0) {
//      usbMIDI.sendNoteOff(note,0,channel);
      usbMIDI.sendNoteOn(note,normP,channel);
    } else if (normP <= 0) {
//      usbMIDI.sendNoteOff(note,0,channel);   
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


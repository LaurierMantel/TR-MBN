
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <CapacitiveSensor.h>

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
CapacitiveSensor sustainSensor = CapacitiveSensor(4,2);

void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));
  if (!bmp.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}

void loop() {
    Serial.print(F("Pressure = "));
    pressure = bmp.readPressure();

    int normP = (pressure - minPressure)/normDivisor + 10;
    Serial.print("Slurp my gooch");
    Serial.print(normDivisor);
    Serial.print("normP: ");
    Serial.println(normP);
    if (normP != prevVal) {
      usbMIDI.sendNoteOff(note,0,channel);   
      prevVal = normP;
    }
    if (normP > velocityBuckets) {
    //  usbMIDI.sendNoteOff(note,0,channel);
      Serial.println("1");
      usbMIDI.sendNoteOn(note,velocityBuckets,channel);
    } else if (normP > 0) {
     // usbMIDI.sendNoteOff(note,0,channel);
      Serial.println("2");
      usbMIDI.sendNoteOn(note,normP,channel);
    } else if (normP <= 0) {
      Serial.println("3");
      usbMIDI.sendNoteOff(note,0,channel);   
    }
//    long capacitiveTotal = sustainSensor.capacitiveSensor(30);
//    Serial.println(capacitiveTotal);
//    Serial.println("Capacitance");
//    if(pressure > initialPressure){
//      usbMIDI.sendNoteOn(note,velocity,channel); // Turn the note ON
//    }
//    // NEED TO TEST THIS VALUE
//    else if (capacitiveTotal < 10) {
//      usbMIDI.sendNoteOff(note,0,channel); // Turn the note OFF - don't forget to do this ;)      
//    }
//    Serial.println();
    delay(100);
}


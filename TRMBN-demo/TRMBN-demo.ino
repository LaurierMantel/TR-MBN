
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK 16
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10
float initialPressure = 0.0;
Adafruit_BMP280 bmp; // I2C
int channel = 1; // Defines the MIDI channel to send messages on (values from 1-16)
int velocity = 100; // Defines the velocity that the note plays at (values from 0-127)
int note = 46; // b flat on bass clef
float pressure = -1.0;

void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));
  
  if (!bmp.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
  else {
    initialPressure = bmp.readPressure();
    Serial.print("Initial pressure: ");
    Serial.print(initialPressure);
    Serial.println();
  }
}

void loop() {
    Serial.print(F("Pressure = "));
    pressure = bmp.readPressure();
    Serial.print(pressure);
    Serial.println(" Pa");
    if(pressure > initialPressure){
      usbMIDI.sendNoteOn(note,velocity,channel); // Turn the note ON
    }
    else {
      usbMIDI.sendNoteOff(note,0,channel); // Turn the note OFF - don't forget to do this ;)      
    }
    Serial.println();
    delay(1000);
}


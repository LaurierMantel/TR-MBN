#include <Adafruit_MPL3115A2.h>

Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();

long startTime;

void setup() {
  Serial.begin(9600);
  if (! baro.begin()) {
    Serial.println("Couldnt find sensor");
    return;
  }
}

void loop() {
  startTime = millis();
  float rawPressure = baro.getPressure();
  Serial.print("Raw Pressure = "); Serial.println(rawPressure);
  Serial.println(millis() - startTime);
}



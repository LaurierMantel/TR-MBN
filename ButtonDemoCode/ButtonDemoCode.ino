#include <Bounce.h>

const int buttonPin = 12;
Bounce pushbutton = Bounce(buttonPin, 10);  // 10 ms debounce

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(57600);
  Serial.println("Pushbutton Bounce library test:");
}

byte previousState = HIGH;         // what state was the button last time
unsigned int count = 0;            // how many times has it changed to low
unsigned long countAt = 0;         // when count changed
unsigned int countPrinted = 0;     // last count printed

void loop() {
  if (pushbutton.update()) {
    if (pushbutton.fallingEdge()) {
      count = count + 1;
      countAt = millis();
    }
  } else {
    if (count != countPrinted) {
      unsigned long nowMillis = millis();
      if (nowMillis - countAt > 100) {
        Serial.print("count: ");
        Serial.println(count);
        countPrinted = count;
      }
    }
  }
}

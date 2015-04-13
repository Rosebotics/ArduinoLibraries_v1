#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <MiniQCom.h>
#include <MiniQ.h>

MiniQ miniQ;

void setup() {
  Serial.begin(9600);
  miniQ.begin();
  
  miniQ.setLed(255, 0, 0);
  delay(1000);
  miniQ.setLed(0, 255, 0);
  delay(1000);
  miniQ.setLed(0, 0, 255);
  delay(1000);
  miniQ.setLed(0, 0, 0);
}

void loop() {
  if (Serial.available()) {
    char newByte = Serial.read();
    if (newByte == 'r') {
      miniQ.setLed(150, 0, 0);
    } else if (newByte == 'g') {
      miniQ.setLed(0, 150, 0);
    } else if (newByte == 'b') {
      miniQ.setLed(0, 0, 150);
    } else if (newByte == 'y') {
      miniQ.setLed(150, 150, 0);
    } else if (newByte == 'c') {
      miniQ.setLed(0, 150, 150);
    } else if (newByte == 'm') {
      miniQ.setLed(150, 0, 150);
    } else if (newByte == 'w') {
      miniQ.setLed(150, 150, 150);
    } else if (newByte == 'o') {
      miniQ.setLed(0, 0, 0);
    }
  }
}

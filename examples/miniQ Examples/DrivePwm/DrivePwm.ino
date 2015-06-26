#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <MiniQCom.h>
#include <MiniQ.h>

MiniQ miniQ;

void setup() {
  Serial.begin(9600);
  miniQ.begin();

  // Start with a forward back.
  miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, 200, 200); // Forwards
  delay(2000);
  miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, 0, 0);     // Stop
  delay(2000);
  miniQ.setDrivePwm(WHEEL_DIRECTION_REVERSE, WHEEL_DIRECTION_REVERSE, 200, 200); // Back
  delay(2000);
  miniQ.setDrivePwm(WHEEL_DIRECTION_REVERSE, WHEEL_DIRECTION_REVERSE, 0, 0);     // Stop
}

void loop() {
  if (Serial.available()) {
    char newByte = Serial.read();
    if (newByte == 'f') {
      miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, 200, 200);
    } else if (newByte == 'b') {
      miniQ.setDrivePwm(WHEEL_DIRECTION_REVERSE, WHEEL_DIRECTION_REVERSE, 200, 200);
    } else if (newByte == 's') {
      miniQ.setDrivePwm(WHEEL_DIRECTION_REVERSE, WHEEL_DIRECTION_REVERSE, 0, 0);
    }
  }
}

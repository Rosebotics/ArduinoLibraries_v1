#include <RobotAsciiCom.h>

RobotAsciiCom robotCom;

void setup() {
  Serial.begin(115200);
  robotCom.registerPwmCallback(pwmCallback);
}

void pwmCallback(int leftDutyCycle, int rightCutyCycle) {
  // TODO: Carry out the request using a libraries instead of hardcoding.
}

void loop() {
}


/** Send all bytes received to the RobotAsciiCom object. */
void serialEvent() {
  while (Serial.available()) {
    robotCom.handleRxByte(Serial.read());
  }
}

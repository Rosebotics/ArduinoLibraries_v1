#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <MiniQCom.h>
#include <MiniQ.h>

MiniQ miniQ;
MiniQCom miniQCom;

  
void setup() {
  Serial.begin(9600);
  miniQ.begin();
  miniQCom.registerDrivePwmCallback(updateDrivePwm);
  
  // I2C communication not used here, just a reference.
  Wire.begin(4);
  Wire.onReceive(receiveEvent); // Useful only to the slave (the miniQ)
  Wire.onRequest(requestEvent); // Useful only to the slave (the miniQ)
}

// Function that executes whenever data is received from master.
void receiveEvent(int howMany) {
  Serial.print("Received even with length ");
  Serial.println(howMany);
  while (Wire.available()) {
    miniQCom.handleRxByte(Wire.read());
  }
}

// Function that executes whenever data is requested by master.
void requestEvent() {
  // TODO: Send the appropriate message based on the state.
}

void updateDrivePwm(boolean leftIsForward, boolean rightIsForward, byte leftDutyCycle, byte rightDutyCycle) {
  Serial.print("DrivePwm - ");
  if (leftIsForward) {
    Serial.print("Left Forward @ ");
  } else {
    Serial.print("Left Reverse @ ");
  }
  Serial.print(leftDutyCycle);
  if (rightIsForward) {
    Serial.print("  Right Forward @ ");
  } else {
    Serial.print("  Right Reverse @ ");
  }
  Serial.print(rightDutyCycle);
  Serial.println(".");
  
  miniQ.setDrivePwm(leftIsForward, rightIsForward, leftDutyCycle, rightDutyCycle);
}


void loop() {
}



#include <Wire.h>
#include <MiniQCom.h>

#define PIN_PUSHBUTTON 7
#define PRESSED 0 

#define MINI_Q_ADDRESS 4
MiniQCom miniQCom(true, MINI_Q_ADDRESS);

void setup() {
  Serial.begin(9600);
  pinMode(PIN_PUSHBUTTON, INPUT_PULLUP);
  Wire.begin(); // No address required for the master.
  
  // Start the program with a 
  sendForwardBack();
}

void sendForwardBack() {
   // Start with a forward back.
  miniQCom.sendDrivePwm(1 ,1,200, 200); // Forwards
  delay(2000);
  miniQCom.sendDrivePwm(1, 1, 0, 0);     // Stop
  delay(2000);
  miniQCom.sendDrivePwm(0, 0, 200, 200); // Back
  delay(2000);
  miniQCom.sendDrivePwm(0, 0, 0, 0);     // Stop
}

void loop() {
  if (digitalRead(PIN_PUSHBUTTON) == PRESSED) {
    sendForwardBack();
    delay(1000); // Simplified debounce mechanism
  }
  
  if (Serial.available()) {
    char newByte = Serial.read();
    if (newByte == 'f') {
      Serial.println("Sending forward");
      miniQCom.sendDrivePwm(1 ,1, 200, 200);
    } else if (newByte == 'b') {
      Serial.println("Sending back");
      miniQCom.sendDrivePwm(0, 0, 200, 200);
    } else if (newByte == 's') {
      Serial.println("Sending stop");
      miniQCom.sendDrivePwm(0, 0, 0, 0);
    }
  }
}
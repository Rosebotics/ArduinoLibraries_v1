#include <Wire.h>
#include <MiniQCom.h>

MiniQCom miniQCom(false, 255); // Address is not actually used in library for a slave.

void setup() {
  Serial.begin(9600);
  miniQCom.registerDrivePwmCallback(updateDrivePwm);
  
  // I2C communication not used here, just a reference.
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void updateDrivePwm(boolean leftIsForward, boolean rightIsForward, byte leftDutyCycle, byte rightDutyCycle) {
  Serial.print("Update wheel pwm - ");
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
}

void loop() {
  runTest(0);
//  runTest(1);
//  runTest(2);
  
  // Output should be... (ignoring line breaks)
  // Test 0: Update wheel pwm - Left Forward @ 100  Right Forward @ 100
  // Test 1: 
  // Test 2: 
  // Test 3: 
  // Test 4: 
  // Test 5: 
  // Test 6: 
  // Test 7: 
  // Test 8: 
  // Test 9: 
  
  while(1);
}

// Function that executes whenever data is received from master.
void receiveEvent(int howMany) {
  while (Wire.available()) {
    miniQCom.handleRxByte(Wire.read());
  }
}

// Function that executes whenever data is requested by master.
void requestEvent() {
  // TODO: Send the appropriate message based on the state.
}


void runTest(int testNumber) {
  Serial.print("Test ");
  Serial.print(testNumber);
  Serial.print(": ");
  switch(testNumber) {
    case 0:
      test0();
      break;
    case 1:
      test1();
      break;
    case 2:
      test2();
      break;
    case 3:
      test3();
      break;
    case 4:
      test4();
      break;
    case 5:
      test5();
      break;
    default:
      Serial.println("Increase test counter size.");
      break;
  }
  Serial.println("");
}


void test0() {
  miniQCom.handleRxByte(START_BYTE);
  miniQCom.handleRxByte(DRIVE_PWM_COMMAND_LENGTH);
  miniQCom.handleRxByte(COMMAND_DRIVE_PWM);
  miniQCom.handleRxByte(3); // Both forward.
  miniQCom.handleRxByte(100);
  miniQCom.handleRxByte(100);
  // Manually calculate crc 0+3+100+100 = 203
  miniQCom.handleRxByte(53);
}

void test1() {
}

void test2() {
}

void test3() {
}

void test4() {
}

void test5() {
}

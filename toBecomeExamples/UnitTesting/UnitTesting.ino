#include <Wire.h>
#include <MiniQCom.h>

MiniQCom miniQCom(false, 255); // Address is not actually used in library for a slave.

void setup() {
  Serial.begin(9600);
  Serial.println("Hello");
  miniQCom.registerDrivePwmCallback(updateDrivePwm);
  miniQCom.registerDriveSpeedArcCallback(updateDriveSpeedArc);
  miniQCom.registerSensorMaskCallback(updateSensorMask);
  
  // I2C communication not used here, just a reference.
  Wire.begin(4);
  Wire.onReceive(receiveEvent); // Useful only to the slave (the miniQ)
  Wire.onRequest(requestEvent); // Useful only to the slave (the miniQ)
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
}

void updateDriveSpeedArc(int speedMmPerS, int arcMm) {
  Serial.print("DriveSpeedArc - ");
  Serial.print(speedMmPerS);
  Serial.print(" mm/s");
  if (arcMm == 0) {
    Serial.println(" straight ahead.");
    return;
  }
  Serial.print(" @ an arc of ");
  Serial.print(abs(arcMm));
  Serial.print(" mm to the ");
  if (arcMm > 0) {
    Serial.println("left.");
  } else {
    Serial.println("right.");
  }
}

void updateSensorMask(sensorMaskSetAddOrRemove_t setAddOrRemove, unsigned long sensorMask) {
  switch(setAddOrRemove) {
    case SENSOR_MASK_SET:
      Serial.print("SetSensorMask - ");
      break;
    case SENSOR_MASK_ADD:
      Serial.print("AddToSensorMask - ");
      break;
    case SENSOR_MASK_REMOVE:
      Serial.print("RemoveFromSensorMask - ");
      break;
  }
  Serial.print(sensorMask);
  Serial.print(" -->");
  
  if (sensorMask & BIT_LEFT_MOTOR_ENCODER) {
    Serial.print(" LEFT_MOTOR_ENCODER");
  }
  if (sensorMask & BIT_RIGHT_MOTOR_ENCODER) {
    Serial.print(" RIGHT_MOTOR_ENCODER");
  }
  if (sensorMask & BIT_FAR_LEFT_IR_LINE_SENSOR) {
    Serial.print(" FAR_LEFT_IR_LINE_SENSOR");
  }
  if (sensorMask & BIT_MID_LEFT_IR_LINE_SENSOR) {
    Serial.print(" MID_LEFT_IR_LINE_SENSOR");
  }
  if (sensorMask & BIT_CENTER_IR_LINE_SENSOR) {
    Serial.print(" CENTER_IR_LINE_SENSOR");
  }
  if (sensorMask & BIT_MID_RIGHT_IR_LINE_SENSOR) {
    Serial.print(" MID_RIGHT_IR_LINE_SENSOR");
  }
  if (sensorMask & BIT_FAR_RIGHT_IR_LINE_SENSOR) {
    Serial.print(" FAR_RIGHT_IR_LINE_SENSOR");
  }
  if (sensorMask & BIT_PHOTO_DETECTORS) {
    Serial.print(" PHOTO_DETECTORS");
  }
  if (sensorMask & BIT_IR_MODULE_COUNT) {
    Serial.print(" IR_MODULE_COUNT");
  }
  if (sensorMask & BIT_IR_MODULE_BYTE) {
    Serial.print(" IR_MODULE_BYTE");
  }
  if (sensorMask & BIT_KEY_BUTTONS) {
    Serial.print(" KEY_BUTTONS");
  }
  if (sensorMask & BIT_BATTERY_VOLTAGE_TENTHS) {
    Serial.print(" BATTERY_VOLTAGE_TENTHS");
  }
  Serial.println(".");
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

// Update to be the number of tests to run.
int numTests = 5;

void loop() {
  delay(5000);  // The MiniQ doesn't reset when the Serial Monitor program launches
  runTests();
  
  // Output should be... (ignoring line breaks)
  // Test 0: DrivePwm - Left Forward @ 100  Right Forward @ 100.
  // Test 1: DrivePwm - Left Forward @ 0  Right Reverse @ 255.
  // Test 2: DriveSpeedArc - 20000 mm/s @ an arc of 2 mm to the right.
  // Test 3: DriveSpeedArc - 0 mm/s straight ahead.
  // Test 4: SetSensorMask - 124 --> FAR_LEFT_IR_LINE_SENSOR MID_LEFT_IR_LINE_SENSOR CENTER_IR_LINE_SENSOR MID_RIGHT_IR_LINE_SENSOR FAR_RIGHT_IR_LINE_SENSOR.
  // Test 5: 
  // Test 6: 
  // Test 7: 
  // Test 8: 
  // Test 9: 
  
  // Just in case you missed the printed results the first time.
  delay(5000);
  Serial.println("\n---------\n");
  runTests();
  while(1) {
    delay(3000);
    Serial.println(".");
  };
}

void serialEvent() {
  while(Serial.available()) {
    char sentByte = Serial.read();
    if (sentByte == 'r') {
      runTests();
    }
  }
}

void runTests() {
  for (int i = 0; i < numTests; ++i) {
      runTest(i);
  }
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
}

//sendDrivePwm
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

//sendDrivePwm
void test1() {
  miniQCom.handleRxByte(START_BYTE);
  miniQCom.handleRxByte(DRIVE_PWM_COMMAND_LENGTH);
  miniQCom.handleRxByte(COMMAND_DRIVE_PWM);
  miniQCom.handleRxByte(2); // Left forward, Right reverse
  miniQCom.handleRxByte(0);
  miniQCom.handleRxByte(255);
  // Manually calculate crc 0+2+0+255 = 257
  miniQCom.handleRxByte(255);
}

//sendDriveSpeedArc
void test2() {
  miniQCom.handleRxByte(START_BYTE);
  miniQCom.handleRxByte(DRIVE_SPEED_ARC_COMMAND_LENGTH);
  miniQCom.handleRxByte(COMMAND_DRIVE_SPEED_ARC);
  miniQCom.handleRxByte(32); // LSB of 20000 
  miniQCom.handleRxByte(78); // MSB of 20000 (78 * 256 + 32)
  miniQCom.handleRxByte(254); // LSB of -2
  miniQCom.handleRxByte(255); // MSB of -2
  // Manually calculate crc 1+32+78+254+255 = 620%256 = 108
  miniQCom.handleRxByte(148);
}

//sendDriveSpeedArc
void test3() {
  miniQCom.handleRxByte(START_BYTE);
  miniQCom.handleRxByte(DRIVE_SPEED_ARC_COMMAND_LENGTH);
  miniQCom.handleRxByte(COMMAND_DRIVE_SPEED_ARC);
  miniQCom.handleRxByte(0); // LSB of 20000 
  miniQCom.handleRxByte(0); // MSB of 20000 (78 * 256 + 32)
  miniQCom.handleRxByte(0); // LSB of -2
  miniQCom.handleRxByte(0); // MSB of -2
  // Manually calculate crc 1+0+0+0+0 = 1
  miniQCom.handleRxByte(255);
}

//sendSetSensorMask
void test4() {
  miniQCom.handleRxByte(START_BYTE);
  miniQCom.handleRxByte(SENSOR_MASK_COMMAND_LENGTH);
  miniQCom.handleRxByte(COMMAND_SET_SENSOR_MASK);
  miniQCom.handleRxByte(BIT_FAR_LEFT_IR_LINE_SENSOR | BIT_MID_LEFT_IR_LINE_SENSOR |
                        BIT_CENTER_IR_LINE_SENSOR | BIT_MID_RIGHT_IR_LINE_SENSOR |
                        BIT_FAR_RIGHT_IR_LINE_SENSOR); // 00000000 00000000 00000000 01111100
  miniQCom.handleRxByte(0);
  miniQCom.handleRxByte(0);
  miniQCom.handleRxByte(0);
  // Manually calculate crc 2+124+0+0+0 = 126
  miniQCom.handleRxByte(130);
}

//sendAddToSensorMask
//sendRemoveFromSensorMask
//sendBuzzerTone
//sendLed
//sendSendIr
//sendIrModuleMode




void test5() {
}

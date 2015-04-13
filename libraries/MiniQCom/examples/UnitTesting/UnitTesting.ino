#include <Wire.h>
#include <MiniQCom.h>

MiniQCom miniQCom(false, 255); // Address is not actually used in library for a slave.

void setup() {
  Serial.begin(9600);
  miniQCom.registerDrivePwmCallback(updateDrivePwm);
  miniQCom.registerDriveSpeedArcCallback(updateDriveSpeedArc);
  miniQCom.registerSensorMaskCallback(updateSensorMask);
  miniQCom.registerBuzzerToneCallback(updateBuzzerToneCallback);
  miniQCom.registerLedCallback(updateLedCallback);
  miniQCom.registerSendIrCallback(updateSendIrCallback);
  miniQCom.registerIrModuleMode(updateIrModuleMode);
  
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

void updateSensorMask(sensorMaskSetAddOrRemove_t setAddOrRemove, unsigned long sensorMaskParameter) {
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
  Serial.print(sensorMaskParameter); // Prints the sensorMaskParameter value.
  Serial.print(" -->");
  
  // Then print the saved values.
  if (miniQCom.sensorMask & BIT_LEFT_MOTOR_ENCODER) {
    Serial.print(" LEFT_MOTOR_ENCODER");
  }
  if (miniQCom.sensorMask & BIT_RIGHT_MOTOR_ENCODER) {
    Serial.print(" RIGHT_MOTOR_ENCODER");
  }
  if (miniQCom.sensorMask & BIT_FAR_LEFT_IR_LINE_SENSOR) {
    Serial.print(" FAR_LEFT_IR_LINE_SENSOR");
  }
  if (miniQCom.sensorMask & BIT_MID_LEFT_IR_LINE_SENSOR) {
    Serial.print(" MID_LEFT_IR_LINE_SENSOR");
  }
  if (miniQCom.sensorMask & BIT_CENTER_IR_LINE_SENSOR) {
    Serial.print(" CENTER_IR_LINE_SENSOR");
  }
  if (miniQCom.sensorMask & BIT_MID_RIGHT_IR_LINE_SENSOR) {
    Serial.print(" MID_RIGHT_IR_LINE_SENSOR");
  }
  if (miniQCom.sensorMask & BIT_FAR_RIGHT_IR_LINE_SENSOR) {
    Serial.print(" FAR_RIGHT_IR_LINE_SENSOR");
  }
  if (miniQCom.sensorMask & BIT_PHOTO_DETECTORS) {
    Serial.print(" PHOTO_DETECTORS");
  }
  if (miniQCom.sensorMask & BIT_IR_MODULE_COUNT) {
    Serial.print(" IR_MODULE_COUNT");
  }
  if (miniQCom.sensorMask & BIT_IR_MODULE_BYTE) {
    Serial.print(" IR_MODULE_BYTE");
  }
  if (miniQCom.sensorMask & BIT_KEY_BUTTONS) {
    Serial.print(" KEY_BUTTONS");
  }
  if (miniQCom.sensorMask & BIT_BATTERY_VOLTAGE_TENTHS) {
    Serial.print(" BATTERY_VOLTAGE_TENTHS");
  }
  Serial.println(".");
}


void updateBuzzerToneCallback(unsigned int frequency, unsigned long durationMs) {
  Serial.print("BuzzerTone - Play frequency ");
  Serial.print(frequency);
  Serial.print("Hz for a duration of ");
  Serial.print(durationMs);
  Serial.println(" milliseconds.");
}

void updateLedCallback(byte red, byte green, byte blue) {
  Serial.print("Led - R ");
  Serial.print(red);
  Serial.print("  G ");
  Serial.print(green);
  Serial.print("  B ");
  Serial.print(blue);
  Serial.println(".");
}

void updateSendIrCallback(byte byteToSend, unsigned int durationMs) {
  Serial.println("SendIr - Not likily to get implemented. :)");
}

void updateIrModuleMode(irModuleMode_t irModuleMode) {
  Serial.print("IrModuleMode - ");
  switch(irModuleMode) {
    case IR_MODULE_MODE_OFF:
      Serial.println("IR_MODULE_MODE_OFF.");
      break;
    case IR_MODULE_MODE_COUNT:
      Serial.println("IR_MODULE_MODE_COUNT.");
      break;
    case IR_MODULE_MODE_BYTE:
      Serial.println("IR_MODULE_MODE_BYTE.");
      break;
  }
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
int numTests = 11;

void loop() {
  delay(5000);  // The MiniQ doesn't reset when the Serial Monitor program launches
  runTests();
  
  // Output should be... (ignoring line breaks)
  // Test 0: DrivePwm - Left Forward @ 100  Right Forward @ 100.
  // Test 1: DrivePwm - Left Forward @ 0  Right Reverse @ 255.
  // Test 2: DriveSpeedArc - 20000 mm/s @ an arc of 2 mm to the right.
  // Test 3: DriveSpeedArc - 0 mm/s straight ahead.
  // Test 4: SetSensorMask - 124 --> FAR_LEFT_IR_LINE_SENSOR MID_LEFT_IR_LINE_SENSOR CENTER_IR_LINE_SENSOR MID_RIGHT_IR_LINE_SENSOR FAR_RIGHT_IR_LINE_SENSOR.
  // Test 5: AddToSensorMask - 1024 --> FAR_LEFT_IR_LINE_SENSOR MID_LEFT_IR_LINE_SENSOR CENTER_IR_LINE_SENSOR MID_RIGHT_IR_LINE_SENSOR FAR_RIGHT_IR_LINE_SENSOR KEY_BUTTONS.
  // Test 6: RemoveFromSensorMask - 56 --> MID_LEFT_IR_LINE_SENSOR CENTER_IR_LINE_SENSOR MID_RIGHT_IR_LINE_SENSOR KEY_BUTTONS.
  // Test 7: BuzzerTone - Play frequency 257Hz for a duration of 16909060 milliseconds.
  // Test 8: Led - R 1  G 2  B 3.
  // Test 9: SendIr - Not implemented
  // Test 10: IrModuleMode - IR_MODULE_MODE_COUNT.
 
  
  // Just in case you missed the printed results the first time.
  delay(5000);
  Serial.println("\n---------\n");
  runTests();
  while(1) {
    while(Serial.available()) {
      char sentByte = Serial.read();
      if (sentByte == 'r') {
        runTests();
      }
    }
  };
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
    case 6:
      test6();
      break;
    case 7:
      test7();
      break;
    case 8:
      test8();
      break;
    case 9:
      test9();
      break;
    case 10:
      test10();
      break;
    case 11:
      test11();
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
void test5() {
  miniQCom.handleRxByte(START_BYTE);
  miniQCom.handleRxByte(SENSOR_MASK_COMMAND_LENGTH);
  miniQCom.handleRxByte(COMMAND_ADD_TO_SENSOR_MASK);
  miniQCom.handleRxByte(0); // Add BIT_KEY_BUTTONS
  miniQCom.handleRxByte(4); // 00000000 00000000 00000100 00000000
  miniQCom.handleRxByte(0);
  miniQCom.handleRxByte(0);
  // Manually calculate crc 3+0+4+0+0 = 7
  miniQCom.handleRxByte(249);
}


//sendRemoveFromSensorMask
void test6() {
  miniQCom.handleRxByte(START_BYTE);
  miniQCom.handleRxByte(SENSOR_MASK_COMMAND_LENGTH);
  miniQCom.handleRxByte(COMMAND_REMOVE_FROM_SENSOR_MASK);
  miniQCom.handleRxByte(BIT_FAR_LEFT_IR_LINE_SENSOR | BIT_FAR_RIGHT_IR_LINE_SENSOR); // 00000000 00000000 00000000 01000100
  miniQCom.handleRxByte(0);
  miniQCom.handleRxByte(0);
  miniQCom.handleRxByte(0);
  // Manually calculate crc 4+68+0+0+0 = 72
  miniQCom.handleRxByte(184);
}

//sendBuzzerTone
void test7() {
  miniQCom.handleRxByte(START_BYTE);
  miniQCom.handleRxByte(BUZZER_TONE_COMMAND_LENGTH);
  miniQCom.handleRxByte(COMMAND_BUZZER_TONE);
  miniQCom.handleRxByte(1);
  miniQCom.handleRxByte(1); // 257
  miniQCom.handleRxByte(4); // Total is 16909060
  miniQCom.handleRxByte(3); // 3 * 256
  miniQCom.handleRxByte(2); // 2 * 256 * 256
  miniQCom.handleRxByte(1); // 1*256*256*256
  // Manually calculate crc  = 5+1+1+4+3+2+1 = 17
  miniQCom.handleRxByte(239);
}

//sendLed
void test8() {
  miniQCom.handleRxByte(START_BYTE);
  miniQCom.handleRxByte(LED_COMMAND_LENGTH);
  miniQCom.handleRxByte(COMMAND_LED);
  miniQCom.handleRxByte(1);
  miniQCom.handleRxByte(2);
  miniQCom.handleRxByte(3);
  // Manually calculate crc  = 6+1+2+3 = 12
  miniQCom.handleRxByte(244);
}

//sendSendIr
void test9() {
  miniQCom.handleRxByte(START_BYTE);
  miniQCom.handleRxByte(SEND_IR_COMMAND_LENGTH);
  miniQCom.handleRxByte(COMMAND_SEND_IR);
  miniQCom.handleRxByte(64);
  miniQCom.handleRxByte(2);
  miniQCom.handleRxByte(2); // 514
  // Manually calculate crc  = 7+64+2+2 = 75
  miniQCom.handleRxByte(181);
}

//sendIrModuleMode
void test10() {
  miniQCom.handleRxByte(START_BYTE);
  miniQCom.handleRxByte(IR_MODULE_MODE_COMMAND_LENGTH);
  miniQCom.handleRxByte(COMMAND_IR_MODULE_MODE);
  miniQCom.handleRxByte(IR_MODULE_MODE_COUNT);
  // Manually calculate crc  = 8 + 1 = 9
  miniQCom.handleRxByte(247);
}

// Placeholder
void test11() {
//  miniQCom.handleRxByte(START_BYTE);
//  miniQCom.handleRxByte(_COMMAND_LENGTH);
//  miniQCom.handleRxByte(COMMAND_);
//  miniQCom.handleRxByte(0);
//  miniQCom.handleRxByte(0);
//  // Manually calculate crc  = 
//  miniQCom.handleRxByte();
}

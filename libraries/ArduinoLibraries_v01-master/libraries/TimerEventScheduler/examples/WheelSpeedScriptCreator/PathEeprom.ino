
int nextFreeEepromAddress = 2;  // Skip 2 bytes for length of recording info.
unsigned long scriptStartTime = 0;

#define BYTES_PER_WHEEL_SPEED_COMMAND 8   // Four bytes for the time, four bytes for the command itself.
#define SCRIPT_LENGTH_MSB_ADDRESS 0
#define SCRIPT_LENGTH_LSB_ADDRESS 1

void printRecording() {
  Serial.println();
  Serial.println("private void runScriptBlue01() {");
  int eepromAddress = 2;
  temporaryVariableForScriptLength = getScriptLengthFromEeprom();
  for (int i = 0; i <= temporaryVariableForScriptLength; i++) {
    unsigned long eventTime = getScriptTimeAtAddress(eepromAddress);
    eepromAddress += 4;
    byte leftMode = EEPROM.read(eepromAddress);
    eepromAddress++;
    byte rightMode = EEPROM.read(eepromAddress);
    eepromAddress++;
    byte leftDutyCycle = EEPROM.read(eepromAddress);
    eepromAddress++;
    byte rightDutyCycle = EEPROM.read(eepromAddress);
    eepromAddress++;

    // This is a bit ugly to maintain, but seems to work ok as Java code.
    Serial.println("  mCommandHandler.postDelayed(new Runnable() {");
    Serial.println("    @Override");
    Serial.println("    public void run() {");
    
    if (i < temporaryVariableForScriptLength) {
      Serial.print("      sendCommand(\"WHEEL SPEED ");
      if (leftMode == WHEEL_MODE_BRAKE) {
        Serial.print("BRAKE ");
      } else if (leftMode == WHEEL_MODE_REVERSE) {
        Serial.print("REVERSE ");
      } else {
        Serial.print("FORWARD ");
      }
      Serial.print(leftDutyCycle);
      if (rightMode == WHEEL_MODE_BRAKE) {
        Serial.print(" BRAKE ");
      } else if (rightMode == WHEEL_MODE_REVERSE) {
        Serial.print(" REVERSE ");
      } else {
        Serial.print(" FORWARD ");
      }
      Serial.print(rightDutyCycle);
      Serial.println("\");");
    } else {
      Serial.println("      setState(State.WAITING_FOR_GPS);");
    }
    Serial.println("    }");
    Serial.print("  }, ");
    Serial.print(eventTime);
    Serial.println(");");


// --------------- Original printing method that is one line per command

//    if (i < 10) {
//      Serial.print(" ");
//    }
//    Serial.print(i);
//    Serial.print(".  WHEEL SPEED ");
//    if (leftMode == WHEEL_MODE_BRAKE) {
//      Serial.print("BRAKE ");
//    } else if (leftMode == WHEEL_MODE_REVERSE) {
//      Serial.print("REVERSE ");
//    } else {
//      Serial.print("FORWARD ");
//    }
//    Serial.print(leftDutyCycle);
//    if (rightMode == WHEEL_MODE_BRAKE) {
//      Serial.print(" BRAKE ");
//    } else if (rightMode == WHEEL_MODE_REVERSE) {
//      Serial.print(" REVERSE ");
//    } else {
//      Serial.print(" FORWARD ");
//    }
//    Serial.print(rightDutyCycle);
//    
//    if (leftDutyCycle < 10) {
//      Serial.print(" ");
//    }
//    if (leftDutyCycle < 100) {
//      Serial.print(" ");
//    }
//    if (rightDutyCycle < 10) {
//      Serial.print(" ");
//    }
//    if (rightDutyCycle < 100) {
//      Serial.print(" ");
//    }
//    if (rightMode == WHEEL_MODE_BRAKE) {
//      Serial.print("  ");
//    }
//    if (leftMode == WHEEL_MODE_BRAKE) {
//      Serial.print("  ");
//    }
//    Serial.print("   post at ");
//    Serial.print(eventTime);
//    Serial.println(" ms");
  }
  Serial.println("}");
  Serial.println();
}

void startRecording() {
  temporaryVariableForScriptLength = 0;
  scriptStartTime = millis();
  nextFreeEepromAddress = 2;
}

void endRecording() {
  // Save the time only at the end.  Don't increment command counter.
  unsigned long scriptTime = getScriptTime();
  byte time_3 = (scriptTime >> 24) & (0xFF);
  byte time_2 = (scriptTime >> 16) & (0xFF);
  byte time_1 = (scriptTime >> 8) & (0xFF);
  byte time_0 = (scriptTime) & (0xFF);
  EEPROM.write(nextFreeEepromAddress, time_3);
  nextFreeEepromAddress++;
  EEPROM.write(nextFreeEepromAddress, time_2);
  nextFreeEepromAddress++;
  EEPROM.write(nextFreeEepromAddress, time_1);
  nextFreeEepromAddress++;
  EEPROM.write(nextFreeEepromAddress, time_0);
  nextFreeEepromAddress++;  
  writeScriptLengthToEeprom(temporaryVariableForScriptLength);
}

int getScriptLengthFromEeprom() {
  int returnScriptLength = EEPROM.read(SCRIPT_LENGTH_MSB_ADDRESS);
  returnScriptLength = returnScriptLength << 8;
  returnScriptLength += EEPROM.read(SCRIPT_LENGTH_LSB_ADDRESS);
  return returnScriptLength;
}

int writeScriptLengthToEeprom(int scriptLength) {  
  byte scriptLengthMsb = (scriptLength >> 8) & 0xFF;
  byte scriptLengthLsb = (scriptLength) & 0xFF;
  EEPROM.write(SCRIPT_LENGTH_MSB_ADDRESS, scriptLengthMsb);
  EEPROM.write(SCRIPT_LENGTH_LSB_ADDRESS, scriptLengthLsb);
}
      
      
void writeWheelSpeedCommandToEeprom(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) {
  temporaryVariableForScriptLength++; // Increment the number of commands saved.
  unsigned long scriptTime = getScriptTime();
  byte time_3 = (scriptTime >> 24) & (0xFF);
  byte time_2 = (scriptTime >> 16) & (0xFF);
  byte time_1 = (scriptTime >> 8) & (0xFF);
  byte time_0 = (scriptTime) & (0xFF);
  EEPROM.write(nextFreeEepromAddress, time_3);
  nextFreeEepromAddress++;
  EEPROM.write(nextFreeEepromAddress, time_2);
  nextFreeEepromAddress++;
  EEPROM.write(nextFreeEepromAddress, time_1);
  nextFreeEepromAddress++;
  EEPROM.write(nextFreeEepromAddress, time_0);
  nextFreeEepromAddress++;
  EEPROM.write(nextFreeEepromAddress, leftMode);
  nextFreeEepromAddress++;
  EEPROM.write(nextFreeEepromAddress, rightMode);
  nextFreeEepromAddress++;
  EEPROM.write(nextFreeEepromAddress, leftDutyCycle);
  nextFreeEepromAddress++;
  EEPROM.write(nextFreeEepromAddress, rightDutyCycle);
  nextFreeEepromAddress++;
}

unsigned long getScriptTime() {
  return millis() - scriptStartTime;
}

void runWheelSpeedCommandAtAddress(int eepromAddress) {
  byte leftMode = EEPROM.read(eepromAddress);
  byte rightMode = EEPROM.read(eepromAddress + 1);
  byte leftDutyCycle = EEPROM.read(eepromAddress + 2);
  byte rightDutyCycle = EEPROM.read(eepromAddress + 3);
  wtc.sendWheelSpeed(leftMode, rightMode, leftDutyCycle, rightDutyCycle);
  displayCommandOnLcd(leftMode, rightMode, leftDutyCycle, rightDutyCycle);
}

unsigned long getScriptTimeAtAddress(int eepromAddress) {
  unsigned long returnTime = EEPROM.read(eepromAddress);
  returnTime = returnTime << 8;
  returnTime += EEPROM.read(eepromAddress + 1);
  returnTime = returnTime << 8;
  returnTime += EEPROM.read(eepromAddress + 2);
  returnTime = returnTime << 8;
  returnTime += EEPROM.read(eepromAddress + 3);
  return returnTime;
}




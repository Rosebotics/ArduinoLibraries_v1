/**
 * This code runs on the ADK board to send signals to the Wild Thumper controller.
 * It is designed to work with an XBee.  Messages sent will be saved to the EEPROM.
 */

#include <WildThumperCom.h>
#include <LiquidCrystal.h>
#include <TimerEvent.h>
#include <TimerEventScheduler.h>
#include <EEPROM.h>

#define TEAM_NUMBER 4


/***  Pin I/O   ***/ 
#define PIN_LED_1 64
#define PIN_LED_2 65
#define PIN_LED_3 66
#define PIN_LED_4 67
#define PIN_LED_5 68
#define PIN_LED_6 69
#define PIN_RIGHT_BUTTON 2
#define PIN_LEFT_BUTTON 3
#define PIN_SELECT_BUTTON 21
#define PIN_CONTRAST_ANALOG 8
#define PIN_HORZ_ANALOG 0
#define PIN_VERT_ANALOG 1

/*** Interrupt flags ***/
volatile int mainEventFlags = 0;
#define FLAG_RIGHT_BUTTON     0x0001
#define FLAG_LEFT_BUTTON      0x0002
#define FLAG_SELECT_BUTTON    0x0004
#define FLAG_PLAYBACK_SCRIPT  0x0008

TimerEventScheduler tes;
TimerEvent leftButtonDebounceTimerEvent(leftButtonDebounceCallback, 0);
TimerEvent rightButtonDebounceTimerEvent(rightButtonDebounceCallback, 0);
TimerEvent selectButtonDebounceTimerEvent(selectButtonDebounceCallback, 0);

LiquidCrystal lcd(14, 15, 16, 17, 18, 19, 20);
#define LINE_1 0
#define LINE_2 1

#define WHEEL_MODE_REVERSE 0
#define WHEEL_MODE_BRAKE   1
#define WHEEL_MODE_FORWARD 2
#define CENTER_DEADBAND 5
#define CHANGE_THRESHOLD 5

WildThumperCom wtc(TEAM_NUMBER);
int lastSentVertReading = 0;
int lastSentHorzReading = 0;
byte leftMode, rightMode, leftDutyCycle, rightDutyCycle;
int initialHorzReading, initialVertReading;

// Possible states the recorder might be in.
#define NORMAL_DRIVE_RECORDING_OFF  0
#define RECORDING_ACTIVE            1
#define PLAYBACK_ACTIVE             2
int state = NORMAL_DRIVE_RECORDING_OFF;
boolean recordingLedOn = false;

// Variables for other tabs that are used here too.
int temporaryVariableForScriptLength = 0;


void setup() {
  Serial.begin(9600);
  initialHorzReading = analogRead(PIN_HORZ_ANALOG) - 512;
  initialVertReading = analogRead(PIN_VERT_ANALOG) - 512;
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(PIN_LED_3, OUTPUT);
  pinMode(PIN_LED_4, OUTPUT);
  pinMode(PIN_LED_5, OUTPUT);
  pinMode(PIN_LED_6, OUTPUT);
  digitalWrite(PIN_LED_1, LOW);
  digitalWrite(PIN_LED_2, LOW);
  digitalWrite(PIN_LED_3, LOW);
  digitalWrite(PIN_LED_4, LOW);
  digitalWrite(PIN_LED_5, LOW);
  digitalWrite(PIN_LED_6, LOW);
  pinMode(PIN_LEFT_BUTTON, INPUT_PULLUP);
  pinMode(PIN_RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(PIN_SELECT_BUTTON, INPUT_PULLUP);
  attachInterrupt(0, int0_isr, FALLING);
  attachInterrupt(1, int1_isr, FALLING);
  attachInterrupt(2, int2_isr, FALLING);
  tes.addTimerEvent(&leftButtonDebounceTimerEvent);
  tes.addTimerEvent(&rightButtonDebounceTimerEvent);
  tes.addTimerEvent(&selectButtonDebounceTimerEvent);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Recording off");
  setState(NORMAL_DRIVE_RECORDING_OFF);
  setupExtensionForScripts();
}

void loop() {
  loopExtensionForScripts();
  // Read both analog sensors
  int horzReading = analogRead(PIN_HORZ_ANALOG) - 512 - initialHorzReading;
  int vertReading = analogRead(PIN_VERT_ANALOG) - 512 - initialVertReading;
  // Make near center readings exactly center
  if (abs(vertReading) < CENTER_DEADBAND) {
    vertReading = 0;
  }
  if (abs(horzReading) < CENTER_DEADBAND) {
    horzReading = 0;
  }
  
  // Handle button clicks.
  if (mainEventFlags & FLAG_RIGHT_BUTTON) {
    mainEventFlags &= ~FLAG_RIGHT_BUTTON;
    if (state == RECORDING_ACTIVE) {
      setState(NORMAL_DRIVE_RECORDING_OFF);
    } else {
      setState(RECORDING_ACTIVE);
    }
  }
  if (mainEventFlags & FLAG_LEFT_BUTTON) {
    mainEventFlags &= ~FLAG_LEFT_BUTTON;
    if (state == PLAYBACK_ACTIVE) {
      setState(NORMAL_DRIVE_RECORDING_OFF);
    } else {
      setState(PLAYBACK_ACTIVE);
    }
  }
  if (mainEventFlags & FLAG_SELECT_BUTTON) {
    mainEventFlags &= ~FLAG_SELECT_BUTTON;
    if (state == NORMAL_DRIVE_RECORDING_OFF) {
      printRecording();
    }
    setState(NORMAL_DRIVE_RECORDING_OFF);    
  } 
    
  // Determine if the change is worth mentioning to the Thumper.
  if (abs(vertReading - lastSentVertReading) > CHANGE_THRESHOLD ||
      abs(horzReading - lastSentHorzReading) > CHANGE_THRESHOLD) {
      lastSentVertReading = vertReading;
      lastSentHorzReading = horzReading;
      
      // Mix the signals to convert to wheel duty cycles.
      int leftMotor = (vertReading - horzReading) / 2;
      int rightMotor = (vertReading + horzReading) / 2;
      if (abs(leftMotor) < CENTER_DEADBAND) {
        leftMode = WHEEL_MODE_BRAKE;
      } else if (leftMotor > 0) {
        leftMode = WHEEL_MODE_FORWARD;
      } else {
        leftMode = WHEEL_MODE_REVERSE;
      }
      if (abs(rightMotor) < CENTER_DEADBAND) {
        rightMode = WHEEL_MODE_BRAKE;
      } else if (rightMotor > 0) {
        rightMode = WHEEL_MODE_FORWARD;
      } else {
        rightMode = WHEEL_MODE_REVERSE;
      }
      leftDutyCycle = min(abs(leftMotor), 255);
      rightDutyCycle = min(abs(rightMotor), 255);
      
      // Send it to the Thumper and record if in the record state.
      wtc.sendWheelSpeed(leftMode, rightMode, leftDutyCycle, rightDutyCycle);
      if (state == RECORDING_ACTIVE) {
        writeWheelSpeedCommandToEeprom(leftMode, rightMode, leftDutyCycle, rightDutyCycle);
      }
      
      // Print the value on the LCD.
      displayCommandOnLcd(leftMode, rightMode, leftDutyCycle, rightDutyCycle);
      if (state == RECORDING_ACTIVE) {
        // Display the number of commands saved.
        lcd.setCursor(6, LINE_2);
        lcd.print(temporaryVariableForScriptLength);
      }
  }
  
  // Flash when recording
  if (state == RECORDING_ACTIVE) {
    if (recordingLedOn) {
      digitalWrite(PIN_LED_6, LOW);
      recordingLedOn = false;
    } else {
      digitalWrite(PIN_LED_6, HIGH);
      recordingLedOn = true;
    }
  }
  delay(100);
}

void setState(int newState) {
  if (state == RECORDING_ACTIVE && newState != RECORDING_ACTIVE) {
    endRecording();
  }
  digitalWrite(PIN_LED_1, LOW);
  digitalWrite(PIN_LED_2, LOW);
  digitalWrite(PIN_LED_3, LOW);
  digitalWrite(PIN_LED_4, LOW);
  digitalWrite(PIN_LED_5, LOW);
  digitalWrite(PIN_LED_6, LOW);
  lcd.clear();
  lcd.home();
  switch (newState) {
    case NORMAL_DRIVE_RECORDING_OFF:
      digitalWrite(PIN_LED_2, HIGH);
      lcd.print("Recording off");
      break;
    case PLAYBACK_ACTIVE:
      digitalWrite(PIN_LED_4, HIGH);
      lcd.print("Playback mode");
      startPlayback();
      break;
    case RECORDING_ACTIVE:
      digitalWrite(PIN_LED_6, HIGH);
      lcd.print("Begin recording");
      recordingLedOn = true;
      startRecording();
      break;
  }
  state = newState;
}

void int0_isr() {
  // In 20 ms see if the button is still pressed (software debounce).
  rightButtonDebounceTimerEvent.setTimerTicksRemaining(20);
}

void int1_isr() {
  // In 20 ms see if the button is still pressed (software debounce).
  leftButtonDebounceTimerEvent.setTimerTicksRemaining(20);
}

void int2_isr() {
  // In 20 ms see if the button is still pressed (software debounce).
  selectButtonDebounceTimerEvent.setTimerTicksRemaining(20);
}

int rightButtonDebounceCallback() {
  if (!digitalRead(PIN_RIGHT_BUTTON)) {
    mainEventFlags |= FLAG_RIGHT_BUTTON;
  }
  return 0; // Don't call this Timer Event again (0 is off).
}

int leftButtonDebounceCallback() {
  if (!digitalRead(PIN_LEFT_BUTTON)) {
    mainEventFlags |= FLAG_LEFT_BUTTON;
  }
  return 0;  // Don't call this Timer Event again (0 is off).
}

int selectButtonDebounceCallback() {
  if (!digitalRead(PIN_SELECT_BUTTON)) {
    mainEventFlags |= FLAG_SELECT_BUTTON;
  }
  return 0; // Don't call this Timer Event again (0 is off).
}

void displayCommandOnLcd(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) {
  lcd.clear();
  lcd.home();
  switch (leftMode) {
    case WHEEL_MODE_REVERSE:
      lcd.print("Reverse");
      break;
    case WHEEL_MODE_BRAKE:
      lcd.print("Brake");
      break;
    case WHEEL_MODE_FORWARD:
      lcd.print("Forward");
      break;
  }
  lcd.setCursor(0, LINE_2);
  lcd.print(leftDutyCycle);
  lcd.setCursor(9, LINE_1);
  switch (rightMode) {
    case WHEEL_MODE_REVERSE:
      lcd.print("Reverse");
      break;
    case WHEEL_MODE_BRAKE:
      lcd.print("  Brake");
      break;
    case WHEEL_MODE_FORWARD:
      lcd.print("Forward");
      break;
  }
  lcd.setCursor(13, LINE_2);
  if (rightDutyCycle < 100) {
    lcd.print(" ");
  }
  if (rightDutyCycle < 10) {
    lcd.print(" ");
  }
  lcd.print(rightDutyCycle);
}


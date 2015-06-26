/**
 * This code runs on the ADK board to send signals to the Wild Thumper controller.
 * It is designed to work with an XBee.
 * This program should be used when you are calibrating your code to drive certain arcs.
 */

#include <WildThumperCom.h>
#include <LiquidCrystal.h>
#include <TimerEvent.h>
#include <TimerEventScheduler.h>

#define TEAM_NUMBER 4


/***  Pin I/O   ***/ 
#define PIN_RIGHT_BUTTON 2
#define PIN_LEFT_BUTTON 3
#define PIN_SELECT_BUTTON 21
#define PIN_HORZ_ANALOG 0
#define PIN_VERT_ANALOG 1

/*** Interrupt flags ***/
volatile int mainEventFlags = 0;
#define FLAG_RIGHT_BUTTON     0x0001
#define FLAG_LEFT_BUTTON      0x0002
#define FLAG_SELECT_BUTTON    0x0004

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
#define DEFAULT_DUTY_CYCLE 255.0

WildThumperCom wtc(TEAM_NUMBER);

// Possible states the timer might be in.
#define TIMER_STATE_OFF     0
#define TIMER_STATE_RUNNING 1
#define TIMER_STATE_PAUSED  2
int timerState = TIMER_STATE_OFF;
unsigned long timerStartValue;
unsigned long timerValue; // Useful when paused or stopped.
int holdingLeftButtonCounter = 0;

// Possible driving states.
#define DRIVE_STATE_STOPPED     0
#define DRIVE_STATE_DRIVING     1
int driveState = DRIVE_STATE_STOPPED;
float leftDutyCycle = DEFAULT_DUTY_CYCLE, rightDutyCycle = DEFAULT_DUTY_CYCLE;  // Really bytes but easier to make ints then cast.

#define LOW_THRESHOLD 50
#define LOW_MIDDLE_THRESHOLD 450
#define HIGH_MIDDLE_THRESHOLD 573
#define HIGH_THRESHOLD 973

#define SMALL_CHANGE 0.1
#define LARGE_CHANGE 0.5

void setup() {
  Serial.begin(9600);
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
  lcd.home();
  lcd.print("L");
  lcd.setCursor(15, LINE_1);
  lcd.print("R");
  setDriveState(DRIVE_STATE_STOPPED);
  setTimerState(TIMER_STATE_OFF);
}

void loop() {
  if (mainEventFlags & FLAG_RIGHT_BUTTON) {
    mainEventFlags &= ~FLAG_RIGHT_BUTTON;
    if (driveState == DRIVE_STATE_DRIVING) {
      setDriveState(DRIVE_STATE_STOPPED);
    } else {
      setDriveState(DRIVE_STATE_DRIVING);
    }
  }
  if (mainEventFlags & FLAG_LEFT_BUTTON) {
    mainEventFlags &= ~FLAG_LEFT_BUTTON;
    if (holdingLeftButtonCounter < 1000) {
      if (timerState == TIMER_STATE_RUNNING) {
        setTimerState(TIMER_STATE_PAUSED);
      } else {
        setTimerState(TIMER_STATE_RUNNING);
      }
    }
  }
  if (mainEventFlags & FLAG_SELECT_BUTTON) {
    mainEventFlags &= ~FLAG_SELECT_BUTTON;
    leftDutyCycle = DEFAULT_DUTY_CYCLE;
    rightDutyCycle = DEFAULT_DUTY_CYCLE;
  }
  if (!digitalRead(PIN_LEFT_BUTTON)) {
    holdingLeftButtonCounter++;
    if (holdingLeftButtonCounter > 100 && timerState != TIMER_STATE_OFF) {
      setTimerState(TIMER_STATE_OFF);
    }
  } else {
    holdingLeftButtonCounter = 0;
  }
  
  
  // Only allow duty cycle changes while stopped.
  if (driveState == DRIVE_STATE_STOPPED) {
    int horzReading = analogRead(PIN_HORZ_ANALOG);
    int vertReading = analogRead(PIN_VERT_ANALOG);
    if (horzReading < LOW_THRESHOLD) {
      // Hard Right!
      if (leftDutyCycle < 255) {
        leftDutyCycle += LARGE_CHANGE;
      } else {
        rightDutyCycle -= LARGE_CHANGE;
      }
    } else if (horzReading < LOW_MIDDLE_THRESHOLD) {
      // Small right
      if (leftDutyCycle < 255) {
        leftDutyCycle += SMALL_CHANGE;
      } else {
        rightDutyCycle -= SMALL_CHANGE;
      }
    } else if (horzReading < HIGH_MIDDLE_THRESHOLD) {
      // No left / right change.
    } else if (horzReading < HIGH_THRESHOLD) {
      // Small left.
      if (rightDutyCycle < 255) {
        rightDutyCycle += SMALL_CHANGE;
      } else {
        leftDutyCycle -= SMALL_CHANGE;
      }
    } else  {
      // Hard left!
      if (rightDutyCycle < 255) {
        rightDutyCycle += LARGE_CHANGE;
      } else {
        leftDutyCycle -= LARGE_CHANGE;
      }
    }
    if (vertReading < LOW_THRESHOLD) {
      // Slow way down.
      leftDutyCycle -= LARGE_CHANGE;
      rightDutyCycle -= LARGE_CHANGE;
    } else if (vertReading < LOW_MIDDLE_THRESHOLD) {
      // Slow some.
      leftDutyCycle -= SMALL_CHANGE;
      rightDutyCycle -= SMALL_CHANGE;
    } else if (vertReading < HIGH_MIDDLE_THRESHOLD) {
      // No vertical change.
    } else if (vertReading < HIGH_THRESHOLD) {
      // Speed up some.
      leftDutyCycle += SMALL_CHANGE;
      rightDutyCycle += SMALL_CHANGE;
    } else  {
      // Speed up a lot.
      leftDutyCycle += LARGE_CHANGE;
      rightDutyCycle += LARGE_CHANGE;
    }
    leftDutyCycle = constrain(leftDutyCycle, 0, 255);
    rightDutyCycle = constrain(rightDutyCycle, 0, 255);
  }
  updateTimerValue();
  updateWheelSpeedsOnLcd();
  delay(20); // Small delay
}

void setDriveState(int newDriveState) {
  lcd.setCursor(5, LINE_1);
  switch (newDriveState) {
    case DRIVE_STATE_STOPPED:
      wtc.sendWheelSpeed(WHEEL_MODE_BRAKE, WHEEL_MODE_BRAKE, 0, 0);
      setTimerState(TIMER_STATE_PAUSED);
      lcd.print("Stopped ");
      break;
    case DRIVE_STATE_DRIVING:
      wtc.sendWheelSpeed(WHEEL_MODE_FORWARD, WHEEL_MODE_FORWARD, (byte)leftDutyCycle, (byte)rightDutyCycle);
      restartTimer();
      lcd.print("Driving");
      break;
  }
  driveState = newDriveState;
}

void updateTimerValue() {
  switch (timerState) {
    case TIMER_STATE_OFF:
      timerValue = 0;
      break;
    case TIMER_STATE_RUNNING:
      timerValue = millis() - timerStartValue;
      break;
    case TIMER_STATE_PAUSED:
      // No change to timer value.
      break;
  }
  updateTimerOnLcd();
}

void restartTimer() {
  timerValue = 0;
  timerState = TIMER_STATE_OFF;  // Note I did NOT use setState.
  setTimerState(TIMER_STATE_RUNNING);
}

void setTimerState(int newTimerState) {
  switch (newTimerState) {
    case TIMER_STATE_OFF:
      timerValue = 0;
      break;
    case TIMER_STATE_RUNNING:
      if (timerState == TIMER_STATE_OFF) {
        timerStartValue = millis();
      } else {
        timerStartValue = millis() - timerValue;
      }
      break;
    case TIMER_STATE_PAUSED:
      // No change.
      break;
  }
  timerState = newTimerState;
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

void updateTimerOnLcd() {
  lcd.setCursor(4, LINE_2);
  int seconds = timerValue / 1000;
  int milliseconds = timerValue % 1000;
  if (seconds < 100) {
    lcd.print(" ");
  }
  if (seconds < 10) {
    lcd.print(" ");
  }
  lcd.print(seconds);
  lcd.print(".");
  if (milliseconds < 100) {
    lcd.print("0");
  }
  if (milliseconds < 10) {
    lcd.print("0");
  }
  lcd.print(milliseconds);
}

void updateWheelSpeedsOnLcd() {
  lcd.setCursor(0, LINE_2);
  if ((int)leftDutyCycle < 100) {
    lcd.print(" ");
  }
  if ((int)leftDutyCycle < 10) {
    lcd.print(" ");
  }
  lcd.print((int)leftDutyCycle);
  lcd.setCursor(13, LINE_2);
  if ((int)rightDutyCycle < 100) {
    lcd.print(" ");
  }
  if ((int)rightDutyCycle < 10) {
    lcd.print(" ");
  }
  lcd.print((int)rightDutyCycle);
}


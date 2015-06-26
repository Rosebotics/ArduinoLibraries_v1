/**
 * Displays the Battery Voltage and Wheel Current on the ADK board LCD.
 * Use this program to calibrate your wheel current readings as
 * discussed in the lab text.
 * 
 * Right button turns the H-Bridge on.
 * Left button turns the H-Bridge off.
 */
 
#include <LiquidCrystal.h>
#include <WildThumperCom.h>
#include <TimerEvent.h>
#include <TimerEventScheduler.h>

#define TEAM_NUMBER 4  // Replace this with your team number.

/***  Pin I/O   ***/ 
#define PIN_RIGHT_BUTTON 2
#define PIN_LEFT_BUTTON 3

/*** Interrupt flags ***/
volatile int mainEventFlags = 0;
#define FLAG_RIGHT_BUTTON                  0x0001
#define FLAG_LEFT_BUTTON                   0x0002

TimerEventScheduler tes;
// Create timer events that are initially disabled (call in 0 ms = never)
TimerEvent leftButtonDebounceTimerEvent(leftButtonDebounceCallback, 0);
TimerEvent rightButtonDebounceTimerEvent(rightButtonDebounceCallback, 0);

LiquidCrystal lcd(14, 15, 16, 17, 18, 19, 20);
#define LINE_1 0
#define LINE_2 1

WildThumperCom wildThumperCom(TEAM_NUMBER);

void setup() {
  Serial.begin(9600);  // Change if you are using a different baudrate.
  pinMode(PIN_RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LEFT_BUTTON, INPUT_PULLUP);
  attachInterrupt(0, int0_isr, FALLING);
  attachInterrupt(1, int1_isr, FALLING);
  tes.addTimerEvent(&leftButtonDebounceTimerEvent);
  tes.addTimerEvent(&rightButtonDebounceTimerEvent);
  lcd.begin(16, 2);
    
  // Register callbacks for commands you might receive from the Wild Thumper.
  wildThumperCom.registerBatteryVoltageReplyCallback(batteryVoltageReplyFromThumper);
  wildThumperCom.registerWheelCurrentReplyCallback(wheelCurrentReplyFromThumper);
  
  // Start both sides at a 100% duty cycle by default
  wildThumperCom.sendWheelSpeed(WHEEL_SPEED_MODE_FORWARD, WHEEL_SPEED_MODE_FORWARD, 255, 255);

  lcd.clear();
  lcd.home();
  lcd.print("WheelCurrentCal");
}


void batteryVoltageReplyFromThumper(int batteryMillivolts) {
  // Display battery voltage on LCD.
  lcd.home();
  lcd.print("Battery   ");
  lcd.setCursor(10, LINE_1);
  lcd.print(batteryMillivolts / 1000);
  lcd.print(".");
  if (batteryMillivolts % 1000  < 100) {
    lcd.print("0");
  }
  if (batteryMillivolts % 1000 < 10) {
    lcd.print("0");
  }
  lcd.print(batteryMillivolts % 1000);
  lcd.print("V");
}

void wheelCurrentReplyFromThumper(int leftWheelMotorsMilliamps, int rightWheelMotorsMilliamps) {
  // Display wheel currents on LCD.
  lcd.setCursor(0, LINE_2);
  lcd.print(leftWheelMotorsMilliamps / 1000);
  lcd.print(".");
  if (leftWheelMotorsMilliamps % 1000  < 100) {
    lcd.print("0");
  }
  if (leftWheelMotorsMilliamps % 1000 < 10) {
    lcd.print("0");
  }
  lcd.print(leftWheelMotorsMilliamps % 1000);
  lcd.print("A    ");
  lcd.print(rightWheelMotorsMilliamps / 1000);
  lcd.print(".");
  if (rightWheelMotorsMilliamps % 1000  < 100) {
    lcd.print("0");
  }
  if (rightWheelMotorsMilliamps % 1000 < 10) {
    lcd.print("0");
  }
  lcd.print(rightWheelMotorsMilliamps % 1000);
  lcd.print("A");
}

void loop() {
  if (mainEventFlags & FLAG_RIGHT_BUTTON) {
    mainEventFlags &= ~FLAG_RIGHT_BUTTON;
    wildThumperCom.sendWheelSpeed(WHEEL_SPEED_MODE_FORWARD, WHEEL_SPEED_MODE_FORWARD, 255, 255);  
  }
  if (mainEventFlags & FLAG_LEFT_BUTTON) {
    mainEventFlags &= ~FLAG_LEFT_BUTTON;
    wildThumperCom.sendWheelSpeed(WHEEL_SPEED_MODE_BRAKE, WHEEL_SPEED_MODE_BRAKE, 0, 0);
  }
  delay(200);
  wildThumperCom.sendWheelCurrentRequest();
  delay(200);
  wildThumperCom.sendBatteryVoltageRequest();
  delay(200);
}

/** Send all bytes received to the Wild Thumper Com object. */
void serialEvent() {
  while (Serial.available()) {
    wildThumperCom.handleRxByte(Serial.read());
  }
}

// Fancier debounce version using TES
void int0_isr() {
  // In 20 ms see if the button is still pressed (software debounce).
  rightButtonDebounceTimerEvent.setTimerTicksRemaining(20);
}

int rightButtonDebounceCallback() {
  if (!digitalRead(PIN_RIGHT_BUTTON)) {
    mainEventFlags |= FLAG_RIGHT_BUTTON;
  }
  return 0; // Don't call this Timer Event again (0 is off).
}

void int1_isr() {
  // In 20 ms see if the button is still pressed (software debounce).
  leftButtonDebounceTimerEvent.setTimerTicksRemaining(20);
}

int leftButtonDebounceCallback() {
  if (!digitalRead(PIN_LEFT_BUTTON)) {
    mainEventFlags |= FLAG_LEFT_BUTTON;
  }
  return 0;  // Don't call this Timer Event again (0 is off).
}


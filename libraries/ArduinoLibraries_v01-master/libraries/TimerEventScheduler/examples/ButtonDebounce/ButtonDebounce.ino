/**
 * This implements a pushbutton debounce delay with interrupts.
 *
 * It changes the age displayed on the LCD screen (simple demo thing).
 * It pretends like there is a VERY long task in the loop function but
 * the number of presses to the buttons will be debounced and accurate.
 * However the LCD will not update until after the long task is complete.
 */
#include <TimerEvent.h>
#include <TimerEventScheduler.h>
#include <LiquidCrystal.h>
 
#define PIN_RIGHT_BUTTON 2
#define PIN_LEFT_BUTTON 3
#define LINE_1 0
#define LINE_2 1

TimerEventScheduler tes;
// Create timer events that are initially disabled (call in 0 ms = never)
TimerEvent leftButtonDebounceTimerEvent(leftButtonDebounceCallback, 0);
TimerEvent rightButtonDebounceTimerEvent(rightButtonDebounceCallback, 0);

int age = 35;
int ageLastWrite = 0;
LiquidCrystal lcd(14, 15, 16, 17, 18, 19, 20);

void setup() {
  pinMode(PIN_LEFT_BUTTON, INPUT_PULLUP);
  pinMode(PIN_RIGHT_BUTTON, INPUT_PULLUP);
  attachInterrupt(0, int0_isr, FALLING);
  attachInterrupt(1, int1_isr, FALLING);
  tes.addTimerEvent(&leftButtonDebounceTimerEvent);
  tes.addTimerEvent(&rightButtonDebounceTimerEvent);
  lcd.begin(16, 2);
  lcd.setCursor(0, LINE_1);
  lcd.print("Dave Fisher is");
}

void int0_isr() {
  // In 20 ms see if the button is still pressed (software debounce).
  rightButtonDebounceTimerEvent.setTimerTicksRemaining(20);
}

void int1_isr() {
  // In 20 ms see if the button is still pressed (software debounce).
  leftButtonDebounceTimerEvent.setTimerTicksRemaining(20);
}

int leftButtonDebounceCallback() {
  if (!digitalRead(PIN_LEFT_BUTTON)) {
    age--;
  }
  return 0;  // Don't call this Timer Event again (0 is off).
}

int rightButtonDebounceCallback() {
  if (!digitalRead(PIN_RIGHT_BUTTON)) {
    age++;
  }
  return 0; // Don't call this Timer Event again (0 is off).
}

// The loop routine runs over and over again forever:
void loop() {
  if (age != ageLastWrite) {
    lcd.setCursor(1, LINE_2);
    lcd.print(age);
    lcd.print(" years old. ");
    ageLastWrite = age;
  }
  
  // Pretend like there is some REALLY long task needed in the loop.
  delay(5000);
}

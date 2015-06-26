#include <TimerEvent.h>
#include <TimerEventScheduler.h>

/*** Interrupt flags ***/
volatile int mainEventFlags = 0;
#define FLAG_TIMER_EVENT_1 0x01
#define FLAG_TIMER_EVENT_2 0x02

TimerEventScheduler tes;
TimerEvent t1(timerEventCallback1, 1000);
TimerEvent t2(timerEventCallback2, 3000);

int timerEventCallback1() {
  mainEventFlags |= FLAG_TIMER_EVENT_1;
  return 1000;
}

int timerEventCallback2() {
  mainEventFlags |= FLAG_TIMER_EVENT_2;
  return 3000;
}

void setup() {
  Serial.begin(57600);
  delay(100);
  tes.addTimerEvent(&t1);
  tes.addTimerEvent(&t2);
}

void loop() {
  if (mainEventFlags & FLAG_TIMER_EVENT_1) {
    mainEventFlags &= ~FLAG_TIMER_EVENT_1;
    Serial.print("Timer event 1 callback occurred at ");
    Serial.print(millis() / 1000);
    Serial.println(" seconds.");
  }
  if (mainEventFlags & FLAG_TIMER_EVENT_2) {
    mainEventFlags &= ~FLAG_TIMER_EVENT_2;
    Serial.print("Timer event 2 callback occurred at ");
    Serial.print(millis() / 1000);
    Serial.println(" seconds.");
    Serial.println();
  }
}

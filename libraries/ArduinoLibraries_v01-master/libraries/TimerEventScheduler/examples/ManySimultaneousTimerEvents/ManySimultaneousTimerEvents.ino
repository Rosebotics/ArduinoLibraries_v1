/**
 * Silly example to stress test having MANY simultaneious timer events.
 * This example has
 */

#include <TimerEvent.h>
#include <TimerEventScheduler.h>
#define NUM_TIMER_FLAGS 25

/*** Interrupt flags ***/
volatile long mainEventFlags = 0;
long currentFlag;
#define FLAG_TIMER_EVENT_01 0x00000001
#define FLAG_TIMER_EVENT_02 0x00000002
#define FLAG_TIMER_EVENT_03 0x00000004
#define FLAG_TIMER_EVENT_04 0x00000008
#define FLAG_TIMER_EVENT_05 0x00000010
#define FLAG_TIMER_EVENT_06 0x00000020
#define FLAG_TIMER_EVENT_07 0x00000040
#define FLAG_TIMER_EVENT_08 0x00000080
#define FLAG_TIMER_EVENT_09 0x00000100
#define FLAG_TIMER_EVENT_10 0x00000200
#define FLAG_TIMER_EVENT_11 0x00000400
#define FLAG_TIMER_EVENT_12 0x00000800
#define FLAG_TIMER_EVENT_13 0x00001000
#define FLAG_TIMER_EVENT_14 0x00002000
#define FLAG_TIMER_EVENT_15 0x00004000
#define FLAG_TIMER_EVENT_16 0x00008000
#define FLAG_TIMER_EVENT_17 0x00010000
#define FLAG_TIMER_EVENT_18 0x00020000
#define FLAG_TIMER_EVENT_19 0x00040000
#define FLAG_TIMER_EVENT_20 0x00080000
#define FLAG_TIMER_EVENT_21 0x00100000
#define FLAG_TIMER_EVENT_22 0x00200000
#define FLAG_TIMER_EVENT_23 0x00400000
#define FLAG_TIMER_EVENT_24 0x00800000
#define FLAG_TIMER_EVENT_25 0x01000000

TimerEventScheduler tes;
// One new timer event begins firing each second up to 25.
TimerEvent te01(timerEventCallback01,  1000);
TimerEvent te02(timerEventCallback02,  2000);
TimerEvent te03(timerEventCallback03,  3000);
TimerEvent te04(timerEventCallback04,  4000);
TimerEvent te05(timerEventCallback05,  5000);
TimerEvent te06(timerEventCallback06,  6000);
TimerEvent te07(timerEventCallback07,  7000);
TimerEvent te08(timerEventCallback08,  8000);
TimerEvent te09(timerEventCallback09,  9000);
TimerEvent te10(timerEventCallback10, 10000);
TimerEvent te11(timerEventCallback11, 11000);
TimerEvent te12(timerEventCallback12, 12000);
TimerEvent te13(timerEventCallback13, 13000);
TimerEvent te14(timerEventCallback14, 14000);
TimerEvent te15(timerEventCallback15, 15000);
TimerEvent te16(timerEventCallback16, 16000);
TimerEvent te17(timerEventCallback17, 17000);
TimerEvent te18(timerEventCallback18, 18000);
TimerEvent te19(timerEventCallback19, 19000);
TimerEvent te20(timerEventCallback20, 20000);
TimerEvent te21(timerEventCallback21, 21000);
TimerEvent te22(timerEventCallback22, 22000);
TimerEvent te23(timerEventCallback23, 23000);
TimerEvent te24(timerEventCallback24, 24000);
TimerEvent te25(timerEventCallback25, 25000);

int timerEventCallback01() {
  mainEventFlags |= FLAG_TIMER_EVENT_01;
  return 1000;
}

int timerEventCallback02() {
  mainEventFlags |= FLAG_TIMER_EVENT_02;
  return 1000;
}

int timerEventCallback03() {
  mainEventFlags |= FLAG_TIMER_EVENT_03;
  return 1000;
}

int timerEventCallback04() {
  mainEventFlags |= FLAG_TIMER_EVENT_04;
  return 1000;
}

int timerEventCallback05() {
  mainEventFlags |= FLAG_TIMER_EVENT_05;
  return 1000;
}

int timerEventCallback06() {
  mainEventFlags |= FLAG_TIMER_EVENT_06;
  return 1000;
}

int timerEventCallback07() {
  mainEventFlags |= FLAG_TIMER_EVENT_07;
  return 1000;
}

int timerEventCallback08() {
  mainEventFlags |= FLAG_TIMER_EVENT_08;
  return 1000;
}

int timerEventCallback09() {
  mainEventFlags |= FLAG_TIMER_EVENT_09;
  return 1000;
}

int timerEventCallback10() {
  mainEventFlags |= FLAG_TIMER_EVENT_10;
  return 1000;
}
int timerEventCallback11() {
  mainEventFlags |= FLAG_TIMER_EVENT_11;
  return 1000;
}

int timerEventCallback12() {
  mainEventFlags |= FLAG_TIMER_EVENT_12;
  return 1000;
}

int timerEventCallback13() {
  mainEventFlags |= FLAG_TIMER_EVENT_13;
  return 1000;
}

int timerEventCallback14() {
  mainEventFlags |= FLAG_TIMER_EVENT_14;
  return 1000;
}

int timerEventCallback15() {
  mainEventFlags |= FLAG_TIMER_EVENT_15;
  return 1000;
}

int timerEventCallback16() {
  mainEventFlags |= FLAG_TIMER_EVENT_16;
  return 1000;
}

int timerEventCallback17() {
  mainEventFlags |= FLAG_TIMER_EVENT_17;
  return 1000;
}

int timerEventCallback18() {
  mainEventFlags |= FLAG_TIMER_EVENT_18;
  return 1000;
}

int timerEventCallback19() {
  mainEventFlags |= FLAG_TIMER_EVENT_19;
  return 1000;
}

int timerEventCallback20() {
  mainEventFlags |= FLAG_TIMER_EVENT_20;
  return 1000;
}

int timerEventCallback21() {
  mainEventFlags |= FLAG_TIMER_EVENT_21;
  return 1000;
}

int timerEventCallback22() {
  mainEventFlags |= FLAG_TIMER_EVENT_22;
  return 1000;
}

int timerEventCallback23() {
  mainEventFlags |= FLAG_TIMER_EVENT_23;
  return 1000;
}

int timerEventCallback24() {
  mainEventFlags |= FLAG_TIMER_EVENT_24;
  return 1000;
}

int timerEventCallback25() {
  mainEventFlags |= FLAG_TIMER_EVENT_25;
  // Just to be silly disable all timer events that are multiples of 5 (including this one).
  te05.setTimerTicksRemaining(0);
  te10.setTimerTicksRemaining(0);
  te15.setTimerTicksRemaining(0);
  te20.setTimerTicksRemaining(0);
  return 0;
}

void setup() {
  Serial.begin(57600);
  delay(100);
  tes.addTimerEvent(&te01);
  tes.addTimerEvent(&te02);
  tes.addTimerEvent(&te03);
  tes.addTimerEvent(&te04);
  tes.addTimerEvent(&te05);
  tes.addTimerEvent(&te06);
  tes.addTimerEvent(&te07);
  tes.addTimerEvent(&te08);
  tes.addTimerEvent(&te09);
  tes.addTimerEvent(&te10);
  tes.addTimerEvent(&te11);
  tes.addTimerEvent(&te12);
  tes.addTimerEvent(&te13);
  tes.addTimerEvent(&te14);
  tes.addTimerEvent(&te15);
  tes.addTimerEvent(&te16);
  tes.addTimerEvent(&te17);
  tes.addTimerEvent(&te18);
  tes.addTimerEvent(&te19);
  tes.addTimerEvent(&te20);
  tes.addTimerEvent(&te21);
  tes.addTimerEvent(&te22);
  tes.addTimerEvent(&te23);
  tes.addTimerEvent(&te24);
  tes.addTimerEvent(&te25);
}
  
void loop() {
  // If a timer event has fired print the number.
  for (int i = 0; i < NUM_TIMER_FLAGS; i++) {
    currentFlag = 0x00000001L << i;
    if (mainEventFlags & currentFlag) {
      mainEventFlags &= ~currentFlag;
      if (i == 0) {
        Serial.println();
      }
      Serial.print(i+1);
      Serial.print(" ");
    }
  }
  delay(100);
}

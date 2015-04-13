#include <MiniQ.h>

MiniQ miniQ;

void setup() {
  miniQ.begin();
}

void loop() {
  miniQ.setDrivePwm(1,1,200,200);
  delay(2000);
  miniQ.setDrivePwm(1,1,0,0);
  delay(2000);
  miniQ.setDrivePwm(0,0,200,200);
  delay(2000);
  miniQ.setDrivePwm(0,0,0,0);
  delay(5000);
}

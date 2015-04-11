#include <MiniQ.h>

MiniQ miniQ;

void setup() {
  // put your setup code here, to run once:

}

void loop() {

  miniQ.setWheelPwm(1,1,200,200);
  delay(2000);
  miniQ.setWheelPwm(1,1,0,0);
  delay(2000);
  miniQ.setWheelPwm(0,0,200,200);
  delay(2000);
  miniQ.setWheelPwm(0,0,0,0);
  delay(2000);
  

}

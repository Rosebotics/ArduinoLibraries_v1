
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <MiniQCom.h>
#include <MiniQ.h>
#include <IRremote.h>

MiniQ miniQ;
IRrecv irrecv(PIN_CENTER_IR_MODULE);
decode_results results;

int count;//count the motor speed pulse

void setup() {
  miniQ.begin();
  irrecv.enableIRIn();
  miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, 180, 200); // Forward    
}

void loop() {
  if (irrecv.decode(&results)) {
    dump(&results);
    irrecv.resume();
  }
}

void dump(decode_results* results) {
  if(results->value==0x00fd00ff) {
    miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, 0, 0);     // Stop
  }
  if(results->value==0x00fd807f) {
    miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, 180, 200); // Forward    
  }
  if(results->value==0x00fd906f) {
    miniQ.setDrivePwm(WHEEL_DIRECTION_REVERSE, WHEEL_DIRECTION_REVERSE, 180, 200); // Back
  }
  if(results->value==0x00fd20df) {
    miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_REVERSE, 180, 200); // Turn right
    delay(20);
    miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, 0, 0);     // Stop
  }
  if(results->value==0x00fd609f) {
    miniQ.setDrivePwm(WHEEL_DIRECTION_REVERSE, WHEEL_DIRECTION_FORWARD, 180, 200); // Turn right
    delay(20);
    miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, 0, 0);     // Stop
  }
}

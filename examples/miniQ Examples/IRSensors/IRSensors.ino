#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <MiniQCom.h>
#include <MiniQ.h>

int fl, ml, c, mr, fr;
#define THRESHOLD 600

MiniQ miniQ;

void setup() {
  Serial.begin(9600);
  miniQ.begin();
}

void loop() {
  byte leftspeed, rightspeed;
  miniQ.getIrLineSensors(&fl, &ml, &c, &mr, &fr); 
  Serial.println(fl);
  Serial.println(ml);
  Serial.println(c);
  Serial.println(mr);
  Serial.println(fr);
  Serial.println("--");
  //delay(1000);
  
  if (fr > THRESHOLD && fl > THRESHOLD) {
    rightspeed = 255;
    leftspeed = 0;
    miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, leftspeed, rightspeed);
    delay(480);  // UTurn
  } else if (fr > THRESHOLD) {
    rightspeed = 180;
    leftspeed = 0;
  } else if (fl > THRESHOLD) {
    leftspeed = 180;
    rightspeed = 0;
  } else{
    rightspeed = 70;  
    leftspeed = 63;
  }
    
  miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, leftspeed, rightspeed);
}

#include <RobotAsciiCom.h>

#define PIN_LEFT_PWM        5
#define PIN_RIGHT_PWM       6
#define PIN_LEFT_DIRECTION  7
#define PIN_RIGHT_DIRECTION 8


RobotAsciiCom robotCom;

void setup() {
  Serial.begin(115200);               //initial the Serial
  delay(100);  
  pinMode(PIN_LEFT_PWM, OUTPUT);      
  pinMode(PIN_RIGHT_PWM, OUTPUT);      
  pinMode(PIN_LEFT_DIRECTION, OUTPUT);      
  pinMode(PIN_RIGHT_DIRECTION, OUTPUT);      
  robotCom.registerPwmCallback(pwmCallback);
}

void pwmCallback(int leftDutyCycle, int rightDutyCycle) {
  if (leftDutyCycle > 0) {
    digitalWrite(PIN_LEFT_DIRECTION, HIGH);
  } else {
    digitalWrite(PIN_LEFT_DIRECTION, LOW);
    leftDutyCycle = -leftDutyCycle;
  }
  if (rightDutyCycle > 0) {
    digitalWrite(PIN_RIGHT_DIRECTION, HIGH);
  } else {
    digitalWrite(PIN_RIGHT_DIRECTION, LOW);
    rightDutyCycle = -rightDutyCycle;
  }  
  analogWrite(PIN_LEFT_PWM, leftDutyCycle);
  analogWrite(PIN_RIGHT_PWM, rightDutyCycle);
}

void loop() {
}

/** Send all bytes received to the RobotAsciiCom object. */
void serialEvent() {
  while (Serial.available()) {
    robotCom.handleRxByte(Serial.read());
  }
}


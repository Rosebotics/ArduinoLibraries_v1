#include "Arduino.h"
#include "MiniQCom.h"
#include "Adafruit_NeoPixel.h"
#include "MiniQ.h"

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel rgb_light = Adafruit_NeoPixel(1, PIN_RGB_LED,
    NEO_GRB + NEO_KHZ800);

MiniQ::MiniQ() {
  // Everything happens within the begin area.
}

void MiniQ::begin() {
  pinMode(PIN_LEFT_MOTOR_ENABLE_PWM, OUTPUT);
  pinMode(PIN_RIGHT_MOTOR_ENABLE_PWM, OUTPUT);
  pinMode(PIN_LEFT_MOTOR_DIRECTION, OUTPUT);
  pinMode(PIN_RIGHT_MOTOR_DIRECTION, OUTPUT);
  pinMode(PIN_RIGHT_MOTOR_ENCODER, INPUT_PULLUP);
  pinMode(PIN_LEFT_MOTOR_ENCODER, INPUT_PULLUP);
  pinMode(PIN_IR_LED_LEFT, OUTPUT);
  pinMode(PIN_IR_LED_RIGHT, OUTPUT);
  pinMode(PIN_CENTER_IR_MODULE, INPUT_PULLUP);
  //pinMode(PIN_RGB_LED, OUTPUT);  // TODO: Learn more about the RGB LED.
  pinMode(PIN_BUZZER, OUTPUT);

  rgb_light.begin(); // This initializes the NeoPixel library.
  rgb_light.show(); // Initialize all the pixels to 'off'.
}

void MiniQ::setDrivePwm(byte leftWheelDirection, byte rightWheelDirection,
    byte leftDutyCycle, byte rightDutyCycle) {
  digitalWrite(PIN_LEFT_MOTOR_DIRECTION, leftWheelDirection);
  digitalWrite(PIN_RIGHT_MOTOR_DIRECTION, rightWheelDirection);
  analogWrite(PIN_LEFT_MOTOR_ENABLE_PWM, leftDutyCycle);
  analogWrite(PIN_RIGHT_MOTOR_ENABLE_PWM, rightDutyCycle);
}

void MiniQ::setDriveSpeedArc(int speedMmPerS, int arcMm) {
  // TODO: Implement.
}

void MiniQ::setSensorMask(sensorMaskSetAddOrRemove_t setAddOrRemove,
    unsigned long newSensorMask) {
  // Probably won't need this method, but a good placeholder.
}

void MiniQ::setBuzzerTone(unsigned int frequency, unsigned long durationMs) {
  if (frequency == 0) {
    noTone (PIN_BUZZER);
  } else {
    tone(PIN_BUZZER, frequency, durationMs);
  }
  delay(durationMs);  // TODO: Reimplement this with the TimerEventScheduler.
}

void MiniQ::setLed(byte red, byte green, byte blue) {
  rgb_light.setPixelColor(0, rgb_light.Color(red, green, blue));
  rgb_light.show();
}

void MiniQ::setSendIr(byte byteToSend, unsigned int durationMs) {

}

void MiniQ::setIrModuleMode(irModuleMode_t irModuleMode) {

}

void MiniQ::getIrLineSensors(int* farLeftIrLineSensorPtr,
    int* midLeftIrLineSensorPtr, int* centerIrLineSensorPtr,
    int* midRightIrLineSensorPtr, int* farRightIrLineSensorPtr) {
  *farLeftIrLineSensorPtr = (int) analogRead(PIN_FAR_LEFT_IR_LINE_SENSOR);
  *midLeftIrLineSensorPtr = (int) analogRead(PIN_MID_LEFT_IR_LINE_SENSOR);
  *centerIrLineSensorPtr = (int) analogRead(PIN_CENTER_IR_LINE_SENSOR);
  *midRightIrLineSensorPtr = (int) analogRead(PIN_MID_RIGHT_IR_LINE_SENSOR);
  *farRightIrLineSensorPtr = (int) analogRead(PIN_FAR_RIGHT_IR_LINE_SENSOR);
}


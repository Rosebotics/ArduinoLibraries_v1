#ifndef MiniQ_h
#define MiniQ_h

#include "Arduino.h"

// Pins
#define PIN_LEFT_MOTOR_ENABLE_PWM 5
#define PIN_RIGHT_MOTOR_ENABLE_PWM 6
#define PIN_LEFT_MOTOR_DIRECTION 12  // 0 is forward, 1 is reverse
#define PIN_RIGHT_MOTOR_DIRECTION 7  // 0 is forward, 1 is reverse
#define PIN_RIGHT_MOTOR_ENCODER 0
#define PIN_LEFT_MOTOR_ENCODER 1

#define PIN_FAR_LEFT_IR_LINE_SENSOR A0
#define PIN_MID_LEFT_IR_LINE_SENSOR A1
#define PIN_CENTER_IR_LINE_SENSOR A2
#define PIN_MID_RIGHT_IR_LINE_SENSOR A3
#define PIN_FAR_RIGHT_IR_LINE_SENSOR A4

#define PIN_IR_LED_LEFT  13
#define PIN_IR_LED_RIGHT 8
#define PIN_PHOTO_DETECTOR_BJTS A5

#define PIN_CENTER_IR_MODULE 17
#define PIN_BATTERY_VOLTAGE_READING A5
#define PIN_ANALOG_READING_FOR_BUTTONS A6
#define PIN_RGB_LED 10
#define PIN_BUZZER 16

// Constants used in the wheel PWM commands.
#define WHEEL_DIRECTION_FORWARD		1
#define WHEEL_DIRECTION_REVERSE     0


class MiniQ {
  public:
	MiniQ();
	void setWheelPwm(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle);
  private:
	byte _placeholder;
};

#endif
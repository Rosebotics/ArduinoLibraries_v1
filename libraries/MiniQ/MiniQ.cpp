#include "Arduino.h"
#include "MiniQ.h"

MiniQ::MiniQ() {
	_placeholder = 0; // TODO: Delete.  Just showing the pattern for private variables.

}

void MiniQ::begin() {
	pinMode(PIN_LEFT_MOTOR_ENABLE_PWM, OUTPUT);
	pinMode(PIN_RIGHT_MOTOR_ENABLE_PWM, OUTPUT);
	pinMode(PIN_RIGHT_MOTOR_DIRECTION, OUTPUT);
	pinMode(PIN_LEFT_MOTOR_DIRECTION, OUTPUT);
	pinMode(PIN_RIGHT_MOTOR_ENCODER, INPUT_PULLUP);
	pinMode(PIN_LEFT_MOTOR_ENCODER, INPUT_PULLUP);
	pinMode(PIN_IR_LED_LEFT, OUTPUT);
	pinMode(PIN_IR_LED_RIGHT, OUTPUT);
	pinMode(PIN_CENTER_IR_MODULE, INPUT_PULLUP);
	//pinMode(PIN_RGB_LED, OUTPUT);  // TODO: Learn more about the RGB LED.
	pinMode(PIN_BUZZER, OUTPUT);
}

void MiniQ::setWheelPwm(byte leftMode, byte rightMode, byte leftDutyCycle,
		byte rightDutyCycle) {

}

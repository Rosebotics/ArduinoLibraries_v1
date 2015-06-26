#ifndef MiniQ_h
#define MiniQ_h

#include "Arduino.h"

// Pins
#define PIN_LEFT_MOTOR_ENABLE_PWM 5
#define PIN_RIGHT_MOTOR_ENABLE_PWM 6
#define PIN_LEFT_MOTOR_DIRECTION 12
#define PIN_RIGHT_MOTOR_DIRECTION 7
#define PIN_LEFT_MOTOR_ENCODER 1
#define PIN_RIGHT_MOTOR_ENCODER 0

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
#define WHEEL_DIRECTION_FORWARD	0
#define WHEEL_DIRECTION_REVERSE 1


// Remote Button IR Signals
#define BUTTON_PRESS_POWER 0x00FD00FF
#define BUTTON_PRESS_VOL_UP 0x00FD807F
#define BUTTON_PRESS_STOP 0x00FD40BF
#define BUTTON_PRESS_PREVIOUS 0x00FD20DF
#define BUTTON_PRESS_PLAY 0x00FDA05F
#define BUTTON_PRESS_NEXT 0x00FD609F
#define BUTTON_PRESS_DOWN 0x00FD10EF
#define BUTTON_PRESS_VOL_DOWN 0x00FD906F
#define BUTTON_PRESS_UP 0x00FD50AF
#define BUTTON_PRESS_0 0x00FD30CF
#define BUTTON_PRESS_EQ 0x00FDB04F
#define BUTTON_PRESS_REPT 0x00FD708F
#define BUTTON_PRESS_1 0x00FD08F7
#define BUTTON_PRESS_2 0x00FD8877
#define BUTTON_PRESS_3 0x00FD48B7
#define BUTTON_PRESS_4 0x00FD28D7
#define BUTTON_PRESS_5 0x00FDA857
#define BUTTON_PRESS_6 0x00FD6897
#define BUTTON_PRESS_7 0x00FD18E7
#define BUTTON_PRESS_8 0x00FD9867
#define BUTTON_PRESS_9 0x00FD58A7

//Encoder Interrupts
#define RIGHT_ENCODER_INTERRUPT 2
#define LEFT_ENCODER_INTERRUPT 3

#define PIN_BUTTON_KEY  4
#define BUTTON_1_THRESHOLD 450
#define BUTTON_2_THRESHOLD 700
#define BUTTON_3_THRESHOLD 950




class MiniQ {
  public:
  MiniQ();
  void begin(void);
  void setDrivePwm(byte leftWheelDirection, byte rightWheelDirection, byte leftDutyCycle, byte rightDutyCycle);
  void setDriveSpeedArc(int speedMmPerS, int arcMm);
  void setSensorMask(sensorMaskSetAddOrRemove_t setAddOrRemove, unsigned long newSensorMask);
  void setBuzzerTone(unsigned int frequency, unsigned long durationMs);
  void setLed(byte red, byte green, byte blue);
  void setSendIr(byte byteToSend, unsigned int durationMs);
  void setIrModuleMode(irModuleMode_t irModuleMode);
//	void getEncoderValues(long* leftMotorEncoderPtr, long* rightMotorEncoderPtr);
  void getIrLineSensors(int* farLeftIrLineSensorPtr, int* midLeftIrLineSensorPtr,
      int* centerIrLineSensorPtr, int* midRightIrLineSensorPtr, int* farRightIrLineSensorPtr);
  protected:
};

#endif

#ifndef MiniQCom_h
#define MiniQCom_h

#include "Arduino.h"

typedef enum {
  SENSOR_MASK_SET, SENSOR_MASK_ADD, SENSOR_MASK_REMOVE,
} sensorMaskSetAddOrRemove_t;

typedef enum {
  KEY_BUTTONS_NO_PRESS,
  KEY_BUTTONS_KEY_1,
  KEY_BUTTONS_KEY_2,
  KEY_BUTTONS_KEY_3
} keyButtons_t;

typedef enum {
  IR_MODULE_MODE_OFF, IR_MODULE_MODE_COUNT, IR_MODULE_MODE_BYTE
} irModuleMode_t;

// Readings from the MiniQ first:
// Note there are 32 possible sensor response messages.
// Sensor message format DataBit9 DataBit8 x ID4 ID3 ID2 ID1 ID0
// Sensors identifiers
#define LEFT_MOTOR_ENCODER			0
#define RIGHT_MOTOR_ENCODER			1
#define FAR_LEFT_IR_LINE_SENSOR		2
#define MID_LEFT_IR_LINE_SENSOR		3
#define CENTER_IR_LINE_SENSOR		4
#define MID_RIGHT_IR_LINE_SENSOR	5
#define FAR_RIGHT_IR_LINE_SENSOR	6
#define PHOTO_DETECTORS				7
#define IR_MODULE_COUNT				8
#define IR_MODULE_BYTE				9
#define KEY_BUTTONS				   10
#define BATTERY_VOLTAGE_TENTHS	   11
// Note, Accelerometer uses I2C.

// Sensorbits (which bit in the sensor mask means you are on)
#define BIT_LEFT_MOTOR_ENCODER			0x00000001L
#define BIT_RIGHT_MOTOR_ENCODER			0x00000002L
#define BIT_FAR_LEFT_IR_LINE_SENSOR		0x00000004L
#define BIT_MID_LEFT_IR_LINE_SENSOR		0x00000008L
#define BIT_CENTER_IR_LINE_SENSOR		0x00000010L
#define BIT_MID_RIGHT_IR_LINE_SENSOR	0x00000020L
#define BIT_FAR_RIGHT_IR_LINE_SENSOR	0x00000040L
#define BIT_PHOTO_DETECTORS				0x00000080L
#define BIT_IR_MODULE_COUNT				0x00000100L
#define BIT_IR_MODULE_BYTE				0x00000200L
#define BIT_KEY_BUTTONS					0x00000400L
#define BIT_BATTERY_VOLTAGE_TENTHS		0x00000800L

// Sensor message length (when sent how many additional data bytes are needed)
#define MSG_LENGTH_LEFT_MOTOR_ENCODER			4 // BYTE0 first
#define MSG_LENGTH_RIGHT_MOTOR_ENCODER			4 // BYTE0 first
#define MSG_LENGTH_FAR_LEFT_IR_LINE_SENSOR		1 // Uses DataBit9 and DataBit8
#define MSG_LENGTH_MID_LEFT_IR_LINE_SENSOR		1 // Uses DataBit9 and DataBit8
#define MSG_LENGTH_CENTER_IR_LINE_SENSOR		1 // Uses DataBit9 and DataBit8
#define MSG_LENGTH_MID_RIGHT_IR_LINE_SENSOR		1 // Uses DataBit9 and DataBit8
#define MSG_LENGTH_FAR_RIGHT_IR_LINE_SENSOR		1 // Uses DataBit9 and DataBit8
#define MSG_LENGTH_PHOTO_DETECTORS				1 // Uses DataBit9 and DataBit8
#define MSG_LENGTH_IR_MODULE_COUNT				1 // Count seen during 15 pulse test (bits 7-4 left count, bits 3-0 right count)
#define MSG_LENGTH_IR_MODULE_BYTE				1 // Last byte received from IR control (with 1 second).
#define MSG_LENGTH_KEY_BUTTONS					0 // DataBit9 and DataBit8 only
#define MSG_LENGTH_BATTERY_VOLTAGE_TENTHS		1 // Units are in tenths of a volt

// Special protocol bytes
#define START_BYTE 			0x7E
#define ESCAPE_BYTE 		0x7D
#define ESCAPE_XOR 			0x20

// Message details
#define MAX_MESSAGE_LENGTH 	64
#define COMMAND_BYTE 		0

// Commands
#define COMMAND_DRIVE_PWM              	0
#define COMMAND_DRIVE_SPEED_ARC        	1
#define COMMAND_SET_SENSOR_MASK			2
#define COMMAND_ADD_TO_SENSOR_MASK		3
#define COMMAND_REMOVE_FROM_SENSOR_MASK 4
#define COMMAND_BUZZER_TONE    			5
#define COMMAND_LED            			6
#define COMMAND_SEND_IR        			7
#define COMMAND_IR_MODULE_MODE			8

// Bytes within commands
// COMMAND_BYTE is byte 0 for all messages.
#define DRIVE_PWM_DIRECTIONS			1
#define DRIVE_PWM_LEFT_DUTY_CYCLE		2
#define DRIVE_PWM_RIGHT_DUTY_CYCLE		3
#define DRIVE_PWM_COMMAND_LENGTH		4

#define DRIVE_SPEED_ARC_SPEED_LSB		1 // Units of mm/s
#define DRIVE_SPEED_ARC_SPEED_MSB		2 // Positive is forwards. Negative is reverse.
#define DRIVE_SPEED_ARC_ARC_LSB			3 // Units of mm
#define DRIVE_SPEED_ARC_ARC_MSB			4 // Positive arcs left. Negative arcs right.
#define DRIVE_SPEED_ARC_COMMAND_LENGTH 	5

// Note, these byte definitions are shared by all 3 sensor mask commands.
#define SENSOR_MASK_BYTE0				1
#define SENSOR_MASK_BYTE1				2
#define SENSOR_MASK_BYTE2				3
#define SENSOR_MASK_BYTE3				4
#define SENSOR_MASK_COMMAND_LENGTH		5

#define BUZZER_TONE_FREQUENCY_LSB		1
#define BUZZER_TONE_FREQUENCY_MSB		2
#define BUZZER_TONE_DURATION_BYTE0		3 // Units in milliseconds
#define BUZZER_TONE_DURATION_BYTE1		4
#define BUZZER_TONE_DURATION_BYTE2		5
#define BUZZER_TONE_DURATION_BYTE3		6
#define BUZZER_TONE_COMMAND_LENGTH		7

#define LED_R							1
#define LED_G							2
#define LED_B							3
#define LED_COMMAND_LENGTH				4

#define SEND_IR_BYTE					1
#define SEND_IR_DURATION_LSB			2
#define SEND_IR_DURATION_MSB			3
#define SEND_IR_COMMAND_LENGTH			4

#define IR_MODULE_MODE					1 // see the irModuleMode_t enum
#define IR_MODULE_MODE_COMMAND_LENGTH	2

class MiniQCom {
public:
  MiniQCom();
  MiniQCom(boolean isMaster, byte miniQAddress);
  void sendDrivePwm(boolean leftIsForward, boolean rightIsForward,
      byte leftDutyCycle, byte rightDutyCycle);
  void sendDriveSpeedArc(int speedMmPerS, int arcMm);
  void sendSetSensorMask(unsigned long newSensorMask);
  void sendAddToSensorMask(unsigned long sensorMaskBitsToAdd);
  void sendRemoveFromSensorMask(unsigned long sensorMaskBitsToRemove);
  void sendBuzzerTone(unsigned int frequency, unsigned long durationMs);
  void sendLed(byte red, byte green, byte blue);
  void sendSendIr(byte byteToSend, unsigned int durationMs);
  void sendIrModuleMode(irModuleMode_t irModuleMode);
  void registerDrivePwmCallback(
      void (*drivePwmCallback)(boolean leftIsForward,
          boolean rightIsForward, byte leftDutyCycle,
          byte rightDutyCycle));
  void registerDriveSpeedArcCallback(
      void (*driveSpeedArcCallback)(int speedMmPerS, int arcMm));
  void registerSensorMaskCallback(
      void (*sensorMaskCallback)(
          sensorMaskSetAddOrRemove_t setAddOrRemove,
          unsigned long sensorMaskParameter));
  void registerBuzzerToneCallback(
      void (*buzzerToneCallback)(unsigned int frequency,
          unsigned long durationMs));
  void registerLedCallback(
      void (*ledCallback)(byte red, byte green, byte blue));
  void registerSendIrCallback(
      void (*sendIrCallback)(byte byteToSend, unsigned int durationMs));
  void registerIrModuleMode(
      void (*irModuleMode)(irModuleMode_t irModuleMode));
  void handleRxByte(byte newRxByte);

  // Sensor variables.
  unsigned long sensorMask;
  long leftMotorEncoder;
  long rightMotorEncoder;
  int farLeftIrLineSensor;
  int midLeftIrLineSensor;
  int centerIrLineSensor;
  int midRightIrLineSensor;
  int farRightIrLineSensor;
  int photoDetectors;
  byte irModuleCountForLeftIr;
  byte irModuleCountForRightIr;
  byte irModuleByte;
  keyButtons_t keyButtons;
  byte batterVoltageTenths;
private:
  void _init(boolean isMaster, byte miniQAddress);
  boolean _isMaster;
  byte _miniQAddress;
  byte _txMessageBuffer[MAX_MESSAGE_LENGTH];
  byte _rxMessageBuffer[MAX_MESSAGE_LENGTH];
  void _sendMessage(byte messageLength);
  void _sendByte(byte unescapedbyte);
  void (*_drivePwmCallback)(boolean leftIsForward, boolean rightIsForward,
      byte leftDutyCycle, byte rightDutyCycle);
  void (*_driveSpeedArcCallback)(int speedMmPerS, int arcMm);
  void (*_sensorMaskCallback)(sensorMaskSetAddOrRemove_t setAddOrRemove,
      unsigned long sensorMaskParameter);
  void (*_buzzerToneCallback)(unsigned int frequency,
      unsigned long durationMs);
  void (*_ledCallback)(byte red, byte green, byte blue);
  void (*_sendIrCallback)(byte byteToSend, unsigned int durationMs);
  void (*_irModuleMode)(irModuleMode_t irModuleMode);
  boolean _lastByteWasStartByte;
  boolean _lastByteWasEscapeByte;
  int _bytesRemainingInMessage;
  int _nextOpenByteInMessageBuffer;
  byte _crc;
  void _parseValidMessage();
};

#endif

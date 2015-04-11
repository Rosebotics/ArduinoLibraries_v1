#ifndef MiniQCom_h
#define MiniQCom_h

#include "Arduino.h"

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
#define PHOTO_DETECTOR_BJTS			7
#define CENTER_IR_MODULE_COUNT		8
#define CENTER_IR_MODULE_BYTE		9
#define KEY_BUTTONS				   10
#define BATTERY_VOLTAGE			   11
// Note, Accelerometer uses I2C.

// Sensorbits (which bit in the sensor mask means you are on)
#define BIT_LEFT_MOTOR_ENCODER			0x00000001
#define BIT_RIGHT_MOTOR_ENCODER			0x00000002
#define BIT_FAR_LEFT_IR_LINE_SENSOR		0x00000004
#define BIT_MID_LEFT_IR_LINE_SENSOR		0x00000008
#define BIT_CENTER_IR_LINE_SENSOR		0x00000010
#define BIT_MID_RIGHT_IR_LINE_SENSOR	0x00000020
#define BIT_FAR_RIGHT_IR_LINE_SENSOR	0x00000040
#define BIT_PHOTO_DETECTOR_BJTS			0x00000080
#define BIT_CENTER_IR_MODULE_COUNT		0x00000100
#define BIT_CENTER_IR_MODULE_BYTE		0x00000200
#define BIT_KEY_BUTTONS					0x00000400
#define BIT_BATTERY_VOLTAGE				0x00000800

// Sensor message length (when sent how many additional data bytes are needed)
#define MSG_LENGTH_LEFT_MOTOR_ENCODER			4 // BYTE0 first
#define MSG_LENGTH_RIGHT_MOTOR_ENCODER			4 // BYTE0 first
#define MSG_LENGTH_FAR_LEFT_IR_LINE_SENSOR		1 // Uses DataBit9 and DataBit8
#define MSG_LENGTH_MID_LEFT_IR_LINE_SENSOR		1 // Uses DataBit9 and DataBit8
#define MSG_LENGTH_CENTER_IR_LINE_SENSOR		1 // Uses DataBit9 and DataBit8
#define MSG_LENGTH_MID_RIGHT_IR_LINE_SENSOR		1 // Uses DataBit9 and DataBit8
#define MSG_LENGTH_FAR_RIGHT_IR_LINE_SENSOR		1 // Uses DataBit9 and DataBit8
#define MSG_LENGTH_PHOTO_DETECTOR_BJTS			1 // Uses DataBit9 and DataBit8
#define MSG_LENGTH_CENTER_IR_MODULE_COUNT		1 // Count seen during 15 pulse test (bits 7-4 left count, bits 3-0 right count)
#define MSG_LENGTH_CENTER_IR_MODULE_BYTE		1 // Last byte received from IR control (with 1 second).
#define MSG_LENGTH_KEY_BUTTONS					0 // DataBit9 and DataBit8 only
#define MSG_LENGTH_BATTERY_VOLTAGE				1 // Units are in tenths of a volt


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
#define DRIVE_PWM_COMMAND_LENGTH		4
// COMMAND_BYTE is 0
#define DRIVE_PWM_DIRECTIONS			1
#define DRIVE_PWM_LEFT_DUTY_CYCLE		2
#define DRIVE_PWM_RIGHT_DUTY_CYCLE		3
// Constants used within the drive pwm command.
#define DRIVE_PWM_DIRECTION_FORWARD			0
#define DRIVE_PWM_DIRECTION_REVERSE      	1

#define DRIVE_SPEED_ARC_COMMAND_LENGTH 	5
// COMMAND_BYTE is 0
#define DRIVE_SPEED_ARC_SPEED_LSB		1 // Units of mm/s
#define DRIVE_SPEED_ARC_SPEED_MSB		2 // Positive is forwards. Negative is reverse.
#define DRIVE_SPEED_ARC_ARC_LSB			3 // Units of mm
#define DRIVE_SPEED_ARC_ARC_MSB			4 // Positive arcs left. Negative arcs right.

// Note, these byte definitions are shared by all 3 sensor mask commands.
#define SENSOR_MASK_COMMAND_LENGTH		5
// COMMAND_BYTE is 0
#define SENSOR_MASK_BYTE0				1
#define SENSOR_MASK_BYTE1				2
#define SENSOR_MASK_BYTE2				3
#define SENSOR_MASK_BYTE3				4

#define BUZZER_TONE_COMMAND_LENGTH		5
// COMMAND_BYTE is 0
#define BUZZER_TONE_FREQUENCY_LSB		1
#define BUZZER_TONE_FREQUENCY_MSB		2
#define BUZZER_TONE_DURATION_BYTE0		3 // Units in milliseconds
#define BUZZER_TONE_DURATION_BYTE1		4
#define BUZZER_TONE_DURATION_BYTE2		5
#define BUZZER_TONE_DURATION_BYTE3		6

#define LED_COMMAND_LENGTH				4
// COMMAND_BYTE is 0
#define LED_R							1
#define LED_G							2
#define LED_B							3						

#define SEND_IR_COMMAND_LENGTH			4 			
// COMMAND_BYTE is 0
#define SEND_IR_BYTE					1
#define SEND_IR_DURATION_LSB			2
#define SEND_IR_DURATION_MSB			3

#define IR_MODULE_MODE_COMMAND_LENGTH	2
// COMMAND_BYTE is 0
#define IR_MODULE_MODE					1
// Constants used within the IR_MODULE_MODE command.
#define IR_MODULE_MODE_OFF				0
#define IR_MODULE_MODE_COUNT			1 // Attempting to find obstacles by counting number of edges from 20 pulses
#define IR_MODULE_MODE_BYTE				2 // Attempting to receive an IR byte from a remote or another robot.


class MiniQCom
{
  public:
	MiniQCom(boolean isMaster, byte slaveAddress);
	void sendDrivePwm(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle);
	void sendBatteryVoltageRequest();
	void sendBatteryVoltageReply(int batteryMillivolts);
	void registerDrivePwmCallback(void (* drivePwmCallback)(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) );
    void registerBatteryVoltageRequestCallback(void (* batteryVoltageRequestCallback)(void) );
    void registerBatteryVoltageReplyCallback(void (* batteryVoltageReplyCallback)(int batteryMillivolts) );	
    void handleRxByte(byte newRxByte);
  private:
	boolean _isMaster;
	byte _slaveAddress;
	byte _txMessageBuffer[MAX_MESSAGE_LENGTH];
	byte _rxMessageBuffer[MAX_MESSAGE_LENGTH];
	void _sendMessage(byte messageLength);
	void _sendByte(byte unescapedbyte);
	void (* _drivePwmCallback)(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle);
    void (* _batteryVoltageRequestCallback)(void);
    void (* _batteryVoltageReplyCallback)(int batteryMillivolts);
    boolean _lastByteWasStartByte;
	boolean _lastByteWasEscapeByte;
	int _bytesRemainingInMessage;
	int _nextOpenByteInMessageBuffer;
	byte _crc;
	void _parseValidMessage();
};

#endif
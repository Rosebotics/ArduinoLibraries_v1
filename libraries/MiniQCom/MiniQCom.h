#ifndef MiniQCom_h
#define MiniQCom_h

#include "Arduino.h"

// Special protocol bytes
#define START_BYTE 			0x7E
#define ESCAPE_BYTE 		0x7D
#define ESCAPE_XOR 			0x20

// Message details
#define MAX_MESSAGE_LENGTH 	32
#define COMMAND_BYTE 		0

// Commands
#define COMMAND_WHEEL_SPEED              0
#define COMMAND_BATTERY_VOLTAGE_REQUEST  1
#define COMMAND_BATTERY_VOLTAGE_REPLY    2

// Bytes within commands
#define WHEEL_PWM_MESSAGE_LENGTH		4
// Note COMMAND_BYTE is 0 + parameter bytes needed
#define WHEEL_PWM_DIRECTIONS			1
#define WHEEL_PWM_LEFT_DUTY_CYCLE		2
#define WHEEL_PWM_RIGHT_DUTY_CYCLE		3

#define BATTERY_VOLTAGE_REQUEST_LENGTH 	1
// COMMAND_BYTE is 0

#define BATTERY_VOLTAGE_REPLY_LENGTH 	3
// COMMAND_BYTE is 0
#define BATTERY_VOLTAGE_REPLY_LSB 		1
#define BATTERY_VOLTAGE_REPLY_MSB 		2

// Constants used in the wheel PWM commands.
#define WHEEL_PWM_MODE_BRAKE		0
#define WHEEL_PWM_MODE_FORWARD		1
#define WHEEL_PWM_MODE_REVERSE      2

class MiniQCom
{
  public:
	MiniQCom();
	void sendWheelPwm(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle);
	void sendBatteryVoltageRequest();
	void sendBatteryVoltageReply(int batteryMillivolts);
	void registerWheelPwmCallback(void (* wheelPwmCallback)(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) );
    void registerBatteryVoltageRequestCallback(void (* batteryVoltageRequestCallback)(void) );
    void registerBatteryVoltageReplyCallback(void (* batteryVoltageReplyCallback)(int batteryMillivolts) );	
    void handleRxByte(byte newRxByte);
  private:
	byte _txMessageBuffer[MAX_MESSAGE_LENGTH];
	byte _rxMessageBuffer[MAX_MESSAGE_LENGTH];
	void _sendMessage(byte messageLength);
	void _sendByte(byte unescapedbyte);
	void (* _wheelPwmCallback)(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle);
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
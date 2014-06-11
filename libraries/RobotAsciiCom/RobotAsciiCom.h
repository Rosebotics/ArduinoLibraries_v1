#ifndef RobotAsciiCom_h
#define RobotAsciiCom_h

#include "Arduino.h"

#define MAX_ASCII_MESSAGE_LENGTH 	64 // If you have longer message change this!
#define MESSAGE_TERMINATOR 			10


class RobotAsciiCom {
  public:
    RobotAsciiCom();
    void handleRxByte(byte newRxByte);
    void handleRxBytes(byte newRxBytes[], int length);
	void registerPwmCallback(void (* pwmCallback)(int leftDutyCycle, int rightDutyCycle) );
	void registerCustomStringCallback( void (* customStringCallback)(String customString) );
    void registerBatteryVoltageRequestCallback(void (* batteryVoltageRequestCallback)(void) );
    void registerCurrentRequestCallback(void (* currentRequestCallback)(void) );
	int prepareBatteryVoltageReply(int batteryMillivolts, char buf[], int maxLength);
	int prepareCurrentReply(int leftMotorMilliamps, int rightMotorMilliamps, char buf[], int maxLength);
  private:
    char _rxMessageBuffer[MAX_ASCII_MESSAGE_LENGTH];
    int _nextOpenByteInMessageBuffer;
    void (* _pwmCallback)(int leftDutyCycle, int rightDutyCycle);
    void (* _customStringCallback)(String customString);
    void (* _batteryVoltageRequestCallback)(void);
    void (* _currentRequestCallback)(void);
    void _parseStringCommand(String command);
};

#endif

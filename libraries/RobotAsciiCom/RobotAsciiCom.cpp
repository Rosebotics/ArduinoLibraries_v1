#include "Arduino.h"
#include "RobotAsciiCom.h"

RobotAsciiCom::RobotAsciiCom() {
	_nextOpenByteInMessageBuffer = 0;
	_pwmCallback = NULL;
	_batteryVoltageRequestCallback = NULL;
	_currentRequestCallback = NULL;
}

/**
 * Convenience method to handle multiple bytes.
 * Just calls the handleRxByte method with each byte individually.
 */
void RobotAsciiCom::handleRxBytes(byte newRxBytes[], int length) {
	for (int i = 0; i < length; i++) {
		handleRxByte(newRxBytes[i]);
	}
}

/**
 * Handle a newly received byte.
 * If the byte is the MESSAGE_TERMINATOR process the message.
 * If the byte is not the MESSAGE_TERMINATOR then just save it.
 */
void RobotAsciiCom::handleRxByte(byte newRxByte) {
	if (newRxByte == MESSAGE_TERMINATOR) {
		// Convert the rx message buffer to a String and parse.
		_rxMessageBuffer[_nextOpenByteInMessageBuffer] = '\0';
		String rxStr = String(_rxMessageBuffer);
		_parseStringCommand(rxStr);  // The real work happens here.
		_nextOpenByteInMessageBuffer = 0;
	} else {
		// Mid message.  Save the byte.
		_rxMessageBuffer[_nextOpenByteInMessageBuffer] = newRxByte;
		_nextOpenByteInMessageBuffer++;
	}
}

void RobotAsciiCom::registerPwmCallback(
		void (*pwmCallback)(int leftDutyCycle, int rightDutyCycle)) {
	_pwmCallback = pwmCallback;
}

void RobotAsciiCom::registerBatteryVoltageRequestCallback(
		void (*batteryVoltageRequestCallback)(void)) {
	_batteryVoltageRequestCallback = batteryVoltageRequestCallback;
}

void RobotAsciiCom::registerCurrentRequestCallback(
		void (*currentRequestCallback)(void)) {
	_currentRequestCallback = currentRequestCallback;
}

void RobotAsciiCom::registerCustomStringCallback(
		void (*customStringCallback)(String customString)) {
	_customStringCallback = customStringCallback;
}

int RobotAsciiCom::prepareBatteryVoltageReply(int batteryMillivolts, char buf[],
		int maxLength) {
	int batteryVoltageReplyLength = 27;
	// 123456789012345678901234567
	// BATTERY VOLTAGE REPLY 1.234
	// BATTERY VOLTAGE REPLY %d.%03d"
	if (maxLength < batteryVoltageReplyLength) {
		return -1;
	}
	sprintf(buf, "BATTERY VOLTAGE REPLY %d.%03d", batteryMillivolts / 1000,
			batteryMillivolts % 1000);
	if (batteryMillivolts / 1000 > 10) {
		batteryVoltageReplyLength++;
	}
	return batteryVoltageReplyLength;
}

int RobotAsciiCom::prepareCurrentReply(int leftMotorMilliamps,
		int rightMotorMilliamps, char buf[], int maxLength) {
	int currentReplyLength = 25;
	// 1234567890123456789012345
	// CURRENT REPLY 1.234 2.345
	// CURRENT REPLY %d.%03d %d.%03d
	sprintf(buf, "CURRENT REPLY %d.%03d %d.%03d",
			leftMotorMilliamps / 1000, leftMotorMilliamps % 1000,
			rightMotorMilliamps / 1000, rightMotorMilliamps % 1000);
	if (leftMotorMilliamps / 1000 > 10) {
		currentReplyLength++;
	}
	if (rightMotorMilliamps / 1000 > 10) {
		currentReplyLength++;
	}
	return currentReplyLength;
}

/**
 * Process the complete message.  Note, uses minimal error checking!
 * CONSIDER: This function is too big.  Break up commands to helper functions.
 */
void RobotAsciiCom::_parseStringCommand(String command) {
	int spaceIndex = command.indexOf(' ');
	if (command.startsWith("PWM")) {
		command = command.substring(spaceIndex + 1); // Removes the word PWM
		spaceIndex = command.indexOf(' ');
		String leftDutyCycleStr = command.substring(0, spaceIndex);
		String rightDutyCycleStr = command.substring(spaceIndex + 1);
		if (_pwmCallback != NULL) {
			_pwmCallback(leftDutyCycleStr.toInt(), rightDutyCycleStr.toInt());
		}
	} else if (command.startsWith("BATTERY VOLTAGE REQUEST")) {
		if (_batteryVoltageRequestCallback != NULL) {
			_batteryVoltageRequestCallback();
		}
	} else if (command.startsWith("CURRENT REQUEST")) {
		if (_currentRequestCallback != NULL) {
			_currentRequestCallback();
		}
	} else if (command.startsWith("CUSTOM")) {
		String customStr = command.substring(spaceIndex + 1);
		if (_customStringCallback != NULL) {
			_customStringCallback(customStr);
		}
	}
}

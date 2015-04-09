#include "Arduino.h"
#include "MiniQCom.h"
#include "Wire.h"

MiniQCom::MiniQCom(boolean isMaster, byte slaveAddress) {
	_wheelPwmCallback = NULL;
	_batteryVoltageRequestCallback = NULL;
	_batteryVoltageReplyCallback = NULL;
	_lastByteWasStartByte = false;
	_lastByteWasEscapeByte = false;
	_bytesRemainingInMessage = -1;
	_isMaster = isMaster;
	_slaveAddress = slaveAddress;
}


void MiniQCom::sendWheelPwn(byte leftMode, byte rightMode,
		byte leftDutyCycle, byte rightDutyCycle) {
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_WHEEL_PWM;
	_txMessageBuffer[WHEEL_PWM_DIRECTIONS] = (leftMode << 1) | rightMode;
	_txMessageBuffer[WHEEL_PWM_LEFT_DUTY_CYCLE] = leftDutyCycle;
	_txMessageBuffer[WHEEL_PWM_RIGHT_DUTY_CYCLE] = rightDutyCycle;
	_sendMessage (WHEEL_PWM_MESSAGE_LENGTH);
}


void MiniQCom::sendBatteryVoltageRequest() {
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_BATTERY_VOLTAGE_REQUEST;
	_sendMessage (BATTERY_VOLTAGE_REQUEST_LENGTH);
}


void MiniQCom::sendBatteryVoltageReply(int batteryMillivolts) {
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_BATTERY_VOLTAGE_REPLY;
	_txMessageBuffer[BATTERY_VOLTAGE_REPLY_LSB] = (byte) batteryMillivolts;
	_txMessageBuffer[BATTERY_VOLTAGE_REPLY_MSB] = (byte)(
			batteryMillivolts >> 8);
	_sendMessage (BATTERY_VOLTAGE_REPLY_LENGTH);
}


void MiniQCom::_sendMessage(byte messageLength) {
	byte crc = 0;
	if (_isMaster) {
	  Wire.beginTransmission(_slaveAddress);
	}
	Wire.write(START_BYTE);
	_sendByte(messageLength);
	for (int i = 0; i < messageLength; i++) {
		_sendByte (_txMessageBuffer[i]);
		crc += _txMessageBuffer[i];
	}
	_sendByte(-crc);
	if (_isMaster) {
	  Wire.endTransmission();
	}
}


void MiniQCom::_sendByte(byte unescapedbyte) {
	if (unescapedbyte == START_BYTE || unescapedbyte == ESCAPE_BYTE) {
		Wire.write(ESCAPE_BYTE);
		Wire.write(unescapedbyte ^ ESCAPE_XOR);
	} else {
		Wire.write(unescapedbyte);
	}
}


void MiniQCom::registerWheelPwmCallback(
		void (*wheelPwmCallback)(byte leftMode, byte rightMode,
				byte leftDutyCycle, byte rightDutyCycle)) {
	_wheelPwmCallback = wheelPwmCallback;
}


void MiniQCom::registerBatteryVoltageRequestCallback(
void (*batteryVoltageRequestCallback)(void)) {
	_batteryVoltageRequestCallback = batteryVoltageRequestCallback;
}


void MiniQCom::registerBatteryVoltageReplyCallback(
		void (*batteryVoltageReplyCallback)(int batteryMillivolts)) {
	_batteryVoltageReplyCallback = batteryVoltageReplyCallback;
}


void MiniQCom::handleRxByte(byte newRxByte) {
	// Highest priority is the start byte.
	if (newRxByte == START_BYTE) {
		_lastByteWasStartByte = true;
		_lastByteWasEscapeByte = false;
		return;
	} else if (!_lastByteWasStartByte) {
		// Make sure we are expecting bytes (ignore unexpected bytes).
		if (_bytesRemainingInMessage < 0) {
			return;
		}
	}
	// Next handle the escaping mechanism.
	if (_lastByteWasEscapeByte) {
		newRxByte ^= ESCAPE_XOR;
	} else {
		if (newRxByte == ESCAPE_BYTE) {
			_lastByteWasEscapeByte = true;
			return;
		}
	}
	_lastByteWasEscapeByte = false;
	// Next handle the length byte
	if (_lastByteWasStartByte) {
		// This is the length byte.
		_bytesRemainingInMessage = newRxByte;
		_crc = 0;
		_nextOpenByteInMessageBuffer = 0;
		_lastByteWasStartByte = false;
		return;
	}
	// Handle this data byte.
	_crc += newRxByte;
	if (_bytesRemainingInMessage > 0) {
		// Receiving bytes in the message.
		_rxMessageBuffer[_nextOpenByteInMessageBuffer] = newRxByte;
		_nextOpenByteInMessageBuffer++;
	} else {
		// Message just finished time to validate the CRC byte.
		if (_crc == 0) {
			_parseValidMessage();
		}
	}
	_bytesRemainingInMessage--;
}


void MiniQCom::_parseValidMessage() {
	switch (_rxMessageBuffer[COMMAND_BYTE]) {
	case COMMAND_WHEEL_PWM:
		if (_wheelPwmCallback != NULL) {
			_wheelPwmCallback((_rxMessageBuffer[WHEEL_PWM_DIRECTIONS] >> 1) & 0x01,
					_rxMessageBuffer[WHEEL_PWM_DIRECTIONS] & 0x01,
					_rxMessageBuffer[WHEEL_PWM_LEFT_DUTY_CYCLE],
					_rxMessageBuffer[WHEEL_PWM_RIGHT_DUTY_CYCLE]);
		}
		break;
	case COMMAND_BATTERY_VOLTAGE_REQUEST:
		if (_batteryVoltageRequestCallback != NULL) {
			_batteryVoltageRequestCallback();
		}
		break;
	case COMMAND_BATTERY_VOLTAGE_REPLY:
		if (_batteryVoltageReplyCallback != NULL) {
			int batteryVoltageMillivolts =
					_rxMessageBuffer[BATTERY_VOLTAGE_REPLY_MSB];
			batteryVoltageMillivolts = batteryVoltageMillivolts << 8;
			batteryVoltageMillivolts +=
					_rxMessageBuffer[BATTERY_VOLTAGE_REPLY_LSB];
			_batteryVoltageReplyCallback(batteryVoltageMillivolts);
		}
		break;
	default:
		// Silently do nothing with unknown commands
		break;
	}
}

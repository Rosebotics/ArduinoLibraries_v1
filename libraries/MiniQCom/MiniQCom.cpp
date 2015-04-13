#include "Arduino.h"
#include "MiniQCom.h"
#include "Wire.h"

MiniQCom::MiniQCom(boolean isMaster, byte miniQAddress) {
	// Just making sure the default state for all variables is cleared out.
	_isMaster = isMaster;
	_miniQAddress = miniQAddress;
	_sensorMask = 0x00000000; // No active sensors.
    // Sensor variables.
    leftMotorEncoder = 0;
    rightMotorEncoder = 0;
    farLeftIrLineSensor = 0;
    midLeftIrLineSensor = 0;
    centerIrLineSensor = 0;
    midRightIrLineSensor = 0;
    farRightIrLineSensor = 0;
    photoDetectors = 0;
    irModuleCountForLeftIr = 0;
    irModuleCountForRightIr = 0;
    irModuleByte = 0;
    keyButtons = KEY_BUTTONS_NO_PRESS;
    batterVoltageTenths = 0;
	_drivePwmCallback = NULL;
	_driveSpeedArcCallback = NULL;
	_sensorMaskCallback = NULL;
	_buzzerToneCallback = NULL;
	_ledCallback = NULL;
	_sendIrCallback = NULL;
	_irModuleMode = NULL;
	_lastByteWasStartByte = false;
	_lastByteWasEscapeByte = false;
	_bytesRemainingInMessage = -1;
	_nextOpenByteInMessageBuffer = 0;
}


void MiniQCom::sendDrivePwm(boolean leftIsForward, boolean rightIsForward,
		byte leftDutyCycle, byte rightDutyCycle) {
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_DRIVE_PWM;
	_txMessageBuffer[DRIVE_PWM_DIRECTIONS] = ((byte)leftIsForward << 1) | (byte)rightIsForward;
	_txMessageBuffer[DRIVE_PWM_LEFT_DUTY_CYCLE] = leftDutyCycle;
	_txMessageBuffer[DRIVE_PWM_RIGHT_DUTY_CYCLE] = rightDutyCycle;
	_sendMessage(DRIVE_PWM_COMMAND_LENGTH);
}


void MiniQCom::sendDriveSpeedArc(int speedMmPerS, int arcMm) {
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_DRIVE_SPEED_ARC;
	_txMessageBuffer[DRIVE_SPEED_ARC_SPEED_LSB] = (byte)speedMmPerS;
	_txMessageBuffer[DRIVE_SPEED_ARC_SPEED_MSB] = (byte)(speedMmPerS >> 8);
	_txMessageBuffer[DRIVE_SPEED_ARC_ARC_LSB] = (byte)arcMm;
	_txMessageBuffer[DRIVE_SPEED_ARC_ARC_MSB] = (byte)(arcMm >> 8);
	_sendMessage(DRIVE_SPEED_ARC_COMMAND_LENGTH);	
}


void MiniQCom::sendSetSensorMask(unsigned long sensorMask) {
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_SET_SENSOR_MASK;
	_txMessageBuffer[SENSOR_MASK_BYTE0] = (byte)sensorMask;
	_txMessageBuffer[SENSOR_MASK_BYTE1] = (byte)(sensorMask >> 8);
	_txMessageBuffer[SENSOR_MASK_BYTE2] = (byte)(sensorMask >> 16);
	_txMessageBuffer[SENSOR_MASK_BYTE3] = (byte)(sensorMask >> 24);
	_sendMessage(SENSOR_MASK_COMMAND_LENGTH);
}


void MiniQCom::sendAddToSensorMask(unsigned long sensorMaskBitsToAdd) {
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_ADD_TO_SENSOR_MASK;
	_txMessageBuffer[SENSOR_MASK_BYTE0] = (byte)sensorMaskBitsToAdd;
	_txMessageBuffer[SENSOR_MASK_BYTE1] = (byte)(sensorMaskBitsToAdd >> 8);
	_txMessageBuffer[SENSOR_MASK_BYTE2] = (byte)(sensorMaskBitsToAdd >> 16);
	_txMessageBuffer[SENSOR_MASK_BYTE3] = (byte)(sensorMaskBitsToAdd >> 24);
	_sendMessage(SENSOR_MASK_COMMAND_LENGTH);
}


void MiniQCom::sendRemoveFromSensorMask(unsigned long sensorMaskBitsToRemove) {
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_REMOVE_FROM_SENSOR_MASK;
	_txMessageBuffer[SENSOR_MASK_BYTE0] = (byte)sensorMaskBitsToRemove;
	_txMessageBuffer[SENSOR_MASK_BYTE1] = (byte)(sensorMaskBitsToRemove >> 8);
	_txMessageBuffer[SENSOR_MASK_BYTE2] = (byte)(sensorMaskBitsToRemove >> 16);
	_txMessageBuffer[SENSOR_MASK_BYTE3] = (byte)(sensorMaskBitsToRemove >> 24);
	_sendMessage(SENSOR_MASK_COMMAND_LENGTH);
}


void MiniQCom::sendBuzzerTone(unsigned int frequency, unsigned long durationMs) {
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_BUZZER_TONE;
	_txMessageBuffer[BUZZER_TONE_FREQUENCY_LSB] = (byte)frequency;
	_txMessageBuffer[BUZZER_TONE_FREQUENCY_MSB] = (byte)(frequency >> 8);
	_txMessageBuffer[BUZZER_TONE_DURATION_BYTE0] = (byte)durationMs;
	_txMessageBuffer[BUZZER_TONE_DURATION_BYTE1] = (byte)(durationMs >> 8);
	_txMessageBuffer[BUZZER_TONE_DURATION_BYTE2] = (byte)(durationMs >> 16);
	_txMessageBuffer[BUZZER_TONE_DURATION_BYTE3] = (byte)(durationMs >> 24);
	_sendMessage(BUZZER_TONE_COMMAND_LENGTH);
}


void MiniQCom::sendLed(byte red, byte green, byte blue) {
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_LED;
	_txMessageBuffer[LED_R] = red;
	_txMessageBuffer[LED_G] = green;
	_txMessageBuffer[LED_B] = blue;
	_sendMessage(LED_COMMAND_LENGTH);	
}


void MiniQCom::sendSendIr(byte byteToSend, unsigned int durationMs) {
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_SEND_IR;
	_txMessageBuffer[SEND_IR_BYTE] = byteToSend;
	_txMessageBuffer[SEND_IR_DURATION_LSB] = (byte)durationMs;
	_txMessageBuffer[SEND_IR_DURATION_MSB] = (byte)(durationMs >> 8);
	_sendMessage(SEND_IR_COMMAND_LENGTH);
}


void MiniQCom::sendIrModuleMode(irModuleMode_t irModuleMode) {
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_IR_MODULE_MODE;
	_txMessageBuffer[IR_MODULE_MODE] = (byte)irModuleMode;  // Already is a byte.
	_sendMessage(IR_MODULE_MODE_COMMAND_LENGTH);
}


void MiniQCom::_sendMessage(byte messageLength) {
	byte crc = 0;
	if (_isMaster) {
	  Wire.beginTransmission(_miniQAddress);
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


void MiniQCom::registerDrivePwmCallback(void (*drivePwmCallback)(boolean leftIsForward, boolean rightIsForward, byte leftDutyCycle, byte rightDutyCycle)) {
	_drivePwmCallback = drivePwmCallback;
}


void MiniQCom::registerDriveSpeedArcCallback(void (*driveSpeedArcCallback)(int speedMmPerS, int arcMm)) {
	_driveSpeedArcCallback = driveSpeedArcCallback;
}


void MiniQCom::registerSensorMaskCallback(void (*sensorMaskCallback)(sensorMaskSetAddOrRemove_t setAddOrRemove, unsigned long sensorMask)) {
	_sensorMaskCallback = sensorMaskCallback;
}


void MiniQCom::registerBuzzerToneCallback(void (*buzzerToneCallback)(unsigned int frequency, unsigned long durationMs)) {
	_buzzerToneCallback = buzzerToneCallback;
}


void MiniQCom::registerLedCallback(void (*ledCallback)(byte red, byte green, byte blue)) {
	_ledCallback = ledCallback;
}


void MiniQCom::registerSendIrCallback(void (*sendIrCallback)(byte byteToSend, unsigned int durationMs)) {
	_sendIrCallback = sendIrCallback;
}


void MiniQCom::registerIrModuleMode(void (*irModuleMode)(irModuleMode_t irModuleMode)) {
	_irModuleMode = irModuleMode;
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
	case COMMAND_DRIVE_PWM:
		if (_drivePwmCallback != NULL) {
			_drivePwmCallback((_rxMessageBuffer[DRIVE_PWM_DIRECTIONS] >> 1) & 0x01,
					_rxMessageBuffer[DRIVE_PWM_DIRECTIONS] & 0x01,
					_rxMessageBuffer[DRIVE_PWM_LEFT_DUTY_CYCLE],
					_rxMessageBuffer[DRIVE_PWM_RIGHT_DUTY_CYCLE]);
		}
		break;
	case COMMAND_DRIVE_SPEED_ARC:
		if (_driveSpeedArcCallback != NULL) {
			int speedMmPerS = _rxMessageBuffer[DRIVE_SPEED_ARC_SPEED_MSB];
			speedMmPerS = speedMmPerS << 8;
			speedMmPerS += _rxMessageBuffer[DRIVE_SPEED_ARC_SPEED_LSB];	
			int arcMm = _rxMessageBuffer[DRIVE_SPEED_ARC_ARC_MSB];
			arcMm = arcMm << 8;
			arcMm += _rxMessageBuffer[DRIVE_SPEED_ARC_ARC_LSB];
			_driveSpeedArcCallback(speedMmPerS, arcMm);
		}
		break;
	case COMMAND_SET_SENSOR_MASK:
		if (_sensorMaskCallback != NULL) {
			unsigned long sensorMask = _rxMessageBuffer[SENSOR_MASK_BYTE3];
			sensorMask = sensorMask << 8;
			sensorMask += _rxMessageBuffer[SENSOR_MASK_BYTE2];
			sensorMask = sensorMask << 8;
			sensorMask += _rxMessageBuffer[SENSOR_MASK_BYTE1];
			sensorMask = sensorMask << 8;
			sensorMask += _rxMessageBuffer[SENSOR_MASK_BYTE0];
			_sensorMask = sensorMask;  // Set the mask.
			_sensorMaskCallback(SENSOR_MASK_SET, sensorMask);
		}
		break;
	case COMMAND_ADD_TO_SENSOR_MASK:
		if (_sensorMaskCallback != NULL) {
			unsigned long sensorMask = _rxMessageBuffer[SENSOR_MASK_BYTE3];
			sensorMask = sensorMask << 8;
			sensorMask += _rxMessageBuffer[SENSOR_MASK_BYTE2];
			sensorMask = sensorMask << 8;
			sensorMask += _rxMessageBuffer[SENSOR_MASK_BYTE1];
			sensorMask = sensorMask << 8;
			sensorMask += _rxMessageBuffer[SENSOR_MASK_BYTE0];
			_sensorMask |= sensorMask; // OR the two masks.
			_sensorMaskCallback(SENSOR_MASK_ADD, sensorMask);
		}
		break;
	case COMMAND_REMOVE_FROM_SENSOR_MASK:
		if (_sensorMaskCallback != NULL) {
			unsigned long sensorMask = _rxMessageBuffer[SENSOR_MASK_BYTE3];
			sensorMask = sensorMask << 8;
			sensorMask += _rxMessageBuffer[SENSOR_MASK_BYTE2];
			sensorMask = sensorMask << 8;
			sensorMask += _rxMessageBuffer[SENSOR_MASK_BYTE1];
			sensorMask = sensorMask << 8;
			sensorMask += _rxMessageBuffer[SENSOR_MASK_BYTE0];
			_sensorMask &= ~sensorMask; // Remove only certain sensors.
			_sensorMaskCallback(SENSOR_MASK_REMOVE, sensorMask);
		}
		break;
	case COMMAND_BUZZER_TONE:
		if (_buzzerToneCallback != NULL) {
			unsigned int frequency = _rxMessageBuffer[BUZZER_TONE_FREQUENCY_MSB];
			frequency = frequency << 8;
			frequency += _rxMessageBuffer[BUZZER_TONE_FREQUENCY_LSB];	
			unsigned long durationMs = _rxMessageBuffer[BUZZER_TONE_DURATION_BYTE3];
			durationMs = durationMs << 8;
			durationMs += _rxMessageBuffer[BUZZER_TONE_DURATION_BYTE2];
			durationMs = durationMs << 8;
			durationMs += _rxMessageBuffer[BUZZER_TONE_DURATION_BYTE1];
			durationMs = durationMs << 8;
			durationMs += _rxMessageBuffer[BUZZER_TONE_DURATION_BYTE0];
			_buzzerToneCallback(frequency, durationMs);
		}
		break;
	case COMMAND_LED:
		if (_ledCallback != NULL) {
			_ledCallback(_rxMessageBuffer[LED_R],
						 _rxMessageBuffer[LED_G],
						 _rxMessageBuffer[LED_B]);
		}
		break;
	case COMMAND_SEND_IR:
		if (_sendIrCallback != NULL) {
			unsigned int durationMs = _rxMessageBuffer[SEND_IR_DURATION_MSB];
			durationMs = durationMs << 8;
			durationMs += _rxMessageBuffer[SEND_IR_DURATION_LSB];	
			_sendIrCallback(_rxMessageBuffer[SEND_IR_BYTE], durationMs);
		}
		break;
	case COMMAND_IR_MODULE_MODE:
		if (_irModuleMode != NULL) {
			_irModuleMode((irModuleMode_t)_rxMessageBuffer[IR_MODULE_MODE]);
		}
		break;
	default:
		// Silently do nothing with unknown commands
		break;
	}
}

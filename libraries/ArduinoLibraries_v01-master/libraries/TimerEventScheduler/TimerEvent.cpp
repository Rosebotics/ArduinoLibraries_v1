#include "Arduino.h"
#include "TimerEvent.h"

TimerEvent::TimerEvent(int (* timerEventCallback)(void), int timerTicksRemaining) {
	_timerEventCallback = timerEventCallback;
	_timerTicksRemaining = timerTicksRemaining;
}


void TimerEvent::setTimerTicksRemaining(int timerTicksRemaining) {
	_timerTicksRemaining = timerTicksRemaining;
}


int TimerEvent::getTimerTicksRemaining() {
	return _timerTicksRemaining;
}


int TimerEvent::timerEventCallback() {
	return _timerEventCallback();
}
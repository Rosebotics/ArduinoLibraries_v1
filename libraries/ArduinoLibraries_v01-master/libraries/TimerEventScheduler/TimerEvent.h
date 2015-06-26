#ifndef TimerEvent_h
#define TimerEvent_h

/**
 * Stores a callback function and a number of milliseconds until it is called.
 * 
 * This class is just a simple struct for storing info about a timer event.
 * I considered just making it a struct, but made a really simple class instead.
 * I figured that would be easier syntax for student use.
 */

#include "Arduino.h"

#define DEBOUNCE_TIME 20

class TimerEvent
{
  public:
	TimerEvent(int (* timerEventCallback)(void), int timerTicksRemaining);
	int timerEventCallback();
	int getTimerTicksRemaining(void);
	void setTimerTicksRemaining(int timerTicksRemaining);
  protected:
	int (* _timerEventCallback)(void);
	int _timerTicksRemaining;
};

#endif
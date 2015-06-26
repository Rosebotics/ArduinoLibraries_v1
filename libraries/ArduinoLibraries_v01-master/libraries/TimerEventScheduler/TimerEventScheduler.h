#ifndef TimerEventScheduler_h
#define TimerEventScheduler_h

/**
 * Creates a mechanism to schedule timer events.  Creates a callback using Timer2
 * that overflows every 1 ms.  Every overflow the timer events get closer to being
 * called.  When a Timer Event in the array hits 0 timer ticks remaining a callback
 * function is called.  The return value of the callback sets the next timer ticks
 * remaining value so the callback can be called again.
 * 
 * Note, uses C, instead of C++, technics for the dynamic array of pointers.
 */

#include "Arduino.h"
#include "TimerEvent.h"

#define INITIAL_TIMER_EVENT_TABLE_CAPACITY 10

class TimerEventScheduler
{
  public:
	TimerEventScheduler();
	void addTimerEvent(TimerEvent *timerEventPtr);
  protected:
	void _doubleTimerEventTableCapacity(void);
	int _getNextTableIndex(void);
	// Low level Timer2 work from MsTimer2 playground code.
	void _setupTimer2(void);
	void _start(void);
	void _stop(void);
};

#endif



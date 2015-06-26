#include "Arduino.h"
#include "TimerEventScheduler.h"

volatile byte _tcnt2;
TimerEvent **_timerEventTable;  // Array of pointers to TimerEvents.
int _timerEventTableSize;
int _timerEventTableCapacity;

void _timer2OverflowISR(void);  // Made global for ISR.

// Create a Timer Event Table that is managed by the scheduler.
// A TimerEventScheduler has a Timer Event Table.
TimerEventScheduler::TimerEventScheduler() {
	_timerEventTableSize = 0;
	_timerEventTableCapacity = INITIAL_TIMER_EVENT_TABLE_CAPACITY;
	_timerEventTable = (TimerEvent **) malloc(
			_timerEventTableCapacity * sizeof(TimerEvent *));
	if (_timerEventTable == NULL) {
		while (1) {
			Serial.println("Did not malloc space for the timer event table.");
		};  // If the chip fails to allocate memory trap here for debug.
	}
}

/**
 * Function that is called when Timer2 overflows (which happen every 1 mS)
 * 
 * Decrements the timer ticks remaining on all active Timer Events.
 * If a Timer Event hits 0 then call the callback function.
 * The return value from the callback function sets the next callback.
 */ 
void _timer2OverflowISR(void) {
	int ticksRemaining;
	for (int tableIndex = 0; tableIndex < _timerEventTableSize; tableIndex++) {
		ticksRemaining = _timerEventTable[tableIndex]->getTimerTicksRemaining();
		if (ticksRemaining != 0) {
			// We have found an active timer event.
			ticksRemaining--;
			_timerEventTable[tableIndex]->setTimerTicksRemaining(ticksRemaining);
			if (ticksRemaining == 0) {
				// Just now the count reached zero, call the callback
				ticksRemaining = _timerEventTable[tableIndex]->timerEventCallback();
				_timerEventTable[tableIndex]->setTimerTicksRemaining(ticksRemaining);
			}
		}
	}
}

// Add a new Timer Event to the Timer Event Table.
void TimerEventScheduler::addTimerEvent(TimerEvent *timerEventPtr) {
	if (_timerEventTableSize == 0) {
		// First Timer Event added.  Start Timer2.
		// Note, this was intentionally NOT done in the constructor.
		// Things in the constructor happen too early for timer setup to work right.
		_setupTimer2();
		_start();
	}
	int timerEventIndex = _getNextTableIndex();
	_timerEventTable[timerEventIndex] = timerEventPtr;
}

// Increase the size of the Timer Event Table.
void TimerEventScheduler::_doubleTimerEventTableCapacity(void) {
	_timerEventTableCapacity *= 2;
	_timerEventTable = (TimerEvent **) realloc(_timerEventTable,
			_timerEventTableCapacity * sizeof(TimerEvent *));
	if (_timerEventTable == NULL) {
		while (1) {
			Serial.println("Did not realloc space for the timer event table.");
		};  // If the chip fails to allocate memory trap here for debug.
	}
}

// Get the next open index in the Timer Event Table.
int TimerEventScheduler::_getNextTableIndex(void) {
	int nextTableIndex = _timerEventTableSize;
	_timerEventTableSize++;
	if (_timerEventTableSize > _timerEventTableCapacity) {
		_doubleTimerEventTableCapacity();
	}
	return nextTableIndex;
}

// --------------------- Low level Timer 2 interrupt stuff ---------------------
// ---------------- from http://playground.arduino.cc/Main/MsTimer2 ------------
// ---------- written by Javier Valencia <javiervalencia80@gmail.com> ----------

// Setup Timer 2 to overflow every 1 ms.
void TimerEventScheduler::_setupTimer2() {
	float prescaler = 0.0;

#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
	TIMSK2 &= ~(1<<TOIE2);
	TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
	TCCR2B &= ~(1<<WGM22);
	ASSR &= ~(1<<AS2);
	TIMSK2 &= ~(1<<OCIE2A);
	
	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2B |= (1<<CS22);
		TCCR2B &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2B |= (1<<CS21);
		TCCR2B &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 128
		TCCR2B |= ((1<<CS22) | (1<<CS20));
		TCCR2B &= ~(1<<CS21);
		prescaler = 128.0;
	}
#elif defined (__AVR_ATmega8__)
	TIMSK &= ~(1<<TOIE2);
	TCCR2 &= ~((1<<WGM21) | (1<<WGM20));
	TIMSK &= ~(1<<OCIE2);
	ASSR &= ~(1<<AS2);
	
	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2 |= (1<<CS22);
		TCCR2 &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2 |= (1<<CS21);
		TCCR2 &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 128
		TCCR2 |= ((1<<CS22) && (1<<CS20));
		TCCR2 &= ~(1<<CS21);
		prescaler = 128.0;
	}
#elif defined (__AVR_ATmega128__)
	TIMSK &= ~(1<<TOIE2);
	TCCR2 &= ~((1<<WGM21) | (1<<WGM20));
	TIMSK &= ~(1<<OCIE2);
	
	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2 |= ((1<<CS21) | (1<<CS20));
		TCCR2 &= ~(1<<CS22);
		prescaler = 64.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2 |= (1<<CS21);
		TCCR2 &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 256
		TCCR2 |= (1<<CS22);
		TCCR2 &= ~((1<<CS21) | (1<<CS20));
		prescaler = 256.0;
	}
#elif defined (__AVR_ATmega32U4__)
	TCCR4B = 0;
	TCCR4A = 0;
	TCCR4C = 0;
	TCCR4D = 0;
	TCCR4E = 0;
	if (F_CPU >= 16000000L) {
		TCCR4B = (1<<CS43) | (1<<PSR4);
		prescaler = 128.0;
	} else if (F_CPU >= 8000000L) {
		TCCR4B = (1<<CS42) | (1<<CS41) | (1<<CS40) | (1<<PSR4);
		prescaler = 64.0;
	} else if (F_CPU >= 4000000L) {
		TCCR4B = (1<<CS42) | (1<<CS41) | (1<<PSR4);
		prescaler = 32.0;
	} else if (F_CPU >= 2000000L) {
		TCCR4B = (1<<CS42) | (1<<CS40) | (1<<PSR4);
		prescaler = 16.0;
	} else if (F_CPU >= 1000000L) {
		TCCR4B = (1<<CS42) | (1<<PSR4);
		prescaler = 8.0;
	} else if (F_CPU >= 500000L) {
		TCCR4B = (1<<CS41) | (1<<CS40) | (1<<PSR4);
		prescaler = 4.0;
	} else {
		TCCR4B = (1<<CS41) | (1<<PSR4);
		prescaler = 2.0;
	}
	_tcnt2 = (int)((float)F_CPU * 0.001 / prescaler) - 1;
	OCR4C = _tcnt2;
	return;
#else
#error Unsupported CPU type
#endif

	_tcnt2 = 256 - (int)((float)F_CPU * 0.001 / prescaler);
}

void TimerEventScheduler::_start() {
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
	TCNT2 = _tcnt2;
	TIMSK2 |= (1<<TOIE2);
#elif defined (__AVR_ATmega128__)
	TCNT2 = _tcnt2;
	TIMSK |= (1<<TOIE2);
#elif defined (__AVR_ATmega8__)
	TCNT2 = _tcnt2;
	TIMSK |= (1<<TOIE2);
#elif defined (__AVR_ATmega32U4__)
	TIFR4 = (1<<TOV4);
	TCNT4 = 0;
	TIMSK4 = (1<<TOIE4);
#endif
}

void TimerEventScheduler::_stop() {
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
	TIMSK2 &= ~(1<<TOIE2);
#elif defined (__AVR_ATmega128__)
	TIMSK &= ~(1<<TOIE2);
#elif defined (__AVR_ATmega8__)
	TIMSK &= ~(1<<TOIE2);
#elif defined (__AVR_ATmega32U4__)
	TIMSK4 = 0;
#endif
}

#if defined (__AVR_ATmega32U4__)
ISR(TIMER4_OVF_vect) {
#else
ISR(TIMER2_OVF_vect) {
#endif
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega328P__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
	TCNT2 = _tcnt2;
#elif defined (__AVR_ATmega128__)
	TCNT2 = _tcnt2;
#elif defined (__AVR_ATmega8__)
	TCNT2 = _tcnt2;
#elif defined (__AVR_ATmega32U4__)
	// not necessary on 32u4's high speed timer4
#endif
	_timer2OverflowISR(); // Overflow function called every 1 ms.
}


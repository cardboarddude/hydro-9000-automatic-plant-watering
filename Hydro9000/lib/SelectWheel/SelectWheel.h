#ifndef SelectWheel_h
#define SelectWheel_h

#include "Arduino.h"

class SelectWheel {
	public: 
		unsigned char pinA, pinB;
        static const unsigned char PROGMEM incrementSize = 2;
        volatile bool isPinAHigh = false, isPinBHigh = false;
        volatile int clockwiseTurns = 0;
        volatile bool hasChanged = false;
        
        SelectWheel();
		SelectWheel(unsigned char pinA, unsigned char pinB);
		int getClockwiseTurns();
		int getCounterClockwiseTurns();
		void resetTurns();
        void readPinA();
        void readPinB();
        void setPinARising();
        void setPinBFalling();
};

#endif

#ifndef SelectWheel_h
#define SelectWheel_h

#include "Arduino.h"

class SelectWheel {
	public: 
		int pinA, pinB;
        volatile bool isPinAHigh = false, isPinBHigh = false;
        volatile int clockwiseTurns = 0;
        volatile bool hasChanged = false;
        
        SelectWheel();
		SelectWheel(int, int);
		int getClockwiseTurns();
		int getCounterClockwiseTurns();
		void resetTurns();
        void readPinA();
        void readPinB();
        void setPinARising();
        void setPinBFalling();
        void save();
};

#endif
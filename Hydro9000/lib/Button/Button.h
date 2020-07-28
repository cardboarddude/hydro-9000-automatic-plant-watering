
#ifndef Button_h
#define Button_h

#include "Arduino.h"

class Button {
	public:
        enum LedState {
            ON,
            OFF,
            BLINKING,
            BLINKING_ON_LONG_OFF_SHORT
        };

        static unsigned long currentMillis;
        int stateWhenLedOn = HIGH, stateWhenButtonPressed = HIGH;
        int lastBlinkMS = 0, blinkOnDurationMS = 300, blinkOffDurationMS = 150;
        bool hasLed = false;
        bool isLedOn, isPressed, wasPressed;

        Button();
        Button(int buttonPin);
		Button(int buttonPin, int ledPin);
        bool isClicked();
        void setLedState(Button::LedState state);
        void update();
        void updateLed();
        void turnLedOn();
        void turnLedOff();
        
	private:
        int ledPin, buttonPin;
        Button::LedState ledState;
};

#endif
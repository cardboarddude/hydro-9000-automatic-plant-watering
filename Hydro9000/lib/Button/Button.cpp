#include "Arduino.h"
#include "Button.h"

Button::Button() {}
Button::Button(int buttonPin) {
    this->buttonPin = buttonPin;
    pinMode(this->buttonPin, INPUT);
}
Button::Button(int buttonPin, int ledPin) {
    this->buttonPin = buttonPin;
    pinMode(this->buttonPin, INPUT);
    this->hasLed = true;
    this->ledPin = ledPin;
    pinMode(this->ledPin, OUTPUT);
}
bool Button::isClicked() {
    return this->isPressed && !this->wasPressed;
}
void Button::setLedState(Button::LedState state) {
    this->ledState = state;
}
void Button::update() {
    this->wasPressed = this->isPressed;
    this->isPressed = digitalRead(this->buttonPin) == this->stateWhenButtonPressed;
    
    if (this->hasLed) {
        this->updateLed();
    }
}
void Button::updateLed() {
    switch(this->ledState) {
        case Button::LedState::ON:
            this->turnLedOn(); 
            break;
        case Button::LedState::OFF:
            this->turnLedOff(); 
            break;
        case Button::LedState::BLINKING:
            int timeElapsed = Button::currentMillis - this->lastBlinkMS;
            if (this->isLedOn && timeElapsed >= this->blinkOnDurationMS) {
                this->turnLedOff();
            } else if (!this->isLedOn && timeElapsed >= this->blinkOffDurationMS) {
                this->turnLedOn();
            } 
            break;
        default:
            Serial.print("Unknown led state '"+String(this->ledState)+"'");
    }
}
void Button::turnLedOn() {
    digitalWrite(this->ledPin, this->stateWhenLedOn);
    this->isLedOn = true;
    if (this->ledState == Button::LedState::BLINKING) {
        this->lastBlinkMS = Button::currentMillis;
    }
}
void Button::turnLedOff() {
    int stateWhenOff = (this->stateWhenLedOn == HIGH ? LOW : HIGH);
    digitalWrite(this->ledPin, stateWhenOff);
    this->isLedOn = false;
    if (this->ledState == Button::LedState::BLINKING) {
        this->lastBlinkMS = Button::currentMillis;
    }
}
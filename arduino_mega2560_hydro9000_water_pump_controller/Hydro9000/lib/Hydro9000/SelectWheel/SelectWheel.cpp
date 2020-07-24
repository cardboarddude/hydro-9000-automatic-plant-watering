#include "Arduino.h"
#include "EEPROM.h"
#include "SelectWheel.h"

SelectWheel::SelectWheel() {}
SelectWheel::SelectWheel(int pinA, int pinB) {
    this->pinA = pinA;
    this->pinB = pinB;
    pinMode(this->pinA, INPUT);
    pinMode(this->pinB, INPUT);
    EEPROM.get(0, this->clockwiseTurns);
}
int SelectWheel::getClockwiseTurns() {
	return this->clockwiseTurns;
}
int SelectWheel::getCounterClockwiseTurns() {
	return -1 * this->clockwiseTurns;
}
void SelectWheel::resetTurns() {
	this->clockwiseTurns = 0;
}
void SelectWheel::readPinA() {
    this->isPinAHigh = digitalRead(this->pinA) == HIGH;
}
void SelectWheel::readPinB() {
    this->isPinBHigh = digitalRead(this->pinB) == HIGH;
}
void SelectWheel::setPinARising() {
  bool isClockwise = !this->isPinAHigh && !this->isPinBHigh;
  if (isClockwise) {
    this->clockwiseTurns++;
  }
  this->isPinAHigh = true;
  this->hasChanged = true;
}
void SelectWheel::setPinBFalling() {
  bool isClockwise = this->isPinAHigh && this->isPinBHigh;
  if (isClockwise) {
    this->clockwiseTurns--;
  }
  this->isPinBHigh = false;
  this->hasChanged = true;
}
void SelectWheel::save() {
    if (this->hasChanged) {
        EEPROM.put(0, this->getClockwiseTurns());
        this->hasChanged = false;
    }
}
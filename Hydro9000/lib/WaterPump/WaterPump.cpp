#include "Arduino.h"
#include "WaterPump.h"

WaterPump::WaterPump() {}
WaterPump::WaterPump(int motorPin) {
    this->motorPin = motorPin;
    this->isRunning = false;
}
void WaterPump::setup() {
    pinMode(this->motorPin, OUTPUT);
}
void WaterPump::start() {
    this->isRunning = true;
    digitalWrite(this->motorPin, HIGH);
}
void WaterPump::stop() {
    this->isRunning = false;
    digitalWrite(this->motorPin, LOW);
}
void WaterPump::toggle() {
    if (this->isRunning) {
        this->stop();
    } else {
        this->start();
    }
}
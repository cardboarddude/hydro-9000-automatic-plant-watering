#include "MoistureSensor.h"
#include "Arduino.h"
#include "EEPROM.h"

MoistureSensor::MoistureSensor() {}
MoistureSensor::MoistureSensor(int pin) {
	this->analogPin = pin;
}
void MoistureSensor::setup(int eepromAddress) {
	this->eepromAddress = eepromAddress;
	EEPROM.get(eepromAddress, this->goalReading);
}
double MoistureSensor::getMoistureLevel() {
	return this->readingToPercentage(this->currentReading);
}
double MoistureSensor::readingToPercentage(double reading, bool doZeroToOne = true) {
	double percent = (this->maxReading - reading) / (this->maxReading - this->minReading);
	if (doZeroToOne) {
		return ((percent < 0) ? 0 : ((percent > 1) ? 1 : percent));
	} else {
		return percent;
	}
}
double MoistureSensor::getAverageReading() {
	if (this->readingCount == 0) {
		return 0;
	}
	
	return this->readingTotal / this->readingCount;
}
void MoistureSensor::resetReadings() {
	this->readingTotal = 0;
	this->readingCount = 0;
}
void MoistureSensor::read() {
	this->currentReading = analogRead(this->analogPin);
	// Serial.print(this->currentReading);
	// Serial.print(" - ");
	this->readingCount++;
	this->readingTotal += this->currentReading;
}
bool MoistureSensor::hasMetGoal() {
	return this->goalReading <= this->getAverageReading();
}
void MoistureSensor::setGoal(double percentage) {
	this->goalReading = this->maxReading - (percentage * (this->maxReading - this->minReading));
}
double MoistureSensor::getGoalPercentage() {
	double percent = (this->maxReading - this->goalReading) / (this->maxReading - this->minReading);
	return percent;
}
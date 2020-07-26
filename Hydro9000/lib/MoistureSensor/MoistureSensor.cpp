#include "MoistureSensor.h"
#include "Arduino.h"
#include "EEPROM.h"

MoistureSensor::MoistureSensor() {}
MoistureSensor::MoistureSensor(int pin, int address) {
	this->analogPin = pin;
	this-> eepromHistoryAddress = address;
}
void MoistureSensor::setup() {
	this->loadData();
	this->writeSerial();
}
float MoistureSensor::getMoistureLevel() {
	float percent = (this->maxReading - this->currentReading) / (this->maxReading - this->minReading);
	return ((percent < 0) ? 0 : ((percent > 1) ? 1 : percent));
}
void MoistureSensor::read() {
	this->currentReading = analogRead(this->analogPin);

	if (this->isNextMinute()) {
		this->pushLastMinuteReadings();

		if (this->isNextQuarterHour()) {
			this->pushLastQuarterHourReadings();
		}

		this->readingTotalSinceLastMinute = this->currentReading;
		this->readingCountSinceLastMinute = 1;
	} else {
		this->readingTotalSinceLastMinute += this->currentReading;
		this->readingCountSinceLastMinute++;
	}
}
bool MoistureSensor::hasMetGoal() {
	return this->goalReading >= this->currentReading;
}
void MoistureSensor::setGoal(float percentage) {
	this->goalReading = this->maxReading - (percentage * (this->maxReading - this->minReading));
}
float MoistureSensor::getGoalPercentage() {
	float percent = (this->maxReading - this->goalReading) / (this->maxReading - this->minReading);
	return percent;
}
void MoistureSensor::loadData() {
	int address = this->eepromHistoryAddress;
	EEPROM.get(address, this->goalReading);
	address += sizeof(this->goalReading);
	EEPROM.get(address, this->readingHistoryInMinutesIndex);
	address += sizeof(this->readingHistoryInMinutesIndex);
	EEPROM.get(address, this->readingHistoryInQuarterHoursIndex);
	address += sizeof(this->readingHistoryInQuarterHoursIndex);
	EEPROM.get(address, this->readingHistoryInMinutes);
	address += sizeof(this->readingHistoryInMinutes);
	EEPROM.get(address, this->readingHistoryInQuarterHours);
}
void MoistureSensor::saveData() {
	int address = this->eepromHistoryAddress;
	EEPROM.put(address, this->goalReading);
	address += sizeof(this->goalReading);
	EEPROM.put(address, this->readingHistoryInMinutesIndex);
	address += sizeof(this->readingHistoryInMinutesIndex);
	EEPROM.put(address, this->readingHistoryInQuarterHoursIndex);
	address += sizeof(this->readingHistoryInQuarterHoursIndex);
	EEPROM.put(address, this->readingHistoryInMinutes);
	address += sizeof(this->readingHistoryInMinutes);
	EEPROM.put(address, this->readingHistoryInQuarterHours);
}
bool MoistureSensor::isNextMinute() {
	return this->lastMinuteMillis + 60000 <= this->currentMillis;
}
bool MoistureSensor::isNextQuarterHour() {
	return this->lastMinuteMillis + 900000 <= this->currentMillis;
}
void MoistureSensor::pushLastMinuteReadings() {
	if (this->readingCountSinceLastMinute > 0) {
		Serial.println("Pushing last minute: "+String(this->lastMinuteMillis)+" "+String(this->readingCountSinceLastMinute)
		+" "+String(this->readingHistoryInMinutesIndex)+" "+String(this->currentReading)+". ");
		this->readingHistoryInMinutes[this->readingHistoryInMinutesIndex] = this->readingTotalSinceLastMinute / this->readingCountSinceLastMinute;
		this->readingHistoryInMinutesIndex = (this->HISTORY_SIZE + this->readingHistoryInMinutesIndex + 1) % this->HISTORY_SIZE;
		this->readingTotalSinceLastMinute = 0;
		this->readingCountSinceLastMinute = 0;
		this->lastMinuteMillis = this->currentMillis;
	}
}
void MoistureSensor::pushLastQuarterHourReadings() {
	double readingTotalSinceLastQuarterHour = 0;

	for (int i = 1; i <= 15; i++) {
		int minuteHistoryIndex = (this->HISTORY_SIZE + this->readingHistoryInMinutesIndex - i) % this->HISTORY_SIZE;
		readingTotalSinceLastQuarterHour += this->readingHistoryInMinutes[minuteHistoryIndex];
	}

	this->readingHistoryInQuarterHours[this->readingHistoryInQuarterHoursIndex] = readingTotalSinceLastQuarterHour / 15;
	this->readingHistoryInQuarterHoursIndex = (this->HISTORY_SIZE + this->readingHistoryInQuarterHoursIndex + 1) % this->HISTORY_SIZE;
	this->lastQuarterHourMillis = this->currentMillis;
}
void MoistureSensor::writeSerial() {
	Serial.println(
		" Goal reading: "+String(this->goalReading)
		+"Minutes index: "+String(this->readingHistoryInMinutesIndex)
		+" Quarter hours index: "+String(this->readingHistoryInQuarterHoursIndex)
		);

	printArray(this->readingHistoryInMinutes, MoistureSensor::HISTORY_SIZE);
	printArray(this->readingHistoryInQuarterHours, MoistureSensor::HISTORY_SIZE);
}

void MoistureSensor::printArray(short arr[], int size) {
	String out = "[ ";
	for (int i = 0; i < size; i++) {
		out += arr[i];
		if (i + 1 == size) {
			out += "]";
		} else {
			out += ", ";
			if ((i + 1) % 10 == 0) {
				out += '\r';
			}
		}
	}
	Serial.println(out);
}
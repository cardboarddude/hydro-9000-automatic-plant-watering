#include "HistoricalData.h"
#include "Arduino.h"
#include "EEPROM.h"
#include <vector>

HistoricalData::HistoricalData() {}
void HistoricalData::loadData(unsigned int eepromAddress) {
	this->eepromAddress = eepromAddress;
	unsigned int address = this->eepromAddress;
	EEPROM.get(address, this->readingHistoryInMinutesIndex);
	address += sizeof(this->readingHistoryInMinutesIndex);
	EEPROM.get(address, this->readingHistoryInQuarterHoursIndex);
	address += sizeof(this->readingHistoryInQuarterHoursIndex);
	EEPROM.get(address, this->readingHistoryInMinutes);
	address += sizeof(this->readingHistoryInMinutes);
	EEPROM.get(address, this->readingHistoryInQuarterHours);
}
void HistoricalData::saveData() {
	int address = this->eepromAddress;
	EEPROM.put(address, this->readingHistoryInMinutesIndex);
	address += sizeof(this->readingHistoryInMinutesIndex);
	EEPROM.put(address, this->readingHistoryInQuarterHoursIndex);
	address += sizeof(this->readingHistoryInQuarterHoursIndex);
	EEPROM.put(address, this->readingHistoryInMinutes);
	address += sizeof(this->readingHistoryInMinutes);
	EEPROM.put(address, this->readingHistoryInQuarterHours);
}
void HistoricalData::pushLastSecondReadings(short value) {
	this->readingHistoryInSeconds[this->readingHistoryInSecondsIndex] = value;
	this->readingHistoryInSecondsIndex = (HistoricalData::HISTORY_SIZE + this->readingHistoryInSecondsIndex + 1) % HistoricalData::HISTORY_SIZE;
}
void HistoricalData::pushLastMinuteReadings() {
	Serial.println("========== Saving last minute ==========");
	double readingTotalSinceLastMinute = 0;
	int i;

	for (i = 1; i <= 60 && i < HistoricalData::HISTORY_SIZE; i++) {
		int secondHistoryIndex = (HistoricalData::HISTORY_SIZE + this->readingHistoryInSecondsIndex - i) % HistoricalData::HISTORY_SIZE;
		readingTotalSinceLastMinute += this->readingHistoryInSeconds[secondHistoryIndex];
	}

	if (i-1 > 0) {
		this->readingHistoryInMinutes[this->readingHistoryInMinutesIndex] = readingTotalSinceLastMinute / (i-1);
		this->readingHistoryInMinutesIndex = (HistoricalData::HISTORY_SIZE + this->readingHistoryInMinutesIndex + 1) % HistoricalData::HISTORY_SIZE;
	}
}
void HistoricalData::pushLastQuarterHourReadings() {
	Serial.println("========== Saving last quarter hour ==========");
	double readingTotalSinceLastQuarterHour = 0;
	int i;

	for (i = 1; i <= 15 && i < HistoricalData::HISTORY_SIZE; i++) {
		int minuteHistoryIndex = (HistoricalData::HISTORY_SIZE + this->readingHistoryInMinutesIndex - i) % HistoricalData::HISTORY_SIZE;
		readingTotalSinceLastQuarterHour += this->readingHistoryInMinutes[minuteHistoryIndex];
	}

	if (i-1 > 0) {
		this->readingHistoryInQuarterHours[this->readingHistoryInQuarterHoursIndex] = readingTotalSinceLastQuarterHour / (i-1);
		this->readingHistoryInQuarterHoursIndex = (HistoricalData::HISTORY_SIZE + this->readingHistoryInQuarterHoursIndex + 1) % HistoricalData::HISTORY_SIZE;
	}
}
void HistoricalData::writeSerial() {
	Serial.println(
		" Seconds index: "+String(this->readingHistoryInSecondsIndex)
		+" Minutes index: "+String(this->readingHistoryInMinutesIndex)
		+" Quarter hours index: "+String(this->readingHistoryInQuarterHoursIndex)
		);

	printArray(this->readingHistoryInMinutes, HistoricalData::HISTORY_SIZE);
	printArray(this->readingHistoryInQuarterHours, HistoricalData::HISTORY_SIZE);
}
std::vector<unsigned short> HistoricalData::getDataInSeconds(unsigned int seconds) {
	std::vector<unsigned short> data;

	if (seconds > HistoricalData::HISTORY_SIZE) {
		seconds = HistoricalData::HISTORY_SIZE;
	}
	
	for (unsigned short i = 0; i < seconds; i++) {
		unsigned short index = (HistoricalData::HISTORY_SIZE + this->readingHistoryInSecondsIndex - i - 1) % HistoricalData::HISTORY_SIZE;
		data.push_back(this->readingHistoryInSeconds[index]);
	}

	return data;
}
std::vector<unsigned short> HistoricalData::getDataInMinutes(unsigned int minutes) {
	std::vector<unsigned short> data;

	if (minutes > HistoricalData::HISTORY_SIZE) {
		minutes = HistoricalData::HISTORY_SIZE;
	}

	for (unsigned short i = 0; i < minutes; i++) {
		unsigned short index = (HistoricalData::HISTORY_SIZE + this->readingHistoryInMinutesIndex - i) % HistoricalData::HISTORY_SIZE;
		data.push_back(this->readingHistoryInMinutes[index]);
	}

	return data;
}
std::vector<unsigned short> HistoricalData::getDataInQuarterHours(unsigned int quarterHours) {
	std::vector<unsigned short> data;
	
	if (quarterHours > HistoricalData::HISTORY_SIZE) {
		quarterHours = HistoricalData::HISTORY_SIZE;
	}

	for (unsigned short i = 0; i < quarterHours; i++) {
		unsigned short index = (HistoricalData::HISTORY_SIZE + this->readingHistoryInQuarterHoursIndex - i) % HistoricalData::HISTORY_SIZE;
		data.push_back(this->readingHistoryInQuarterHours[index]);
	}

	return data;
}
// double HistoricalData::getAveragePerMinute(unsigned int periodInMinutes) {
// 	if (HistoricalData::HISTORY_SIZE >= periodInMinutes) {
// 		return this->getAverageFromHistoryInMinutes(periodInMinutes);
// 	} else if (HistoricalData::HISTORY_SIZE * 15 >= periodInMinutes) {
// 		return this->getAverageFromHistoryInQuarterHours(periodInMinutes);
// 	} else {
// 		return -1;
// 	}
// }
double HistoricalData::getAverageFromHistoryInMinutes(unsigned int periodInMinutes) {
	unsigned int total = 0;

	for (unsigned short i = 0; i < HistoricalData::HISTORY_SIZE; i++) {
		unsigned short index = (HistoricalData::HISTORY_SIZE + this->readingHistoryInMinutesIndex - i) % HistoricalData::HISTORY_SIZE;
		total += this->readingHistoryInMinutes[index];
	}

	return total / periodInMinutes;
}
double HistoricalData::getAverageFromHistoryInQuarterHours(unsigned int periodInMinutes) {
	unsigned int total = 0;

	for (unsigned short i = 0; i < HistoricalData::HISTORY_SIZE; i++) {
		unsigned short index = (HistoricalData::HISTORY_SIZE + this->readingHistoryInQuarterHoursIndex - i) % HistoricalData::HISTORY_SIZE;
		total += this->readingHistoryInQuarterHours[index];
	}

	return total / periodInMinutes;
}
void HistoricalData::printArray(unsigned short arr[], int size) {
	String out = "[ ";
	for (int i = 0; i < size; i++) {
		out += String(arr[i]);
		if (i + 1 >= size) {
			out += " ]";
			Serial.println(out);
		} else {
			out += ", ";
			if ((i + 1) % 25 == 0) {
				Serial.println(out);
				out = "";
			}
		}
	}
}
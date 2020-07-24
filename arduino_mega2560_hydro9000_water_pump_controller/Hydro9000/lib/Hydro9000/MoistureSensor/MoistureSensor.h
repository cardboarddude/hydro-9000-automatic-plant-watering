
#ifndef MoistureSensor_h
#define MoistureSensor_h

#include "Arduino.h"

class MoistureSensor {
	public: 
		static unsigned long currentMillis;
		static const int HISTORY_SIZE = 100;
		int analogPin;
		int maxReading = 678, minReading = 240;
		float currentReading, goalReading;
		int eepromHistoryAddress;
		short readingHistoryInMinutes[HISTORY_SIZE];
		byte readingHistoryInMinutesIndex = 0;
		short readingHistoryInQuarterHours[HISTORY_SIZE];
		byte readingHistoryInQuarterHoursIndex = 0;
		double readingTotalSinceLastMinute = 0;
		short readingCountSinceLastMinute = 0;
		double lastMinuteMillis = 0, lastQuarterHourMillis = 0;
		
		static const int addressOffset = 
			sizeof(goalReading)
			+ sizeof(readingHistoryInMinutesIndex)
			+ sizeof(readingHistoryInQuarterHoursIndex)
			+ sizeof(readingHistoryInMinutes)
			+ sizeof(readingHistoryInQuarterHours);

		MoistureSensor();
		MoistureSensor(int analogPin, int eepromHistoryAddress);
		void setup();
		float getMoistureLevel();
		void read();
		void setGoal(float percentage);
		float getGoalPercentage();
		bool hasMetGoal();
		void loadData();
		void saveData();
		bool isNextMinute();
		bool isNextQuarterHour();
		void pushLastMinuteReadings();
		void pushLastQuarterHourReadings();
		void writeSerial();
		void printArray(short arr[], int size);
};

#endif
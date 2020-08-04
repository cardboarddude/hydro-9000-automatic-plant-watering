
#ifndef HistoricalData_h
#define HistoricalData_h

#include "Arduino.h"
#include <vector>

class HistoricalData {
	private:
		unsigned int eepromAddress;
		byte readingHistoryInSecondsIndex = 0;
		byte readingHistoryInMinutesIndex = 0;
		byte readingHistoryInQuarterHoursIndex = 0;

		double getAverageFromHistoryInMinutes(unsigned int periodInMinutes);
		double getAverageFromHistoryInQuarterHours(unsigned int periodInMinutes);

	public:		
		static const unsigned short PROGMEM HISTORY_SIZE = 25;
		static unsigned long currentMillis;

		unsigned short readingHistoryInSeconds[HISTORY_SIZE];
		unsigned short readingHistoryInMinutes[HISTORY_SIZE];
		unsigned short readingHistoryInQuarterHours[HISTORY_SIZE];

		static const int addressOffset = 
		      sizeof(readingHistoryInSecondsIndex)
		    + sizeof(readingHistoryInMinutesIndex)
			+ sizeof(readingHistoryInQuarterHoursIndex)
			+ sizeof(readingHistoryInSeconds)
			+ sizeof(readingHistoryInMinutes)
			+ sizeof(readingHistoryInQuarterHours);

		HistoricalData();
		void loadData(unsigned int eepromAddress);
		void saveData();
		void pushLastSecondReadings(short value);
		void pushLastMinuteReadings();
		void pushLastQuarterHourReadings();
		void writeSerial();
		void printArray(unsigned short arr[], int size);
		// double getAveragePerMinute(unsigned int periodInMinutes);
		std::vector<unsigned short> getDataInSeconds(unsigned int seconds);
		std::vector<unsigned short> getDataInMinutes(unsigned int minutes);
		std::vector<unsigned short> getDataInQuarterHours(unsigned int quarterHours);
};

#endif
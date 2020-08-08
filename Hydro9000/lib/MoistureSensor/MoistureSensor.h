
#ifndef MoistureSensor_h
#define MoistureSensor_h

#include "Arduino.h"

class MoistureSensor {
	private: 
		int analogPin;
		double currentReading, goalReading;
		unsigned int readingTotal = 0;
		unsigned int readingCount = 0;
		unsigned int eepromAddress;

	public:
		static const int addressOffset = 
		      sizeof(goalReading);
		int maxReading = 678, minReading = 240;

		MoistureSensor();
		MoistureSensor(int analogPin);

		void setup(int eepromAddress);
		void read();
		void resetReadings();

		void setGoal(double percentage);

		bool hasMetGoal();
		double readingToPercentage(double reading, bool doZeroToOne = true);
		double getMoistureLevel();
		double getGoalPercentage();
		double getAverageReading();
};

#endif
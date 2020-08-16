
#ifndef PlantController_h
#define PlantController_h

#include "Arduino.h"
#include "../WaterPump/WaterPump.h"
#include "../MoistureSensor/MoistureSensor.h"
#include "../HistoricalData/HistoricalData.h"
#include <vector>

class PlantController {
	public:
        static const int MINIMUM_PUMP_RUN_DURATION_MS PROGMEM = 20000;
        static const int WAIT_AFTER_GOAL_MET_MS PROGMEM = 5000;
        
        // In milliseconds
		enum TimePeriodInMS {
            SECOND = 1000,
            MINUTE = 60000,
            QUARTER_HOUR = 900000,
            HOUR = 3600000,
			THREE_HOURS = 10800000,
			DAY = 86400000
		};
		static unsigned long currentMillis;
        
        String name;
        String displayName;
        bool isActive = true;

        PlantController();
        PlantController(MoistureSensor& sensor, WaterPump& pump);
        void setup(unsigned int eepromAddress);
        void update();
        void stopPump();
        void startPump();
        void togglePump();
        double getGoal();
        double getCurrentPercentage();
        unsigned int getAddressOffset();
        std::vector<unsigned short> getHistory(unsigned long period);
        std::vector<double> getHistoryPercentage(unsigned long period);
        bool isNextSecond();
		bool isNextMinute();
		bool isNextQuarterHour();
		bool hasMetGoal();
		bool isRunning();
		bool hasPumpRunMinimumDuration();
		void setGoal(double percentage);
        void updateReadings();
        
    private:
        MoistureSensor sensor;
        WaterPump pump;
        HistoricalData history;
        bool doSaveData = false;
		unsigned int readingTotalSinceLastSecond = 0;
		unsigned int readingTotalSinceLastMinute = 0;
		unsigned short readingCountSinceLastMinute = 0;
		unsigned long lastSecondMillis = 0, lastMinuteMillis = 0, lastQuarterHourMillis = 0;
		unsigned long lastMetGoalAt = 0, startedPumpAt = 0;
        unsigned int addressOffset, eepromAddress;
};

#endif
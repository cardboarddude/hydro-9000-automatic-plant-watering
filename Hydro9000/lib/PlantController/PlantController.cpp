#include "Arduino.h"
#include "PlantController.h"
#include "../WaterPump/WaterPump.h"

PlantController::PlantController() {
}
PlantController::PlantController(MoistureSensor& sensor, WaterPump& pump) {
    this->sensor = sensor;
    this->pump = pump;
}
void PlantController::setup(unsigned int eepromAddress) {
    this->eepromAddress = eepromAddress;
    this->sensor.setup(this->eepromAddress);
    this->addressOffset = this->sensor.addressOffset;
    this->pump.setup();
    this->history.loadData(this->eepromAddress+this->sensor.addressOffset);
    this->addressOffset += this->history.addressOffset;
}
void PlantController::update() {
    this->updateReadings();
}
void PlantController::updateReadings() {
    this->sensor.read();

    if (this->isNextSecond()) {
        this->history.pushLastSecondReadings(this->sensor.getAverageReading());
        this->sensor.resetReadings();
        this->lastSecondMillis = PlantController::currentMillis;
        this->doSaveData = true;
    }
    if (this->isNextMinute()) {
        this->history.pushLastMinuteReadings();
        this->lastMinuteMillis = PlantController::currentMillis;
        this->doSaveData = true;
    }
    if (this->isNextQuarterHour()) {
        this->history.pushLastQuarterHourReadings();
        this->lastQuarterHourMillis = PlantController::currentMillis;
        this->doSaveData = true;
    }
    if (this->doSaveData) {
        this->history.saveData();
        this->doSaveData = false;
    }
}
bool PlantController::hasMetGoal() {
    bool hasMetGoal = this->sensor.hasMetGoal();

    if (hasMetGoal) {
        this->lastMetGoalAt = PlantController::currentMillis;
    } else {
        hasMetGoal = PlantController::currentMillis - this->lastMetGoalAt < PlantController::WAIT_AFTER_GOAL_MET_MS;
    }

    return hasMetGoal;
}
bool PlantController::hasPumpRunMinimumDuration() {
    return PlantController::MINIMUM_PUMP_RUN_DURATION_MS <= PlantController::currentMillis - this->startedPumpAt;
}
void PlantController::stopPump() {
    this->pump.stop();
}
void PlantController::togglePump() {
    this->pump.toggle();
}
void PlantController::startPump() {
    this->startedPumpAt = PlantController::currentMillis;
    this->pump.start();
}
double PlantController::getGoal() {
    return this->sensor.getGoalPercentage();
}
double PlantController::getCurrentPercentage() {
    return this->sensor.getMoistureLevel();
}
unsigned int PlantController::getAddressOffset() {
    return this->addressOffset;
}
std::vector<unsigned short> PlantController::getHistory(unsigned long milliseconds) {
    if (milliseconds <= HistoricalData::HISTORY_SIZE * TimePeriodInMS::SECOND) {
        return this->history.getDataInSeconds(milliseconds / TimePeriodInMS::SECOND);
    } else if (milliseconds <= HistoricalData::HISTORY_SIZE * TimePeriodInMS::MINUTE) {
        return this->history.getDataInMinutes(milliseconds / TimePeriodInMS::MINUTE);
    } else /*if (milliseconds < HistoricalData::HISTORY_SIZE * TimePeriodInMS::QUARTER_HOUR)*/ {
        return this->history.getDataInQuarterHours(milliseconds / TimePeriodInMS::QUARTER_HOUR);
    }
}
std::vector<double> PlantController::getHistoryPercentage(unsigned long period) {
    std::vector<unsigned short> readingHistory = this->getHistory(period);
    std::vector<double> percentageHistory;

    for (unsigned int i = 0; i < readingHistory.size(); i++) {
        percentageHistory.push_back(this->sensor.readingToPercentage(readingHistory.at(i)));
    }

    return percentageHistory;
}
bool PlantController::isNextSecond() {
	return this->lastMinuteMillis + TimePeriodInMS::SECOND <= PlantController::currentMillis;
}
bool PlantController::isNextMinute() {
	return this->lastMinuteMillis + TimePeriodInMS::MINUTE <= PlantController::currentMillis;
}
bool PlantController::isNextQuarterHour() {
	return this->lastQuarterHourMillis + TimePeriodInMS::QUARTER_HOUR <= PlantController::currentMillis;
}
bool PlantController::isRunning() {
    return this->pump.isRunning;
}
void PlantController::setGoal(double percentage) {
    Serial.println("Now saving for "+this->displayName+" ("+this->name+") = "+String(percentage));
    this->sensor.setGoal(percentage);
}
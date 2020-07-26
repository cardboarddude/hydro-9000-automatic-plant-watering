#include "Arduino.h"
#include "PlantController.h"
#include "../WaterPump/WaterPump.h"

PlantController::PlantController() {
}
PlantController::PlantController(MoistureSensor sensor, WaterPump pump) {
    this->sensor = sensor;
    this->pump = pump;
}
void PlantController::setup() {
    this->sensor.setup();
    this->pump.setup();
}
void PlantController::update() {
    this->sensor.read();

    if (this->sensor.isNextMinute()) {
        this->sensor.pushLastMinuteReadings();
        this->doSaveData = true;
    }
    if (this->sensor.isNextQuarterHour()) {
        this->sensor.pushLastQuarterHourReadings();
        this->doSaveData = true;
    }
}
void PlantController::stopPump() {
    this->pump.stop();
}
float PlantController::getGoal() {
    return this->sensor.getGoalPercentage();
}
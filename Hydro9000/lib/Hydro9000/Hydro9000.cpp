#include "Hydro9000.h"
#include "../Screen/Screen.h"
#include "../ControlPanel/ControlPanel.h"
#include "../PlantController/PlantController.h"
#include "../ScreenBarGraph/ScreenBarGraph.h"

Hydro9000::Hydro9000() {
}
void Hydro9000::addPlantController(PlantController& controller) {
    this->plantControllers[plantCount++] = controller;
}
void Hydro9000::addControlPanel(ControlPanel& controlPanel) {
    this->controlPanel = controlPanel;
}
void Hydro9000::setup() {
    PlantController::currentMillis = Hydro9000::currentMillis;
    unsigned int address = 0;
    Serial.println("Setting up PlantControllers");
    for (unsigned int i = 0; i < this->plantCount; i++) {
        if (i > 0) {
            address += this->plantControllers[i-1].getAddressOffset();
        }
        this->plantControllers[i].setup(address);
    }
    Serial.println("Setting up ControlPanel");
    this->controlPanel.setup();
}
void Hydro9000::update() {
    Hydro9000::setCurrentMillis(millis());
    Serial.println("Updating@"+String(Hydro9000::currentMillis));

    this->updatePlantControllers();
    this->controlPanel.update(this->getData());

    if (this->controlPanel.isEmergency()) {
        this->doEmergencyStop();
    }
}
void Hydro9000::updatePlantControllers() {
    for (unsigned int i = 0; i < this->plantCount; i++) {
        this->plantControllers[i].update();
    }
}
std::vector<double> Hydro9000::getData() {
    std::vector<double> data;

    switch (this->controlPanel.activeScreenName) {
    case ControlPanel::ScreenName::CURRENT_MOISTURE_LEVELS:
        for (unsigned int i = 0; i < this->plantCount; i++) {
            data.push_back(this->plantControllers[i].getCurrentPercentage());
        }
        break;
    case ControlPanel::ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS:
        data = this->plantControllers[0].getHistoryPercentage(PlantController::TimePeriodInMS::SECOND*50);
        break;
    case ControlPanel::ScreenName::MOISTURE_LEVEL_HISTORY_MINUTES:
        data = this->plantControllers[0].getHistoryPercentage(PlantController::TimePeriodInMS::MINUTE*50);
        break;
    case ControlPanel::ScreenName::MOISTURE_LEVEL_HISTORY_QUARTER_HOURS:
        data = this->plantControllers[0].getHistoryPercentage(PlantController::TimePeriodInMS::QUARTER_HOUR*50);
        break;
    }

    return data;
}
void Hydro9000::doEmergencyStop() {
    for (unsigned int i = 0; i < this->plantCount; i++) {
        this->plantControllers[i].stopPump();
    }
}
std::vector<double> Hydro9000::getGoals() {
    std::vector<double> goals;

    for (unsigned int i = 0; i < this->plantCount; i++) {
        goals.push_back(this->plantControllers[i].getGoal());
    }

    return goals;
}
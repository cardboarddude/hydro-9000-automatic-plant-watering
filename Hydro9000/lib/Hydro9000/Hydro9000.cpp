#include "Hydro9000.h"
#include "../Screen/Screen.h"
#include "../ControlPanel/ControlPanel.h"
#include "../PlantController/PlantController.h"
#include "../ScreenBarGraph/ScreenBarGraph.h"
#include <vector>

Hydro9000::Hydro9000() {
}
void Hydro9000::addPlantController(PlantController controller) {
    this->plantControllers[plantCount++] = controller;
}
void Hydro9000::addControlPanel(ControlPanel& controlPanel) {
    this->controlPanel = controlPanel;
}
void Hydro9000::setup() {
    PlantController::currentMillis = Hydro9000::currentMillis;
    unsigned int address = 0;
    std::vector<String> plantNames;
    Serial.println("Setting up PlantControllers");
    for (unsigned char i = 0; i < this->plantCount; i++) {
        if (i > 0) {
            address += this->plantControllers[i-1].getAddressOffset();
        }
        this->plantControllers[i].setup(address);
        plantNames.push_back(this->plantControllers[i].displayName);
    }
    Serial.println("Setting up ControlPanel");
    this->controlPanel.setup(plantNames);
}
void Hydro9000::update() {
    std::vector<double> controlPanelUpdateResult;

    Hydro9000::setCurrentMillis(millis());
    // Serial.println("Updating@"+String(Hydro9000::currentMillis));

    this->updatePlantControllers();
    std::vector<double> data = this->getData();
    controlPanelUpdateResult = this->controlPanel.update(data, this->isPumpRunning());

    if (this->controlPanel.activeScreenName == ControlPanel::ScreenName::PUMP_CONTROL) {
        this->updatePumps(controlPanelUpdateResult);
    }

    if (this->controlPanel.isEmergency()) {
        this->doEmergencyStop();
    }
}
void Hydro9000::updatePlantControllers() {
    for (unsigned int i = 0; i < this->plantCount; i++) {
        this->plantControllers[i].update();
    }
}
void Hydro9000::updatePumps(std::vector<double> pumpStatus) {
    for (unsigned int i = 0; i < this->plantCount && i < pumpStatus.size(); i++) {
        if (pumpStatus.at(i) >= 1) {
            Serial.println(" ========= STARTING PUMP ========= "+String(i)+" ========= ");
            this->plantControllers[i].togglePump();
        } else {
            Serial.println("STOPPING PUMP "+String(i));
            this->plantControllers[i].stopPump();
        }
    }
}
std::vector<double> Hydro9000::getData() {
    std::vector<double> data;

    if (this->controlPanel.activeScreenName == ControlPanel::ScreenName::CURRENT_MOISTURE_LEVELS) {
        for (unsigned int i = 0; i < this->plantCount; i++) {
            data.push_back(this->plantControllers[i].getCurrentPercentage());
        }
    } else if (this->controlPanel.activeScreenName == ControlPanel::ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS) {
        data = this->plantControllers[0].getHistoryPercentage(PlantController::TimePeriodInMS::SECOND*HistoricalData::HISTORY_SIZE);
    } else if (this->controlPanel.activeScreenName ==  ControlPanel::ScreenName::MOISTURE_LEVEL_HISTORY_MINUTES) {
        data = this->plantControllers[0].getHistoryPercentage(PlantController::TimePeriodInMS::MINUTE*HistoricalData::HISTORY_SIZE);
    } else if (this->controlPanel.activeScreenName == ControlPanel::ScreenName::MOISTURE_LEVEL_HISTORY_QUARTER_HOURS) {
        data = this->plantControllers[0].getHistoryPercentage(PlantController::TimePeriodInMS::QUARTER_HOUR*HistoricalData::HISTORY_SIZE);
    } else if (this->controlPanel.activeScreenName ==  ControlPanel::ScreenName::PUMP_CONTROL) {
        for (unsigned int i = 0; i < this->plantCount; i++) {
            data.push_back(this->plantControllers[i].isRunning());
        }
    }

    return data;
}
void Hydro9000::doEmergencyStop() {
    for (unsigned int i = 0; i < this->plantCount; i++) {
        this->plantControllers[i].stopPump();
    }
}
bool Hydro9000::isPumpRunning() {
    for (unsigned int i = 0; i < this->plantCount; i++) {
        if (this->plantControllers[i].isRunning()) {
            return true;
        }
    }

    return false;
}
std::vector<double> Hydro9000::getGoals() {
    std::vector<double> goals;

    for (unsigned int i = 0; i < this->plantCount; i++) {
        goals.push_back(this->plantControllers[i].getGoal());
    }

    return goals;
}
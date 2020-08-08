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
    String plantNames[Hydro9000::MAX_COUNT];
    unsigned char i;
    Serial.println("Setting up PlantControllers");
    for (i = 0; i < this->plantCount; i++) {
        if (i > 0) {
            address += this->plantControllers[i-1].getAddressOffset();
        }
        this->plantControllers[i].setup(address);
        plantNames[i] = this->plantControllers[i].displayName;
    }
    Serial.println("Setting up ControlPanel");
    this->controlPanel.setup(plantNames, i);
}
void Hydro9000::update() {
    std::vector<double> controlPanelUpdateResult;

    Hydro9000::setCurrentMillis(millis());
    unsigned char previousScreenId = static_cast<unsigned char>(this->controlPanel.activeScreenName);
    // Serial.println("Updating@"+String(Hydro9000::currentMillis));

    this->updatePlantControllers();
    std::vector<double> data = this->getData();
    controlPanelUpdateResult = this->controlPanel.update(data, this->isPumpRunning());

    if (this->controlPanel.isDisplayActive() && this->controlPanel.activeScreenName == ControlPanel::ScreenName::PUMP_CONTROL) {
        this->isManualWateringActive = true;
        this->updatePumps(controlPanelUpdateResult);
    } else {
        this->isManualWateringActive = false;
    }
    // Serial.println("Screen Id: "+String(previousScreenId));
    if (controlPanelUpdateResult.size() > 0 && previousScreenId == ControlPanel::ScreenName::GOAL_PLANT_SELECTION) {
        // Serial.println("Selected plant: "+String(controlPanelUpdateResult.at(0)));
        this->selectedPlantId = controlPanelUpdateResult.at(0);
    } else if (controlPanelUpdateResult.size() > 0 && this->controlPanel.activeScreenName == ControlPanel::ScreenName::SET_GOALS) {
        this->saveGoal(controlPanelUpdateResult.at(0));
    }

    if (this->controlPanel.isEmergency()) {
        this->doEmergencyStop();
    }
}
bool Hydro9000::isAutoWateringActive() {
    return !(this->isManualWateringActive && this->controlPanel.isDisplayActive());
}
void Hydro9000::saveGoal(double percentage) {
    // Serial.println("Saving goal of "+String(percentage)+" for index "+String(this->selectedPlantId));
    this->plantControllers[this->selectedPlantId].setGoal(percentage);
}
void Hydro9000::updatePlantControllers() {
    for (unsigned int i = 0; i < this->plantCount; i++) {
        this->plantControllers[i].update();

        if (this->isAutoWateringActive()) {
            if (this->plantControllers[i].isActive && !this->plantControllers[i].hasMetGoal()) {
                if (!this->isPumpRunning()) {
                    Serial.println("Watering plant "+this->plantControllers[i].displayName+"  Current: "+String(this->plantControllers[i].getCurrentPercentage())+" Goal: "+String(this->plantControllers[i].getGoal()));
                    this->plantControllers[i].startPump();
                }
            } else {
                this->plantControllers[i].stopPump();
            }
        }
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
    } else if (this->controlPanel.activeScreenName ==  ControlPanel::ScreenName::SET_GOALS) {
        data.push_back(this->plantControllers[this->selectedPlantId].getGoal());
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
#include "Hydro9000.h"
#include "../Screen/Screen.h"
#include "../ControlPanel/ControlPanel.h"
#include "../PlantController/PlantController.h"
#include "../ScreenBarGraph/ScreenBarGraph.h"

Hydro9000::Hydro9000() {
}
void Hydro9000::addPlantController(PlantController& controller) {
    this->plantControllers.push_back(controller);
}
void Hydro9000::addControlPanel(ControlPanel& controlPanel) {
    this->controlPanel = controlPanel;
}
void Hydro9000::setup() {
    MoistureSensor::currentMillis = Hydro9000::currentMillis;
    for (unsigned int i = 0; i < this->plantControllers.size(); i++) {
        this->plantControllers.at(i).setup();
    }
    this->controlPanel.setup();
}
void Hydro9000::update() {
    Hydro9000::setCurrentMillis(millis());
    std::vector<double> data;
    for (unsigned int i = 0; i < this->plantControllers.size(); i++) {
        this->plantControllers.at(i).update();
        Serial.print(this->plantControllers.at(i).getCurrentPercentage());Serial.print(" ");
        data.push_back(this->plantControllers.at(i).getCurrentPercentage());
    }
    this->controlPanel.update(data);

    if (this->controlPanel.isEmergency()) {
        this->doEmergencyStop();
    }
}
void Hydro9000::doEmergencyStop() {
    for (unsigned int i = 0; i < this->plantControllers.size(); i++) {
        this->plantControllers.at(i).stopPump();
    }
}
std::vector<float> Hydro9000::getGoals() {
    std::vector<float> goals;

    for (unsigned int i = 0; i < this->plantControllers.size(); i++) {
        goals.push_back(this->plantControllers.at(i).getGoal());
    }

    return goals;
}
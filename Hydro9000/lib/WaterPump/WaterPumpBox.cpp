#include "Arduino.h"
#include "WaterPumpBox.h"


WaterPumpBox::WaterPumpBox() {

}
WaterPumpBox::WaterPumpBox(std::vector<WaterPump> pumps) {
    this->pumps = pumps;
}
WaterPumpBox::WaterPumpBox(WaterPump pumps[], int size) {
    for (int i = 0; i < size; i++) {
        this->pumps.push_back(pumps[i]);
    }
}
void WaterPumpBox::addPump(WaterPump pump) {
    this->pumps.push_back(pump);
}
void WaterPumpBox::setup() {
    for (int i = 0; i < this->pumps.size(); i++) {
        this->pumps.at(i).setup();
    }
}
void WaterPumpBox::startPump(int pumpIndex) {
    if (pumpIndex < this->pumps.size()) {
        this->pumps.at(pumpIndex).start();
    }
}
void WaterPumpBox::stopPump(int pumpIndex) {
    if (pumpIndex < this->pumps.size()) {
        this->pumps.at(pumpIndex).stop();
    }
}
void WaterPumpBox::togglePump(int pumpIndex) {
    if (pumpIndex < this->pumps.size()) {
        this->pumps.at(pumpIndex).toggle();
    }
}
void WaterPumpBox::stopAllPumpsExcept(int pumpIndex) {
    for (int i = 0; i < this->pumps.size(); i++) {
        if (i != pumpIndex) {
            this->pumps.at(i).stop();
        }
    }
}
bool WaterPumpBox::isPumpRunning() {
    for (int i = 0; i < this->pumps.size(); i++) {
        if (this->pumps.at(i).isRunning) {
            return true;
        }
    }

    return false;
}
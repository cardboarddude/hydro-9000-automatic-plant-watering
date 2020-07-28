#include "Arduino.h"
#include "ControlPanel.h"
#include "../SelectWheel/SelectWheel.h"
#include "../SelectMenu/SelectMenu.h"
#include "../Button/Button.h"
#include "../Screen/Screen.h"
#include "../ScreenBarGraph/ScreenBarGraph.h"
#include "../ScreenImage/ScreenImage.h"
#include "../Hydro9000/Assets.h"
#include <Map>
#include <typeinfo>

ControlPanel::ControlPanel() {
}
void ControlPanel::addSelectWheel(SelectWheel& selectWheel) {
    this->selectWheel = selectWheel;
}
void ControlPanel::setup() {
    Screen::setup();
    this->setupScreens();

    if (this->screens.size() > 0) {
        this->activeScreenName = ScreenName::HOME;
    }
}
void ControlPanel::setupScreens() {
    this->screens[ScreenName::HOME] = new ScreenImage(String("HYDRO 9000"), LEAF_LOGO_BMP, 33, 44);
    this->screens[ScreenName::CURRENT_MOISTURE_LEVELS] = new ScreenBarGraph(String("WATER LVLS"));
}
void ControlPanel::addSelectMenu(SelectMenu& selectMenu) {
    this->menu = selectMenu;
}
void ControlPanel::addButton(ButtonName buttonName, Button& button) {
    this->buttons[buttonName] = button;
}
bool ControlPanel::canShowDisplay() {
    return this->buttons[ButtonName::KEY_SWITCH].isPressed;
}
void ControlPanel::addScreen(ScreenName sType, Screen& screen) {
    this->screens[sType] = &screen;
}
void ControlPanel::turnOffDisplay() {

}
void ControlPanel::turnOffLeds() {
    for (auto itr = this->buttons.begin(); itr != this->buttons.end(); itr++) {
        if (itr->second.hasLed) {
            itr->second.turnLedOff();
        }
    }
}
void ControlPanel::update(std::vector<double>& data) {
    this->updateButtons();
    bool isOn = this->buttons[ButtonName::KEY_SWITCH].isPressed;
    bool wasJustSwitchedOn = this->buttons[ButtonName::KEY_SWITCH].isClicked();
    
    if (!isOn) {
        Screen::turnOffDisplay();
        this->activeScreenName = ScreenName::HOME;
    } else {
        if (wasJustSwitchedOn) {
            Screen::setup();
        } else if (this->wasAnyButtonClicked()) {
            this->doNavigation();
        }

        Screen* activeScreen = this->screens[this->activeScreenName];
        activeScreen->clear();

        if (this->activeScreenName == ScreenName::CURRENT_MOISTURE_LEVELS) {
            for (unsigned int i = 0; i < data.size(); i++) {
                (static_cast<ScreenBarGraph*>(this->screens[ScreenName::CURRENT_MOISTURE_LEVELS]))->addBar(data.at(i), String(int(data.at(i)*100))+"%");
            }
        }
        
        activeScreen->doDisplay();
    }

}
void ControlPanel::doNavigation() {
    switch (this->activeScreenName) {
    case ScreenName::HOME:
        this->activeScreenName = ScreenName::CURRENT_MOISTURE_LEVELS;
        break;
    case ScreenName::CURRENT_MOISTURE_LEVELS:
        this->activeScreenName = ScreenName::HOME;
        break;
    default:
        Serial.println("Unknown screen selected: "+String(this->activeScreenName));
        break;
    }
}
void ControlPanel::updateButtons() {
    for (auto itr = this->buttons.begin(); itr != this->buttons.end(); itr++) {
        itr->second.update();
    }
}
bool ControlPanel::wasAnythingClicked() {
    for (auto itr = this->buttons.begin(); itr != this->buttons.end(); itr++) {
        if (itr->second.isClicked()) {
            return true;
        }
    }

    return false;
}
bool ControlPanel::wasAnyButtonClicked() {
    return 
        this->buttons[ButtonName::BLUE].isClicked() ||
        this->buttons[ButtonName::RED].isClicked() ||
        this->buttons[ButtonName::SELECT_WHEEL].isClicked();
}
bool ControlPanel::isEmergency() {
    return this->buttons[ButtonName::EMERGENCY_STOP].isPressed;
}
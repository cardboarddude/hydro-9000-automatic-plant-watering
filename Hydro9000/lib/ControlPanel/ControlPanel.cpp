#include "Arduino.h"
#include "ControlPanel.h"
#include "../SelectWheel/SelectWheel.h"
#include "../ScreenMenu/ScreenMenu.h"
#include "../Button/Button.h"
#include "../Screen/Screen.h"
#include "../ScreenLineGraph/ScreenLineGraph.h"
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
    
    this->screens.insert({ControlPanel::ScreenName::HOME, new ScreenImage(String("HYDRO 9000"), LEAF_LOGO_BMP, 33, 44)});
    
    this->screens.insert({ControlPanel::ScreenName::CURRENT_MOISTURE_LEVELS, new ScreenBarGraph(String("WATER LVLS"))});
    
    this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS] = new ScreenLineGraph(
        String("HISTORY"), String("Seconds"), String("Moist.")
        );
    
    // this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_MINUTES] = ScreenLineGraph(
    //     String("HISTORY"), String("Minutes"), String("Moist.")
    //     );
    // this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_QUARTER_HOURS] = ScreenLineGraph(
    //     String("HISTORY"), String("Quarter Hours"), String("Moist.")
    //     );
    Serial.println("Setting up ScreenMenu");
    std::vector<String> plantNames = {"Thyme", "Rosemerry"};
    
    Serial.println("Setting up ScreenMenu2");
    this->screens[ScreenName::MAIN_MENU] = new ScreenMenu(
        String("MENU"), plantNames
        );
    
    Serial.println("Adding to ScreenMenu");
    Screen* currentScreen = this->screens[ScreenName::MAIN_MENU];
    (static_cast<ScreenMenu*>(currentScreen))->addItem(String("Basil"));
    
    (static_cast<ScreenMenu*>(currentScreen))->addItem(String("Cilantro"));
    
    Serial.println("Done adding to ScreenMenu");
}
void ControlPanel::addScreenMenu(ScreenMenu menu) {
    this->menu = menu;
}
void ControlPanel::addButton(ButtonName buttonName, Button& button) {
    this->buttons[buttonName] = button;
}
bool ControlPanel::canShowDisplay() {
    return this->buttons[ButtonName::KEY_SWITCH].isPressed;
}
void ControlPanel::addScreen(ScreenName sType, Screen screen) {
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
void ControlPanel::update(std::vector<double> data) {
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
                (static_cast<ScreenBarGraph*>(activeScreen))->addBar(data.at(i), String(int(data.at(i)*100))+"%");
            }
        } else if (this->activeScreenName == ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS
            || this->activeScreenName == ScreenName::MOISTURE_LEVEL_HISTORY_MINUTES
            || this->activeScreenName == ScreenName::MOISTURE_LEVEL_HISTORY_QUARTER_HOURS) {
            for (unsigned int i = 0; i < data.size(); i++) {
                (static_cast<ScreenLineGraph*>(activeScreen))->addPoint(data.at(i));
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
        this->activeScreenName = ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS;
        break;
    case ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS:
        this->activeScreenName = ScreenName::MAIN_MENU;
        break;
    case ScreenName::MAIN_MENU:
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
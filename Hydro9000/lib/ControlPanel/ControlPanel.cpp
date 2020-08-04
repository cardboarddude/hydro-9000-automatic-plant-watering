#include "Arduino.h"
#include "ControlPanel.h"
#include "../SelectWheel/selectWheel.h"
#include "../ScreenMenu/ScreenMenu.h"
#include "../Button/Button.h"
#include "../Screen/Screen.h"
#include "../ScreenLineGraph/ScreenLineGraph.h"
#include "../ScreenBarGraph/ScreenBarGraph.h"
#include "../ScreenImage/ScreenImage.h"
#include "../ScreenPump/ScreenPump.h"
#include "../Hydro9000/Assets.h"
#include <Map>
#include <typeinfo>

ControlPanel::ControlPanel() {
}
void ControlPanel::addSelectWheel(SelectWheel& selectWheel) {
    this->selectWheel = &selectWheel;
}
void ControlPanel::setup(std::vector<String> plantNames) {
    Screen::setup();
    this->setupScreens(plantNames);

    if (this->screens.size() > 0) {
        this->activeScreenName = ScreenName::HOME;
    }
}
void ControlPanel::setupScreens(std::vector<String> plantNames) {
    this->screens.insert({
        ControlPanel::ScreenName::HOME, 
        new ScreenImage(static_cast<unsigned char>(ControlPanel::ScreenName::HOME), String("HYDRO 9000"), LEAF_LOGO_BMP, 33, 44)
    });
    this->screens.insert({
        ControlPanel::ScreenName::CURRENT_MOISTURE_LEVELS, 
        new ScreenBarGraph(static_cast<unsigned char>(ControlPanel::ScreenName::CURRENT_MOISTURE_LEVELS), String("WATER LVLS"))
    });
    this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS] = new ScreenLineGraph(
        static_cast<unsigned char>(ControlPanel::ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS), 
        String("HISTORY"), String("Seconds"), String("Moist.")
    );
    
    this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_MINUTES] = new ScreenLineGraph(
        static_cast<unsigned char>(ControlPanel::ScreenName::MOISTURE_LEVEL_HISTORY_MINUTES), 
        String("HISTORY"), String("Minutes"), String("Moist.")
    );
    this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_QUARTER_HOURS] = new ScreenLineGraph(
        static_cast<unsigned char>(ControlPanel::ScreenName::MOISTURE_LEVEL_HISTORY_QUARTER_HOURS), 
        String("HISTORY"), String("Quarter Hours"), String("Moist.")
        );
        {
            ScreenPump* pumpScreen = new ScreenPump(
                static_cast<unsigned char>(ControlPanel::ScreenName::PUMP_CONTROL), 
                String("RUN PUMP"), plantNames
                );
            pumpScreen->addPump("Go Back");
            this->screens[ScreenName::PUMP_CONTROL] = pumpScreen;
        }
    Serial.println("Setting up ScreenMenu");
    std::vector<String> mainMenuItems = {"Current Water Level", "Run Pumps", "View History", "Set Goals"/*, "Calibrate Sensors"*/};
    std::vector<String> historyMenuItems = {"Seconds", "Minutes", "QuarterHours", "Go Back"};
    ScreenMenu* mainMenu = new ScreenMenu(
        static_cast<unsigned char>(ScreenName::MAIN_MENU), String("MENU"), mainMenuItems
    );
    ScreenMenu* historyMenu = new ScreenMenu(
        static_cast<unsigned char>(ScreenName::HISTORY_MENU), ("HISTORY"), historyMenuItems
    );
    historyMenu->addItem(*this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS]);
    historyMenu->addItem(*this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_MINUTES]);
    historyMenu->addItem(*this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_QUARTER_HOURS]);
    historyMenu->addItem(*mainMenu);
    mainMenu->addItem(*this->screens[ScreenName::CURRENT_MOISTURE_LEVELS]);
    mainMenu->addItem(*this->screens[ScreenName::PUMP_CONTROL]);
    mainMenu->addItem(*historyMenu);
    mainMenu->addItem(*historyMenu);
    this->screens[ScreenName::MAIN_MENU] = mainMenu;
    this->screens[ScreenName::HISTORY_MENU] = historyMenu;
    Serial.println("Done adding to ScreenMenu");
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
std::vector<double> ControlPanel::update(std::vector<double> data) {
    std::vector<double> result;

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
            result = this->doAction();
        }

        this->doScreenDisplay(data);
    }

    return result;
}
void ControlPanel::doScreenDisplay(std::vector<double> data) {
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
    } else if (this->activeScreenName == ScreenName::MAIN_MENU
        || this->activeScreenName == ScreenName::HISTORY_MENU) {
        // Serial.println("CC Turns: "+String(this->selectWheel->getClockwiseTurns()));
        // Serial.print(String(digitalRead(this->selectWheel->pinA))+"|"+String(digitalRead(this->selectWheel->pinB))+"|");
        // Serial.print(String(this->selectWheel->isPinAHigh)+"|"+String(this->selectWheel->isPinBHigh)+"|"+String(this->selectWheel->hasChanged)+"|");
        (static_cast<ScreenMenu*>(activeScreen))->selectItem(this->selectWheel->getClockwiseTurns() / this->selectWheel->incrementSize);
    } else if (this->activeScreenName == ScreenName::PUMP_CONTROL) {
        (static_cast<ScreenPump*>(activeScreen))->selectPump(this->selectWheel->getClockwiseTurns() / this->selectWheel->incrementSize);
    }
    
    activeScreen->doDisplay();
}
void ControlPanel::goToSelectedScreen() {
    ScreenMenu* activeMenu = (static_cast<ScreenMenu*>(this->screens[this->activeScreenName]));
    
    this->activeScreenName = static_cast<ScreenName>(activeMenu->getSelectedItem().id);
}
std::vector<double> ControlPanel::doAction() {
    std::vector<double> actionResult;

    if (this->activeScreenName == ScreenName::HOME) {
        this->activeScreenName = ScreenName::MAIN_MENU;
    } else if (this->activeScreenName == ScreenName::CURRENT_MOISTURE_LEVELS) {
        this->activeScreenName = ScreenName::MAIN_MENU;
    } else if (this->activeScreenName == ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS
        || this->activeScreenName == ScreenName::MOISTURE_LEVEL_HISTORY_MINUTES
        || this->activeScreenName == ScreenName::MOISTURE_LEVEL_HISTORY_QUARTER_HOURS) {

        this->activeScreenName = ScreenName::HISTORY_MENU;
    } else if (this->activeScreenName == ScreenName::MAIN_MENU
        || this->activeScreenName == ScreenName::HISTORY_MENU) {

        Screen* activeScreen = this->screens[this->activeScreenName];
        ScreenMenu* menu = (static_cast<ScreenMenu*>(activeScreen));
        // Serial.print("Selected '"+menu->getSelectedItemDisplayName()+"'. ");
        // Serial.print("Selected '"+String(menu->getSelectedItem().id)+"'. ");
        this->activeScreenName = static_cast<ScreenName>(menu->getSelectedItem().id);
        this->selectWheel->resetTurns();
    
    } else if (this->activeScreenName == ScreenName::PUMP_CONTROL) {
        ScreenPump* pumpScreen = (static_cast<ScreenPump*>(this->screens[this->activeScreenName]));
        Serial.println(" =============== actionResult =============== ");
        for (unsigned char i = 0; i < pumpScreen->pumpCount; i++) {
            bool isSelected = pumpScreen->selectedPumpIndex == i;
            actionResult.push_back(isSelected);
            Serial.print("|");
            Serial.print(isSelected);
            Serial.print("|");
        }
        if (pumpScreen->getSelectedPumpDisplayName() == "Go Back") {
            this->activeScreenName = ScreenName::MAIN_MENU;
        }
    } else {
        Serial.println("Unknown screen selected: "+String(this->activeScreenName));
    }

    return actionResult;
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
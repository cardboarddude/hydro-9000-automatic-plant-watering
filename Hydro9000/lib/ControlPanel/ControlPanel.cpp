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
void ControlPanel::setup(String plantNames[], unsigned char plantNameCount) {
    Screen::setup();
    this->setupScreens(plantNames, plantNameCount);

    if (this->screens.size() > 0) {
        this->activeScreenName = ScreenName::HOME;
    }
}
void ControlPanel::setupScreens(String plantNames[], unsigned char plantNameCount) {
    this->screens.insert({
        ControlPanel::ScreenName::HOME, 
        new ScreenImage(static_cast<unsigned char>(ControlPanel::ScreenName::HOME), "HYDRO 9000", LEAF_LOGO_BMP, 33, 44)
    });
    this->screens.insert({
        ControlPanel::ScreenName::CURRENT_MOISTURE_LEVELS, 
        new ScreenBarGraph(static_cast<unsigned char>(ControlPanel::ScreenName::CURRENT_MOISTURE_LEVELS), "WATER LVLS")
    });
    this->screens.insert({
        ControlPanel::ScreenName::SET_GOALS, 
        new ScreenBarGraph(static_cast<unsigned char>(ControlPanel::ScreenName::SET_GOALS), "SET GOAL")
    });
    this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS] = new ScreenLineGraph(
        static_cast<unsigned char>(ControlPanel::ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS), 
        "HISTORY", "Seconds", "Moist."
    );
    
    this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_MINUTES] = new ScreenLineGraph(
        static_cast<unsigned char>(ControlPanel::ScreenName::MOISTURE_LEVEL_HISTORY_MINUTES), 
        "HISTORY", "Minutes", "Moist."
    );
    this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_QUARTER_HOURS] = new ScreenLineGraph(
        static_cast<unsigned char>(ControlPanel::ScreenName::MOISTURE_LEVEL_HISTORY_QUARTER_HOURS), 
        "HISTORY", "Quarter Hours", "Moist."
        );
    {
        ScreenPump* pumpScreen = new ScreenPump(
            static_cast<unsigned char>(ControlPanel::ScreenName::PUMP_CONTROL), 
            "RUN PUMP", plantNames, plantNameCount
            );
        pumpScreen->addPump("Go Back");
        this->screens[ScreenName::PUMP_CONTROL] = pumpScreen;
    }
    Serial.println("Setting up ScreenMenu");
    ScreenMenu* mainMenu = new ScreenMenu(
        static_cast<unsigned char>(ScreenName::MAIN_MENU), "MENU"
    );
    ScreenMenu* historyMenu = new ScreenMenu(
        static_cast<unsigned char>(ScreenName::HISTORY_MENU), "HISTORY"
    );
    ScreenMenu* goalSetting = new ScreenMenu(
        static_cast<unsigned char>(ScreenName::GOAL_PLANT_SELECTION), "GOALS", plantNames, plantNameCount
    );
    for (unsigned char i = 0; i < plantNameCount; i++) {
        goalSetting->addItem(*this->screens[ScreenName::SET_GOALS]);
    }

    historyMenu->addItem("Seconds", *this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS]);
    historyMenu->addItem("Minutes", *this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_MINUTES]);
    historyMenu->addItem("QuarterHours", *this->screens[ScreenName::MOISTURE_LEVEL_HISTORY_QUARTER_HOURS]);
    historyMenu->addItem("Go Back", *mainMenu);
    mainMenu->addItem("Current Water Level", *this->screens[ScreenName::CURRENT_MOISTURE_LEVELS]);
    mainMenu->addItem("Run Pumps", *this->screens[ScreenName::PUMP_CONTROL]);
    mainMenu->addItem("View History", *historyMenu);
    mainMenu->addItem("Set Goals", *goalSetting);

    this->screens[ScreenName::GOAL_PLANT_SELECTION] = goalSetting;
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
bool ControlPanel::isDisplayActive() {
    return this->buttons[ButtonName::KEY_SWITCH].isPressed;
}
std::vector<double> ControlPanel::update(std::vector<double> data, bool isPumpRunning) {
    std::vector<double> result;

    this->updateButtons();
    bool wasJustSwitchedOn = this->buttons[ButtonName::KEY_SWITCH].isClicked();
    
    if (!this->isDisplayActive()) {
        Screen::turnOffDisplay();
        this->activeScreenName = ScreenName::HOME;
        this->buttons[ControlPanel::ButtonName::RED].setLedState(Button::LedState::OFF);
        this->buttons[ControlPanel::ButtonName::BLUE].setLedState(Button::LedState::OFF);
    } else {
        if (wasJustSwitchedOn) {
            Screen::setup();
        } else if (this->wasAnyButtonClicked()) {
            result = this->doNavigation();
        }

        this->buttons[ControlPanel::ButtonName::RED].setLedState(Button::LedState::ON);

        if (isPumpRunning) {
            this->buttons[ControlPanel::ButtonName::BLUE].setLedState(Button::LedState::BLINKING);
        } else {
            this->buttons[ControlPanel::ButtonName::BLUE].setLedState(Button::LedState::ON);
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
        (static_cast<ScreenLineGraph*>(activeScreen))->reserveMemory(HistoricalData::HISTORY_SIZE);
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
    } else if (this->activeScreenName == ScreenName::SET_GOALS) {
        int turns = this->selectWheel->getCounterClockwiseTurns() / this->selectWheel->incrementSize;
        if (turns > 100) {
            turns = 100;
        } else if (turns < 0) {
            turns = 0;
        }
        (static_cast<ScreenBarGraph*>(activeScreen))->addBar(turns / 100.0, "Goal "+String(turns)+"%");
        if (data.size() > 0) {
            (static_cast<ScreenBarGraph*>(activeScreen))->addBar(data.at(0), "Curr. "+String(int(data.at(0)*100))+"%");
        } else {
            (static_cast<ScreenBarGraph*>(activeScreen))->addBar(0, "No Goal Set");
        }
    } else if (this->activeScreenName == ScreenName::GOAL_PLANT_SELECTION) {
        (static_cast<ScreenMenu*>(activeScreen))->selectItem(this->selectWheel->getClockwiseTurns() / this->selectWheel->incrementSize);
    }
    
    activeScreen->doDisplay();
}
void ControlPanel::goToSelectedScreen() {
    ScreenMenu* activeMenu = (static_cast<ScreenMenu*>(this->screens[this->activeScreenName]));
    
    this->activeScreenName = static_cast<ScreenName>(activeMenu->getSelectedItem().id);
}
std::vector<double> ControlPanel::doNavigation() {
    std::vector<double> actionResult;

    if (this->activeScreenName == ScreenName::HOME) {
        this->activeScreenName = ScreenName::MAIN_MENU;
    } else if (this->activeScreenName == ScreenName::CURRENT_MOISTURE_LEVELS) {
        this->activeScreenName = ScreenName::MAIN_MENU;
        this->selectWheel->resetTurns();
    } else if (this->activeScreenName == ScreenName::MOISTURE_LEVEL_HISTORY_SECONDS
        || this->activeScreenName == ScreenName::MOISTURE_LEVEL_HISTORY_MINUTES
        || this->activeScreenName == ScreenName::MOISTURE_LEVEL_HISTORY_QUARTER_HOURS) {

        (static_cast<ScreenLineGraph*>(this->screens[this->activeScreenName]))->unreserveMemory();
        this->activeScreenName = ScreenName::HISTORY_MENU;
        this->selectWheel->resetTurns();
    } else if (this->activeScreenName == ScreenName::MAIN_MENU) {
        Screen* activeScreen = this->screens[this->activeScreenName];
        ScreenMenu* menu = (static_cast<ScreenMenu*>(activeScreen));
        this->activeScreenName = static_cast<ScreenName>(menu->getSelectedItem().id);
        this->selectWheel->resetTurns();
    } else if (this->activeScreenName == ScreenName::HISTORY_MENU) {
        if (!this->buttons[ButtonName::RED].isPressed) {
            Screen* activeScreen = this->screens[this->activeScreenName];
            ScreenMenu* menu = (static_cast<ScreenMenu*>(activeScreen));
            this->activeScreenName = static_cast<ScreenName>(menu->getSelectedItem().id);
        } else {
            this->activeScreenName = ScreenName::MAIN_MENU;
        }
        this->selectWheel->resetTurns();
    } else if (this->activeScreenName == ScreenName::GOAL_PLANT_SELECTION) {
        if (!this->buttons[ButtonName::RED].isPressed) {
            actionResult.push_back((static_cast<ScreenMenu*>(this->screens[this->activeScreenName]))->selectedItemIndex);
            (static_cast<ScreenBarGraph*>(this->screens[ScreenName::SET_GOALS]))->addTitle((static_cast<ScreenMenu*>(this->screens[this->activeScreenName]))->getSelectedItemDisplayName());
            this->activeScreenName = ScreenName::SET_GOALS;
        } else {
            this->activeScreenName = ScreenName::MAIN_MENU;
        }
        this->selectWheel->resetTurns();
    } else if (this->activeScreenName == ScreenName::PUMP_CONTROL) {
        if (!this->buttons[ButtonName::RED].isPressed) {
            ScreenPump* pumpScreen = (static_cast<ScreenPump*>(this->screens[this->activeScreenName]));
            
            for (unsigned char i = 0; i < pumpScreen->pumpCount; i++) {
                bool isSelected = pumpScreen->selectedPumpIndex == i;
                actionResult.push_back(isSelected);
            }

            if (pumpScreen->getSelectedPumpDisplayName() == "Go Back") {
                this->activeScreenName = ScreenName::MAIN_MENU;
            }
        } else {
            this->activeScreenName = ScreenName::MAIN_MENU;
        }
        this->selectWheel->resetTurns();
    } else if (this->activeScreenName == ScreenName::SET_GOALS) {
        if (!this->buttons[ButtonName::RED].isPressed) {
            int turns = this->selectWheel->getCounterClockwiseTurns() / this->selectWheel->incrementSize;
            if (turns > 100) {
                turns = 100;
            } else if (turns < 0) {
                turns = 0;
            }
            actionResult.push_back(turns / 100.0);
        } else {
            this->activeScreenName = ScreenName::GOAL_PLANT_SELECTION;
            this->selectWheel->resetTurns();
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
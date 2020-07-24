#include "Arduino.h"
#include "ControlPanel.h"
#include "../Button/Button.h"
#include "../DisplayScreen/Screen/Screen.h"
#include "../DisplayScreen/ScreenImage/ScreenImage.h"
#include "../DisplayScreen/ScreenBarGraph/ScreenBarGraph.h"
#include "Assets.h"
#include "SelectWheel/SelectWheel.h"
#include "DisplayScreen/SelectMenu/SelectMenu.h"
#include <Map>

ControlPanel::ControlPanel() {
}
void ControlPanel::addSelectWheel(SelectWheel selectWheel) {
    this->selectWheel = selectWheel;
}
void ControlPanel::setup() {
    Screen::setup();
    this->activeScreen = this->screens.at(0);
}
void ControlPanel::setupScreens() {
    this->screens.push_back(ScreenImage(String("HYDRO 9000"), LEAF_LOGO_BMP, 33, 44));
    this->screens.push_back(ScreenBarGraph(String("CURRENT LVL")));
}
void ControlPanel::addSelectMenu(SelectMenu selectMenu) {
    this->menu = selectMenu;
}
void ControlPanel::addButton(Button::Name buttonName, Button button) {
    this->buttons[buttonName] = button;
}
bool ControlPanel::canShowDisplay() {
    return this->buttons[Button::Name::KEY_SWITCH].isPressed;
}
void ControlPanel::addScreen(Screen screen) {
    this->screens.push_back(screen);
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
void ControlPanel::update() {
    bool isOn = this->buttons[Button::Name::KEY_SWITCH].isPressed;
    if (!isOn) {
        Screen::turnOffDisplay();
    } else {
        this->activeScreen.doDisplay();
    }
}
bool ControlPanel::isEmergency() {
    return this->buttons[Button::Name::EMERGENCY_STOP].isPressed;
}
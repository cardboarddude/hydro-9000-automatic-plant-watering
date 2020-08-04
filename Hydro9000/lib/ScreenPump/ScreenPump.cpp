#include "Arduino.h"
#include "ScreenPump.h"
#include "../Screen/Screen.h"
#include <Adafruit_SSD1306.h>
#include <vector>

ScreenPump::ScreenPump() : Screen::Screen() {
}
ScreenPump::ScreenPump(unsigned char id, String title) : Screen::Screen(id, title) {
    
}
ScreenPump::ScreenPump(unsigned char id, String title, std::vector<String> pumpNames) : Screen::Screen(id, title) {
    Serial.println(pumpNames.size());
    for (unsigned int i = 0; i < pumpNames.size(); i++) {
        this->addPump(pumpNames.at(i));
    }
}
void ScreenPump::selectPump(unsigned char index) {
    this->selectedPumpIndex = index % this->pumpCount;
}
void ScreenPump::changeSelection(int indexChange) {
    this->selectedPumpIndex = (this->selectedPumpIndex+indexChange) % this->pumpCount;
}
void ScreenPump::selectNextPump() {
    this->selectedPumpIndex++;

    if (this->selectedPumpIndex >= this->pumpCount) {
        this->selectedPumpIndex = 0;
    }
}
void ScreenPump::selectPreviousPump() {
    this->selectedPumpIndex--;

    if (this->selectedPumpIndex < 0) {
        this->selectedPumpIndex = this->pumpCount - 1;
    }
}
void ScreenPump::doDisplay() {
    this->doBackgroundDisplay();
    this->doPumpNameDisplay();
    Screen::doDisplay();
}
void ScreenPump::doBackgroundDisplay() {
    Screen::display->fillRect(
        this->CONTENT_AREA_START_X, this->CONTENT_AREA_START_Y, this->CONTENT_AREA_WIDTH, this->CONTENT_AREA_HEIGHT, WHITE);
}
void ScreenPump::doPumpNameDisplay() {
    DisplayText pumpName(this->pumpNames[this->selectedPumpIndex],
        this->CONTENT_AREA_START_X, this->CONTENT_AREA_START_Y, this->CONTENT_AREA_WIDTH, this->CONTENT_AREA_HEIGHT);
    pumpName.fontSize = 2;
    pumpName.fontColor = BLACK;
    pumpName.isCenterAlignedHorizontal = true;
    pumpName.isCenterAlignedVertical = true;
    pumpName.doDisplay();
}
void ScreenPump::addPump(String displayName) {
    this->pumpNames[this->pumpCount++] = displayName;
}
String ScreenPump::getSelectedPumpDisplayName() {
    if (this->pumpCount > 0) {
        return this->pumpNames[this->selectedPumpIndex];
    } else {
        return "";
    }
}
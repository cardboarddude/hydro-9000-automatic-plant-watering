#include "Arduino.h"
#include "ScreenMenu.h"
#include "../Screen/Screen.h"
#include <Adafruit_SSD1306.h>
#include <vector>

ScreenMenu::ScreenMenu() : Screen::Screen() {
}
ScreenMenu::ScreenMenu(unsigned char id, String title) : Screen::Screen(id, title) {
    
}
ScreenMenu::ScreenMenu(unsigned char id, String title, String itemDisplayNames[], unsigned char displayNameCount) : Screen::Screen(id, title) {
    for (unsigned int i = 0; i < displayNameCount; i++) {
        this->addItemName(itemDisplayNames[i]);
    }
}
void ScreenMenu::selectItem(unsigned char index) {
    this->selectedItemIndex = index % this->itemCount;
}
void ScreenMenu::changeSelection(int indexChange) {
    this->selectedItemIndex = (this->selectedItemIndex+indexChange) % this->itemCount;
}
void ScreenMenu::selectNextItem() {
    this->selectedItemIndex++;

    if (this->selectedItemIndex >= this->itemCount) {
        this->selectedItemIndex = 0;
    }
}
void ScreenMenu::selectPreviousItem() {
    this->selectedItemIndex--;

    if (this->selectedItemIndex < 0) {
        this->selectedItemIndex = this->itemCount - 1;
    }
}
void ScreenMenu::doDisplay() {
    for (unsigned int i = 0; i < this->itemCount; i++) {
        this->itemDisplayNames[i].doDisplay();
    }

    this->doDisplaySelector();
    Screen::doDisplay();
}
void ScreenMenu::doDisplaySelector() {
    Screen::display->setTextColor(WHITE);
    Screen::display->setRotation(0);
    Screen::display->fillTriangle(
        this->CONTENT_AREA_START_X, this->CONTENT_AREA_START_Y + (this->getMenuItemHeight(this->selectedItemIndex) * this->selectedItemIndex),
        this->CONTENT_AREA_START_X + this->SELECTOR_WIDTH, this->CONTENT_AREA_START_Y + (this->getMenuItemHeight(this->selectedItemIndex) * this->selectedItemIndex) + this->SELECTOR_HEIGHT/2,
        this->CONTENT_AREA_START_X, this->CONTENT_AREA_START_Y + (this->getMenuItemHeight(this->selectedItemIndex) * this->selectedItemIndex) + this->SELECTOR_HEIGHT,
        WHITE
    );
}
unsigned int ScreenMenu::getMenuItemHeight(unsigned char index) {
    if (index < 0 || index >= this->itemNameCount) {
        unsigned char padding = 1;
        return DisplayText::getFontCharHeight(DisplayText::DEFAULT_FONT_SIZE)+(padding*2);
    } else {
        return this->itemDisplayNames[index].getHeight();
    }
}
void ScreenMenu::addItemName(String displayName) {
    unsigned char menuItemHeight = this->getMenuItemHeight(this->itemNameCount-1);
    int startX = ScreenMenu::SELECTOR_PADDING_RIGHT + ScreenMenu::SELECTOR_WIDTH;
    int endX = Screen::display->width();
    int startY = this->CONTENT_AREA_START_Y + (menuItemHeight * this->itemNameCount);
    int endY = startY + menuItemHeight;
    DisplayText newMenuItem(displayName, startX, startY, endX, endY);
    newMenuItem.topBottomPadding=1;
    this->itemDisplayNames[this->itemNameCount++] = newMenuItem;
}
void ScreenMenu::addItem(Screen& screen) {
    this->items[this->itemCount++] = &screen;
}
void ScreenMenu::addItem(String displayName, Screen& screen) {
    this->addItemName(displayName);
    this->addItem(screen);
}
// void ScreenMenu::addItemSubMenu(String displayName, ScreenMenu& subMenu) {
//     this->itemDisplayNames.push_back(displayName);
//     if (this->childMenus.size()+1 < this->itemDisplayNames.size()) {
//         this->childMenus.resize(this->itemDisplayNames.size()-1);
//     }
//     std::vector<ScreenMenu*>::iterator itr = this->childMenus.end();
//     this->childMenus.insert(itr + 1, &subMenu);
// }
// bool ScreenMenu::isSubMenuSelected() {
//     return (this->childMenus.size() > this->selectedItemIndex && this->childMenus.at(this->selectedItemIndex) != NULL);
// }
// bool ScreenMenu::isActionSelected() {
//     return (this->actions.size() > this->selectedItemIndex && this->actions.at(this->selectedItemIndex) != NULL);
// }
// ScreenMenu& ScreenMenu::getSelectedSubMenu() {
//     if (this->childMenus.size() > this->selectedItemIndex) {
//         return *(this->childMenus.at(this->selectedItemIndex));
//     }
// }
Screen& ScreenMenu::getSelectedItem() {
    if (this->itemCount > this->selectedItemIndex) {
        return *(this->items[this->selectedItemIndex]);
    }
}
String ScreenMenu::getSelectedItemDisplayName() {
    if (this->itemNameCount > 0) {
        return this->itemDisplayNames[this->selectedItemIndex].text;
    } else {
        return "";
    }
}
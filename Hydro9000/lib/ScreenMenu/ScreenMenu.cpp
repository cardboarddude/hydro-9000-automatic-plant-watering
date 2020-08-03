#include "Arduino.h"
#include "ScreenMenu.h"
#include "../Screen/Screen.h"
#include <Adafruit_SSD1306.h>
#include <vector>

ScreenMenu::ScreenMenu() : Screen::Screen() {
}
ScreenMenu::ScreenMenu(String title) : Screen::Screen(title) {
    
}
ScreenMenu::ScreenMenu(String title, std::vector<String>& itemDisplayNames) : Screen::Screen(title) {
    Serial.println("init screen menu ");
    Serial.println(itemDisplayNames.size());
    for (unsigned int i = 0; i < itemDisplayNames.size(); i++) {
        this->addItem(itemDisplayNames[i]);
    }
}
void ScreenMenu::selectItem(unsigned char index) {

}
void ScreenMenu::selectNextItem() {
    this->selectedItemIndex++;

    if (this->selectedItemIndex >= this->menuItemCount) {
        this->selectedItemIndex = 0;
    }
}
void ScreenMenu::selectPreviousItem() {
    this->selectedItemIndex--;

    if (this->selectedItemIndex < 0) {
        this->selectedItemIndex = this->menuItemCount - 1;
    }
}
void ScreenMenu::doDisplay() {
    for (unsigned int i = 0; i < this->menuItemCount; i++) {
        this->menuItems[i].doDisplay();
    }

    Serial.print("displaying selector "+String(this->menuItemCount));
    this->doDisplaySelector();
    Serial.print("displaying title ");
    Screen::doDisplay();
}
void ScreenMenu::doDisplaySelector() {
    Screen::display->setTextColor(WHITE);
    Screen::display->setRotation(0);
    Screen::display->fillTriangle(
        this->contentAreaStartX, this->contentAreaStartY + (this->getMenuItemHeight(this->selectedItemIndex) * this->selectedItemIndex),
        this->contentAreaStartX + this->SELECTOR_WIDTH, this->contentAreaStartY + (this->getMenuItemHeight(this->selectedItemIndex) * this->selectedItemIndex) + this->SELECTOR_HEIGHT/2,
        this->contentAreaStartX, this->contentAreaStartY + (this->getMenuItemHeight(this->selectedItemIndex) * this->selectedItemIndex) + this->SELECTOR_HEIGHT,
        WHITE
    );
}
unsigned int ScreenMenu::getMenuItemHeight(unsigned char index) {
    if (index < 0 || index >= this->menuItemCount) {
        unsigned char padding = 1;
        return DisplayText::getFontCharHeight(DisplayText::DEFAULT_FONT_SIZE)+(padding*2);
    } else {
        return this->menuItems[index].getHeight();
    }
}
void ScreenMenu::addItem(String displayName) {
    unsigned char menuItemHeight = this->getMenuItemHeight(this->menuItemCount-1);
    int startX = ScreenMenu::SELECTOR_PADDING_RIGHT + ScreenMenu::SELECTOR_WIDTH;
    int endX = Screen::display->width();
    int startY = this->contentAreaStartY + (menuItemHeight * this->menuItemCount);
    int endY = startY + menuItemHeight;
    DisplayText newMenuItem(displayName, startX, startY, endX, endY);
    newMenuItem.topBottomPadding=1;
    this->menuItems[this->menuItemCount++] = newMenuItem;
}
// void ScreenMenu::addItemAction(String displayName, VoidFunction action) {
//     this->itemDisplayNames.push_back(displayName);
//     if (this->actions.size()+1 < this->itemDisplayNames.size()) {
//         this->actions.resize(this->itemDisplayNames.size()-1);
//     }
//     std::vector<VoidFunction>::iterator itr = this->actions.end();
//     this->actions.insert(itr + 1, action);
// }
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
// VoidFunction ScreenMenu::getSelectedAction() {
//     if (this->actions.size() > this->selectedItemIndex) {
//         return this->actions.at(this->selectedItemIndex);
//     }
// }
// String ScreenMenu::getSelectedItemDisplayName() {
//     if (this->itemDisplayNames.size() > this->selectedItemIndex) {
//         return this->itemDisplayNames.at(this->selectedItemIndex);
//     } else {
//         return "";
//     }
// }
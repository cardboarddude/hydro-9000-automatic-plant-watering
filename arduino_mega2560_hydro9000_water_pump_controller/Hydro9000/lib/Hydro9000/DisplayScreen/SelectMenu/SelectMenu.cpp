#include "Arduino.h"
#include "SelectMenu.h"
#include <Adafruit_SSD1306.h>
#include <vector>

SelectMenu::SelectMenu() {}
SelectMenu::SelectMenu(String title, Adafruit_SSD1306& display) {
    this->title = title;
    this->display = &display;
}
SelectMenu::SelectMenu(String title, std::vector<String>& itemDisplayNames, Adafruit_SSD1306& display) {
    this->title = title;
    this->itemDisplayNames = itemDisplayNames;
    this->display = &display;
}
void SelectMenu::selectItem(int index) {

}
void SelectMenu::selectNextItem() {
    this->selectedItemIndex++;

    if (this->selectedItemIndex >= this->itemDisplayNames.size()) {
        this->selectedItemIndex = 0;
    }
}
void SelectMenu::selectPreviousItem() {
    this->selectedItemIndex--;

    if (this->selectedItemIndex < 0) {
        this->selectedItemIndex = this->itemDisplayNames.size() - 1;
    }
}
void SelectMenu::doDisplay() {
    this->display->setTextColor(WHITE);
    this->display->setCursor(this->TITLE_START_X, this->TITLE_START_Y);
    this->display->setTextSize(this->TITLE_TEXT_SIZE);
    this->display->println(this->title);

    this->display->setTextSize(this->ITEM_TEXT_SIZE);
    for (int i = 0; i < this->itemDisplayNames.size(); i++) {
        this->display->setCursor(this->ITEM_START_X, this->ITEM_START_Y + (this->ITEM_HEIGHT * i));
        this->display->println(this->itemDisplayNames.at(i));
    }

    this->display->fillTriangle(
        this->SELECTED_ITEM_INDICATOR_START_X, this->ITEM_START_Y + (this->ITEM_HEIGHT * this->selectedItemIndex),
        this->SELECTED_ITEM_INDICATOR_START_X + this->SELECTED_ITEM_INDICATOR_WIDTH, this->ITEM_START_Y + (this->ITEM_HEIGHT * this->selectedItemIndex) + this->SELECTED_ITEM_INDICATOR_HEIGHT/2,
        this->SELECTED_ITEM_INDICATOR_START_X, this->ITEM_START_Y + (this->ITEM_HEIGHT * this->selectedItemIndex) + this->SELECTED_ITEM_INDICATOR_HEIGHT,
        WHITE
        );
    this->display->display();
}
void SelectMenu::addItem(String displayName) {
    this->itemDisplayNames.push_back(displayName);
}
void SelectMenu::addItemAction(String displayName, VoidFunction action) {
    this->itemDisplayNames.push_back(displayName);
    if (this->actions.size()+1 < this->itemDisplayNames.size()) {
        this->actions.resize(this->itemDisplayNames.size()-1);
    }
    std::vector<VoidFunction>::iterator itr = this->actions.end();
    this->actions.insert(itr + 1, action);
}
void SelectMenu::addItemSubMenu(String displayName, SelectMenu& subMenu) {
    this->itemDisplayNames.push_back(displayName);
    if (this->childMenus.size()+1 < this->itemDisplayNames.size()) {
        this->childMenus.resize(this->itemDisplayNames.size()-1);
    }
    std::vector<SelectMenu*>::iterator itr = this->childMenus.end();
    this->childMenus.insert(itr + 1, &subMenu);
}
bool SelectMenu::isSubMenuSelected() {
    return (this->childMenus.size() > this->selectedItemIndex && this->childMenus.at(this->selectedItemIndex) != NULL);
}
bool SelectMenu::isActionSelected() {
    return (this->actions.size() > this->selectedItemIndex && this->actions.at(this->selectedItemIndex) != NULL);
}
SelectMenu& SelectMenu::getSelectedSubMenu() {
    if (this->childMenus.size() > this->selectedItemIndex) {
        return *(this->childMenus.at(this->selectedItemIndex));
    }
}
VoidFunction SelectMenu::getSelectedAction() {
    if (this->actions.size() > this->selectedItemIndex) {
        return this->actions.at(this->selectedItemIndex);
    }
}
String SelectMenu::getSelectedItemDisplayName() {
    if (this->itemDisplayNames.size() > this->selectedItemIndex) {
        return this->itemDisplayNames.at(this->selectedItemIndex);
    } else {
        return "";
    }
}
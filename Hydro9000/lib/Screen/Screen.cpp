#include "Screen.h"
#include "Adafruit_SSD1306.h"
#include "../DisplayText/DisplayText.h"

Screen::Screen() {
}
Screen::Screen(String title) {
    this->addTitle(title);
}
Screen::Screen(DisplayText title) {
    this->title = title;
}
void Screen::addTitle(DisplayText title) {
    title.text.toUpperCase();
    this->title = title;
}
void Screen::addTitle(String title) {
    title.toUpperCase();
    DisplayText text(title, 0, 0);
    text.fontSize = 2;
    this->title = text;
}
void Screen::doDisplay() {
    if (this->title.text != NULL) {
        this->title.doDisplay();
    }
    Screen::display.display();
}
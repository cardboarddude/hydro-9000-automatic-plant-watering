#include "Screen.h"
#include "Adafruit_SSD1306.h"
#include "../DisplayText/DisplayText.h"

Screen::Screen() {
}
Screen::Screen(unsigned char id) {
    this->id = id;
}
Screen::Screen(unsigned char id, String title) : Screen(id) {
    this->id = id;
    this->addTitle(title);
}
Screen::Screen(unsigned char id, DisplayText& title) : Screen(id) {
    this->title = title;
}
void Screen::addTitle(DisplayText& title) {
    title.text.toUpperCase();
    this->title = title;
}
void Screen::addTitle(String title) {
    title.toUpperCase();
    DisplayText text(title, 0, 0, -1, -1);
    text.fontSize = 2;
    this->title = text;
}
void Screen::doDisplay() {
    if (this->title.text != NULL) {
        this->title.doDisplay();
    }
    Screen::display->display();
}
void Screen::clear() {
    Screen::display->clearDisplay();
}
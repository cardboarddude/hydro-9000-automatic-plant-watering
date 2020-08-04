#include "ScreenLineGraph.h"
#include "../DisplayText/DisplayText.h"
#include "../Screen/Screen.h"
#include "../Point/Point.h"

#include "Adafruit_SSD1306.h"

ScreenLineGraph::ScreenLineGraph() : Screen::Screen() {}
ScreenLineGraph::ScreenLineGraph(unsigned char id, String title, String xAxisLabel, String yAxisLabel) : Screen::Screen(id, title) {
    this->xAxisLabel = DisplayText(
        xAxisLabel, 
        this->startX, this->startY+this->height,
        this->startX+this->width,
        this->CONTENT_AREA_START_Y+this->CONTENT_AREA_HEIGHT);
    this->yAxisLabel = DisplayText(
        yAxisLabel, 
        Screen::display->height() - (this->startY+this->height),
        this->CONTENT_AREA_START_X,
        Screen::display->height() - this->CONTENT_AREA_START_Y,
        this->startX,
        3);
}
void ScreenLineGraph::addPoint(double value) {
    if (this->isDynamicMinMax && value > this->maxValue) {
        this->maxValue = value;
    } else {
        this->maxValue = 1;
    }
    if (this->isDynamicMinMax && value < this->minValue) {
        this->minValue = value;
    } else {
        this->minValue = 0;
    }
    if (!this->isDynamicMinMax) {
        if (value > this->maxValue) {
            value = this->maxValue;
        } else if (value < this->minValue) {
            value = this->maxValue;
        }
    }
    this->values.push_back(value);
}
void ScreenLineGraph::doDisplay() {
    this->doDisplayPoints();
    this->xAxisLabel.doDisplay();
    this->yAxisLabel.doDisplay();
    Screen::doDisplay();
}
void ScreenLineGraph::clear() {
    this->values.clear();

    Screen::clear();
}
void ScreenLineGraph::doDisplayPoints() {
    if (this->values.size() == 0) return;
    
    double horizontalScale = this->width / this->values.size();
    double verticleScale = this->height / (this->maxValue - this->minValue);
    for (unsigned int i = 0; i < this->values.size()-1; i++) {
        // Serial.print(" ("+String(this->startX + (i+1) * horizontalScale)+", ");
        // Serial.print(String(this->startY + this->height - (this->values.at(i) * verticleScale))+") ");
        int startX = this->startX + (i+1) * horizontalScale;
        int startY = this->startY + this->height - (this->values.at(i) * verticleScale);
        int endX = this->startX + (i+2) * horizontalScale;
        int endY = this->startY + this->height - (this->values.at(i+1) * verticleScale);
        Screen::display->drawLine(startX, startY, endX, endY, WHITE);
        // Screen::display->drawPixel(
        //     this->startX + (i+1) * horizontalScale, 
        //     this->startY + this->height - (this->values.at(i) * verticleScale), 
        //     WHITE);
    }
}
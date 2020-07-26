#include "ScreenBarGraph.h"
#include "../DisplayText/DisplayText.h"
#include "../Screen/Screen.h"
#include "../Point/Point.h"

#include "Adafruit_SSD1306.h"

ScreenBarGraph::ScreenBarGraph() : Screen::Screen() {}
ScreenBarGraph::ScreenBarGraph(String title) : Screen::Screen(title) {}
void ScreenBarGraph::addBar(double value, String label) {
    this->values.push_back(value);
    this->labels.push_back(label);
}
void ScreenBarGraph::doDisplay() {
    Screen::doDisplay();
    int barWidth = this->getBarWidth();
    for (int i = 0; i < this->values.size(); i++) {
        auto barSize = this->getBarPosition(i, barWidth);
        this->doDisplayBar(barSize);
        this->doDisplayLabel(i, barSize);
    }
}
void ScreenBarGraph::doDisplayBar(Point position[2]) {
    this->display.fillRect(position[0].x, position[0].y, position[1].x, position[1].y, WHITE);
}
void ScreenBarGraph::doDisplayLabel(int labelIndex, Point barPosition[2]) {
    DisplayText label = this->getLabel(labelIndex, barPosition);
    label.doDisplay();
}
DisplayText ScreenBarGraph::getLabel(int labelIndex, Point barPosition[2]) {
    DisplayText label(
        this->labels.at(labelIndex),
        barPosition[0].x,
        barPosition[0].y,
        barPosition[1].x
    );
    label.fontSize = 1;
    label.topBottomPadding = 3;
    label.isCenterAlignedHorizontal = true;
    label.fontColor = BLACK;

    bool isLabelCloseToBottomOfScreen = ((barPosition[0].y + label.getHeight()) > this->startY + this->height);
    if (isLabelCloseToBottomOfScreen) {
        // Show label above bar
        label.startY = barPosition[0].y - label.getHeight();
        label.fontColor = WHITE;
    }
    return label;
}
Point* ScreenBarGraph::getBarPosition(int index, int barWidth) {
    Point points[2];
    if (barWidth == -1) {
        barWidth = getBarWidth();
    }
    int barHeight = this->getBarHeight(index);
    //topLeftCorner
    points[0] = Point(
        index * (barWidth + this->barSpacing) + this->startX,
        this->startY + this->height - barHeight
    );
    //bottomRightCorner
    points[1] = Point(
        index * (barWidth + this->barSpacing) + barWidth + this->startX,
        this->startY + this->height
    );

    return points;
}
int ScreenBarGraph::getBarHeight(int index) {
    double heightAsPercentageOfGraphHeight = (this->values.at(index) / (this->maxValue - this->minValue));
    return this->height * heightAsPercentageOfGraphHeight;
}
int ScreenBarGraph::getBarWidth() {
    if (this->values.size() > 0) {
        int totalSpacing = this->barSpacing * (this->values.size() - 1);
        return (this->width - totalSpacing) / this->values.size();
    } else {
        return this->width;
    }
}
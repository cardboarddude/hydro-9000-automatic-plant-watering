#include "DisplayText.h"
#include "../Point/Point.h"

DisplayText::DisplayText(String text, int startX, int startY, int endX, int endY) {
    this->text = text;
    this->startX = startX;
    this->startY = startY;
    this->endX = endX;
    this->endY = endY;
}
void DisplayText::doDisplay() {
    DisplayText::display.setTextSize(this->fontSize);
    Point point = this->getCursorPoint();
    DisplayText::display.setCursor(point.x, point.y);
    DisplayText::display.setTextColor(this->fontColor);
}
Point DisplayText::getCursorPoint() {
    Point point;

    if (this->isCenterAlignedHorizontal && this->isCenterAlignedVertical) {
        point = this->getCenterAlignedCursorPoint();
    } else if (this->isCenterAlignedHorizontal) {
        point = this->getHorizontalCenterAlignedCursorPoint();
    } else if (this->isCenterAlignedVertical) {
        point = this->getVerticalCenterAlignedCursorPoint();
    } else {
        point.x = this->startX;
        point.y = this->startY + this->topBottomPadding;
    }

    return point;
}
Point DisplayText::getCenterAlignedCursorPoint() {
    Point point(
        this->getHorizontalCenterAlignedCursorPoint().x,
        this->getVerticalCenterAlignedCursorPoint().y
    );
    return point;
}
Point DisplayText::getHorizontalCenterAlignedCursorPoint() {
    Point point(this->startX, this->startY + this->topBottomPadding);

    point.x += (this->getWidth() / 2) + ((this->endX - this->startX) / 2);

    return point;
}
Point DisplayText::getVerticalCenterAlignedCursorPoint() {
    Point point(this->startX, this->startY + this->topBottomPadding);

    point.y += (this->getHeight() / 2) + ((this->endY - this->startY) / 2);

    return point;
}
int DisplayText::getWidth() {
    return this->text.length() * this->getFontCharWidth();
}
// Will break if text wraps onto next line
int DisplayText::getHeight() {
    return this->getFontCharHeight() + this->topBottomPadding * 2;
}
int DisplayText::getFontCharWidth() {
    switch (this->fontSize) {
        case 1: return 6;
        case 2: return 12;
        case 3: return 18;
        default: return -1;
    }
}
int DisplayText::getFontCharHeight() {
    switch (this->fontSize) {
        case 1: return 8;
        case 2: return 18;
        case 3: return 24;
        default: return -1;
    }
}
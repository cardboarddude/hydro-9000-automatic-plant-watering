#include "DisplayText.h"
#include "../Point/Point.h"

// Text is positioned using the bottom left of the first character as the origin
DisplayText::DisplayText(String text, int startX, int startY, int endX, int endY) {
    this->text = text;
    this->startX = startX;
    this->startY = startY;
    this->endX = endX;
    this->endY = endY;
}
// direction: 0 thru 3 corresponding to 4 cardinal rotations
DisplayText::DisplayText(String text, int startX, int startY, int endX, int endY, byte direction) : DisplayText(text, startX, startY, endX, endY) {
    this->direction = direction;
}
void DisplayText::doDisplay() {
    DisplayText::display->setTextSize(this->fontSize);
    Point point = this->getCursorPoint();
    DisplayText::display->setCursor(point.x, point.y);
    DisplayText::display->setRotation(this->direction);
    DisplayText::display->setTextColor(this->fontColor);
    DisplayText::display->print(this->text);
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
int DisplayText::getVerticalPadding() {
    return this->topBottomPadding;
}
int DisplayText::getHorizontalPadding() {
    return this->leftRightPadding;
}
Point DisplayText::getHorizontalCenterAlignedCursorPoint() {
    Point point(this->startX + this->getHorizontalPadding(), this->startY + this->getVerticalPadding());

    point.x += ((this->endX - this->startX) / 2) - (this->getWidth() / 2);
    return point;
}
Point DisplayText::getVerticalCenterAlignedCursorPoint() {
    Point point(this->startX + this->getHorizontalPadding(), this->startY + this->getVerticalPadding());

    point.y += (this->getHeight() / 2) + ((this->endY - this->startY) / 2);

    return point;
}
int DisplayText::getWidth() {
    return this->text.length() * DisplayText::getFontCharWidth(this->fontSize);
}
// Will break if text wraps onto next line
int DisplayText::getHeight() {
    return DisplayText::getFontCharHeight(this->fontSize) + this->topBottomPadding * 2;
}
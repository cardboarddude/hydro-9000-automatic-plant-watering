#include "ScreenImage.h"
#include "DisplayText.h"
#include "Screen.h"
#include "Point.h"

#include "Adafruit_SSD1306.h"

ScreenImage::ScreenImage() : Screen::Screen() {}
ScreenImage::ScreenImage(String title, unsigned char* imageBitmap, int height, int width) : Screen(title) {
    this->imageContent = imageBitmap;
    this->imageHeight = height;
    this->imageWidth = width;
}
ScreenImage::ScreenImage(DisplayText title, unsigned char* imageBitmap, int height, int width) : Screen(title) {
    this->imageContent = imageBitmap;
    this->imageHeight = height;
    this->imageWidth = width;
}
void ScreenImage::doDisplay() {
    Point imageTopLeftPoint = this->getImageTopLeftPoint();

    Screen::display.drawBitmap(imageTopLeftPoint.x, imageTopLeftPoint.y, this->imageContent,
        this->imageWidth, this->imageHeight, WHITE);
    Screen::doDisplay();
}
Point ScreenImage::getImageTopLeftPoint() {
    int offsetX = ((this->contentAreaWidth - this->imageWidth) / 2) + this->contentAreaStartX;
    int offsetY = ((this->contentAreaHeight - this->imageHeight) / 2) + this->contentAreaStartY;
    return Point(offsetX, offsetY);
}
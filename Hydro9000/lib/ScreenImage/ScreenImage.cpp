#include "ScreenImage.h"
#include "../DisplayText/DisplayText.h"
#include "../Screen/Screen.h"
#include "../Point/Point.h"

#include "Adafruit_SSD1306.h"

ScreenImage::ScreenImage() : Screen::Screen() {}
ScreenImage::ScreenImage(unsigned char id, String title, const unsigned char* imageBitmap, int width, int height) : Screen::Screen(id, title) {
    this->imageContent = imageBitmap;
    this->imageHeight = height;
    this->imageWidth = width;
}
ScreenImage::ScreenImage(unsigned char id, DisplayText& title, const unsigned char* imageBitmap, int width, int height) : Screen::Screen(id, title) {
    this->imageContent = imageBitmap;
    this->imageHeight = height;
    this->imageWidth = width;
}
void ScreenImage::doDisplay() {
    Point imageTopLeftPoint = this->getImageTopLeftPoint();
    
    Screen::display->drawBitmap(imageTopLeftPoint.x, imageTopLeftPoint.y, this->imageContent,
        this->imageWidth, this->imageHeight, WHITE);
    Screen::doDisplay();
}
Point ScreenImage::getImageTopLeftPoint() {
    int offsetX = ((this->CONTENT_AREA_WIDTH - this->imageWidth) / 2) + this->CONTENT_AREA_START_X;
    int offsetY = ((this->CONTENT_AREA_HEIGHT - this->imageHeight) / 2) + this->CONTENT_AREA_START_Y;
    return Point(offsetX, offsetY);
}
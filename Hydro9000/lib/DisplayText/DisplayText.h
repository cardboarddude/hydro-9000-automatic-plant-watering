#ifndef DisplayText_h
#define DisplayText_h

#include "Adafruit_SSD1306.h"
#include "../Point/Point.h"

class DisplayText {
    public:
        static Adafruit_SSD1306* display;

        String text;
        int fontSize = 1, fontColor = WHITE, topBottomPadding = 0;
        int startX, startY;
        int endX, endY;
        bool isCenterAlignedHorizontal = false, isCenterAlignedVertical = false;
        
        DisplayText() {};
        DisplayText(String text, int startX, int startY, int endX = -1, int endY = -1);
        void doDisplay();
        Point getCursorPoint();
        Point getCenterAlignedCursorPoint();
        Point getHorizontalCenterAlignedCursorPoint();
        Point getVerticalCenterAlignedCursorPoint();
        int getWidth();
        int getHeight();
        int getFontCharWidth();
        int getFontCharHeight();
};

#endif
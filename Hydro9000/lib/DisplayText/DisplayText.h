#ifndef DisplayText_h
#define DisplayText_h

#include "Adafruit_SSD1306.h"
#include "../Point/Point.h"

class DisplayText {
    public:
        static Adafruit_SSD1306* display;
        static const byte DEFAULT_FONT_SIZE = 1;
        String text;
        byte fontSize = DEFAULT_FONT_SIZE;
        int fontColor = WHITE, topBottomPadding = 0, leftRightPadding = 0;
        int startX, startY;
        int endX, endY;
        byte direction = 0;
        bool isCenterAlignedHorizontal = false, isCenterAlignedVertical = false;
        
        static int getFontCharWidth(int fontSize) {
            switch (fontSize) {
                case 1: return 6;
                case 2: return 12;
                case 3: return 18;
                default: return -1;
            }
        }
        static int getFontCharHeight(int fontSize) {
            switch (fontSize) {
                case 1: return 8;
                case 2: return 18;
                case 3: return 24;
                default: return -1;
            }
        }

        DisplayText() {};
        DisplayText(String text, int startX, int startY, int endX, int endY);
        DisplayText(String text, int startX, int startY, int endX, int endY, byte direction);
        void doDisplay();
        Point getCursorPoint();
        Point getCenterAlignedCursorPoint();
        Point getHorizontalCenterAlignedCursorPoint();
        Point getVerticalCenterAlignedCursorPoint();
        int getVerticalPadding();
        int getHorizontalPadding();
        int getWidth();
        int getHeight();
        int getFontCharWidth();
        int getFontCharHeight();
};

#endif
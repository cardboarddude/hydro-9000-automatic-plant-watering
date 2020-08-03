#ifndef Screen_h
#define Screen_h

#include "Arduino.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include "../DisplayText/DisplayText.h"

class Screen {
	public:
        static Adafruit_SSD1306* display;
        static bool isSetup;

        unsigned char contentAreaStartX = 0, contentAreaStartY = 20;
        unsigned char contentAreaHeight = 43, contentAreaWidth = 128;
        unsigned char contentAreaVirtualHeight = contentAreaWidth;
        
        Screen();
        Screen(String title);
        Screen(DisplayText& title);
        virtual void doDisplay();
        virtual void clear();
        void addTitle(String title);
        void addTitle(DisplayText& title);
        static void setup()  {
            // Address 0x3D for 128x64
            Screen::isSetup = display->begin(SSD1306_SWITCHCAPVCC, 0x3C);

            if(!Screen::isSetup) {
                Serial.println(F("SSD1306 allocation failed"));
            } else {
                Screen::display->clearDisplay();
            }
        }
        static void turnOffDisplay() {
            Screen::display->clearDisplay();
            Screen::display->display();
        }
    protected:
        DisplayText title;
};

#endif
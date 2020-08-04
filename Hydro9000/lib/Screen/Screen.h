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

        static const unsigned char CONTENT_AREA_START_X = 0, CONTENT_AREA_START_Y = 20;
        static const unsigned char CONTENT_AREA_HEIGHT = 43, CONTENT_AREA_WIDTH = 128;
        unsigned char id;
        
        Screen();
        Screen(unsigned char id);
        Screen(unsigned char id, String title);
        Screen(unsigned char id,DisplayText& title);
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
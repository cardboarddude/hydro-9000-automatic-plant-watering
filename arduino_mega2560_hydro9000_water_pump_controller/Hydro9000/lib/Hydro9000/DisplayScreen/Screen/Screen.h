#ifndef Screen_h
#define Screen_h

#include "Arduino.h"
#include "../DisplayText/DisplayText.h"

class Screen {
	public:
        static Adafruit_SSD1306 display;
        static bool isSetup;

        int contentAreaStartX = 0, contentAreaStartY = 21;
        int contentAreaHeight = 43, contentAreaWidth = 128;
        int contentAreaVirtualHeight = contentAreaWidth;
        
        Screen();
        Screen(String title);
        Screen(DisplayText title);
        virtual void doDisplay();
        void addTitle(String title);
        void addTitle(DisplayText title);
        static void setup()  {
            if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
                Serial.println(F("SSD1306 allocation failed"));
                for(;;);
            }
            Screen::isSetup = true;
        }
        static void turnOffDisplay() {
            Screen::display.clearDisplay();
            Screen::display.display();
        }
    protected:
        DisplayText title;
};

#endif
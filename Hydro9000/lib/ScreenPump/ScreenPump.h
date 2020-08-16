
#ifndef ScreenPump_h
#define ScreenPump_h

#include "Arduino.h"
#include "../Screen/Screen.h"
#include "../ControlPanel/ControlPanel.h"

class ScreenPump : public Screen {
    private:
        static const unsigned char PROGMEM SELECTOR_HEIGHT = 8, SELECTOR_WIDTH = 8, SELECTOR_PADDING_RIGHT = 2;
        static const unsigned char PROGMEM PUMP_NAMES_MAX_SIZE = 10;
        
	public: 
        unsigned char selectedPumpIndex = 0;

        unsigned char pumpCount = 0;
		String pumpNames[PUMP_NAMES_MAX_SIZE];
		
        ScreenPump();
		ScreenPump(unsigned char id, String title);
		ScreenPump(unsigned char id, String title, String pumpNames[], unsigned char pumpCount);
        void addPump(String menuPumpName);
        virtual void doDisplay();
        void doBackgroundDisplay();
        void doPumpNameDisplay();
		void selectNextPump();
		void selectPreviousPump();
		void selectPump(int menuPumpIndex);
        void changeSelection(int indexChange);
        String getSelectedPumpDisplayName();
};

#endif

#ifndef ControlPanel_h
#define ControlPanel_h

#include "Arduino.h"
#include <Map>
#include "../SelectWheel/SelectWheel.h"
#include "../SelectMenu/SelectMenu.h"
#include "../Button/Button.h"
#include "../Screen/Screen.h"

class ControlPanel {
	private:
        SelectWheel selectWheel;
        SelectMenu menu;
        std::map<Button::Name, Button> buttons;
		Screen activeScreen;
        std::vector<Screen> screens;

	public:
		static unsigned long currentMillis;
		static void setCurrentMillis(unsigned long millis) {
			ControlPanel::currentMillis = millis;
			Button::currentMillis = millis;
		}

		ControlPanel();
		void setup();
        void addSelectMenu(SelectMenu menu);
        void addSelectWheel(SelectWheel selectWheel);
        void addButton(Button::Name buttonName, Button button);
		void addScreen(Screen screen);
		bool canShowDisplay();
		void turnOffDisplay();
		void turnOffLeds();
		void update();
		bool isEmergency();
		void setupScreens();
};

#endif
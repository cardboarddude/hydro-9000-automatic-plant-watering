
#ifndef ControlPanel_h
#define ControlPanel_h

#include "Arduino.h"
#include <Map>
#include "../SelectWheel/SelectWheel.h"
#include "../SelectMenu/SelectMenu.h"
#include "../Button/Button.h"
#include "../Screen/Screen.h"

class ControlPanel {
	public:
		static unsigned long currentMillis;
		static void setCurrentMillis(unsigned long millis) {
			ControlPanel::currentMillis = millis;
			Button::currentMillis = millis;
		}
		enum ScreenName{
			HOME, 
			CURRENT_MOISTURE_LEVELS
		};
        enum ButtonName {
            RED,
            BLUE,
            SELECT_WHEEL,
            KEY_SWITCH,
            EMERGENCY_STOP
        };

		ControlPanel();
		void setup();
        void addSelectMenu(SelectMenu& menu);
        void addSelectWheel(SelectWheel& selectWheel);
        void addButton(ControlPanel::ButtonName buttonName, Button& button);
		void addScreen(ScreenName sType, Screen& screen);
		bool canShowDisplay();
		void turnOffDisplay();
		void turnOffLeds();
		void update(std::vector<double>& data);
		bool isEmergency();
		void setupScreens();
		void updateButtons();
		bool wasAnyButtonClicked();
		bool wasAnythingClicked();
		void doNavigation();

	private:
        SelectWheel selectWheel;
        SelectMenu menu;
        std::map<ControlPanel::ButtonName, Button> buttons;
		ScreenName activeScreenName;
        std::map<ScreenName, Screen*> screens;
};

#endif
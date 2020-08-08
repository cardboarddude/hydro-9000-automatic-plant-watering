
#ifndef ControlPanel_h
#define ControlPanel_h

#include "Arduino.h"
#include <Map>
#include "../PlantController/PlantController.h"
#include "../SelectWheel/SelectWheel.h"
#include "../ScreenMenu/ScreenMenu.h"
#include "../Button/Button.h"
#include "../Screen/Screen.h"

class ControlPanel {
	public:
		static unsigned long currentMillis;
		static void setCurrentMillis(unsigned long millis) {
			ControlPanel::currentMillis = millis;
			Button::currentMillis = millis;
			PlantController::currentMillis = millis;
		}
		enum ScreenName {
			HOME, 
			MAIN_MENU,
			HISTORY_MENU,
			CURRENT_MOISTURE_LEVELS,
			MOISTURE_LEVEL_HISTORY_SECONDS,
			MOISTURE_LEVEL_HISTORY_MINUTES,
			MOISTURE_LEVEL_HISTORY_QUARTER_HOURS,
			GOAL_PLANT_SELECTION,
			SET_GOALS,
			PUMP_CONTROL
		};
        enum ButtonName {
            RED,
            BLUE,
            SELECT_WHEEL,
            KEY_SWITCH,
            EMERGENCY_STOP
        };
		ScreenName activeScreenName;

		ControlPanel();
		void setup(String plantNames[], unsigned char plantNameCount);
        void addSelectWheel(SelectWheel& selectWheel);
        void addButton(ControlPanel::ButtonName buttonName, Button& button);
		void addScreen(ScreenName sType, Screen screen);
		bool canShowDisplay();
		void turnOffDisplay();
		void turnOffLeds();
		std::vector<double> update(std::vector<double> data, bool isPumpRunning);
		bool isEmergency();
		bool isDisplayActive();
		void setupScreens(String plantNames[], unsigned char plantNameCount);
		void updateButtons();
		bool wasAnyButtonClicked();
		bool wasAnythingClicked();
		std::vector<double> doNavigation();
		void goToSelectedScreen();
		void doScreenDisplay(std::vector<double> data);
		ScreenName getScreenName(String displayName);

	private:
        SelectWheel* selectWheel;
        std::map<ControlPanel::ButtonName, Button> buttons;
		// Use pointers to allow for type casting
        std::map<ScreenName, Screen*> screens;
};

#endif
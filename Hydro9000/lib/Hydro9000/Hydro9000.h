
#ifndef Hydro9000_h
#define Hydro9000_h

#include "Arduino.h"
#include "../Screen/Screen.h"
#include "../ControlPanel/ControlPanel.h"
#include "../PlantController/PlantController.h"
#include "../ScreenBarGraph/ScreenBarGraph.h"
#include "../ScreenMenu/ScreenMenu.h"

class Hydro9000 {
	public: 
		const static unsigned char MAX_COUNT = 5;
		static unsigned long currentMillis;
		static void setCurrentMillis(unsigned long millis) {
			Hydro9000::currentMillis = millis;
			ControlPanel::setCurrentMillis(millis);
		}
		bool isMultiplePumpsAtSameAllowed = false, isManualWateringActive = false;
		unsigned char plantCount = 0;
		unsigned char selectedPlantId;

		Hydro9000();
		void update();
		void addPlantController(PlantController controller);
		void addControlPanel(ControlPanel& controlPanel);
		void setup();
		void updatePlantControllers();
		std::vector<double> getData();
		void doEmergencyStop();
		std::vector<double> getGoals();
		void updatePumps(std::vector<double> pumpStatus);
		bool isPumpRunning();
		void saveGoal(double percentage);
		bool isAutoWateringActive();

	private:
		PlantController plantControllers[Hydro9000::MAX_COUNT];
		// PlantController activePlantController;
		ControlPanel controlPanel;
};

#endif
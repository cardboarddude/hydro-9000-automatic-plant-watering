
#ifndef Hydro9000_h
#define Hydro9000_h

#include "Arduino.h"
#include "PlantController/PlantController.h"
#include "ControlPanel/ControlPanel.h"

class Hydro9000 {
	private:
		std::vector<PlantController> plantControllers;
		PlantController activePlantController;
		ControlPanel controlPanel;

	public: 
		static unsigned long currentMillis;
		static void setCurrentMillis(unsigned long millis) {
			Hydro9000::currentMillis = millis;
			ControlPanel::setCurrentMillis(millis);
		}
		bool isMultiplePumpsAtSameAllowed = false;

		Hydro9000();
		void update();
		void addPlantController(PlantController controller);
		void addControlPanel(ControlPanel controlPanel);
		void setup();
		void doEmergencyStop();
		std::vector<float> getGoals();
};

#endif

#ifndef PlantController_h
#define PlantController_h

#include "Arduino.h"
#include "../WaterPump/WaterPump.h"
#include "../MoistureSensor/MoistureSensor.h"

class PlantController {
	public:
        PlantController();
        PlantController(MoistureSensor sensor, WaterPump pump);
        void setup();
        void update();
        void stopPump();
        float getGoal();
        String name;
        String displayName;
        
    private:
        MoistureSensor sensor;
        WaterPump pump;
        bool doSaveData = false;
};

#endif
#ifndef WaterPump_h
#define WaterPump_h

#include "Arduino.h"
#include <vector>

class WaterPump {
    private: 
        int motorPin;

    public:
        bool isRunning;
        
        WaterPump();
        WaterPump(int motorPin);
        void setup();
        void start();
        void stop();
        void toggle();
};

#endif
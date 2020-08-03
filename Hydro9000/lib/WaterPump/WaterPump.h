#ifndef WaterPump_h
#define WaterPump_h

#include "Arduino.h"
#include <vector>

class WaterPump {
    private: 
        unsigned char motorPin;

    public:
        bool isRunning;
        
        WaterPump();
        WaterPump(unsigned char motorPin);
        void setup();
        void start();
        void stop();
        void toggle();
};

#endif
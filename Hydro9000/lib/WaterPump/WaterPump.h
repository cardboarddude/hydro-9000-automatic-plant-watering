#ifndef WaterPump_h
#define WaterPump_h

#include "Arduino.h"
#include <vector>

class WaterPump {
    private: 

    public:
        bool isRunning;
        unsigned char motorPin;
        
        WaterPump();
        WaterPump(unsigned char motorPin);
        void setup();
        void start();
        void stop();
        void toggle();
};

#endif
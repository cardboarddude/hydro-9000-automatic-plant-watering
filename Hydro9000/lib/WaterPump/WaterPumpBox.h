#ifndef WaterPumpBox_h
#define WaterPumpBox_h

#include "Arduino.h"
#include "WaterPump.h"
#include <vector>

class WaterPumpBox {
    private:
        std::vector<WaterPump> pumps;
        
    public:
        WaterPumpBox();
        WaterPumpBox(std::vector<WaterPump> pumps);
        WaterPumpBox(WaterPump pumps[], unsigned int size);
        void addPump(WaterPump pump);
        void setup();
        void startPump(unsigned int pumpIndex);
        void stopPump(unsigned int pumpIndex);
        void togglePump(unsigned int pumpIndex);
        void stopAllPumps();
        void stopAllPumpsExcept(unsigned int pumpIndex);
        bool isPumpRunning();
};

#endif
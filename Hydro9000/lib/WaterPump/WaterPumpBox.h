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
        WaterPumpBox(WaterPump pumps[], int size);
        void addPump(WaterPump pump);
        void setup();
        void startPump(int pumpIndex);
        void stopPump(int pumpIndex);
        void togglePump(int pumpIndex);
        void stopAllPumps();
        void stopAllPumpsExcept(int pumpIndex);
        bool isPumpRunning();
};

#endif
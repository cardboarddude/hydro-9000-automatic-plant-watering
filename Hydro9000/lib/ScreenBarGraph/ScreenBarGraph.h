#ifndef ScreenBarGraph_h
#define ScreenBarGraph_h

#include "../Screen/Screen.h"
#include "vector"

class ScreenBarGraph : public Screen {
	public:
        unsigned char startX = this->CONTENT_AREA_START_X, startY = this->CONTENT_AREA_START_Y;
        unsigned char width = this->CONTENT_AREA_WIDTH, height = this->CONTENT_AREA_HEIGHT;
        unsigned char barSpacing = 1;
        double minValue = 0.0, maxValue = 1.0;
        
        ScreenBarGraph();
        ScreenBarGraph(unsigned char id, String title);
        virtual void doDisplay();
        virtual void clear();
        void addBar(double value, String label);

        void doDisplayBar(std::vector<Point>& position);
        void doDisplayLabel(int labelIndex, std::vector<Point>& barPosition);
        DisplayText getLabel(int labelIndex, std::vector<Point>& barPosition);
        std::vector<Point> getBarPosition(int index, int barWidth = -1);
        unsigned char getBarHeight(unsigned char index);
        unsigned char getBarWidth();

    private: 
        std::vector<String> labels;
        std::vector<double> values;
};

#endif
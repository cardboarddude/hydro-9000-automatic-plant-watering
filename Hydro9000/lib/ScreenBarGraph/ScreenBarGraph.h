#ifndef ScreenBarGraph_h
#define ScreenBarGraph_h

#include "../Screen/Screen.h"
#include "vector"

class ScreenBarGraph : public Screen {
	public:
        int startX = this->contentAreaStartX, startY = this->contentAreaStartY;
        int width = this->contentAreaWidth, height = this->contentAreaHeight;
        int barSpacing = 1;
        double minValue = 0.0, maxValue = 1.0;
        
        ScreenBarGraph();
        ScreenBarGraph(String title);
        virtual void doDisplay();
        void addBar(double value, String label);

        void doDisplayBar(Point position[2]);
        void doDisplayLabel(int labelIndex, Point barPosition[2]);
        DisplayText getLabel(int labelIndex, Point barPosition[2]);
        Point* getBarPosition(int index, int barWidth = -1);
        int getBarHeight(int index);
        int getBarWidth();

    private: 
        std::vector<double> values;
        std::vector<String> labels;
};

#endif
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
        virtual void clear();
        void addBar(double value, String label);

        void doDisplayBar(std::vector<Point>& position);
        void doDisplayLabel(int labelIndex, std::vector<Point>& barPosition);
        DisplayText getLabel(int labelIndex, std::vector<Point>& barPosition);
        std::vector<Point> getBarPosition(int index, int barWidth = -1);
        int getBarHeight(int index);
        int getBarWidth();

    private: 
        std::vector<String> labels;
        std::vector<double> values;
};

#endif
#ifndef ScreenLineGraph_h
#define ScreenLineGraph_h

#include "../Screen/Screen.h"
#include "vector"

class ScreenLineGraph : public Screen {
	public:
        unsigned char startX = this->CONTENT_AREA_START_X+DisplayText::getFontCharHeight(DisplayText::DEFAULT_FONT_SIZE);
        unsigned char startY = this->CONTENT_AREA_START_Y;
        unsigned char width = this->CONTENT_AREA_WIDTH-DisplayText::getFontCharHeight(DisplayText::DEFAULT_FONT_SIZE);
        unsigned char height = this->CONTENT_AREA_HEIGHT-DisplayText::getFontCharHeight(DisplayText::DEFAULT_FONT_SIZE);
        double minValue = 2147483648, maxValue = -2147483648;
        bool isDynamicMinMax = false;
        
        ScreenLineGraph();
        ScreenLineGraph(unsigned char id, String title, String xAxisLabel, String yAxisLabel);
        virtual void doDisplay();
        virtual void clear();
        void addPoint(double value);

        void doDisplayPoints();

    private:
        std::vector<double> values;
        DisplayText xAxisLabel, yAxisLabel;
};

#endif
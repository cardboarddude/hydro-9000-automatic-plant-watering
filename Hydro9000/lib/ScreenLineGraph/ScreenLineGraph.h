#ifndef ScreenLineGraph_h
#define ScreenLineGraph_h

#include "../Screen/Screen.h"
#include "vector"
#include <limits>

class ScreenLineGraph : public Screen {
	public:
        unsigned char startX = this->contentAreaStartX+DisplayText::getFontCharHeight(DisplayText::DEFAULT_FONT_SIZE);
        unsigned char startY = this->contentAreaStartY;
        unsigned char width = this->contentAreaWidth-DisplayText::getFontCharHeight(DisplayText::DEFAULT_FONT_SIZE);
        unsigned char height = this->contentAreaHeight-DisplayText::getFontCharHeight(DisplayText::DEFAULT_FONT_SIZE);
        double minValue = std::numeric_limits<double>::max(), maxValue = std::numeric_limits<double>::min();
        bool isDynamicMinMax = false;
        
        ScreenLineGraph();
        ScreenLineGraph(String title, String xAxisLabel, String yAxisLabel);
        virtual void doDisplay();
        virtual void clear();
        void addPoint(double value);

        void doDisplayPoints();

    private:
        std::vector<double> values;
        DisplayText xAxisLabel, yAxisLabel;
};

#endif
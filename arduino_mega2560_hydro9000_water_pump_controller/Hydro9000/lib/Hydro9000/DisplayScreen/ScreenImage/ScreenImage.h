#ifndef ScreenImage_h
#define ScreenImage_h

#include "Screen.h"

class ScreenImage : public Screen {
	public:

        ScreenImage();
        ScreenImage(String title, const unsigned char* imageBitmap, int height, int width);
        ScreenImage(DisplayText title, const unsigned char* imageBitmap, int height, int width);
        virtual void doDisplay();
        Point getImageTopLeftPoint();
    private: 
        const unsigned char* imageContent;
        int imageHeight, imageWidth;
};

#endif
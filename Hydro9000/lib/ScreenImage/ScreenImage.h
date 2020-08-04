#ifndef ScreenImage_h
#define ScreenImage_h

#include "../Screen/Screen.h"

class ScreenImage : public Screen {
	public:

        ScreenImage();
        ScreenImage(unsigned char id, String title, const unsigned char* imageBitmap, int width, int height);
        ScreenImage(unsigned char id, DisplayText& title, const unsigned char* imageBitmap, int width, int height);
        virtual void doDisplay();
        Point getImageTopLeftPoint();
    private: 
        const unsigned char* imageContent;
        int imageHeight, imageWidth;
};

#endif
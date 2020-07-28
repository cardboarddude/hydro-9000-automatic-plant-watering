#ifndef Point_h
#define Point_h

#include <WString.h>

class Point {
    public:
        int x, y;

        Point();
        Point(int x, int y);
        String toString();
};

#endif
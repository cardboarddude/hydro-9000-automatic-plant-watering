#include "Point.h"

Point::Point() {}
Point::Point(int x = 0, int y = 0) {
    this->x = x;
    this->y = y;
}
String Point::toString() {
    return "("+String(this->x)+", "+String(this->y)+")";
}
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include "dynamicarray.h"
#include "vector2.h"

typedef VECTOR2(float) Point;
CREATE_ARRAY_TYPE(Point);

typedef struct{
    unsigned char *buf;
    size_t w;
    size_t h;
    size_t stride;
}ImageBuffer;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} BufColor;

void fill_polygon(ARRAY(Point) pts, ImageBuffer buffer, BufColor color);
int bezier_to(ARRAY(Point)* pts, Point p2, Point p3, Point p4, int n);

#endif //_SHAPE_H_

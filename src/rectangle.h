#ifndef _RECTANGLE_H
#define _RECTANGLE_H

#include "../vendor/raylib/raylib.h"

#define RECTANGLE(type) Rectangle_##type

#define CREATE_RECTANGLE_TYPE(type)                      \
    typedef struct {                                     \
        type x;                                          \
        type y;                                          \
        type w;                                          \
        type h;                                          \
    } RECTANGLE(type);


typedef struct{
    Rectangle rec;
    float roundness;
    int segments;
    Vector2 start;
    Vector2 end;
    Color left;
    Color right;
}RectangleDrawOption;
void DrawRectangleRoundedGradient( RectangleDrawOption opt);

#endif // _RECTANGLE_H

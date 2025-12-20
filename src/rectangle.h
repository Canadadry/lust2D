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


void DrawRectangleRoundedGradient(Rectangle rec,float roundness,Vector2 start,Vector2 end,Color from,Color to);

#endif // _RECTANGLE_H

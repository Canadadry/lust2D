#ifndef _RECTANGLE_H
#define _RECTANGLE_H


#define RECTANGLE(type) Rectangle_##type

#define CREATE_RECTANGLE_TYPE(type)                      \
    typedef struct {                                     \
        type x;                                          \
        type y;                                          \
        type w;                                          \
        type h;                                          \
    } RECTANGLE(type);

#endif // _VECTOR2_H

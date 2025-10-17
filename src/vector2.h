#ifndef _VECTOR2_H
#define _VECTOR2_H


#define VECTOR2(type) Vector2_##type

#define CREATE_VECTOR2_TYPE(type)                        \
    typedef struct {                                     \
        type x;                                          \
        type y;                                          \
    } VECTOR2(type);

CREATE_VECTOR2_TYPE(int)

#endif // _VECTOR2_H

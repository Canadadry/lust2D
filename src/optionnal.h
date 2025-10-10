#ifndef _OPTIONAL_H
#define _OPTIONAL_H

#include <stdbool.h>

#define OPTIONAL(type) Optional_##type

#define CREATE_OPTIONAL_TYPE(type)                        \
    typedef struct {                                      \
        type value;                                       \
        bool ok;                                          \
    } OPTIONAL(type);

#endif // _OPTIONAL_H

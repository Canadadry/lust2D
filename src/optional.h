#ifndef _OPTIONAL_H
#define _OPTIONAL_H

#include <stdbool.h>

#define OPTIONAL(type) Optional_##type

#define OPTIONAL_OK(type,val) (Optional_##type){.value=val,.ok=true}
#define OPTIONAL_NONE(type) (Optional_##type){.ok=false}

#define CREATE_OPTIONAL_TYPE(type)                        \
    typedef struct {                                      \
        type value;                                       \
        bool ok;                                          \
    } OPTIONAL(type);

#endif // _OPTIONAL_H

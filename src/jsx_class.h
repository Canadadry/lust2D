#ifndef _JSX_CLASS_H
#define _JSX_CLASS_H
#include "hashmap.h"
#include "dynamicarray.h"

#include "ui.h"

typedef void (*init_node_fn)(Node*);
typedef void (*init_node_fn1)(Node*,int);

typedef struct{
    const char* regexp;
    const char* scanf;
    init_node_fn1 fn;
} InitNodeFn1;

CREATE_HASHMAP(init_node_fn);
CREATE_ARRAY_TYPE(InitNodeFn1);

void init_init_node_fn_hashmap(HASHMAP(init_node_fn)* hmap,ARRAY(InitNodeFn1)* array1);

#endif //_JSX_CLASS_H

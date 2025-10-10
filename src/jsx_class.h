#ifndef _JSX_CLASS_H
#define _JSX_CLASS_H
#include "hashmap.h"
#include "ui.h"

typedef void (*init_node_fn)(Node*);

CREATE_HASHMAP(init_node_fn);

void init_init_node_fn_hashmap(HASHMAP(init_node_fn)* hmap);

#endif //_JSX_CLASS_H

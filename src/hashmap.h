#ifndef _HASHMAP_H
#define _HASHMAP_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "dynamicarray.h"

typedef enum{
  UpsertActionCreate,
  UpsertActionUpdate,
  UpsertActionDelete,
} UpsertAction;

unsigned long hash(const char *s) ;

#define HASHMAP(VALUE) VALUE##HashMap
#define HASMMAP_KEY_LEN 255

#define CREATE_HASHMAP(VALUE)                                                      \
typedef struct {                                                                   \
    char key[HASMMAP_KEY_LEN];                                                     \
    VALUE value;                                                                   \
    int children[4];                                                               \
} VALUE##HashMapCell;                                                              \
CREATE_ARRAY_TYPE(VALUE##HashMapCell);                                             \
typedef struct {                                                                   \
    ARRAY(VALUE##HashMapCell) data;                                                \
} VALUE##HashMap;                                                                  \
                                                                                   \
VALUE* VALUE##_upsert(VALUE##HashMap *m, const char *key, UpsertAction action);

#define WRITE_HASHMAP_IMPL(VALUE)                                                                    \
                                                                                                     \
WRITE_ARRAY_IMPL(VALUE##HashMapCell);                                                                \
                                                                                                     \
VALUE* VALUE##_upsert(VALUE##HashMap *m, const char *key, UpsertAction action) {                     \
    if(key==NULL){                                                                                   \
        return NULL;                                                                                 \
    }                                                                                                \
    uint64_t h = hash(key);                                                                          \
                                                                                                     \
    for (int cell_index = 0; cell_index < m->data.len; h <<= 2) {                                    \
        VALUE##HashMapCell *cell = &m->data.data[cell_index];                                        \
        if(cell->key[0]==0){                                                                         \
            switch(action){                                                                          \
                case UpsertActionCreate:                                                             \
                    strncpy(cell->key, key, HASMMAP_KEY_LEN);                                        \
                    return &cell->value;                                                             \
                case UpsertActionUpdate:                                                             \
                case UpsertActionDelete:                                                             \
                    return NULL;                                                                     \
            }                                                                                        \
        }                                                                                            \
        if (strncmp(cell->key, key,HASMMAP_KEY_LEN)==0) {                                            \
            switch(action){                                                                          \
                case UpsertActionCreate:                                                             \
                case UpsertActionUpdate:                                                             \
                    return &cell->value;                                                             \
                case UpsertActionDelete:                                                             \
                    cell->key[0] = 0;                                                                \
                    return NULL;                                                                     \
            }                                                                                        \
        }                                                                                            \
                                                                                                     \
        int last_child = (h >> 62) & 0x03;                                                           \
        int next_cell = cell->children[last_child];                                                  \
                                                                                                     \
        if (next_cell == 0) {                                                                        \
            cell->children[last_child]=m->data.len;                                                  \
        }                                                                                            \
        cell_index = next_cell;                                                                      \
    }                                                                                                \
    if(action!=UpsertActionCreate){                                                                  \
        return NULL;                                                                                 \
    }                                                                                                \
                                                                                                     \
    VALUE##HashMapCell new_cell = { 0 };                                                             \
    strncpy(new_cell.key, key, HASMMAP_KEY_LEN);                                                    \
    array_append_##VALUE##HashMapCell(&m->data, new_cell);                                           \
    return &m->data.data[m->data.len - 1].value;                                                     \
}


#endif //_HASHMAP_H

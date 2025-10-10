#ifndef _SPLITTER_H
#define _SPLITTER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    const char* data;
    size_t len;
} SplitterSlice;

typedef struct {
    const char* str;
    const char* start;
    char separator;
} Splitter;

Splitter* init_splitter(Splitter* splitter,const char* str, char separator);
SplitterSlice next_slice(Splitter* splitter);

#endif //_SPLITTER_H

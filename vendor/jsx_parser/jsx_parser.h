#ifndef JSX_PARSER_H
#define JSX_PARSER_H

#include <stdlib.h>

typedef struct{
    void* (*realloc_fn)(void* userdata,void* ptr, size_t size);
    void (*free_fn)(void* userdata,void* ptr);
    void* userdata;
} JSX_Allocator;

typedef struct JSX_Compiler JSX_Compiler;

JSX_Compiler* jsx_new_compiler(const char* createElem,JSX_Allocator allocator);
void jsx_free_compiler(JSX_Compiler* c);
bool jsx_compile(JSX_Compiler* c,const char* in,size_t len );
char* jsx_get_last_error(JSX_Compiler* c);
char* jsx_get_output(JSX_Compiler* c);

#endif

#ifndef _ARGS_H
#define _ARGS_H

typedef struct{
    const char *next;
    int ok;
}HasArgResult;

#define has_arg(argc,argv,...) __has_arg((argc),(argv),__VA_ARGS__,NULL)

HasArgResult __has_arg(int argc, char **argv, ...);

#endif //_ARGS_H

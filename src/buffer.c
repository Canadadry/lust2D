#include "buffer.h"
#include "dynamicarray.h"
#include <string.h>

#define MIN(x,y) ((x)<(y)?(x):(y))

WRITE_ARRAY_IMPL(char)

 void buf_write_string(Buffer* b,const char* str){
     int len =  strlen(str);
    if(!array_reserve_char(b,len)){
        return;
    }
    memcpy(b->data+b->len, str,len);
    b->len+=len;
}

 void buf_write_char(Buffer* b,char c){
     if(!array_reserve_char(b,1)){
         return;
     }
    b->data[b->len]=c;
    b->len++;
}

void buf_clear(Buffer* b){
    b->len=0;
}

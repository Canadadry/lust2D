#ifndef _BUFFER_H
#define _BUFFER_H

#include <stdio.h>
#include "dynamicarray.h"

CREATE_ARRAY_TYPE(char);
typedef ARRAY(char) Buffer;

void buf_write_string(Buffer* b,const char* str);
void buf_write_char(Buffer* b,char c);
void buf_clear(Buffer* b);

#endif

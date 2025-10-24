#ifndef _RAYLIB_JS_H
#define _RAYLIB_JS_H

#include "../vendor/mujs/mujs.h"
#include "../vendor/raylib/raylib.h"
#include "hashmap.h"

CREATE_HASHMAP(Texture)
CREATE_HASHMAP(Font)
CREATE_HASHMAP(Sound)
CREATE_HASHMAP(Image)

typedef struct{
    int width;
    int height;
    const char* title;
} Window;

Window get_window(js_State *J);
Color get_color(js_State *J, int idx);
Rectangle get_rectangle_or(js_State *J, int idx,Rectangle def);
Rectangle get_rectangle(js_State *J, int idx);
void bind_raylib_func(js_State *J,Allocator alloc);

#endif //_JSX_CLASS_H

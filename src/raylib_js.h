#ifndef _RAYLIB_JS_H
#define _RAYLIB_JS_H

#include "../vendor/mujs/mujs.h"
#include "hashmap.h"
#include "../vendor/raylib/raylib.h"

CREATE_HASHMAP(Texture)
CREATE_HASHMAP(Font)

void bind_raylib_func(js_State *J,Allocator alloc);

#endif //_JSX_CLASS_H

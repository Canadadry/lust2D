#ifndef _RAYLIB_JS_H
#define _RAYLIB_JS_H

#include "../vendor/mujs/mujs.h"
#include "hashmap.h"
#include "../vendor/raylib/raylib.h"

CREATE_HASHMAP(Texture)

void bind_raylib_func(js_State *J);

#endif //_JSX_CLASS_H

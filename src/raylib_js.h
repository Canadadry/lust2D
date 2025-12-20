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

typedef struct{
    enum{
        COLOR_PATTERN_SOLID,
        COLOR_PATTERN_GRADIENT
    } kind;
    union{
        Color color;
        struct {
            Color color;
            Vector2 at;
        } stops[2];
    } value;
} ColorPattern;


Window get_window(js_State *J);
ColorPattern get_color_pattern(js_State *J, int idx);
Color get_color(js_State *J, int idx);
Rectangle get_rectangle_or(js_State *J, int idx,Rectangle def);
Rectangle get_rectangle(js_State *J, int idx);
Vector2 get_vector2_or(js_State *J, int idx,Vector2 def);
Vector2 get_vector2(js_State *J, int idx);
Vector2 get_property_vector2(js_State *J, int idx,const char* name);
void bind_raylib_func(js_State *J,Allocator alloc);

#define read_property(J,idx,name,func,varname) \
    do{\
        int top =js_gettop((J));\
       	if( js_hasproperty((J), (idx), (name)) != 0) {\
            js_getproperty(J, idx, name);\
            (varname) = (func)((J),(top));\
           	js_pop((J), 1);\
    	}\
    }while(0)

#endif //_JSX_CLASS_H

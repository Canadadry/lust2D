#include "raylib_js.h"
#include "../vendor/raylib/raylib.h"
#include "js_helper.h"

WRITE_HASHMAP_IMPL(Texture)

static Rectangle get_rectangle(js_State *J, int idx)  {
    if (js_isobject(J, idx) == 0) {
        return (Rectangle){.width = 100, .height = 100};
	}
	return (Rectangle){
		.x =      (float)get_property_number_or(J, idx, "x", 0),
		.y =      (float)get_property_number_or(J, idx, "y", 0),
		.width =  (float)get_property_number_or(J, idx, "w", 100),
		.height = (float)get_property_number_or(J, idx, "h", 100),
	};
}


static Color get_color(js_State *J, int idx)  {
	if (js_isobject(J, idx) == 0) {
		return WHITE;
	};
	return (Color){
  		.r = (unsigned char)get_property_number_or(J, idx, "r", 255),
  		.g = (unsigned char)get_property_number_or(J, idx, "g", 255),
  		.b = (unsigned char)get_property_number_or(J, idx, "b", 255),
  		.a = (unsigned char)get_property_number_or(J, idx, "a", 255),
   	};
}

static void clear_background(js_State *J) {
	ClearBackground(get_color(J, 1));
}

static void draw_rectangle_rec(js_State *J) {
	DrawRectangleRec(get_rectangle(J, 1), get_color(J, 2));
}


void bind_raylib_func(js_State *J){
    js_newcfunction(J, clear_background, "ClearBackground", 0);
	js_setglobal(J, "ClearBackground");
	js_newcfunction(J, draw_rectangle_rec, "DrawRectangleRec", 0);
	js_setglobal(J, "DrawRectangleRec");
}

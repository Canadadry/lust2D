#include "raylib_js.h"
#include "../vendor/raylib/raylib.h"
#include "allocator.h"
#include "hashmap.h"
#include "js_helper.h"
#include <stdio.h>
#include <string.h>


extern HASHMAP(Texture)* hmap_texture;

WRITE_HASHMAP_IMPL(Texture)
WRITE_HASHMAP_IMPL(Font)

CREATE_HASHMAP(KeyboardKey)
WRITE_HASHMAP_IMPL(KeyboardKey)

HASHMAP(KeyboardKey) hmap_keyboard_key;

Window get_window(js_State *J) {
	Window win = {.width=800,.height=600,.title="no title"};
	int top = js_gettop(J);
	js_getglobal(J, "window");
	if(js_isobject(J, top)){
    	win.width = get_property_int_or(J,top,"width",win.width);
    	win.height = get_property_int_or(J,top,"height",win.height);
    	win.title = get_property_string_or(J,top,"title",win.title);
    	js_pop(J,1);
	}
	return win;
}


static Rectangle get_rectangle_or(js_State *J, int idx,Rectangle def)  {
    if (js_isobject(J, idx) == 0) {
        return def;
	}
	return (Rectangle){
		.x =      (float)get_property_number_or(J, idx, "x", 0),
		.y =      (float)get_property_number_or(J, idx, "y", 0),
		.width =  (float)get_property_number_or(J, idx, "w", 100),
		.height = (float)get_property_number_or(J, idx, "h", 100),
	};
}

static Rectangle get_rectangle(js_State *J, int idx)  {
    return get_rectangle_or(J,idx,(Rectangle){.width = 100, .height = 100});
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

static KeyboardKey key_from(const char* name){
    KeyboardKey* key = KeyboardKey_upsert(&hmap_keyboard_key,name, UpsertActionUpdate);
    if(key ==NULL){
        return KEY_NULL;
    }
    return *key;
}

void is_key_pressed(js_State *J) {
    if(!js_isstring(J,-1)){
        js_pushboolean(J,0);
        return;
    }
	js_pushboolean(J,IsKeyPressed(key_from(js_tostring(J,-1))));
}

void is_key_released(js_State *J) {
    if(!js_isstring(J,-1)){
        js_pushboolean(J,0);
        return;
    }
	js_pushboolean(J,IsKeyReleased(key_from(js_tostring(J,-1))));
}

void is_key_up(js_State *J) {
    if(!js_isstring(J,-1)){
        js_pushboolean(J,0);
        return;
    }
	js_pushboolean(J,IsKeyUp(key_from(js_tostring(J,-1))));
}

void is_key_down(js_State *J) {
    if(!js_isstring(J,-1)){
        js_pushboolean(J,0);
        return;
    }
	js_pushboolean(J,IsKeyDown(key_from(js_tostring(J,-1))));
}

void clear_background(js_State *J) {
	ClearBackground(get_color(J, 1));
}

void draw_rectangle_rec(js_State *J) {
	DrawRectangleRec(get_rectangle(J, 1), get_color(J, 2));
}

void draw_image_pro(js_State *J) {
    int idx_src=1;
    int idx_rect_src=2;
    int idx_rect_target=3;
    // int idx_origin=4;
    // int idx_rot=5;
    // int idx_color=6;
    const char* src =js_tostring(J,idx_src);
    Texture* t = Texture_upsert(hmap_texture,src , UpsertActionCreate);
    if(t==NULL){
        js_pushundefined(J);
        return;
    }
    if(t->id==0){
        *t = LoadTexture(src);
    }
    if(t->id!=0){
        DrawTexturePro(
            *t,
            get_rectangle_or(J,idx_rect_src,(Rectangle){
                .x=0,.y=0,
                .width=(float)t->width,
                .height=(float)t->height,
            }),
            get_rectangle(J,idx_rect_target),
            (Vector2){0} , 0, WHITE
        );
    }
    js_pushundefined(J);
}


void init_keyboard_key_hmap(Allocator alloc){
    hmap_keyboard_key.data.alloc=alloc;
    *KeyboardKey_upsert(&hmap_keyboard_key, "apostrophe", UpsertActionCreate)=KEY_APOSTROPHE;
    *KeyboardKey_upsert(&hmap_keyboard_key, "comma", UpsertActionCreate)=KEY_COMMA;
    *KeyboardKey_upsert(&hmap_keyboard_key, "minus", UpsertActionCreate)=KEY_MINUS;
    *KeyboardKey_upsert(&hmap_keyboard_key, "period", UpsertActionCreate)=KEY_PERIOD;
    *KeyboardKey_upsert(&hmap_keyboard_key, "slash", UpsertActionCreate)=KEY_SLASH;
    *KeyboardKey_upsert(&hmap_keyboard_key, "zero", UpsertActionCreate)=KEY_ZERO;
    *KeyboardKey_upsert(&hmap_keyboard_key, "one", UpsertActionCreate)=KEY_ONE;
    *KeyboardKey_upsert(&hmap_keyboard_key, "two", UpsertActionCreate)=KEY_TWO;
    *KeyboardKey_upsert(&hmap_keyboard_key, "three", UpsertActionCreate)=KEY_THREE;
    *KeyboardKey_upsert(&hmap_keyboard_key, "four", UpsertActionCreate)=KEY_FOUR;
    *KeyboardKey_upsert(&hmap_keyboard_key, "five", UpsertActionCreate)=KEY_FIVE;
    *KeyboardKey_upsert(&hmap_keyboard_key, "six", UpsertActionCreate)=KEY_SIX;
    *KeyboardKey_upsert(&hmap_keyboard_key, "seven", UpsertActionCreate)=KEY_SEVEN;
    *KeyboardKey_upsert(&hmap_keyboard_key, "eight", UpsertActionCreate)=KEY_EIGHT;
    *KeyboardKey_upsert(&hmap_keyboard_key, "nine", UpsertActionCreate)=KEY_NINE;
    *KeyboardKey_upsert(&hmap_keyboard_key, "semicolon", UpsertActionCreate)=KEY_SEMICOLON;
    *KeyboardKey_upsert(&hmap_keyboard_key, "equal", UpsertActionCreate)=KEY_EQUAL;
    *KeyboardKey_upsert(&hmap_keyboard_key, "a", UpsertActionCreate)=KEY_A;
    *KeyboardKey_upsert(&hmap_keyboard_key, "b", UpsertActionCreate)=KEY_B;
    *KeyboardKey_upsert(&hmap_keyboard_key, "c", UpsertActionCreate)=KEY_C;
    *KeyboardKey_upsert(&hmap_keyboard_key, "d", UpsertActionCreate)=KEY_D;
    *KeyboardKey_upsert(&hmap_keyboard_key, "e", UpsertActionCreate)=KEY_E;
    *KeyboardKey_upsert(&hmap_keyboard_key, "f", UpsertActionCreate)=KEY_F;
    *KeyboardKey_upsert(&hmap_keyboard_key, "g", UpsertActionCreate)=KEY_G;
    *KeyboardKey_upsert(&hmap_keyboard_key, "h", UpsertActionCreate)=KEY_H;
    *KeyboardKey_upsert(&hmap_keyboard_key, "i", UpsertActionCreate)=KEY_I;
    *KeyboardKey_upsert(&hmap_keyboard_key, "j", UpsertActionCreate)=KEY_J;
    *KeyboardKey_upsert(&hmap_keyboard_key, "k", UpsertActionCreate)=KEY_K;
    *KeyboardKey_upsert(&hmap_keyboard_key, "l", UpsertActionCreate)=KEY_L;
    *KeyboardKey_upsert(&hmap_keyboard_key, "m", UpsertActionCreate)=KEY_M;
    *KeyboardKey_upsert(&hmap_keyboard_key, "n", UpsertActionCreate)=KEY_N;
    *KeyboardKey_upsert(&hmap_keyboard_key, "o", UpsertActionCreate)=KEY_O;
    *KeyboardKey_upsert(&hmap_keyboard_key, "p", UpsertActionCreate)=KEY_P;
    *KeyboardKey_upsert(&hmap_keyboard_key, "q", UpsertActionCreate)=KEY_Q;
    *KeyboardKey_upsert(&hmap_keyboard_key, "r", UpsertActionCreate)=KEY_R;
    *KeyboardKey_upsert(&hmap_keyboard_key, "s", UpsertActionCreate)=KEY_S;
    *KeyboardKey_upsert(&hmap_keyboard_key, "t", UpsertActionCreate)=KEY_T;
    *KeyboardKey_upsert(&hmap_keyboard_key, "u", UpsertActionCreate)=KEY_U;
    *KeyboardKey_upsert(&hmap_keyboard_key, "v", UpsertActionCreate)=KEY_V;
    *KeyboardKey_upsert(&hmap_keyboard_key, "w", UpsertActionCreate)=KEY_W;
    *KeyboardKey_upsert(&hmap_keyboard_key, "x", UpsertActionCreate)=KEY_X;
    *KeyboardKey_upsert(&hmap_keyboard_key, "y", UpsertActionCreate)=KEY_Y;
    *KeyboardKey_upsert(&hmap_keyboard_key, "z", UpsertActionCreate)=KEY_Z;
    *KeyboardKey_upsert(&hmap_keyboard_key, "left_bracket", UpsertActionCreate)=KEY_LEFT_BRACKET;
    *KeyboardKey_upsert(&hmap_keyboard_key, "backslash", UpsertActionCreate)=KEY_BACKSLASH;
    *KeyboardKey_upsert(&hmap_keyboard_key, "right_bracket", UpsertActionCreate)=KEY_RIGHT_BRACKET;
    *KeyboardKey_upsert(&hmap_keyboard_key, "grave", UpsertActionCreate)=KEY_GRAVE;
    *KeyboardKey_upsert(&hmap_keyboard_key, "space", UpsertActionCreate)=KEY_SPACE;
    *KeyboardKey_upsert(&hmap_keyboard_key, "escape", UpsertActionCreate)=KEY_ESCAPE;
    *KeyboardKey_upsert(&hmap_keyboard_key, "enter", UpsertActionCreate)=KEY_ENTER;
    *KeyboardKey_upsert(&hmap_keyboard_key, "tab", UpsertActionCreate)=KEY_TAB;
    *KeyboardKey_upsert(&hmap_keyboard_key, "backspace", UpsertActionCreate)=KEY_BACKSPACE;
    *KeyboardKey_upsert(&hmap_keyboard_key, "insert", UpsertActionCreate)=KEY_INSERT;
    *KeyboardKey_upsert(&hmap_keyboard_key, "delete", UpsertActionCreate)=KEY_DELETE;
    *KeyboardKey_upsert(&hmap_keyboard_key, "right", UpsertActionCreate)=KEY_RIGHT;
    *KeyboardKey_upsert(&hmap_keyboard_key, "left", UpsertActionCreate)=KEY_LEFT;
    *KeyboardKey_upsert(&hmap_keyboard_key, "down", UpsertActionCreate)=KEY_DOWN;
    *KeyboardKey_upsert(&hmap_keyboard_key, "up", UpsertActionCreate)=KEY_UP;
    *KeyboardKey_upsert(&hmap_keyboard_key, "page_up", UpsertActionCreate)=KEY_PAGE_UP;
    *KeyboardKey_upsert(&hmap_keyboard_key, "page_down", UpsertActionCreate)=KEY_PAGE_DOWN;
    *KeyboardKey_upsert(&hmap_keyboard_key, "home", UpsertActionCreate)=KEY_HOME;
    *KeyboardKey_upsert(&hmap_keyboard_key, "end", UpsertActionCreate)=KEY_END;
    *KeyboardKey_upsert(&hmap_keyboard_key, "caps_lock", UpsertActionCreate)=KEY_CAPS_LOCK;
    *KeyboardKey_upsert(&hmap_keyboard_key, "scroll_lock", UpsertActionCreate)=KEY_SCROLL_LOCK;
    *KeyboardKey_upsert(&hmap_keyboard_key, "num_lock", UpsertActionCreate)=KEY_NUM_LOCK;
    *KeyboardKey_upsert(&hmap_keyboard_key, "print_screen", UpsertActionCreate)=KEY_PRINT_SCREEN;
    *KeyboardKey_upsert(&hmap_keyboard_key, "pause", UpsertActionCreate)=KEY_PAUSE;
    *KeyboardKey_upsert(&hmap_keyboard_key, "f_1", UpsertActionCreate)=KEY_F1;
    *KeyboardKey_upsert(&hmap_keyboard_key, "f_2", UpsertActionCreate)=KEY_F2;
    *KeyboardKey_upsert(&hmap_keyboard_key, "f_3", UpsertActionCreate)=KEY_F3;
    *KeyboardKey_upsert(&hmap_keyboard_key, "f_4", UpsertActionCreate)=KEY_F4;
    *KeyboardKey_upsert(&hmap_keyboard_key, "f_5", UpsertActionCreate)=KEY_F5;
    *KeyboardKey_upsert(&hmap_keyboard_key, "f_6", UpsertActionCreate)=KEY_F6;
    *KeyboardKey_upsert(&hmap_keyboard_key, "f_7", UpsertActionCreate)=KEY_F7;
    *KeyboardKey_upsert(&hmap_keyboard_key, "f_8", UpsertActionCreate)=KEY_F8;
    *KeyboardKey_upsert(&hmap_keyboard_key, "f_9", UpsertActionCreate)=KEY_F9;
    *KeyboardKey_upsert(&hmap_keyboard_key, "f_10", UpsertActionCreate)=KEY_F10;
    *KeyboardKey_upsert(&hmap_keyboard_key, "f_11", UpsertActionCreate)=KEY_F11;
    *KeyboardKey_upsert(&hmap_keyboard_key, "f_12", UpsertActionCreate)=KEY_F12;
    *KeyboardKey_upsert(&hmap_keyboard_key, "left_shift", UpsertActionCreate)=KEY_LEFT_SHIFT;
    *KeyboardKey_upsert(&hmap_keyboard_key, "left_control", UpsertActionCreate)=KEY_LEFT_CONTROL;
    *KeyboardKey_upsert(&hmap_keyboard_key, "left_alt", UpsertActionCreate)=KEY_LEFT_ALT;
    *KeyboardKey_upsert(&hmap_keyboard_key, "left_super", UpsertActionCreate)=KEY_LEFT_SUPER;
    *KeyboardKey_upsert(&hmap_keyboard_key, "right_shift", UpsertActionCreate)=KEY_RIGHT_SHIFT;
    *KeyboardKey_upsert(&hmap_keyboard_key, "right_control", UpsertActionCreate)=KEY_RIGHT_CONTROL;
    *KeyboardKey_upsert(&hmap_keyboard_key, "right_alt", UpsertActionCreate)=KEY_RIGHT_ALT;
    *KeyboardKey_upsert(&hmap_keyboard_key, "right_super", UpsertActionCreate)=KEY_RIGHT_SUPER;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kb_menu", UpsertActionCreate)=KEY_KB_MENU;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_0", UpsertActionCreate)=KEY_KP_0;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_1", UpsertActionCreate)=KEY_KP_1;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_2", UpsertActionCreate)=KEY_KP_2;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_3", UpsertActionCreate)=KEY_KP_3;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_4", UpsertActionCreate)=KEY_KP_4;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_5", UpsertActionCreate)=KEY_KP_5;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_6", UpsertActionCreate)=KEY_KP_6;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_7", UpsertActionCreate)=KEY_KP_7;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_8", UpsertActionCreate)=KEY_KP_8;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_9", UpsertActionCreate)=KEY_KP_9;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_decimal", UpsertActionCreate)=KEY_KP_DECIMAL;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_divide", UpsertActionCreate)=KEY_KP_DIVIDE;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_multiply", UpsertActionCreate)=KEY_KP_MULTIPLY;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_subtract", UpsertActionCreate)=KEY_KP_SUBTRACT;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_add", UpsertActionCreate)=KEY_KP_ADD;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_enter", UpsertActionCreate)=KEY_KP_ENTER;
    *KeyboardKey_upsert(&hmap_keyboard_key, "kp_equal", UpsertActionCreate)=KEY_KP_EQUAL;
    *KeyboardKey_upsert(&hmap_keyboard_key, "back", UpsertActionCreate)=KEY_BACK;
    *KeyboardKey_upsert(&hmap_keyboard_key, "menu", UpsertActionCreate)=KEY_MENU;
    *KeyboardKey_upsert(&hmap_keyboard_key, "volume_up", UpsertActionCreate)=KEY_VOLUME_UP;
    *KeyboardKey_upsert(&hmap_keyboard_key, "volume_down", UpsertActionCreate)=KEY_VOLUME_DOWN;
}

int get_mouse_button(js_State *J){
    if(!js_isstring(J, 1)){
        js_pushboolean(J,false);
        js_error(J, "function is_mouse_button_pressed want one string agument");
        return -1;
    }
    MouseButton mouse_button = {0};
    const char * button = js_tostring(J,1);
    if(strcmp(button, "left")==0){
        mouse_button = MOUSE_LEFT_BUTTON;
    }else if(strcmp(button, "right")==0){
        mouse_button = MOUSE_RIGHT_BUTTON;
    }else{
        js_pushboolean(J,false);
        js_error(J, "function is_mouse_button_pressed want one string agument which could be ['left'|'right']");
        return -1;
    }
    return mouse_button;
}

void is_mouse_button_pressed(js_State *J){
    int mouse_button = get_mouse_button(J);
    if(mouse_button<0){
        return;
    }
    js_pushboolean(J,IsMouseButtonPressed(mouse_button));
}

void is_mouse_button_released(js_State *J){
    int mouse_button = get_mouse_button(J);
    if(mouse_button<0){
        return;
    }
    js_pushboolean(J,IsMouseButtonReleased(mouse_button));
}


void is_mouse_button_up(js_State *J){
    int mouse_button = get_mouse_button(J);
    if(mouse_button<0){
        return;
    }
    js_pushboolean(J,IsMouseButtonUp(mouse_button));
}

void is_mouse_button_down(js_State *J){
    int mouse_button = get_mouse_button(J);
    if(mouse_button<0){
        return;
    }
    js_pushboolean(J,IsMouseButtonDown(mouse_button));
}

void get_mouse_x(js_State *J){
    js_pushnumber(J,GetMouseX());
}

void get_mouse_y(js_State *J){
    js_pushnumber(J,GetMouseY());
}

void bind_raylib_func(js_State *J,Allocator alloc){
    init_keyboard_key_hmap(alloc);
    js_newcfunction(J, clear_background, "ClearBackground", 1);
	js_setglobal(J, "ClearBackground");
	js_newcfunction(J, draw_rectangle_rec, "DrawRectangleRec", 2);
	js_setglobal(J, "DrawRectangleRec");
	js_newcfunction(J, draw_image_pro, "DrawImagePro", 3);
	js_setglobal(J, "DrawImagePro");
	js_newcfunction(J, is_key_pressed, "is_key_pressed", 1);
	js_setglobal(J, "is_key_pressed");
	js_newcfunction(J, is_key_released, "is_key_released", 1);
	js_setglobal(J, "is_key_released");
	js_newcfunction(J, is_key_up, "is_key_up", 1);
	js_setglobal(J, "is_key_up");
	js_newcfunction(J, is_key_down, "is_key_down", 1);
	js_setglobal(J, "is_key_down");
	js_newcfunction(J, is_mouse_button_pressed, "is_mouse_button_pressed", 1);
	js_setglobal(J, "is_mouse_button_pressed");
	js_newcfunction(J, is_mouse_button_released, "is_mouse_button_released", 1);
	js_setglobal(J, "is_mouse_button_released");
	js_newcfunction(J, is_mouse_button_down, "is_mouse_button_down", 1);
	js_setglobal(J, "is_mouse_button_down");
	js_newcfunction(J, is_mouse_button_up, "is_mouse_button_up", 1);
	js_setglobal(J, "is_mouse_button_up");
	js_newcfunction(J, get_mouse_x, "get_mouse_x", 1);
	js_setglobal(J, "get_mouse_x");
	js_newcfunction(J, get_mouse_y, "get_mouse_y", 1);
	js_setglobal(J, "get_mouse_y");
}

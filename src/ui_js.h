#ifndef _UI_JS_H
#define _UI_JS_H

#include "../vendor/mujs/mujs.h"
#include "ui.h"

UiColor hex_to_rgba(js_State *J,const char *hex);
void bind_ui_func(js_State *J);
int wrap_content_fn(void *userdata,Painter p,int width);
VECTOR2(int) mesure_content_fn(void *userdata,Painter p);

#endif //_JSX_CLASS_H

#include "canvas_js.h"
#include "hashmap.h"
#include "shape.h"
#include "raylib_js.h"
#include "js_helper.h"
#include "vector2.h"
#include <stdio.h>

ARRAY(Point) canvas_path;
extern HASHMAP(Image)* hmap_image;
extern HASHMAP(Texture)* hmap_texture;
Image* current_canvas=NULL;
Color fill_color=RED;

void js_new_canvas(js_State* J){
    const char* name = js_tostring(J, 1);
    if(name==NULL){
        js_error(J, "must pass a name to create canvas");
        js_pushundefined(J);
        return;
    }
    Image* img = Image_upsert(hmap_image, name, UpsertActionCreate);
    if(img->data!= NULL){
        js_error(J, "cannot create %s canvas, already exist",name);
        js_pushundefined(J);
        return;
    }
    int w = js_tointeger(J, 2);
    int h = js_tointeger(J, 3);
    *img = GenImageColor(w,h,WHITE);
    js_pushundefined(J);
    return;
}

void js_set_canvas(js_State* J){
    const char* name = js_tostring(J, 1);
    if(name==NULL){
        current_canvas=NULL;
        js_pushundefined(J);
        return;
    }
    Image* img = Image_upsert(hmap_image, name, UpsertActionUpdate);
    if(img == NULL){
        js_error(J, "cannot found %s canvas",name);
        js_pushundefined(J);
        return;
    }
    current_canvas=img;
    js_pushundefined(J);
    return;
}

void js_clear_canvas(js_State* J){
    if(current_canvas==NULL){
        js_error(J, "no canvas set");
        js_pushundefined(J);
        return;
    }
    ImageClearBackground(current_canvas,get_color(J,1));
    js_pushundefined(J);
    return;
}

void js_set_fill_color(js_State* J){
    fill_color =get_color(J,1);
    js_pushundefined(J);
    return;
}

static void close_path(){
    fill_polygon(canvas_path,
    (ImageBuffer){
        .buf=current_canvas->data,
        .w=current_canvas->width,
        .h=current_canvas->height
    },
    (BufColor){
        .r=fill_color.r,
        .g=fill_color.g,
        .b=fill_color.b,
        .a=fill_color.a
    });
    canvas_path.len=0;
}

static VECTOR2(float) get_point_or(js_State *J, int idx,VECTOR2(float) def)  {
    if (js_isobject(J, idx) == 0) {
        return def;
	}
	return (VECTOR2(float)){
		.x =      (float)get_property_number_or(J, idx, "x", 0),
		.y =      (float)get_property_number_or(J, idx, "y", 0),
	};
}

void js_move_to(js_State* J){
    if(current_canvas==NULL){
        js_error(J, "no canvas set");
        js_pushundefined(J);
        return;
    }
    if(canvas_path.len>0){
        close_path();
    }
    array_append_Point(&canvas_path,get_point_or(J,1,(VECTOR2(float)){0,0}));
    js_pushundefined(J);
}

void js_bezier_to(js_State* J){
    if(current_canvas==NULL){
        js_error(J, "no canvas set");
        js_pushundefined(J);
        return;
    }
    if(canvas_path.len==0){
        array_append_Point(&canvas_path,(VECTOR2(float)){0,0});
    }
    int n=js_tointeger(J, 4);
    n = n >2?n:10;
    if(!bezier_to(
        &canvas_path,
        get_point_or(J,1,(VECTOR2(float)){0,0}),
        get_point_or(J,2,(VECTOR2(float)){0,0}),
        get_point_or(J,3,(VECTOR2(float)){0,0}),
        n
    )){
        js_error(J, "number of segment must be greaer than 2");
        js_pushundefined(J);
        return;
    }
    js_pushundefined(J);
}

void js_line_to(js_State* J){
    if(current_canvas==NULL){
        js_error(J, "no canvas set");
        js_pushundefined(J);
        return;
    }
    if(canvas_path.len==0){
        array_append_Point(&canvas_path,(VECTOR2(float)){0,0});
    }
    array_append_Point(&canvas_path,get_point_or(J,1,(VECTOR2(float)){0,0}));
    js_pushundefined(J);
}

void js_close(js_State* J){
    if(current_canvas==NULL){
        js_error(J, "no canvas set");
        js_pushundefined(J);
        return;
    }
    if(canvas_path.len>0){
        close_path();
    }
    js_pushundefined(J);
}

void js_save_canvas(js_State* J){
    if(current_canvas==NULL){
        js_error(J, "no canvas set");
        js_pushundefined(J);
        return;
    }
    const char* name = js_tostring(J, 1);
    if(name==NULL){
        js_error(J, "invalide filename");
        js_pushundefined(J);
        return;
    }
    ExportImage(*current_canvas,name);
    js_pushundefined(J);
}

void js_canvas_to_image(js_State* J){
    if(current_canvas==NULL){
        js_error(J, "no canvas set");
        js_pushundefined(J);
        return;
    }

    const char* name = js_tostring(J, 1);
    if(name==NULL){
        js_error(J, "invalide image name");
        js_pushundefined(J);
        return;
    }

    Texture* t = Texture_upsert(hmap_texture,name , UpsertActionCreate);
    if(t==NULL){
        js_error(J, "cannot acces image cache");
        js_pushundefined(J);
        return;
    }

    if(!IsTextureValid(*t)){
        *t = LoadTextureFromImage(*current_canvas);
        js_pushundefined(J);
        return;
    }

    if(t->width != current_canvas->width ||t->height != current_canvas->height){
        UnloadTexture(*t);
        *t = LoadTextureFromImage(*current_canvas);
        js_pushundefined(J);
        return;
    }
    UpdateTexture(*t, current_canvas->data);
    js_pushundefined(J);
    return;
}

void bind_canvas_func(js_State *J,Allocator alloc){
    canvas_path.alloc=alloc;
    js_newcfunction(J, js_new_canvas, "NewCanvas", 3);           js_setglobal(J, "NewCanvas");
    js_newcfunction(J, js_set_canvas, "SetCanvas", 1);           js_setglobal(J, "SetCanvas");
    js_newcfunction(J, js_clear_canvas, "ClearCanvas", 1);       js_setglobal(J, "ClearCanvas");
    js_newcfunction(J, js_set_fill_color, "SetFillColor", 1);    js_setglobal(J, "SetFillColor");
    js_newcfunction(J, js_move_to, "MoveTo", 1);                 js_setglobal(J, "MoveTo");
    js_newcfunction(J, js_bezier_to, "BezierTo", 4);             js_setglobal(J, "BezierTo");
    js_newcfunction(J, js_line_to, "LineTo", 1);                 js_setglobal(J, "LineTo");
    js_newcfunction(J, js_close, "Close", 0);                    js_setglobal(J, "Close");
    js_newcfunction(J, js_save_canvas, "SaveCanvas", 1);         js_setglobal(J, "SaveCanvas");
    js_newcfunction(J, js_canvas_to_image, "CanvasToImage", 1);  js_setglobal(J, "CanvasToImage");


}

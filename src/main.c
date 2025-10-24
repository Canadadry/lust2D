#include "../vendor/raylib/raylib.h"
#include "../vendor/mujs/mujs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "vector2.h"
#include "hashmap.h"
#include "jsx_class.h"
#include "raylib_js.h"
#include "js_helper.h"
#include "ui_js.h"
#include "text.h"
#include "args.h"
#include "allocator.h"

HASHMAP(init_node_fn)* hmap_init_node_fn;
ARRAY(InitNodeFn1)* array_init_node_fn1;
HASHMAP(Texture)* hmap_texture;
HASHMAP(Image)* hmap_image;
HASHMAP(Font)* hmap_font;
HASHMAP(Sound)* hmap_sound;

#define MOUSE_SCALE_MARK_SIZE   12

Tree* ui_tree= NULL;

static inline void* user_realloc(void* userdata,void* ptr, size_t size){
    return realloc(ptr,size);
}

static inline void user_free(void* userdata,void* ptr){
    free(ptr);
}

int main(int argc, char** argv){
    HasArgResult in = has_arg(argc,argv,"-in");
    if(in.ok==0){
        in.next="main.js";
    }
    HasArgResult basedir = has_arg(argc,argv,"-basedir");
    if(basedir.ok==1&&basedir.next!=NULL&&basedir.next[0]!='-'){
        printf("cd to %s \n",basedir.next);
        ChangeDirectory(basedir.next);
    }
    Allocator alloc = (Allocator){
    		.realloc_fn = user_realloc,
    		.free_fn = user_free,
    };
    HASHMAP(Texture) loc_hmap_texture={0};
    loc_hmap_texture.data.alloc=alloc;
    hmap_texture = &loc_hmap_texture;
    HASHMAP(Image) loc_hmap_image={0};
    loc_hmap_image.data.alloc=alloc;
    hmap_image = &loc_hmap_image;
    HASHMAP(Font) loc_hmap_font={0};
    loc_hmap_font.data.alloc=alloc;
    hmap_font = &loc_hmap_font;
    HASHMAP(Sound) loc_hmap_sound={0};
    loc_hmap_sound.data.alloc=alloc;
    hmap_sound = &loc_hmap_sound;
    HASHMAP(init_node_fn) loc_hmap_init_node_fn={0};
    loc_hmap_init_node_fn.data.alloc=alloc;
    hmap_init_node_fn = &loc_hmap_init_node_fn;
    ARRAY(InitNodeFn1) loc_array_init_node_fn1={0};
    loc_array_init_node_fn1.alloc=alloc;
    array_init_node_fn1 = &loc_array_init_node_fn1;
    init_init_node_fn_hashmap(hmap_init_node_fn,array_init_node_fn1);
    Tree loc_ui_tree = (Tree){0};
    tree_init(&loc_ui_tree,alloc);
    loc_ui_tree.wrap_content_fn=wrap_content_fn;
    loc_ui_tree.mesure_content_fn=mesure_content_fn;
    ui_tree = &loc_ui_tree;

    js_State *J = js_newstate(NULL, NULL,  0);
	if (!J) {
		fprintf(stderr, "Could not initialize MuJS.\n");
		exit(1);
	}
	int ret = init_js(J,alloc);
	if(ret != 0){
	    return ret;
	}
	if(js_dofile(J, in.next) !=0){
	    printf("failed while running main.js\n");
	    js_throw(J);
	    return 1;
	}
	if(js_dostring(J, "conf();") !=0){
	    printf("failed while running conf()\n");
	    return 1;
	}

	Window window = get_window(J);
	SetTraceLogLevel(LOG_ERROR);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow((int)window.width, (int)window.height, window.title);
    SetTargetFPS(60);
    SetWindowFocused();
    InitAudioDevice();

   	if(js_dostring(J, "init();") !=0){
	    printf("failed while running init()\n");
	    return 1;
	}

    while (!WindowShouldClose())
    {
        if(IsWindowResized()){
            char txt[1024]={0};
            Vector2 scale = GetWindowScaleDPI();
            snprintf(txt, 1024, "window.width = %d/%f;window.height = %d/%f;",
                GetRenderWidth(),scale.x,
                GetRenderHeight(),scale.y
            );
           	if(js_dostring(J, txt) !=0){
           	    printf("failed while updating  window size\n");
           	    return 1;
           	}
        }
        BeginDrawing();
        if(js_dostring(J, "render();")){
            break;
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

VECTOR2(float) sizer(FontParam param, int codepoint) {
    Font* font = (Font*)param.Font;
	int index = GetGlyphIndex(*font, codepoint);
	float scaleFactor = (float)param.size / (float)(font->baseSize);
	float glyphWidth = 0.0;
	if (codepoint != '\n') {
		glyphWidth = (float)(font->glyphs[index].advanceX);
		if(glyphWidth == 0){
			glyphWidth = font->recs[index].width;
		}
		glyphWidth *= scaleFactor;
	}
	return (VECTOR2(float)){.x=glyphWidth, .y=(float)(font->baseSize) * scaleFactor};
}

void drawer(FontParam param, int codepoint, VECTOR2(int) pos) {
   	DrawTextCodepoint(
		*(Font*)param.Font,
		codepoint,
		(Vector2){.x=(float)(pos.x), .y=(float)(pos.y)},
		param.size,
		param.color.rgba
    );
}

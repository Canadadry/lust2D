#include "../vendor/raylib/raylib.h"
#include "../vendor/mujs/mujs.h"
#include "../vendor/jsx_parser/jsx_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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
#include "rectangle.h"

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

int has_suffix(char* txt,char* suf){
    int len_txt = strlen(txt);
    int len_suf = strlen(suf);

    if(len_suf>len_txt){
        return 0;
    }
    int ret = strcmp(txt+len_txt-len_suf, suf)==0;
    return ret;
}

int run_main_file(js_State *J,Allocator alloc){
    char *files[]={"main.jsx","main.js"};
    int len = sizeof(files)/sizeof(files[0]);
    for(int i=0;i<len;i+=1){
        FILE *f = fopen(files[i], "rb");
    	if (f==NULL) {
            continue;
    	}
        fclose(f);
        if(has_suffix(files[i],".jsx")){
            char *content =NULL;
            int ret = read_full_file(files[i],&content);
           	if(ret!=0){
           	    printf("cannot read %s %s %s\n",files[i], read_full_file_errno(ret),strerror(errno));
          		return 1;
           	}
            JSX_Compiler* compiler = jsx_new_compiler("ui_create(", (JSX_Allocator){
                .realloc_fn = alloc.realloc_fn,
                .free_fn = alloc.free_fn,
            });
            bool ok = jsx_compile(compiler, content, 0);
            if(!ok){
                char * err = jsx_get_last_error(compiler);
                printf("[%s] cannot compile jsx %s\n", files[i],err);
                jsx_free_compiler(compiler);
                return 1;
            }
            const char* compiled = jsx_get_output(compiler);
           	#ifdef BUILD_DEBUG
           	    printf("[%s] compiling jsx %s \n", files[i],compiled);
           	#endif
            js_loadstring(J,files[i],compiled);
            js_pushundefined(J);
           	js_call(J, 0);
            jsx_free_compiler(compiler);
            return 0;
        }
        if(has_suffix(files[i],".js")){
            if(js_dofile(J, files[i]) !=0){
                printf("failed while running %s\n",files[i]);
                js_throw(J);
                return 1;
            }
            return 0;
        }
    }
    return 2;
}

int main(int argc, char** argv){
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

	if(run_main_file(J,alloc) !=0){
	    printf("failed while running main files\n");
	    js_throw(J);
	    return 1;
	}
	if(js_dostring(J, "conf();") !=0){
	    printf("failed while running conf()\n");
	    return 1;
	}

	Window window = get_window(J);
	#ifndef BUILD_DEBUG
	SetTraceLogLevel(LOG_ERROR);
	#endif
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow((int)window.width, (int)window.height, window.title);
    SetTargetFPS(60);
    SetWindowFocused();
    #ifndef BUILD_DEBUG
    InitAudioDevice();
    #endif

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

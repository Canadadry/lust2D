#include "../vendor/raylib/raylib.h"
#include "../vendor/mujs/mujs.h"
#include "../vendor/jsx_parser/jsx_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "allocator.h"
#include "ui.h"
#include "vector2.h"
#include "hashmap.h"
#include "jsx_class.h"
#include "js_helper.h"
#include "raylib_js.h"
#include "ui_js.h"


HASHMAP(init_node_fn)* hmap_init_node_fn;
HASHMAP(Texture)* hmap_texture;


#define MOUSE_SCALE_MARK_SIZE   12

Tree* ui_tree= NULL;

static const char *require_js =
    "function require(name) {\n"
    "    var cache = require.cache;\n"
    "    if (name in cache) return cache[name];\n"
    "    var exports = {};\n"
    "    cache[name] = exports;\n"
    "    var fcontent = \"\";\n"
    "    if(file_exist(name+'.jsx')){\n"
    "        fcontent = read(name+'.jsx');\n"
    "        fcontent = parse_jsx(fcontent);\n"
    "    }\n"
    "    if(file_exist(name+'.js')){\n"
    "        fcontent = read(name+'.js');\n"
    "    }\n"
    "    if(fcontent!=\"\"){\n"
    "        Function('exports', fcontent)(exports);\n"
    "    }\n"
    "    return exports;\n"
    "}\n"
    "require.cache = Object.create(null);\n"
    "\n"
;

static const char *console_js =
	"var console = { log: print, debug: print, warn: print, error: print };"
;

static const char *window_js =
    "var window = { width:100,height:100,title:'lust2D' };"
;

static const char *ui_helper_js =
    "function fit(min,max){"
    "    min = (min === undefined) ? 0 : min;"
    "    max = (max === undefined) ? 0 : max;"
    "    return {kind:\"fit\"};"
    "}"
    "function grow(min,max){"
    "    min = (min === undefined) ? 0 : min;"
    "    max = (max === undefined) ? 0 : max;"
    "    return {kind:\"grow\", min:min, max:max};"
    "}"
;

static void jsB_print(js_State *J){
	int i, top = js_gettop(J);
	for (i = 1; i < top; ++i) {
		const char *s = js_tostring(J, i);
		if (i > 1) putchar(' ');
		fputs(s, stdout);
	}
	putchar('\n');
	js_pushundefined(J);
}

static void jsB_read(js_State *J){
	const char *filename = js_tostring(J, 1);
	FILE *f;
	char *s;
	int n, t;

	f = fopen(filename, "rb");
	if (!f) {
		js_error(J, "cannot open file '%s': %s", filename, strerror(errno));
	}

	if (fseek(f, 0, SEEK_END) < 0) {
		fclose(f);
		js_error(J, "cannot seek in file '%s': %s", filename, strerror(errno));
	}

	n = ftell(f);
	if (n < 0) {
		fclose(f);
		js_error(J, "cannot tell in file '%s': %s", filename, strerror(errno));
	}

	if (fseek(f, 0, SEEK_SET) < 0) {
		fclose(f);
		js_error(J, "cannot seek in file '%s': %s", filename, strerror(errno));
	}

	s = malloc(n + 1);
	if (!s) {
		fclose(f);
		js_error(J, "out of memory");
	}

	t = fread(s, 1, n, f);
	if (t != n) {
		free(s);
		fclose(f);
		js_error(J, "cannot read data from file '%s': %s", filename, strerror(errno));
	}
	s[n] = 0;

	js_pushstring(J, s);
	free(s);
	fclose(f);
}

static void jsB_file_exist(js_State *J){
    int exist = 0;
	const char *filename = js_tostring(J, 1);
	FILE *f = fopen(filename, "rb");
	if (f) {
	    exist=1;
		fclose(f);
	}
	js_pushboolean(J, exist);
}

static inline void* user_realloc(void* userdata,void* ptr, size_t size){
    return realloc(ptr,size);
}

static inline void user_free(void* userdata,void* ptr){
    free(ptr);
}

static void parse_jsx(js_State *J){
    const char* content = js_tostring(J, 1);
	JSX_Compiler* compiler = jsx_new_compiler("ui_create(", (JSX_Allocator){
		.realloc_fn = user_realloc,
		.free_fn = user_free,
	});
	bool ok = jsx_compile(compiler, content, 0);
	if(!ok){
		js_error(J, "cannot compile jsx %s", jsx_get_last_error(compiler));
		js_pushundefined(J);
		jsx_free_compiler(compiler);
		return;
	}
	const char* compiled = jsx_get_output(compiler);
	// printf("compiling jsx %s \n", compiled);
	js_pushstring(J, compiled);
	jsx_free_compiler(compiler);
}

typedef struct{
    int width;
    int height;
    const char* title;
} Window;

static Window get_window(js_State *J) {
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

VECTOR2(int) mesure_content_fn(void *userdata,Painter p){
    VECTOR2(int) ret = {0};
    Texture* t;
    if(p.kind==PAINTER_IMG){
        t = Texture_upsert(hmap_texture,p.value.img.source , UpsertActionUpdate);
        if(t!=NULL){
            ret.x = t->width;
            ret.y = t->height;
        }
    }
    return ret;
}

int wrap_content_fn(void *userdata,Painter p,int width){
    VECTOR2(int) content = {0};
    if(p.kind==PAINTER_IMG){
        content = mesure_content_fn(userdata,p);
        return (int)((float)width*(float)content.y/(float)content.x);
    }
    return 0;
}

int main(int argc, char** argv){
    Allocator alloc = (Allocator){
    		.realloc_fn = user_realloc,
    		.free_fn = user_free,
    };
    HASHMAP(Texture) loc_hmap_texture={0};
    loc_hmap_texture.data.alloc=alloc;
    hmap_texture = &loc_hmap_texture;
    HASHMAP(init_node_fn) loc_hmap_init_node_fn={0};
    loc_hmap_init_node_fn.data.alloc=alloc;
    hmap_init_node_fn = &loc_hmap_init_node_fn;
    init_init_node_fn_hashmap(hmap_init_node_fn);

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

	js_newcfunction(J, jsB_print, "print", 0);
	js_setglobal(J, "print");
	js_newcfunction(J, jsB_read, "read", 0);
	js_setglobal(J, "read");
	js_newcfunction(J, jsB_file_exist, "file_exist", 0);
	js_setglobal(J, "file_exist");
	js_newcfunction(J, parse_jsx, "parse_jsx", 0);
	js_setglobal(J, "parse_jsx");
	bind_raylib_func(J,alloc);
	bind_ui_func(J);
	if (js_dostring(J, require_js) != 0){
	    return 1;
	}
	if (js_dostring(J, console_js) != 0){
	    return 1;
	}
	if (js_dostring(J, window_js) !=0){
	    return 1;
	}
	if (js_dostring(J, ui_helper_js) !=0){
	    return 1;
	}
	if(js_dofile(J, "main.js") !=0){
	    printf("failed while running main.js\n");
	    return 1;
	}
	if(js_dostring(J, "conf();") !=0){
	    printf("failed while running conf()\n");
	    return 1;
	}

	Window window = get_window(J);
	SetTraceLogLevel(LOG_WARNING);
    InitWindow((int)window.width, (int)window.height, window.title);
    SetTargetFPS(60);

   	if(js_dostring(J, "init();") !=0){
	    printf("failed while running init()\n");
	    return 1;
	}

	const char * txt ="";
    while (!WindowShouldClose())
    {
        BeginDrawing();
        if(js_dostring(J, "render();")){
            break;
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

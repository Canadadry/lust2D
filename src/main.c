#include "../vendor/raylib/raylib.h"
#include "../vendor/mujs/mujs.h"
#include "../vendor/jsx_parser/jsx_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ui.h"
#include "vector2.h"
#include "optionnal.h"

typedef const char* string;

CREATE_OPTIONAL_TYPE(double)
CREATE_OPTIONAL_TYPE(string)

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
	//printf("compiling jsx %s \n", compiled);
	js_pushstring(J, compiled);
	jsx_free_compiler(compiler);
}

static inline OPTIONAL(double) get_property_number(js_State *J, int idx,const char* name){
   	if( js_hasproperty(J, idx, name) == 0) {
		return (OPTIONAL(double)){0};
	}
    js_getproperty(J, idx, name);
    if(js_isnumber(J,-1)==0){
       	js_pop(J, -1);
        return (OPTIONAL(double)){0};
    }
	double ret =js_tonumber(J, -1);
	js_pop(J, -1);
	return (OPTIONAL(double)){.value=ret,.ok=true};
}

static double get_property_number_or(js_State *J, int idx,const char* name, double def){
    OPTIONAL(double) ret = get_property_number(J,idx,name);
    if(ret.ok == false){
        return def;
    }
    return ret.value;
}

static double get_property_int_or(js_State *J, int idx,const char* name, int def){
    OPTIONAL(double) ret = get_property_number(J,idx,name);
    if(ret.ok == false){
        return def;
    }
    return (int)ret.value;
}

static inline OPTIONAL(string) get_property_string(js_State *J, int idx,const char* name){
   	if( js_hasproperty(J, idx, name) == 0) {
		return (OPTIONAL(string)){0};
	}
    js_getproperty(J, idx, name);
    if(js_isstring(J,-1)==0){
       	js_pop(J, -1);
        return (OPTIONAL(string)){0};
    }
	const char* ret =js_tostring(J, -1);
	js_pop(J, -1);
	return (OPTIONAL(string)){.value=ret,.ok=true};
}

static const char*  get_property_string_or(js_State *J, int idx,const char* name, const char*  def){
    OPTIONAL(string) ret = get_property_string(J,idx,name);
    if(ret.ok == false){
        return def;
    }
    return ret.value;
}

static UiColor get_color(js_State *J, int idx)  {
	if (js_isobject(J, idx) == 0) {
		return (UiColor){.rgba=WHITE};
	};
	return (UiColor){
    	.rgba=(Color){
    		.r = (unsigned char)get_property_number_or(J, idx, "r", 255),
    		.g = (unsigned char)get_property_number_or(J, idx, "g", 255),
    		.b = (unsigned char)get_property_number_or(J, idx, "b", 255),
    		.a = (unsigned char)get_property_number_or(J, idx, "a", 255),
    	},
	};
}

static inline Size get_property_size(js_State *J, int idx,const char* name){
   	if( js_hasproperty(J, idx, name) == 0) {
		return (Size){0};
	}
    js_getproperty(J, idx, name);
    if(js_isnumber(J,-1)!=0){
        double val = js_tonumber(J,-1);
        js_pop(J, -1);
        return (Size){.kind=SizeKindFixed,.size=(int)val};
    }
    if(js_isobject(J,-1)==0){
        js_pop(J, -1);
        return (Size){0};
    }
    int top = js_gettop(J);
	OPTIONAL(string) kind =get_property_string(J,top,"kind");
	if(kind.ok ==false){
	    js_pop(J, -1);
		return (Size){0};
	}
	if(strncmp(kind.value, "fit", 3)==0){
        Size ret= (Size){
            .kind=SizeKindFit,
            .bound={
                .min=get_property_int_or(J,top,"min",0),
                .max=get_property_int_or(J,top,"max",0),
            },
        };
        js_pop(J, -1);
        return ret;
	}
	if(strncmp(kind.value, "grow", 3)==0){
        Size ret= (Size){
            .kind=SizeKindGrow,
            .bound={
                .min=get_property_int_or(J,top,"min",0),
                .max=get_property_int_or(J,top,"max",0),
            },
        };
        js_pop(J, -1);
        return ret;
	}
	js_pop(J, -1);
	return (Size){0};
}

static inline Layout get_property_layout(js_State *J, int idx,const char* name){
    OPTIONAL(string) val = get_property_string(J, idx, name);
    if(val.ok==false){
        return LayoutHorizontal;
    }
   	if(strncmp(val.value, "h", 1)==0){
        return LayoutHorizontal;
    }
   	if(strncmp(val.value, "v", 1)==0){
        return LayoutVertical;
    }
   	if(strncmp(val.value, "s", 1)==0){
        return LayoutStack;
    }
    return LayoutHorizontal;
}

static inline Align get_property_align(js_State *J, int idx,const char* name){
    OPTIONAL(string) val = get_property_string(J, idx, name);
    if(val.ok==false){
        return AlignBegin;
    }
   	if(strncmp(val.value, "b", 1)==0){
        return AlignBegin;
    }
   	if(strncmp(val.value, "m", 1)==0){
        return AlignMiddle;
    }
   	if(strncmp(val.value, "e", 1)==0){
        return AlignEnd;
    }
    return AlignBegin;
}

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

static void clear_background(js_State *J) {
	ClearBackground(get_color(J, 1).rgba);
}

static void draw_rectangle_rec(js_State *J) {
	DrawRectangleRec(get_rectangle(J, 1), get_color(J, 2).rgba);
}

typedef struct{
    int width;
    int height;
    const char* title;
} Window;

static Window get_window(js_State *J) {
	Window win = {0};
	js_getglobal(J, "window");
	int top =js_gettop(J);
	win.width = get_property_int_or(J,top,"width",800);
	win.height = get_property_int_or(J,top,"height",600);
	win.title = get_property_string_or(J,top,"title","no title");
	js_pop(J,-1);
	return win;
}

static void js_ui_create(js_State *J) {
	int top = js_gettop(J);
	int p = get_property_int_or(J,2,"padding",0);
	Node node = (Node){
	    .pos=(VECTOR2(int)){
    		.x=get_property_int_or(J,2,"x",0),
    		.y=get_property_int_or(J,2,"y",0),
		},
		.size=(VECTOR2(Size)){
		    .x=get_property_size(J,2,"w"),
		    .y=get_property_size(J,2,"h"),
		},
		.margin=get_property_int_or(J,2,"margin",0),
		.layout=get_property_layout(J,2,"layout"),
		.padding=(Padding){
           	.left=p,
           	.right=p,
           	.top=p,
           	.bottom=p,
		},
		.align=(VECTOR2(Align)){
    		.x=get_property_align(J, 2, "h_align"),
    		.y=get_property_align(J, 2, "v_align"),
		},
		.next=-1,
		.first_children=-1,
		.last_children=-1,
	};
	if(js_isstring(J, 1) != 0){
		const char* title = js_tostring(J, 1);
		if(strncmp(title,"rectangle",8)==0){
			node.painter.kind=PAINTER_RECT;
			node.painter.value.rect = (PainterRect){
				.color = get_color(J, 2),
			};
		}else{
			js_error(J, "unknown base ui tag '%s'", title);
			js_pushundefined(J);
			return;
		}
	} else if(js_iscallable(J, 1) != 0){
		js_error(J, "not implemented");
		js_pushundefined(J);
		return;

		// mujs.pushnull(J)
		//    mujs.copy(J,2)
		// if mujs.pcall(J, 2) {
		//        mujs.error(J,"an exception occurred in the javascript callback")
		//        mujs.pushundefined(J)
		// 	return
		// }

		// if mujs.isuserdata(J, -1,"Node"){
		//     append(children,cast(^UiNode)mujs.touserdata(J,-1,"Node")^)
		//    }
		// mujs.pop(J, 1)
	} else {
		js_error(J, "invalid param type");
		js_pushundefined(J);
		return;
	}

	int parent=ui_tree->nodes.len;
	array_append_Node(&ui_tree->nodes, node);
	for(int i= 3; i < top; i += 1){
		if(js_isnumber(J, i) == 0){
			continue;
		}
		int child= js_tointeger(J, i);
		if(child >= ui_tree->nodes.len||child<0) {
			break;
		}
		link_child(ui_tree,parent,child);
	}
	js_pushnumber(J, parent);
}

static void js_ui_draw(js_State *J){
	if (ui_tree->nodes.len<=0){
		js_error(J, "empty ui_node, call create before calling draw");
		js_pushundefined(J);
		return;
	}
	if (js_isnumber(J, 1) == 0 ){
		js_error(J, "node index passed to draw should be a number");
		js_pushundefined(J);
		return;
	}
	int idx = js_tointeger(J, 1);
	if(idx >= ui_tree->nodes.len) {
		js_error(J, "node index passed to draw should lesser than ui_node len");
		js_pushundefined(J);
		return;
	}

	compute(ui_tree, idx);
	draw(*ui_tree, idx);
}

int main(int argc, char** argv){
    Tree loc_ui_tree = (Tree){0};
    tree_init(&loc_ui_tree,(Allocator){
    		.realloc_fn = user_realloc,
    		.free_fn = user_free,
    });
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
	js_newcfunction(J, clear_background, "ClearBackground", 0);
	js_setglobal(J, "ClearBackground");
	js_newcfunction(J, draw_rectangle_rec, "DrawRectangleRec", 0);
	js_setglobal(J, "DrawRectangleRec");
	js_newcfunction(J, js_ui_create, "ui_create", 0);
	js_setglobal(J, "ui_create");
	js_newcfunction(J, js_ui_draw, "ui_draw", 0);
	js_setglobal(J, "ui_draw");
	js_newcfunction(J, parse_jsx, "parse_jsx", 0);
	js_setglobal(J, "parse_jsx");
	js_dostring(J, require_js);
	if (js_dostring(J, console_js) != 0){
	    return 1;
	}
	if (js_dostring(J, window_js) !=0){
	    return 1;
	}
	if(js_dofile(J, "main.js") !=0){
	    return 1;
	}
	if(js_dostring(J, "conf();") !=0){
	    return 1;
	}

	Window window = get_window(J);
	SetTraceLogLevel(LOG_WARNING);
    InitWindow((int)window.width, (int)window.height, window.title);
    SetTargetFPS(60);

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

#include "js_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../vendor/jsx_parser/jsx_parser.h"
#include "allocator.h"
#include "raylib_js.h"
#include "ui_js.h"
#include "canvas_js.h"

Allocator jsx_allocator = {0};

static const char *require_js =
    "var evaluated_func = null;"
    "function require(name) {\n"
    "    var cache = require.cache;\n"
    "    if (name in cache) return cache[name];\n"
    "    var exports = {};\n"
    "    cache[name] = exports;\n"
    "    var fcontent = \"\";\n"
    "    if(file_exist(name+'.jsx')){\n"
    "        name = name+'.jsx';\n"
    "        fcontent = read(name);\n"
    "        fcontent = parse_jsx(fcontent);\n"
    "    }\n"
    "    if(file_exist(name+'.js')){\n"
    "        name = name+'.js';\n"
    "        fcontent = read(name);\n"
    "    }\n"
    "    if(fcontent!=\"\"){\n"
    "        var previous=evaluated_func;"
    "        fcontent = 'evaluated_func= function(exports) {'+fcontent+'};'\n"
    "        named_eval(name,fcontent);\n"
    "        evaluated_func(exports);\n"
    "        evaluated_func=previous;\n"
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

static const char *stacktrace_js =
	"Error.prototype.toString = function() {\n"
	"var s = this.name;\n"
	"if ('message' in this) s += ': ' + this.message;\n"
	"if ('stack' in this) s += this.stack;\n"
	"return s;\n"
	"};\n"
;

static void jsB_named_eval(js_State *J){
    int func_name_idx = 1;
    int func_idx = 2;
    const char * name = js_tostring(J, func_name_idx);
    const char * func = js_tostring(J, func_idx);
    js_loadstring(J,name,func);
    js_pushundefined(J);
   	js_call(J, 0);
}

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


void parse_jsx(js_State *J){
    const char* content = js_tostring(J, 1);
	JSX_Compiler* compiler = jsx_new_compiler("ui_create(", (JSX_Allocator){
		.realloc_fn = jsx_allocator.realloc_fn,
		.free_fn = jsx_allocator.free_fn,
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

int init_js(js_State* J,Allocator alloc){
    jsx_allocator=alloc;
   	js_newcfunction(J, jsB_named_eval, "named_eval", 2);
	js_setglobal(J, "named_eval");
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
	bind_canvas_func(J,alloc);
	if (js_dostring(J, stacktrace_js) != 0){
	    return 1;
	}
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
	return 0;
}

OPTIONAL(double) get_property_number(js_State *J, int idx,const char* name){
   	if( js_hasproperty(J, idx, name) == 0) {
		return (OPTIONAL(double)){0};
	}
    js_getproperty(J, idx, name);
    if(js_isnumber(J,-1)==0){
       	js_pop(J, 1);
        return (OPTIONAL(double)){0};
    }
	double ret =js_tonumber(J, -1);
	js_pop(J, 1);
	return (OPTIONAL(double)){.value=ret,.ok=true};
}

double get_property_number_or(js_State *J, int idx,const char* name, double def){
    OPTIONAL(double) ret = get_property_number(J,idx,name);
    if(ret.ok == false){
        return def;
    }
    return ret.value;
}

int get_property_int_or(js_State *J, int idx,const char* name, int def){
    OPTIONAL(double) ret = get_property_number(J,idx,name);
    if(ret.ok == false){
        return def;
    }
    return (int)ret.value;
}

OPTIONAL(string) get_property_string(js_State *J, int idx,const char* name){
   	if( js_hasproperty(J, idx, name) == 0) {
		return (OPTIONAL(string)){0};
	}
    js_getproperty(J, idx, name);
    if(js_isstring(J,-1)==0){
       	js_pop(J, 1);
        return (OPTIONAL(string)){0};
    }
	const char* ret =js_tostring(J, -1);
	js_pop(J, 1);
	return (OPTIONAL(string)){.value=ret,.ok=true};
}

const char*  get_property_string_or(js_State *J, int idx,const char* name, const char*  def){
    OPTIONAL(string) ret = get_property_string(J,idx,name);
    if(ret.ok == false){
        return def;
    }
    return ret.value;
}

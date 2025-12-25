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
#include "buffer.h"

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


void print_multi_line(const char* str){
    int line =0;
    for (const char *p = str; *p != '\0'; p++) {
        putchar(*p);
        if (*p == '\n') {
            line++;
            printf("%d: ", line);
        }
    }
    putchar('\n');
}


const char* js_inspect_stack_at(js_State *J,int i){
    if(js_isundefined(J,i))          {return ("undefined");}
    else if(js_isnull(J,i))          {return ("null");}
    else if(js_isboolean(J,i))       {return ("boolean");}
    else if(js_isnumber(J,i))        {return ("number");}
    else if(js_isstring(J,i))        {return ("string");}
    else if(js_isprimitive(J,i))     {return ("primitive");}
    else if(js_isobject(J,i))        {return ("object");}
    else if(js_isarray(J,i))         {return ("array");}
    else if(js_isregexp(J,i))        {return ("regexp");}
    else if(js_iscoercible(J,i))     {return ("coercible");}
    else if(js_iscallable(J,i))      {return ("callable");}
    else if(js_iserror(J,i))         {return ("error");}
    else if(js_isnumberobject(J,i))  {return ("numberobject");}
    else if(js_isstringobject(J,i))  {return ("stringobject");}
    else if(js_isbooleanobject(J,i)) {return ("booleanobject");}
    else if(js_isdateobject(J,i))    {return ("dateobject");}
    return ("unknown");
    //else if(js_isuserdata(J,i)){printf("[%s:%d] userdata\n",prefix,i);}

}

void js_dump_stack(js_State *J,const char * prefix){
    int top = js_gettop(J);
    for(int i=0;i<top;i++){
        printf("[%s:%d] %s\n",prefix,i,js_inspect_stack_at(J,i));
    }
}

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

static void jsB_panic(js_State *J){
    Buffer buf;
    STATIC_ZERO_INIT(char,buf,back,1024);
	int i, top = js_gettop(J);
	for (i = 1; i < top; ++i) {
		const char *s = js_tostring(J, i);
		if (i > 1) buf_write_char(&buf,' ');
		buf_write_string(&buf,s);
	}
	buf_write_char(&buf,'\n');
	js_error(J,"%s",buf.data);
	js_pushundefined(J);
}


int read_full_file(const char *filename,char ** file){
	FILE *f;
	int n, t;

	f = fopen(filename, "rb");
	if (!f) {
	    return 1;
	}

	if (fseek(f, 0, SEEK_END) < 0) {
		fclose(f);
		return 2;
	}

	n = ftell(f);
	if (n < 0) {
		fclose(f);
		return 3;
	}

	if (fseek(f, 0, SEEK_SET) < 0) {
		fclose(f);
		return 2;
	}

	*file = malloc(n + 1);
	if (!*file) {
		fclose(f);
		return 4;
	}

	t = fread(*file, 1, n, f);
	if (t != n) {
		free(*file);
		fclose(f);
		return 5;
	}
	(*file)[n] = 0;
	fclose(f);
	return 0;
}

const char* read_full_file_errno(int err){
   	switch(err){
    	case 1: return "cannot open file";
    	case 2: return "cannot seek in file";
    	case 3: return "cannot tell in file";
    	case 4: return "out of memory";
    	case 5: return "cannot read data from file";
	}
	return "";
}

static void jsB_read(js_State *J){
    const char * filename = js_tostring(J, 1);
    char *content =NULL;
	int ret = read_full_file(filename,&content);
	if(ret!=0){
	    js_error(J, "%s %s %s",read_full_file_errno(ret),filename,strerror(errno));
		js_pushundefined(J);
		return;
	}
	js_pushstring(J, content);
	free(content);
}

static void jsB_write(js_State* J){
   	const char *filename = js_tostring(J, 1);
	FILE *f = fopen(filename, "w");
	if (!f) {
		js_error(J, "cannot open file '%s': %s", filename, strerror(errno));
	}

	const char *content = js_tostring(J, 2);
	fwrite(content,sizeof(char),strlen(content),f);
	fclose(f);
	js_pushundefined(J);
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
	JSX_Compiler* compiler = jsx_new_compiler(JSX_CREATE_FUNC"(", (JSX_Allocator){
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
	#ifdef BUILD_DEBUG
	printf("compiling jsx\n");
	print_multi_line(compiled);
	#endif
	js_pushstring(J, compiled);
	jsx_free_compiler(compiler);
}

static inline double map_value(double from_start, double from_end, double to_start, double to_end, double value) {
    double proportion = (value - from_start) / (from_end - from_start);
    return to_start + proportion * (to_end - to_start);
}

static void js_map_value(js_State *J){
    js_pushnumber(J, map_value(
        js_tonumber(J, 1),
        js_tonumber(J, 2),
        js_tonumber(J, 3),
        js_tonumber(J, 4),
        js_tonumber(J, 5)
    ));
}

int init_js(js_State* J,Allocator alloc){
    jsx_allocator=alloc;
   	js_newcfunction(J, jsB_named_eval, "named_eval", 2);
	js_setglobal(J, "named_eval");
   	js_newcfunction(J, jsB_print, "print", 1);
	js_setglobal(J, "print");
	js_newcfunction(J, jsB_panic, "panic", 1);
	js_setglobal(J, "panic");
	js_newcfunction(J, jsB_read, "read", 1);
	js_setglobal(J, "read");
	js_newcfunction(J, jsB_write, "write", 2);
	js_setglobal(J, "write");
	js_newcfunction(J, jsB_file_exist, "file_exist", 0);
	js_setglobal(J, "file_exist");
	js_newcfunction(J, parse_jsx, "parse_jsx", 0);
	js_setglobal(J, "parse_jsx");
	js_newcfunction(J, js_map_value, "map", 5);
	js_setglobal(J, "map");
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

int __get_property_string_enum(js_State *J, int idx,const char* name, ...) {
    va_list args;
    va_start(args, name);

    const char* value = get_property_string_or(J,idx,name,NULL);
    if(value==NULL){
        return 0;
    }

    int i=0;
    const char *arg;
    while ((arg = va_arg(args, const char*))) {
        if(strcmp(arg,value)==0){
            va_end(args);
            return i;
        }
    }
    va_end(args);
    return 0;
}


#define JS_CHECK_ARGS_CASE(type,name) if(strcmp(arg,name)==0 && !js_is##type(J,i)){ \
    printf("[%s:%d] expected a "name" got %s\n",prefix, i,js_inspect_stack_at(J,i));\
    return 0; \
}

int _js_check_args(js_State *J,const char * prefix,...){
    va_list args;
    va_start(args, prefix);
    const char *arg;
    int i=1;
    while ((arg = va_arg(args, const char*))) {
        if(arg==NULL) break;
        JS_CHECK_ARGS_CASE(boolean,"boolean")
        JS_CHECK_ARGS_CASE(number,"number")
        JS_CHECK_ARGS_CASE(string,"string")
        JS_CHECK_ARGS_CASE(primitive,"primitive")
        JS_CHECK_ARGS_CASE(object,"object")
        JS_CHECK_ARGS_CASE(array,"array")
        JS_CHECK_ARGS_CASE(regexp,"regexp")
        JS_CHECK_ARGS_CASE(coercible,"coercible")
        JS_CHECK_ARGS_CASE(callable,"callable")
        JS_CHECK_ARGS_CASE(error,"error")
        JS_CHECK_ARGS_CASE(numberobject,"numberobject")
        JS_CHECK_ARGS_CASE(stringobject,"stringobject")
        JS_CHECK_ARGS_CASE(booleanobject,"booleanobject")
        JS_CHECK_ARGS_CASE(dateobject,"dateobject")
    }
    va_end(args);
    return 1;
}



int __has_property(js_State *J, int idx, ...){
    va_list args;
    va_start(args, idx);
    const char *arg;
    while ((arg = va_arg(args, const char*))) {
        int len =strlen(arg);
        if(len>0 && js_hasproperty(J,idx,arg)==0){
            va_end(args);
            return 0;
        }
    }
    va_end(args);
    return 1;
}

int has_suffix(const char* txt,const char* suf){
    int len_txt = strlen(txt);
    int len_suf = strlen(suf);

    if(len_suf>len_txt){
        return 0;
    }
    int ret = strcmp(txt+len_txt-len_suf, suf)==0;
    return ret;
}

int run_main_file(js_State *J,Allocator alloc,const char* main){
    const char *files[]={main,"main.jsx","main.js"};
    int len = sizeof(files)/sizeof(files[0]);
    for(int i=0;i<len;i+=1){
        if (files[i]==NULL){
            continue;
        }
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
            JSX_Compiler* compiler = jsx_new_compiler(JSX_CREATE_FUNC"(", (JSX_Allocator){
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
           	    printf("[%s] compiling jsx\n", files[i]);
                print_multi_line(compiled);
           	#endif
           	if (js_try(J)) {
          		js_report(J, js_trystring(J, -1, "Error"));
                return 1;
           	}
            js_loadstring(J,files[i],compiled);
            js_pushundefined(J);
           	js_call(J, 0);
            jsx_free_compiler(compiler);
            js_endtry(J);
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

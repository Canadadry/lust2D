#ifndef _JS_HELPER_
#define _JS_HELPER_

#include "../vendor/mujs/mujs.h"
#include "optional.h"
#include "allocator.h"

#define JSX_CREATE_FUNC "createNode"

typedef const char* string;

CREATE_OPTIONAL_TYPE(double)
CREATE_OPTIONAL_TYPE(string)

int read_full_file(const char *filename,char ** file);
const char* read_full_file_errno(int err);

void js_dump_stack(js_State *J,const char * prefix);
int init_js(js_State* J,Allocator alloc);
OPTIONAL(double) get_property_number(js_State *J, int idx,const char* name);
double get_property_number_or(js_State *J, int idx,const char* name, double def);
int get_property_int_or(js_State *J, int idx,const char* name, int def);
OPTIONAL(string) get_property_string(js_State *J, int idx,const char* name);
const char*  get_property_string_or(js_State *J, int idx,const char* name, const char*  def);

#define has_property(J,idx,...) __has_property((J),(idx),__VA_ARGS__, NULL)
int __has_property(js_State *J,int idx,...);

#define get_property_string_enum(J,idx,name,...) __get_property_string_enum((J),(idx),(name),__VA_ARGS__, NULL)
int __get_property_string_enum(js_State *J,int idx,const char* name,...);

#define js_check_args(J,prefix,...) _js_check_args((J),(prefix),__VA_ARGS__, NULL)
int _js_check_args(js_State *J,const char * prefix,...);

int run_main_file(js_State *J,Allocator alloc,const char* main);

#endif //_JS_HELPER_

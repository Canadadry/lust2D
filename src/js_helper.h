#ifndef _JS_HELPER_
#define _JS_HELPER_

#include "../vendor/mujs/mujs.h"
#include "optional.h"
#include "allocator.h"

typedef const char* string;

CREATE_OPTIONAL_TYPE(double)
CREATE_OPTIONAL_TYPE(string)

int read_full_file(const char *filename,char ** file);
const char* read_full_file_errno(int err);

int init_js(js_State* J,Allocator alloc);
OPTIONAL(double) get_property_number(js_State *J, int idx,const char* name);
double get_property_number_or(js_State *J, int idx,const char* name, double def);
int get_property_int_or(js_State *J, int idx,const char* name, int def);
OPTIONAL(string) get_property_string(js_State *J, int idx,const char* name);
const char*  get_property_string_or(js_State *J, int idx,const char* name, const char*  def);

#define get_property_string_enum(J,idx,name,...) __get_property_string_enum((J),(idx),(name),__VA_ARGS__, NULL)
int __get_property_string_enum(js_State *J,int idx,const char* name,...);
#endif //_JS_HELPER_

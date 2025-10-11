#include "js_helper.h"

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

double get_property_int_or(js_State *J, int idx,const char* name, int def){
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

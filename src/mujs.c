#include "../vendor/raylib/raylib.h"
#include "../vendor/mujs/mujs.h"
#include <stdbool.h>
#include "../vendor/jsx_parser/jsx_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "js_helper.h"
#include "args.h"
#include "allocator.h"
#include "vector2.h"
#include "text.h"

static inline void* user_realloc(void* userdata,void* ptr, size_t size){
    return realloc(ptr,size);
}

static inline void user_free(void* userdata,void* ptr){
    free(ptr);
}

int main(int argc, char** argv){
    HasArgResult basedir = has_arg(argc,argv,"-basedir");
    if(basedir.ok==1&&basedir.next!=NULL&&basedir.next[0]!='-'){
        printf("cd to %s \n",basedir.next);
        ChangeDirectory(basedir.next);
    }
    const char* mainfile="main.js";
    HasArgResult main = has_arg(argc,argv,"-i");
    if(main.ok==1&&main.next!=NULL&&main.next[0]!='-'){
        mainfile = main.next;
    }
    Allocator alloc = (Allocator){
    		.realloc_fn = user_realloc,
    		.free_fn = user_free,
    };

    js_State *J = js_newstate(NULL, NULL,  0);
	if (!J) {
		fprintf(stderr, "Could not initialize MuJS.\n");
		exit(1);
	}
	int ret = init_js(J,alloc);
	if(ret != 0){
	    return ret;
	}

	if(run_main_file(J,alloc,mainfile) !=0){
	    printf("failed while running main files\n");
	    js_throw(J);
	    return 1;
	}
    return 0;
}

VECTOR2(float) sizer(FontParam param, int codepoint) {
	return (VECTOR2(float)){0};
}

void drawer(FontParam param, int codepoint, VECTOR2(int) pos) {
}

#include "build.h"

#define TARGET "lust2D"

#define MACOS_FLAGS "-framework CoreVideo -framework CoreAudio -framework IOKit -framework Cocoa -framework OpenGL"

void build_lib(int debug){
    BuildCtx ctx = build_init();
    build_set_src_dir(&ctx, "src");
    build_set_build_dir(&ctx, "build");
    build_make_dir(ctx.build_dir);

    if (!debug){
        build_set_cflags(&ctx, "-Wall -O2");
        build_set_ldflags(&ctx, "-Lvendor/raylib/macos -lraylib -Lvendor/mujs/macos -lmujs -Lvendor/jsx_parser/macos -ljsx_parser "MACOS_FLAGS);
        build_add_static_lib(&ctx, "lib"TARGET".a");
        build_add_entry_point(&ctx, "main.c",TARGET);
    }else{
        build_set_cflags(&ctx, "-Wall -Werror -g -fsanitize=address");
        build_set_ldflags(&ctx, "-fsanitize=address -Lvendor/raylib/macos -lraylib -Lvendor/mujs/macos -lmujsd -Lvendor/jsx_parser/macos -ljsx_parser "MACOS_FLAGS);
        build_add_static_lib(&ctx, "lib"TARGET"d.a");
        build_add_entry_point(&ctx, "main.c",TARGET"d");
    }

    build_compile(&ctx, "*.c");
    build_link_all(&ctx);
}

void build_test(){
    BuildCtx ctx = build_init();
    build_set_src_dir(&ctx, "test");
    build_set_build_dir(&ctx, "build/tests");
    build_set_cflags(&ctx, "-Wall -g -fsanitize=address");
    build_set_ldflags(&ctx, "-Lbuild -fsanitize=address -Lvendor/raylib/macos -lraylib -l"TARGET"d "MACOS_FLAGS);

    build_make_dir(ctx.build_dir);

    build_add_entry_point(&ctx, "main.c", "run_testsd");

    build_compile(&ctx, "*.c");
    build_link_all(&ctx);
}

int main(int argc, char **argv) {

    if (build_has_arg(argc, argv,  "clean")){
        BUILD_RUN_CMD("rm", "-rf", "build");
    }

    build_lib(build_has_arg(argc, argv,  "debug","test"));
    if (build_has_arg(argc, argv,  "test")){
        build_test();
        BUILD_RUN_CMD("./build/tests/run_testsd");
    }

    if (build_has_arg(argc, argv,  "run")){
        if(build_has_arg(argc, argv, "debug","test")){
            BUILD_RUN_CMD("./build/"TARGET"d","example.jsx");
        }else{
            BUILD_RUN_CMD("./build/"TARGET,"example.jsx");
        }
    }

    return 0;
}

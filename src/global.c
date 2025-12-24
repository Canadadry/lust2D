#include "hashmap.h"
#include "jsx_class.h"
#include "raylib_js.h"
#include "js_helper.h"

HASHMAP(init_node_fn)* hmap_init_node_fn;
ARRAY(InitNodeFn1)* array_init_node_fn1;
HASHMAP(Texture)* hmap_texture;
HASHMAP(Image)* hmap_image;
HASHMAP(Font)* hmap_font;
HASHMAP(Sound)* hmap_sound;

Tree* ui_tree= NULL;

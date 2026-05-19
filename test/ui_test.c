#include "minitest.h"
#include "../src/ui.h"
#include "json.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
    const char *name;
    NodeIndex head;
    Tree tree;
    ARRAY(PainterCommand) expected;
}TestCase;

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX_SOURCE_IMG_LEN 128
#define JSON_MAX_NODES 64

bool painter_match(const char* test_name,int i,Painter exp, Painter got){
    if(exp.kind != got.kind){
        TEST_ERRORF(test_name,
            "failed command %d painter dont match got %d want:%d\n",
            i,got.kind,exp.kind
        );
        return false;
    }
    switch(exp.kind){
        case PAINTER_NONE: break;
        case PAINTER_RECT:
        if(exp.value.rect.fill.value.color.value != got.value.rect.fill.value.color.value){
            TEST_ERRORF(test_name,
                "failed command %d painter rect color dont match got %x want:%x\n",
                i,got.value.rect.fill.value.color.value,exp.value.rect.fill.value.color.value
            );
            return false;
        }
        break;
        case PAINTER_IMG:
        if(exp.value.img.color.value != got.value.img.color.value){
            TEST_ERRORF(test_name,
                "failed command %d painter image color dont match got %x want:%x\n",
                i,got.value.img.color.value,exp.value.img.color.value
            );
            return false;
        }
        if(strncmp(exp.value.img.source, exp.value.img.source,MAX_SOURCE_IMG_LEN)!=0){
            TEST_ERRORF(test_name,
                "failed command %d painter image source dont match got %s want:%s\n",
                i,got.value.img.source,exp.value.img.source
            );
            return false;
        }
        case PAINTER_TILE:
        if(exp.value.tile.color.value != got.value.tile.color.value){
            TEST_ERRORF(test_name,
                "failed command %d painter tile color dont match got %x want:%x\n",
                i,got.value.tile.color.value,exp.value.tile.color.value
            );
            return false;
        }
        if(strncmp(exp.value.tile.source, exp.value.tile.source,MAX_SOURCE_IMG_LEN)!=0){
            TEST_ERRORF(test_name,
                "failed command %d painter tile source dont match got %s want:%s\n",
                i,got.value.img.source,exp.value.img.source
            );
            return false;
        }
        case PAINTER_NINE_PATCH:
        if(exp.value.npatch.color.value != got.value.npatch.color.value){
            TEST_ERRORF(test_name,
                "failed command %d painter npatch color dont match got %x want:%x\n",
                i,got.value.npatch.color.value,exp.value.npatch.color.value
            );
            return false;
        }
        if(strncmp(exp.value.npatch.source, exp.value.npatch.source,MAX_SOURCE_IMG_LEN)!=0){
            TEST_ERRORF(test_name,
                "failed command %d painter npatch source dont match got %s want:%s\n",
                i,got.value.npatch.source,exp.value.npatch.source
            );
            return false;
        }
        case PAINTER_TEXT:
        if(exp.value.text.color.value != got.value.text.color.value){
            TEST_ERRORF(test_name,
                "failed command %d painter text color dont match got %x want:%x\n",
                i,got.value.text.color.value,exp.value.text.color.value
            );
            return false;
        }
        if(strncmp(exp.value.text.msg, exp.value.text.msg,MAX_SOURCE_IMG_LEN)!=0){
            TEST_ERRORF(test_name,
                "failed command %d painter text msg dont match got %s want:%s\n",
                i,got.value.text.msg,exp.value.text.msg
            );
            return false;
        }
        break;
    }
    return true;
}

bool painter_command_match(const char* test_name,int i,PainterCommand exp, PainterCommand got){
    if(exp.x == got.x && exp.y == got.y && exp.w == got.w && exp.h == got.h){
        return painter_match(test_name,i,exp.painter,got.painter);
    }
    TEST_ERRORF(test_name,
        "failed command %d dont match\ngot\nx:%d y:%d w:%d h:%d\n want\nx:%d y:%d w:%d h:%d\n",
        i,
        got.x,got.y,got.w,got.h,
        exp.x,exp.y,exp.w,exp.h
    );
    return false;
}

VECTOR2(int) mesure_content_fn(void *userdata,Painter p){
    VECTOR2(int) ret = {0};
    if(p.kind==PAINTER_IMG){
        sscanf(p.value.img.source,"%dx%d",&ret.x,&ret.y);
    }
    return ret;
}

int wrap_content_fn(void *userdata,Painter p,int width){
    VECTOR2(int) content = mesure_content_fn(userdata,p);
    int height = content.x + content.y - width;
    return height < 0 ? 0 : height;
}

/* ---- JSON helpers ---- */

static int jstr_eq(struct json_string_s *s, const char *key) {
    size_t klen = strlen(key);
    return s->string_size == klen && memcmp(s->string, key, klen) == 0;
}

static struct json_object_element_s *jobj_find(struct json_object_s *obj, const char *key) {
    if (!obj) return NULL;
    for (struct json_object_element_s *el = obj->start; el; el = el->next)
        if (jstr_eq(el->name, key)) return el;
    return NULL;
}

static const char *jobj_str(struct json_object_s *obj, const char *key) {
    struct json_object_element_s *el = jobj_find(obj, key);
    if (!el) return NULL;
    struct json_string_s *s = json_value_as_string(el->value);
    return s ? s->string : NULL;
}

static int jobj_int(struct json_object_s *obj, const char *key, int def) {
    struct json_object_element_s *el = jobj_find(obj, key);
    if (!el) return def;
    struct json_number_s *n = json_value_as_number(el->value);
    return n ? (int)strtol(n->number, NULL, 10) : def;
}

static struct json_object_s *jobj_obj(struct json_object_s *obj, const char *key) {
    struct json_object_element_s *el = jobj_find(obj, key);
    if (!el) return NULL;
    return json_value_as_object(el->value);
}

static struct json_array_s *jobj_arr(struct json_object_s *obj, const char *key) {
    struct json_object_element_s *el = jobj_find(obj, key);
    if (!el) return NULL;
    return json_value_as_array(el->value);
}

/* ---- Type parsers ---- */

static SizeKind parse_size_kind(const char *s) {
    if (s && strcmp(s, "fixed") == 0) return SizeKindFixed;
    if (s && strcmp(s, "grow")  == 0) return SizeKindGrow;
    return SizeKindFit;
}

static PreferedUse parse_pref_use(const char *s) {
    if (s && strcmp(s, "to_min") == 0) return PreferedToMin;
    if (s && strcmp(s, "to_max") == 0) return PreferedToMax;
    return PreferedToNone;
}

static Size parse_size(struct json_object_s *obj) {
    Size s = {0};
    if (!obj) return s;
    s.kind = parse_size_kind(jobj_str(obj, "kind"));
    if (s.kind == SizeKindFixed) {
        s.size = jobj_int(obj, "size", 0);
    } else {
        s.bound.min      = jobj_int(obj, "min", 0);
        s.bound.max      = jobj_int(obj, "max", 0);
        s.bound.pref_use = parse_pref_use(jobj_str(obj, "pref_use"));
    }
    return s;
}

static PainterKind parse_painter_kind(const char *s) {
    if (!s) return PAINTER_NONE;
    if (strcmp(s, "img")        == 0) return PAINTER_IMG;
    if (strcmp(s, "rect")       == 0) return PAINTER_RECT;
    if (strcmp(s, "nine_patch") == 0) return PAINTER_NINE_PATCH;
    if (strcmp(s, "tile")       == 0) return PAINTER_TILE;
    if (strcmp(s, "text")       == 0) return PAINTER_TEXT;
    return PAINTER_NONE;
}

static Painter parse_painter(struct json_object_s *obj) {
    Painter p = {0};
    if (!obj) return p;
    p.kind = parse_painter_kind(jobj_str(obj, "kind"));
    switch (p.kind) {
        case PAINTER_IMG: {
            const char *src = jobj_str(obj, "source");
            if (src) strncpy(p.value.img.source, src, SRC_LEN - 1);
            p.value.img.color.value = jobj_int(obj, "color", 0);
            break;
        }
        case PAINTER_NINE_PATCH: {
            const char *src = jobj_str(obj, "source");
            if (src) strncpy(p.value.npatch.source, src, SRC_LEN - 1);
            p.value.npatch.color.value = jobj_int(obj, "color", 0);
            break;
        }
        case PAINTER_TILE: {
            const char *src = jobj_str(obj, "source");
            if (src) strncpy(p.value.tile.source, src, SRC_LEN - 1);
            p.value.tile.color.value = jobj_int(obj, "color", 0);
            break;
        }
        case PAINTER_TEXT: {
            const char *msg = jobj_str(obj, "msg");
            if (msg) strncpy(p.value.text.msg, msg, MSG_LEN - 1);
            p.value.text.color.value = jobj_int(obj, "color", 0);
            break;
        }
        default: break;
    }
    return p;
}

static Layout parse_layout(const char *s) {
    if (s && strcmp(s, "vertical") == 0) return LayoutVertical;
    if (s && strcmp(s, "stack")    == 0) return LayoutStack;
    return LayoutHorizontal;
}

static Align parse_align(const char *s) {
    if (s && strcmp(s, "middle") == 0) return AlignMiddle;
    if (s && strcmp(s, "end")    == 0) return AlignEnd;
    return AlignBegin;
}

static Node parse_node(struct json_object_s *obj) {
    Node n = {0};
    if (!obj) return n;
    n.painter = parse_painter(jobj_obj(obj, "painter"));
    struct json_object_s *pos_obj = jobj_obj(obj, "pos");
    if (pos_obj) {
        n.pos.x = jobj_int(pos_obj, "x", 0);
        n.pos.y = jobj_int(pos_obj, "y", 0);
    }
    struct json_array_s *size_arr = jobj_arr(obj, "size");
    if (size_arr) {
        struct json_array_element_s *el = size_arr->start;
        if (el) { n.size.x = parse_size(json_value_as_object(el->value)); el = el->next; }
        if (el) { n.size.y = parse_size(json_value_as_object(el->value)); }
    }
    n.layout = parse_layout(jobj_str(obj, "layout"));
    struct json_object_s *align_obj = jobj_obj(obj, "align");
    if (align_obj) {
        n.align.x = parse_align(jobj_str(align_obj, "x"));
        n.align.y = parse_align(jobj_str(align_obj, "y"));
    }
    n.margin = jobj_int(obj, "margin", 0);
    struct json_object_s *pad_obj = jobj_obj(obj, "padding");
    if (pad_obj) {
        n.padding.left   = jobj_int(pad_obj, "left",   0);
        n.padding.right  = jobj_int(pad_obj, "right",  0);
        n.padding.top    = jobj_int(pad_obj, "top",    0);
        n.padding.bottom = jobj_int(pad_obj, "bottom", 0);
    }
    n.children_count = jobj_int(obj, "children_count", 0);
    n.first_children = jobj_int(obj, "first_children", -1);
    n.last_children  = jobj_int(obj, "last_children",  -1);
    n.next           = jobj_int(obj, "next",            -1);
    return n;
}

static PainterCommand parse_painter_command(struct json_object_s *obj) {
    PainterCommand cmd = {0};
    if (!obj) return cmd;
    cmd.x = jobj_int(obj, "x", 0);
    cmd.y = jobj_int(obj, "y", 0);
    cmd.w = jobj_int(obj, "w", 0);
    cmd.h = jobj_int(obj, "h", 0);
    cmd.painter = parse_painter(jobj_obj(obj, "painter"));
    return cmd;
}

/* ---- Test runner ---- */

static void run_json_test_case(struct json_object_s *jcase) {
    TestCase tc = {0};
    STATIC_ZERO_INIT(Node,          tc.tree.nodes,           jnodes,     JSON_MAX_NODES);
    STATIC_ZERO_INIT(PainterCommand,tc.tree.commands,        jcommands,  JSON_MAX_NODES);
    STATIC_ZERO_INIT(Growable,      tc.tree.growables,       jgrowables, JSON_MAX_NODES);
    STATIC_ZERO_INIT(ptr_growable,  tc.tree.sorted_growables,jsorted,    JSON_MAX_NODES);
    STATIC_ZERO_INIT(PainterCommand,tc.expected,             jexpected,  JSON_MAX_NODES);
    tc.tree.mesure_content_fn = mesure_content_fn;
    tc.tree.wrap_content_fn   = wrap_content_fn;
    tc.name = jobj_str(jcase, "name");
    tc.head = jobj_int(jcase, "head", 0);

    struct json_array_s *nodes_arr = jobj_arr(jcase, "nodes");
    if (nodes_arr)
        for (struct json_array_element_s *el = nodes_arr->start; el && tc.tree.nodes.len < JSON_MAX_NODES; el = el->next)
            tc.tree.nodes.data[tc.tree.nodes.len++] = parse_node(json_value_as_object(el->value));

    struct json_array_s *exp_arr = jobj_arr(jcase, "expected");
    if (exp_arr)
        for (struct json_array_element_s *el = exp_arr->start; el && tc.expected.len < JSON_MAX_NODES; el = el->next)
            tc.expected.data[tc.expected.len++] = parse_painter_command(json_value_as_object(el->value));

    mt_total++;
    compute(&tc.tree, tc.head);
    if (tc.expected.len != tc.tree.commands.len)
        TEST_ERRORF(tc.name, "failed command len exp %d got %d\n", tc.expected.len, tc.tree.commands.len);
    int min_len = MIN(tc.expected.len, tc.tree.commands.len);
    for (int i = 0; i < min_len; i++)
        painter_command_match(tc.name, i, tc.expected.data[i], tc.tree.commands.data[i]);
}

void test_ui_compute(void) {
    FILE *f = fopen("test/ui_test_cases.json", "r");
    if (!f) {
        fprintf(stderr, "Cannot open test/ui_test_cases.json\n");
        return;
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);
    char *buf = malloc(fsize + 1);
    fread(buf, 1, fsize, f);
    buf[fsize] = '\0';
    fclose(f);

    struct json_value_s *root = json_parse(buf, fsize);
    free(buf);
    if (!root) {
        fprintf(stderr, "Failed to parse test/ui_test_cases.json\n");
        return;
    }

    struct json_array_s *arr = json_value_as_array(root);
    if (arr)
        for (struct json_array_element_s *el = arr->start; el; el = el->next)
            run_json_test_case(json_value_as_object(el->value));

    free(root);
}

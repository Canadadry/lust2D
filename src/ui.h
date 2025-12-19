#ifndef _UI_H
#define _UI_H

#include "../vendor/raylib/raylib.h"
#include "allocator.h"
#include "dynamicarray.h"
#include "rectangle.h"
#include "vector2.h"
#include <stdbool.h>

#define ID_LEN 32
#define SRC_LEN 255
#define MSG_LEN 255

typedef enum {
    PAINTER_NONE,
    PAINTER_RECT,
    PAINTER_IMG,
    PAINTER_NINE_PATCH,
    PAINTER_TILE,
    PAINTER_TEXT,
} PainterKind;

const char* to_painter_kind_name(PainterKind pk);

typedef union{
    Color rgba;
    int value;
} UiColor;

typedef struct{
    UiColor color;
    float at;
}GradientStop;

#define GRADIENT_MAX_POINTS 4
typedef struct {
    GradientStop stop[GRADIENT_MAX_POINTS];
    int count;
    Vector2 start;
    Vector2 end;
}Gradient;

typedef enum{
    COLOR_RAW,
    COLOR_GRADIENT
} ColorKind;

typedef struct{
    ColorKind kind;
    union{
        UiColor color;
        Gradient gradient;
    } value;
} PaintPattern;

typedef enum{
    LayoutHorizontal,
    LayoutVertical,
    LayoutStack,
}Layout;

typedef enum{
    AlignBegin,
    AlignMiddle,
    AlignEnd,
}Align;

typedef enum{
    SizeKindFit,
    SizeKindFixed,
    SizeKindGrow,
}SizeKind;

const char* to_size_kind_name(SizeKind sk);

typedef enum{
    PreferedToNone,
    PreferedToMin,
    PreferedToMax
} PreferedUse;

typedef struct{
    SizeKind kind;
    union {
        struct{
            int min;
            int max;
            PreferedUse pref_use;
        } bound;
        int size;
    };
} Size;

typedef struct{
    int left;
    int right;
    int top;
    int bottom;
}Padding;

CREATE_VECTOR2_TYPE(Size)
CREATE_VECTOR2_TYPE(Align)
CREATE_RECTANGLE_TYPE(int)

typedef struct{
    PaintPattern fill;
    UiColor border_color;
    int boder_width;
    float radius;
    int segment;
}PainterRect;

typedef struct{
    UiColor color;
    char source[255];
}PainterImage;

typedef struct{
    UiColor color;
    char source[255];
    int left;
    int right;
    int top;
    int bottom;
}PainterNinePatch;

typedef struct{
    UiColor color;
    char msg[255];
    char font[255];
    int font_size;
    int spacing;
    int line_spacing;
    VECTOR2(Align) align;
}PainterText;

typedef struct{
    UiColor color;
    char source[255];
   	VECTOR2(int) at;
    VECTOR2(int) padding;
    VECTOR2(int) size;
}PainterTile;

typedef struct{
    PainterKind kind;
    union{
        PainterRect rect;
        PainterImage img;
        PainterNinePatch npatch;
        PainterTile tile;
        PainterText text;
    } value;
}Painter;

typedef int NodeIndex;
typedef struct {
    char            id[ID_LEN];
	Painter         painter;
	VECTOR2(int)    pos;
	VECTOR2(Size)   size;
	Layout          layout;
	VECTOR2(Align)  align;
	int             margin;
	Padding         padding;
	RECTANGLE(int)  computed_box;
	NodeIndex       first_children;
	NodeIndex       last_children;
	NodeIndex       next;
	int             children_count;
}Node;

typedef struct {
    int x;
    int y;
    int w;
    int h;
    Painter	painter;
    char id[ID_LEN];
} PainterCommand;

typedef struct {
	NodeIndex id;
	int val;
	int min;
	int max;
	bool to_remove;
} Growable;

CREATE_ARRAY_TYPE(Node)
CREATE_ARRAY_TYPE(PainterCommand)
CREATE_ARRAY_TYPE(Growable)

typedef Growable* ptr_growable;
CREATE_ARRAY_TYPE(ptr_growable)

typedef VECTOR2(int)(*MesureContentFn)(void* userdata,Painter painter);
typedef int (*WrapContentFn)(void* userdata,Painter painter,int width);

typedef struct {
	ARRAY(Node) nodes;
	ARRAY(PainterCommand) commands;
	ARRAY(Growable) growables;
	ARRAY(ptr_growable) sorted_growables;
	// images: map[cstring]rl.Texture,
	MesureContentFn mesure_content_fn;
	void* mesure_content_userdata;
	WrapContentFn wrap_content_fn;
	void* wrap_content_userdata;
}Tree;

#define STATIC_INIT_TREE(tree,max_size) \
    STATIC_ZERO_INIT(PainterCommand, (tree).commands,commands,max_size); \
    STATIC_ZERO_INIT(Growable, (tree).growables,growables,max_size);\
    STATIC_ZERO_INIT(ptr_growable, (tree).sorted_growables,ptr_growables,max_size);\
    STATIC_ZERO_INIT(Node, (tree).nodes,nodes,max_size);

void tree_init(Tree *tree,Allocator alloc);
void compute(Tree* tree, NodeIndex idx);
void link_child(Tree *tree,NodeIndex parent,NodeIndex child);

int compute_align( Align a, int remaining);
#endif

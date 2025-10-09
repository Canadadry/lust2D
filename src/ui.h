#ifndef _UI_H
#define _UI_H

#include "../vendor/raylib/raylib.h"
#include "allocator.h"
#include "dynamicarray.h"
#include "rectangle.h"
#include "vector2.h"
#include <stdbool.h>

typedef enum {
    PAINTER_NONE,
    PAINTER_RECT,
    PAINTER_IMG,
} PainterKind;

typedef union{
    Color rgba;
    int value;
} UiColor;

typedef struct{
    UiColor color;
}PainterRect;

typedef struct{
    UiColor color;
    const char* source;
}PainterImage;

typedef struct{
    PainterKind kind;
    union{
        PainterRect rect;
        PainterImage img;
    } value;
}Painter;

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

typedef struct{
    SizeKind kind;
    union {
        struct{
            int min;
            int max;
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

CREATE_VECTOR2_TYPE(int)
CREATE_VECTOR2_TYPE(Size)
CREATE_VECTOR2_TYPE(Align)
CREATE_RECTANGLE_TYPE(int)

typedef int NodeIndex;
typedef struct {
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
    PainterCommand commands[max_size]={0}; \
    Growable growables[max_size]={0}; \
    ptr_growable ptr_growables[max_size]={0}; \
    Node nodes[max_size]={0}; \
    tree.nodes.capacity=max_size; \
    tree.nodes.data=nodes; \
    tree.commands.capacity=max_size; \
    tree.commands.data=commands; \
    tree.growables.data=growables; \
    tree.growables.capacity=max_size; \
    tree.sorted_growables.data=ptr_growables; \
    tree.sorted_growables.capacity=max_size

void tree_init(Tree *tree,Allocator alloc);
void compute(Tree* tree, NodeIndex idx);
void draw(Tree tree, NodeIndex idx);
void link_child(Tree *tree,NodeIndex parent,NodeIndex child);

#endif

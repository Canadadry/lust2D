#ifndef _UI_H
#define _UI_H

#include "../vendor/raylib/raylib.h"
#include "dynamicarray.h"

typedef enum {
    PAINTER_NONE,
    PAINTER_RECT,
} PainterKind;

typedef struct{
    Color color;
}PainterRect;

typedef struct{
    PainterKind kind;
    union{
        PainterRect rect;
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

typedef int NodeIndex;
typedef struct {
	Painter   painter;
	int       pos[2];
	Size      size[2];
	Layout    layout;
	Align     align[2];
	int       margin;
	Padding   padding;
	Rectangle computed_box;
	NodeIndex first_children;
	NodeIndex last_children;
	NodeIndex next;
}Node;

typedef struct {
    int x;
    int y;
    int w;
    int h;
    Painter	painter;
} PainterCommand;

CREATE_ARRAY_TYPE(Node)
CREATE_ARRAY_TYPE(PainterCommand)

typedef struct {
	ARRAY(Node) nodes;
	ARRAY(PainterCommand) commands;
	// images: map[cstring]rl.Texture,
}Tree;

void compute(Tree tree, NodeIndex idx);
void draw(Tree tree, NodeIndex idx);
void link_child(Tree *tree,NodeIndex parent,NodeIndex child);

#endif

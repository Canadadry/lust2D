#ifndef _UI_H
#define _UI_H

#include "../vendor/raylib/raylib.h"

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

typedef int NodeIndex;
typedef struct {
	Painter   painter;
	Rectangle rect;
	NodeIndex children;
	NodeIndex next;
}Node;

typedef struct {
	Node* nodes;
	int nodes_len;
	// images: map[cstring]rl.Texture,
}Tree;

void draw(Tree tree, NodeIndex idx);

#endif

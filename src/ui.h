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

typedef int NodeIndex;
typedef struct {
	Painter   painter;
	Rectangle rect;
	NodeIndex first_children;
	NodeIndex last_children;
	NodeIndex next;
}Node;


CREATE_ARRAY_TYPE(Node)

typedef struct {
	ARRAY(Node) nodes;
	// images: map[cstring]rl.Texture,
}Tree;

void draw(Tree tree, NodeIndex idx);
void link_child(Tree *tree,NodeIndex parent,NodeIndex child);

#endif

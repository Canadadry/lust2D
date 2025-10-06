#include "ui.h"

WRITE_ARRAY_IMPL(Node)

WRITE_ARRAY_IMPL(PainterCommand)

void compute(Tree tree, NodeIndex idx){
}

void draw(Tree tree, NodeIndex idx){
	if(idx >= tree.nodes.len|| idx<0) {
		return;
	}
	switch(tree.nodes.data[idx].painter.kind){
	case PAINTER_NONE:
	    break;
	case PAINTER_RECT:
		// DrawRectangleRec(tree.nodes.data[idx].rect, tree.nodes.data[idx].painter.value.rect.color);
        break;
    // case ImagePainterData:
	// 	rl.DrawTexturePro(
	// 		tree.images[p.source],
	// 		rl.Rectangle {
	// 			0,
	// 			0,
	// 			f32(tree.images[p.source].width),
	// 			f32(tree.images[p.source].height),
	// 		},
	// 		tree.nodes[idx].rect,
	// 		rl.Vector2{},
	// 		0,
	// 		rl.WHITE,
	// 	)
	}
	if(tree.nodes.data[idx].first_children>=0){
	    draw(tree, tree.nodes.data[idx].first_children);
	}
	if(tree.nodes.data[idx].next>=0){
	    draw(tree, tree.nodes.data[idx].next);
	}
}


void link_child(Tree *tree,NodeIndex parent,NodeIndex child){
    if (parent==child){
        return;
    }
   	if(parent >= tree->nodes.len|| parent<0) {
		return;
	}

   	if(parent >= tree->nodes.len|| parent<0) {
		return;
	}
    if(
           tree->nodes.data[parent].first_children ==-1
        && tree->nodes.data[parent].last_children  ==-1
    ){
        tree->nodes.data[parent].first_children=child;
        tree->nodes.data[parent].last_children=child;
        return;
    }
    NodeIndex last = tree->nodes.data[parent].last_children;
    tree->nodes.data[last].next=child;
    tree->nodes.data[parent].last_children=child;
}

#include "ui.h"

void draw(Tree tree, NodeIndex idx){
	if(idx >= tree.nodes_len|| idx<0) {
		return;
	}
	switch(tree.nodes[idx].painter.kind){
	case PAINTER_NONE:
	    break;
	case PAINTER_RECT:
		DrawRectangleRec(tree.nodes[idx].rect, tree.nodes[idx].painter.value.rect.color);
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
	if(tree.nodes[idx].children>=0){
	    draw(tree, tree.nodes[idx].children);
	}
	if(tree.nodes[idx].next>=0){
	    draw(tree, tree.nodes[idx].next);
	}
}

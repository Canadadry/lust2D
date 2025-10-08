#include "ui.h"
#include "vector2.h"

#define MAX_ITER 100
#define SAFE_WHILE(cond) \
    for (int _sw_guard = 0; (cond) && _sw_guard < (MAX_ITER); _sw_guard++)
#define MAX(x,y) ((x)>(y)?(x):(y))

WRITE_ARRAY_IMPL(Node)
WRITE_ARRAY_IMPL(PainterCommand)

VECTOR2(int) no_measure_content(void* userdata,Painter painter){
    return (VECTOR2(int)){0};
}

void fit_width_sizing(Node* el, int min,int max) {
	el->computed_box.w += el->padding.left + el->padding.right;
	if(el->layout == LayoutHorizontal) {
		el->computed_box.w += (el->children_count - 1) * el->margin;
	}
	if(el->computed_box.w < min){
		el->computed_box.w = min;
	}
	if(el->computed_box.w > max && max > 0){
		el->computed_box.w = max;
	}
}

void compute_fit_size_width(Tree* tree, NodeIndex idx,NodeIndex parent_id)
{
   	Node* el = &tree->nodes.data[idx];
    NodeIndex child_id = tree->nodes.data[idx].first_children;
	SAFE_WHILE(child_id >=0){
	    compute_fit_size_width(tree,child_id,idx);
	    child_id = tree->nodes.data[child_id].next;
	}

	switch(el->size.x.kind) {
	case SizeKindFixed:
		el->computed_box.w = el->size.x.size;
		break;
	case SizeKindFit:
		fit_width_sizing(el, el->size.x.bound.min, el->size.x.bound.max);
		break;
	case SizeKindGrow:
	    fit_width_sizing(el, el->size.x.bound.min, el->size.x.bound.max);
		el->computed_box.w = MAX(
		    el->computed_box.w,
			tree->mesure_content_fn(tree->mesure_content_userdata, el->painter).x
		);
	}

	if(parent_id<0){
		return;
	}
	switch(tree->nodes.data[parent_id].layout){
	case LayoutHorizontal:
		tree->nodes.data[parent_id].computed_box.w += el->computed_box.w;
		break;
	case LayoutVertical:
		tree->nodes.data[parent_id].computed_box.w = MAX(
		    el->computed_box.w,
			tree->nodes.data[parent_id].computed_box.w
		);
		break;
	case LayoutStack:
    	tree->nodes.data[parent_id].computed_box.w = MAX(
    	    el->computed_box.w,
    		tree->nodes.data[parent_id].computed_box.w
    	);
		break;
	}
}

void compute_shrink_size_width(Tree* tree, NodeIndex idx)
{
}

void compute_grow_size_width(Tree* tree, NodeIndex idx)
{
}

void compute_wrap(Tree* tree, NodeIndex idx)
{
}

void compute_fit_size_height(Tree* tree, NodeIndex idx,NodeIndex parent_id)
{
}

void compute_shrink_size_height(Tree* tree, NodeIndex idx)
{
}

void compute_grow_size_height(Tree* tree, NodeIndex idx)
{
}

VECTOR2(int) get_remaining(Tree* tree,NodeIndex idx){
    return (VECTOR2(int)){0};
}

int compute_align( Align a, int remaining){
    switch (a) {
	case AlignBegin:
		return 0;
	case AlignMiddle:
		return remaining >> 1;
	case AlignEnd:
		return remaining;
	}
}

void compute_position(Tree* tree, NodeIndex idx,int x, int y)
{
   	Node* self = &tree->nodes.data[idx];
	VECTOR2(int) remaining_across = get_remaining(tree, idx);
	int remaining_along =0;

	VECTOR2(int) align =  (VECTOR2(int)){
		compute_align(self->align.x, remaining_across.x),
		compute_align(self->align.y, remaining_across.y),
	};

	self->computed_box.x = self->pos.x + x;
	self->computed_box.y = self->pos.y + y;

	VECTOR2(int) offset = (VECTOR2(int)){.x=self->padding.left, .y=self->padding.top};
	NodeIndex child_id = self->first_children;
	SAFE_WHILE(child_id >=0){
		Node* child = &tree->nodes.data[child_id];
		switch(self->layout){
		case LayoutHorizontal:
			remaining_along =
				self->computed_box.h - self->padding.top - self->padding.bottom -  child->computed_box.h;
			align.y = compute_align( self->align.y, remaining_along);
			break;
		case LayoutVertical:
			remaining_along =
				 self->computed_box.w -  self->padding.left -  self->padding.right -  child->computed_box.w;
			align.x = compute_align( self->align.x, remaining_along);
			break;
		case LayoutStack:
			remaining_along =
				 self->computed_box.h -  self->padding.top -  self->padding.bottom -  child->computed_box.h;
			align.y = compute_align( self->align.y, remaining_along);
			remaining_along =
				 self->computed_box.w -  self->padding.left -  self->padding.right -  child->computed_box.w;
			align.x = compute_align( self->align.x, remaining_along);
			break;
		}
		compute_position(
			tree,
			child_id,
			 self->computed_box.x + offset.x + align.x,
			 self->computed_box.y + offset.y + align.y
		);
		switch(self->layout){
		case LayoutHorizontal:
			offset.x += child->computed_box.w +  self->margin;
			break;
		case LayoutVertical:
			offset.y += child->computed_box.h +  self->margin;
			break;
		case LayoutStack:
		    break;
		}
		child_id = child->next;
	}
}

void compute_draw_command(Tree* tree, NodeIndex idx,NodeIndex command_idx)
{
    array_append_PainterCommand(&tree->commands,(PainterCommand){
        .x       = tree->nodes.data[idx].computed_box.x,
        .y       = tree->nodes.data[idx].computed_box.y,
        .w       = tree->nodes.data[idx].computed_box.w,
        .h       = tree->nodes.data[idx].computed_box.h,
        .painter = tree->nodes.data[idx].painter,
    });
    NodeIndex child_id = tree->nodes.data[idx].first_children;
	SAFE_WHILE(child_id >=0){
	    compute_draw_command(tree,child_id,command_idx+1);
	    child_id = tree->nodes.data[child_id].next;
	}
}

void compute(Tree* tree, NodeIndex idx){
    if (tree->mesure_content_fn==NULL){
        tree->mesure_content_fn=no_measure_content;
    }
    compute_fit_size_width(tree,idx,-1);
    compute_shrink_size_width(tree,idx);
    compute_grow_size_width(tree,idx);
    if(tree->wrap_content_fn!=NULL){
        compute_wrap(tree,idx);
    }
    compute_fit_size_height(tree,idx,-1);
    compute_shrink_size_height(tree,idx);
    compute_grow_size_height(tree,idx);
    compute_position(tree,idx,0,0);
    compute_draw_command(tree,idx,0);
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
        tree->nodes.data[parent].children_count=1;
        tree->nodes.data[parent].first_children=child;
        tree->nodes.data[parent].last_children=child;
        return;
    }
    NodeIndex last = tree->nodes.data[parent].last_children;
    tree->nodes.data[last].next=child;
    tree->nodes.data[parent].last_children=child;
    tree->nodes.data[parent].children_count++;
}

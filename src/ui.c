#include "ui.h"
#include "vector2.h"
#include <stdio.h>
#include <string.h>

#define MAX_ITER 100
#define SAFE_WHILE(cond) \
    for (int _sw_guard = 0; (cond) && _sw_guard < (MAX_ITER); _sw_guard++)
#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

WRITE_ARRAY_IMPL(Node)
WRITE_ARRAY_IMPL(PainterCommand)
WRITE_ARRAY_IMPL(Growable)
WRITE_ARRAY_IMPL(ptr_growable)

VECTOR2(int) no_measure_content(void* userdata,Painter painter){
    return (VECTOR2(int)){0};
}

VECTOR2(int) get_remaining(Tree* tree,NodeIndex parent_id){
    Node* parent = &tree->nodes.data[parent_id];
	VECTOR2(int) remaining = (VECTOR2(int)){
		parent->computed_box.w - parent->padding.left - parent->padding.right,
		parent->computed_box.h - parent->padding.top  - parent->padding.bottom,
	};

	if(parent->children_count == 0){
		return remaining;
	}

    NodeIndex child_id = -1;
	switch(parent->layout){
	case LayoutHorizontal:
	    child_id = parent->first_children;
        SAFE_WHILE(child_id >=0){
           	remaining.x -= tree->nodes.data[child_id].computed_box.w;
            child_id = tree->nodes.data[child_id].next;
        }
		remaining.x -= (parent->children_count - 1) * parent->margin;
		break;
	case LayoutVertical:
        child_id = parent->first_children;
        SAFE_WHILE(child_id >=0){
           	remaining.y -= tree->nodes.data[child_id].computed_box.h;
            child_id = tree->nodes.data[child_id].next;
        }
        remaining.y -= (parent->children_count - 1) * parent->margin;
        break;
    case LayoutStack:
        break;
	}

	return remaining;
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

void fit_height_sizing(Node* el, int min,int max) {
	el->computed_box.h += el->padding.top + el->padding.bottom;
	if(el->layout == LayoutVertical) {
		el->computed_box.h += (el->children_count - 1) * el->margin;
	}
	if(el->computed_box.h < min){
		el->computed_box.h = min;
	}
	if(el->computed_box.h > max && max > 0){
		el->computed_box.h = max;
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
	case LayoutStack:
    	tree->nodes.data[parent_id].computed_box.w = MAX(
    	    el->computed_box.w,
    		tree->nodes.data[parent_id].computed_box.w
    	);
		break;
	}
}
typedef enum{
    DIR_X, DIR_Y,
}Direction;

void set_growable(Tree *tree, NodeIndex parent_id, Direction dir ){
    tree->growables.len=0;
	NodeIndex child_id=-1;
	Node* child = NULL;
	switch(dir){
	case DIR_X:
	    child_id = tree->nodes.data[parent_id].first_children;
		SAFE_WHILE(child_id >=0){
		    child = &tree->nodes.data[child_id];
			if(child->size.x.kind == SizeKindGrow){
    			if(array_append_Growable(&tree->growables, (Growable){
    			    .id=child_id,
    				.val=child->computed_box.w,
    				.min=child->size.x.bound.min,
    				.max=child->size.x.bound.max,
                    .to_remove=false,
    			})!=0){
                    printf("failed to append to growable\n");
                    exit(1);
                }
			}
            child_id = tree->nodes.data[child_id].next;
        }
        break;
	case DIR_Y:
        child_id = tree->nodes.data[parent_id].first_children;
        SAFE_WHILE(child_id >=0){
            child = &tree->nodes.data[child_id];
            if(child->size.y.kind == SizeKindGrow){
                if(array_append_Growable(&tree->growables, (Growable){
                    .id=child_id,
                    .val=child->computed_box.h,
                    .min=child->size.y.bound.min,
                    .max=child->size.y.bound.max,
                    .to_remove=false,
                })!=0){
                    printf("failed to append to growable\n");
                    exit(1);
                }
            }
            child_id = tree->nodes.data[child_id].next;
        }
        break;
    }
}

// Returns a positive number if a is greater than b
// Returns a negative number if a is less than b
// Returns 0 if a is equal to b
int cmp_ptr_growable(const void* left, const void* right){
    ptr_growable ptr_left = *(ptr_growable*)left;
    ptr_growable ptr_right = *(ptr_growable*)right;
    return ptr_left->val-ptr_right->val;
}

void build_sorted_growable(Tree *tree){
    tree->sorted_growables.len=0;
    for(int i=0;i<tree->growables.len;i++){
        if(array_append_ptr_growable(&tree->sorted_growables,&tree->growables.data[i])!=0){
            printf("failed to append to ptr_growable\n");
            exit(1);
        }
    }
    qsort(
        tree->sorted_growables.data,
        tree->sorted_growables.len,
        sizeof(ptr_growable),
        cmp_ptr_growable
    );
}

void add(Tree *tree,int delta, int up_to){
    int end = MIN(up_to + 1, tree->sorted_growables.len);
	for(int i= 0;i< end;i++){
	    Growable* g = tree->sorted_growables.data[i];
		g->val += delta;
		if(g->val > g->max && g->max != 0){
			g->val = g->max;
			g->to_remove=true;
		}
	}

	for(int i = 0;i<tree->sorted_growables.len;i++){
	    if(tree->sorted_growables.data[i]->to_remove){
			tree->sorted_growables.data[i]=tree->sorted_growables.data[tree->sorted_growables.len-1];
			tree->sorted_growables.len--;
			i--;
		}
	}
    qsort(
        tree->sorted_growables.data,
        tree->sorted_growables.len,
        sizeof(ptr_growable),
        cmp_ptr_growable
    );
}

void grow_along_axis(Tree* tree,int remaining) {
	if(tree->growables.len == 0 || remaining <= 0){
		return;
	}
	if(tree->growables.len == 1){
		tree->growables.data[0].val += remaining;
		return;
	}
	build_sorted_growable(tree);
	for(int i=0;i<tree->sorted_growables.len;i++){
	    if(tree->sorted_growables.data[i]->val > tree->sorted_growables.data[0]->val){
			int delta = tree->sorted_growables.data[i]->val - tree->sorted_growables.data[0]->val;
			delta = MIN(delta, remaining / i);
			if(delta == 0){
				break;
			}
			remaining -= delta * i;
			add(tree, delta, i - 1);
		}
	    if(tree->sorted_growables.data[0]->val==tree->sorted_growables.data[tree->sorted_growables.len-1]->val){
			int delta = remaining / tree->sorted_growables.len;
			remaining -= delta * tree->sorted_growables.len;
			add(tree, delta, tree->sorted_growables.len);
			if((remaining / tree->sorted_growables.len) == 0){
				break;
			}
		}
	}
}

void grow_across_axis(Tree *tree,int remaining) {
	for(int i=0;i<tree->growables.len;i++){
	    // doubt, = not += ?
		tree->growables.data[i].val = remaining;
	}
}

void apply_grow_values(Tree *tree, Direction dir) {
	switch(dir){
	case DIR_X:
	    for(int i=0;i<tree->growables.len;i++){
			tree->nodes.data[tree->growables.data[i].id].computed_box.w = tree->growables.data[i].val;
		}
		break;
	case DIR_Y:
	    for(int i=0;i<tree->growables.len;i++){
			tree->nodes.data[tree->growables.data[i].id].computed_box.h = tree->growables.data[i].val;
		}
	}
}

void shrink_along_axis_to_min(Tree *tree){
    for(int i=0;i<tree->growables.len;i++){
		tree->growables.data[i].val = tree->growables.data[i].min;
	}
}

void compute_shrink_size_width(Tree* tree, NodeIndex parent_id)
{
   	Node* parent = &tree->nodes.data[parent_id];
	VECTOR2(int) remaining = get_remaining(tree, parent_id);
	set_growable(tree, parent_id, DIR_X);

	switch(parent->layout){
	case LayoutHorizontal:
		shrink_along_axis_to_min(tree);break;
	case LayoutVertical:
		grow_across_axis(tree, remaining.x);break;
	case LayoutStack:
		grow_across_axis(tree, remaining.x);break;
	}

	apply_grow_values(tree, DIR_X);

    NodeIndex child_id = parent->first_children;
    SAFE_WHILE(child_id >=0){
        compute_shrink_size_width(tree,child_id);
        child_id = tree->nodes.data[child_id].next;
    }
}

void compute_grow_size_width(Tree* tree, NodeIndex parent_id)
{
	VECTOR2(int) remaining = get_remaining(tree, parent_id);
	set_growable(tree, parent_id, DIR_X);

	switch(tree->nodes.data[parent_id].layout){
	case LayoutHorizontal:
		grow_along_axis(tree, remaining.x);break;
	case LayoutVertical:
		grow_across_axis(tree, remaining.x);break;
	case LayoutStack:
		grow_across_axis(tree, remaining.x);break;
	}

	apply_grow_values(tree, DIR_X);

    NodeIndex child_id = tree->nodes.data[parent_id].first_children;
    SAFE_WHILE(child_id >=0){
        compute_grow_size_width(tree,child_id);
        child_id = tree->nodes.data[child_id].next;
    }
}

void compute_wrap(Tree* tree, NodeIndex idx)
{
    Node* el = &tree->nodes.data[idx];
	el->computed_box.h = tree->wrap_content_fn(tree->wrap_content_userdata, el->painter, el->computed_box.w);
	NodeIndex child_id = el->first_children;
    SAFE_WHILE(child_id >=0){
        compute_wrap(tree,child_id);
        child_id = tree->nodes.data[child_id].next;
    }
}

void compute_fit_size_height(Tree* tree, NodeIndex idx,NodeIndex parent_id)
{
   	Node* el = &tree->nodes.data[idx];
    NodeIndex child_id = tree->nodes.data[idx].first_children;
	SAFE_WHILE(child_id >=0){
	    compute_fit_size_height(tree,child_id,idx);
	    child_id = tree->nodes.data[child_id].next;
	}

	switch(el->size.y.kind) {
	case SizeKindFixed:
		el->computed_box.h = el->size.y.size;
		break;
	case SizeKindFit:
		fit_height_sizing(el, el->size.y.bound.min, el->size.y.bound.max);
		break;
	case SizeKindGrow:
	    fit_height_sizing(el, el->size.y.bound.min, el->size.y.bound.max);
		el->computed_box.h = MAX(
		    el->computed_box.h,
			tree->mesure_content_fn(tree->mesure_content_userdata, el->painter).y
		);
		break;
	}

	if(parent_id<0){
		return;
	}
	switch(tree->nodes.data[parent_id].layout){
	case LayoutVertical:
		tree->nodes.data[parent_id].computed_box.h += el->computed_box.h;
		break;
	case LayoutHorizontal:
	case LayoutStack:
    	tree->nodes.data[parent_id].computed_box.h = MAX(
    	    el->computed_box.h,
    		tree->nodes.data[parent_id].computed_box.h
    	);
		break;
	}
}

void compute_shrink_size_height(Tree* tree, NodeIndex parent_id)
{
   	Node* parent = &tree->nodes.data[parent_id];
	VECTOR2(int) remaining = get_remaining(tree, parent_id);
	set_growable(tree, parent_id, DIR_Y);

	switch(parent->layout) {
	case LayoutHorizontal:
		grow_across_axis(tree, remaining.y);break;
	case LayoutVertical:
		shrink_along_axis_to_min(tree);break;
	case LayoutStack:
		grow_across_axis(tree, remaining.y);break;
	}

	apply_grow_values(tree, DIR_Y);

	NodeIndex child_id = tree->nodes.data[parent_id].first_children;
    SAFE_WHILE(child_id >=0){
        compute_shrink_size_height(tree,child_id);
        child_id = tree->nodes.data[child_id].next;
    }
}

void compute_grow_size_height(Tree* tree, NodeIndex parent_id)
{
    VECTOR2(int) remaining = get_remaining(tree, parent_id);
	set_growable(tree, parent_id, DIR_Y);

	switch(tree->nodes.data[parent_id].layout){
	case LayoutHorizontal:
		grow_across_axis(tree,remaining.y);break;
	case LayoutVertical:
		grow_along_axis(tree, remaining.y);break;
	case LayoutStack:
		grow_across_axis(tree, remaining.y);break;
	}

	apply_grow_values(tree, DIR_Y);

    NodeIndex child_id = tree->nodes.data[parent_id].first_children;
    SAFE_WHILE(child_id >=0){
        compute_grow_size_width(tree,child_id);
        child_id = tree->nodes.data[child_id].next;
    }
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
    PainterCommand pc = {0};
    pc.x       = tree->nodes.data[idx].computed_box.x;
    pc.y       = tree->nodes.data[idx].computed_box.y;
    pc.w       = tree->nodes.data[idx].computed_box.w;
    pc.h       = tree->nodes.data[idx].computed_box.h;
    pc.painter = tree->nodes.data[idx].painter;
    memcpy(pc.id,tree->nodes.data[idx].id,ID_LEN);
    if(array_append_PainterCommand(&tree->commands,pc)!=0){
        printf("failed to append to painter command\n");
        exit(1);
    }
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

void tree_init(Tree *tree,Allocator alloc){
    tree->nodes =  array_create_Node(alloc);
    tree->commands =  array_create_PainterCommand(alloc);
    tree->growables =  array_create_Growable(alloc);
    tree->sorted_growables =  array_create_ptr_growable(alloc);
}

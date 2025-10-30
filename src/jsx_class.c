#include "jsx_class.h"
#include "dynamicarray.h"
#include "ui.h"

WRITE_HASHMAP_IMPL(init_node_fn);
WRITE_ARRAY_IMPL(InitNodeFn1);

#define VAL1  5
#define VAL2 10
#define VAL3 15
#define VAL4 20
#define VAL5 25

void init_node_fn_set_fit_x(Node* n) { n->size.x.kind = SizeKindFit; }
void init_node_fn_set_fit_y(Node* n) { n->size.y.kind = SizeKindFit; }
void init_node_fn_set_fit(Node* n) { n->size.x.kind = SizeKindFit; n->size.y.kind = SizeKindFit; }
void init_node_fn_set_grow_x(Node* n) { n->size.x.kind = SizeKindGrow; }
void init_node_fn_set_grow_y(Node* n) { n->size.y.kind = SizeKindGrow; }
void init_node_fn_set_grow(Node* n) { n->size.x.kind = SizeKindGrow; n->size.y.kind = SizeKindGrow; }
void init_node_fn_min_bound_grow(Node* n ){n->size.x.bound.pref_use=PreferedToMin;n->size.y.bound.pref_use=PreferedToMin;}
void init_node_fn_min_bound_grow_x(Node* n ){n->size.x.bound.pref_use=PreferedToMin;}
void init_node_fn_min_bound_grow_y(Node* n ){n->size.y.bound.pref_use=PreferedToMin;}
void init_node_fn_max_bound_grow(Node* n ){n->size.x.bound.pref_use=PreferedToMax;n->size.y.bound.pref_use=PreferedToMax;}
void init_node_fn_max_bound_grow_x(Node* n ){n->size.x.bound.pref_use=PreferedToMax;}
void init_node_fn_max_bound_grow_y(Node* n ){n->size.y.bound.pref_use=PreferedToMax;}
void init_node_fn_set_margin_1(Node* n) { n->margin = VAL1; }
void init_node_fn_set_margin_2(Node* n) { n->margin = VAL2; }
void init_node_fn_set_margin_3(Node* n) { n->margin = VAL3; }
void init_node_fn_set_margin_4(Node* n) { n->margin = VAL4; }
void init_node_fn_set_margin_5(Node* n) { n->margin = VAL5; }
void init_node_fn_set_padding_1(Node* n) { n->padding = (Padding){VAL1, VAL1, VAL1, VAL1}; }
void init_node_fn_set_padding_2(Node* n) { n->padding = (Padding){VAL2, VAL2, VAL2, VAL2}; }
void init_node_fn_set_padding_3(Node* n) { n->padding = (Padding){VAL3, VAL3, VAL3, VAL3}; }
void init_node_fn_set_padding_4(Node* n) { n->padding = (Padding){VAL4, VAL4, VAL4, VAL4}; }
void init_node_fn_set_padding_5(Node* n) { n->padding = (Padding){VAL5, VAL5, VAL5, VAL5}; }
void init_node_fn_set_padding_x_1(Node* n) { n->padding = (Padding){.left=VAL1, .right=VAL1, .top=0, .bottom=0}; }
void init_node_fn_set_padding_x_2(Node* n) { n->padding = (Padding){.left=VAL2, .right=VAL2, .top=0, .bottom=0}; }
void init_node_fn_set_padding_x_3(Node* n) { n->padding = (Padding){.left=VAL3, .right=VAL3, .top=0, .bottom=0}; }
void init_node_fn_set_padding_x_4(Node* n) { n->padding = (Padding){.left=VAL4, .right=VAL4, .top=0, .bottom=0}; }
void init_node_fn_set_padding_x_5(Node* n) { n->padding = (Padding){.left=VAL5, .right=VAL5, .top=0, .bottom=0}; }
void init_node_fn_set_padding_y_1(Node* n) { n->padding = (Padding){.left=0, .right=0, .top=VAL1, .bottom=VAL1}; }
void init_node_fn_set_padding_y_2(Node* n) { n->padding = (Padding){.left=0, .right=0, .top=VAL2, .bottom=VAL2}; }
void init_node_fn_set_padding_y_3(Node* n) { n->padding = (Padding){.left=0, .right=0, .top=VAL3, .bottom=VAL3}; }
void init_node_fn_set_padding_y_4(Node* n) { n->padding = (Padding){.left=0, .right=0, .top=VAL4, .bottom=VAL4}; }
void init_node_fn_set_padding_y_5(Node* n) { n->padding = (Padding){.left=0, .right=0, .top=VAL5, .bottom=VAL5}; }
void init_node_fn_set_padding_l_1(Node* n) { n->padding.left = VAL1; }
void init_node_fn_set_padding_l_2(Node* n) { n->padding.left = VAL2; }
void init_node_fn_set_padding_l_3(Node* n) { n->padding.left = VAL3; }
void init_node_fn_set_padding_l_4(Node* n) { n->padding.left = VAL4; }
void init_node_fn_set_padding_l_5(Node* n) { n->padding.left = VAL5; }
void init_node_fn_set_padding_r_1(Node* n) { n->padding.right = VAL1; }
void init_node_fn_set_padding_r_2(Node* n) { n->padding.right = VAL2; }
void init_node_fn_set_padding_r_3(Node* n) { n->padding.right = VAL3; }
void init_node_fn_set_padding_r_4(Node* n) { n->padding.right = VAL4; }
void init_node_fn_set_padding_r_5(Node* n) { n->padding.right = VAL5; }
void init_node_fn_set_padding_t_1(Node* n) { n->padding.top = VAL1; }
void init_node_fn_set_padding_t_2(Node* n) { n->padding.top = VAL2; }
void init_node_fn_set_padding_t_3(Node* n) { n->padding.top = VAL3; }
void init_node_fn_set_padding_t_4(Node* n) { n->padding.top = VAL4; }
void init_node_fn_set_padding_t_5(Node* n) { n->padding.top = VAL5; }
void init_node_fn_set_padding_b_1(Node* n) { n->padding.bottom = VAL1; }
void init_node_fn_set_padding_b_2(Node* n) { n->padding.bottom = VAL2; }
void init_node_fn_set_padding_b_3(Node* n) { n->padding.bottom = VAL3; }
void init_node_fn_set_padding_b_4(Node* n) { n->padding.bottom = VAL4; }
void init_node_fn_set_padding_b_5(Node* n) { n->padding.bottom = VAL5; }
void init_node_fn_set_layout_v(Node* n) { n->layout = LayoutVertical; }
void init_node_fn_set_layout_h(Node* n) { n->layout = LayoutHorizontal; }
void init_node_fn_set_layout_s(Node* n) { n->layout = LayoutStack; }
void init_node_fn_set_align_x_begin(Node* n) { n->align.x = AlignBegin; }
void init_node_fn_set_align_x_middle(Node* n) { n->align.x = AlignMiddle; }
void init_node_fn_set_align_x_end(Node* n) { n->align.x = AlignEnd; }
void init_node_fn_set_align_y_begin(Node* n) { n->align.y = AlignBegin; }
void init_node_fn_set_align_y_middle(Node* n) { n->align.y = AlignMiddle; }
void init_node_fn_set_align_y_end(Node* n) { n->align.y = AlignEnd; }

void init_node_fn1_set_min(Node* n,int val) {n->size.x.bound.min = val;n->size.y.bound.min = val;}
void init_node_fn1_set_min_x(Node* n,int val) {n->size.x.bound.min = val;}
void init_node_fn1_set_min_y(Node* n,int val) {n->size.y.bound.min = val;}
void init_node_fn1_set_max(Node* n,int val) {n->size.x.bound.max = val;n->size.y.bound.max = val;}
void init_node_fn1_set_max_x(Node* n,int val) {n->size.x.bound.max = val;}
void init_node_fn1_set_max_y(Node* n,int val) {n->size.y.bound.max = val;}

void init_init_node_fn_hashmap(HASHMAP(init_node_fn)* hmap,ARRAY(InitNodeFn1)* array1){
    *(init_node_fn_upsert(hmap, "fit", UpsertActionCreate)) = init_node_fn_set_fit;
    *(init_node_fn_upsert(hmap, "fit-x", UpsertActionCreate)) = init_node_fn_set_fit_x;
    *(init_node_fn_upsert(hmap, "fit-y", UpsertActionCreate)) = init_node_fn_set_fit_y;
    *(init_node_fn_upsert(hmap, "grow", UpsertActionCreate)) = init_node_fn_set_grow;
    *(init_node_fn_upsert(hmap, "grow-x", UpsertActionCreate)) = init_node_fn_set_grow_x;
    *(init_node_fn_upsert(hmap, "grow-y", UpsertActionCreate)) = init_node_fn_set_grow_y;
    *(init_node_fn_upsert(hmap, "min-bound", UpsertActionCreate))   = init_node_fn_min_bound_grow;
    *(init_node_fn_upsert(hmap, "min-bound-x", UpsertActionCreate)) = init_node_fn_min_bound_grow_x;
    *(init_node_fn_upsert(hmap, "min-bound-y", UpsertActionCreate)) = init_node_fn_min_bound_grow_y;
    *(init_node_fn_upsert(hmap, "max-bound", UpsertActionCreate))   = init_node_fn_max_bound_grow;
    *(init_node_fn_upsert(hmap, "max-bound-x", UpsertActionCreate)) = init_node_fn_max_bound_grow_x;
    *(init_node_fn_upsert(hmap, "max-bound-y", UpsertActionCreate)) = init_node_fn_max_bound_grow_y;
    *(init_node_fn_upsert(hmap, "m-1", UpsertActionCreate)) = init_node_fn_set_margin_1;
    *(init_node_fn_upsert(hmap, "m-2", UpsertActionCreate)) = init_node_fn_set_margin_2;
    *(init_node_fn_upsert(hmap, "m-3", UpsertActionCreate)) = init_node_fn_set_margin_3;
    *(init_node_fn_upsert(hmap, "m-4", UpsertActionCreate)) = init_node_fn_set_margin_4;
    *(init_node_fn_upsert(hmap, "m-5", UpsertActionCreate)) = init_node_fn_set_margin_5;
    *(init_node_fn_upsert(hmap, "p-1", UpsertActionCreate)) = init_node_fn_set_padding_1;
    *(init_node_fn_upsert(hmap, "p-2", UpsertActionCreate)) = init_node_fn_set_padding_2;
    *(init_node_fn_upsert(hmap, "p-3", UpsertActionCreate)) = init_node_fn_set_padding_3;
    *(init_node_fn_upsert(hmap, "p-4", UpsertActionCreate)) = init_node_fn_set_padding_4;
    *(init_node_fn_upsert(hmap, "p-5", UpsertActionCreate)) = init_node_fn_set_padding_5;
    *(init_node_fn_upsert(hmap, "px-1", UpsertActionCreate)) = init_node_fn_set_padding_x_1;
    *(init_node_fn_upsert(hmap, "px-2", UpsertActionCreate)) = init_node_fn_set_padding_x_2;
    *(init_node_fn_upsert(hmap, "px-3", UpsertActionCreate)) = init_node_fn_set_padding_x_3;
    *(init_node_fn_upsert(hmap, "px-4", UpsertActionCreate)) = init_node_fn_set_padding_x_4;
    *(init_node_fn_upsert(hmap, "px-5", UpsertActionCreate)) = init_node_fn_set_padding_x_5;
    *(init_node_fn_upsert(hmap, "py-1", UpsertActionCreate)) = init_node_fn_set_padding_y_1;
    *(init_node_fn_upsert(hmap, "py-2", UpsertActionCreate)) = init_node_fn_set_padding_y_2;
    *(init_node_fn_upsert(hmap, "py-3", UpsertActionCreate)) = init_node_fn_set_padding_y_3;
    *(init_node_fn_upsert(hmap, "py-4", UpsertActionCreate)) = init_node_fn_set_padding_y_4;
    *(init_node_fn_upsert(hmap, "py-5", UpsertActionCreate)) = init_node_fn_set_padding_y_5;
    *(init_node_fn_upsert(hmap, "pl-1", UpsertActionCreate)) = init_node_fn_set_padding_l_1;
    *(init_node_fn_upsert(hmap, "pl-2", UpsertActionCreate)) = init_node_fn_set_padding_l_2;
    *(init_node_fn_upsert(hmap, "pl-3", UpsertActionCreate)) = init_node_fn_set_padding_l_3;
    *(init_node_fn_upsert(hmap, "pl-4", UpsertActionCreate)) = init_node_fn_set_padding_l_4;
    *(init_node_fn_upsert(hmap, "pl-5", UpsertActionCreate)) = init_node_fn_set_padding_l_5;
    *(init_node_fn_upsert(hmap, "pr-1", UpsertActionCreate)) = init_node_fn_set_padding_r_1;
    *(init_node_fn_upsert(hmap, "pr-2", UpsertActionCreate)) = init_node_fn_set_padding_r_2;
    *(init_node_fn_upsert(hmap, "pr-3", UpsertActionCreate)) = init_node_fn_set_padding_r_3;
    *(init_node_fn_upsert(hmap, "pr-4", UpsertActionCreate)) = init_node_fn_set_padding_r_4;
    *(init_node_fn_upsert(hmap, "pr-5", UpsertActionCreate)) = init_node_fn_set_padding_r_5;
    *(init_node_fn_upsert(hmap, "pt-1", UpsertActionCreate)) = init_node_fn_set_padding_t_1;
    *(init_node_fn_upsert(hmap, "pt-2", UpsertActionCreate)) = init_node_fn_set_padding_t_2;
    *(init_node_fn_upsert(hmap, "pt-3", UpsertActionCreate)) = init_node_fn_set_padding_t_3;
    *(init_node_fn_upsert(hmap, "pt-4", UpsertActionCreate)) = init_node_fn_set_padding_t_4;
    *(init_node_fn_upsert(hmap, "pt-5", UpsertActionCreate)) = init_node_fn_set_padding_t_5;
    *(init_node_fn_upsert(hmap, "pb-1", UpsertActionCreate)) = init_node_fn_set_padding_b_1;
    *(init_node_fn_upsert(hmap, "pb-2", UpsertActionCreate)) = init_node_fn_set_padding_b_2;
    *(init_node_fn_upsert(hmap, "pb-3", UpsertActionCreate)) = init_node_fn_set_padding_b_3;
    *(init_node_fn_upsert(hmap, "pb-4", UpsertActionCreate)) = init_node_fn_set_padding_b_4;
    *(init_node_fn_upsert(hmap, "pb-5", UpsertActionCreate)) = init_node_fn_set_padding_b_5;
    *(init_node_fn_upsert(hmap, "lv", UpsertActionCreate)) = init_node_fn_set_layout_v;
    *(init_node_fn_upsert(hmap, "lh", UpsertActionCreate)) = init_node_fn_set_layout_h;
    *(init_node_fn_upsert(hmap, "ls", UpsertActionCreate)) = init_node_fn_set_layout_s;
    *(init_node_fn_upsert(hmap, "ax-b", UpsertActionCreate)) = init_node_fn_set_align_x_begin;
    *(init_node_fn_upsert(hmap, "ax-m", UpsertActionCreate)) = init_node_fn_set_align_x_middle;
    *(init_node_fn_upsert(hmap, "ax-e", UpsertActionCreate)) = init_node_fn_set_align_x_end;
    *(init_node_fn_upsert(hmap, "ay-b", UpsertActionCreate)) = init_node_fn_set_align_y_begin;
    *(init_node_fn_upsert(hmap, "ay-m", UpsertActionCreate)) = init_node_fn_set_align_y_middle;
    *(init_node_fn_upsert(hmap, "ay-e", UpsertActionCreate)) = init_node_fn_set_align_y_end;

    array_append_InitNodeFn1(array1, (InitNodeFn1){"^min-x-.*","min-x-%d",init_node_fn1_set_min_x});
    array_append_InitNodeFn1(array1, (InitNodeFn1){"^min-y-.*","min-y-%d",init_node_fn1_set_min_y});
    array_append_InitNodeFn1(array1, (InitNodeFn1){"^min-.*"  ,"min-%d"  ,init_node_fn1_set_min});
    array_append_InitNodeFn1(array1, (InitNodeFn1){"^max-x-.*","max-x-%d",init_node_fn1_set_max_x});
    array_append_InitNodeFn1(array1, (InitNodeFn1){"^max-y-.*","max-y-%d",init_node_fn1_set_max_y});
    array_append_InitNodeFn1(array1, (InitNodeFn1){"^max-.*"  ,"max-%d"  ,init_node_fn1_set_max});
}

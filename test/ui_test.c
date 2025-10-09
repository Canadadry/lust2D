#include "minitest.h"
#include "../src/ui.h"
#include <string.h>

typedef struct{
    const char *name;
    NodeIndex head;
    Tree tree;
    ARRAY(PainterCommand) expected;
}TestCase;

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX_NODE_LEN 5
#define MAX_SOURCE_IMG_LEN 128

bool painter_match(const char* test_name,int i,Painter exp, Painter got){
    if(exp.kind != got.kind){
        TEST_ERRORF(test_name,
            "failed command %d painter dont match got %d want:%d\n",
            i,got.kind,exp.kind
        );
        return false;
    }
    switch(exp.kind){
        case PAINTER_NONE: break;
        case PAINTER_RECT:
        if(exp.value.rect.color.value != got.value.rect.color.value){
            TEST_ERRORF(test_name,
                "failed command %d painter rect color dont match got %x want:%x\n",
                i,got.value.rect.color.value,exp.value.rect.color.value
            );
            return false;
        }
        break;
        case PAINTER_IMG:
        if(exp.value.img.color.value != got.value.img.color.value){
            TEST_ERRORF(test_name,
                "failed command %d painter image color dont match got %x want:%x\n",
                i,got.value.img.color.value,exp.value.img.color.value
            );
            return false;
        }
        if(strncmp(exp.value.img.source, exp.value.img.source,MAX_SOURCE_IMG_LEN)!=0){
            TEST_ERRORF(test_name,
                "failed command %d painter image source dont match got %s want:%s\n",
                i,got.value.img.source,exp.value.img.source
            );
            return false;
        }
        break;
    }
    return true;
}

bool painter_command_match(const char* test_name,int i,PainterCommand exp, PainterCommand got){
    if(exp.x == got.x && exp.y == got.y && exp.w == got.w && exp.h == got.h){
        return painter_match(test_name,i,exp.painter,got.painter);
    }
    TEST_ERRORF(test_name,
        "failed command %d dont match\ngot\nx:%d y:%d w:%d h:%d\n want\nx:%d y:%d w:%d h:%d\n",
        i,
        got.x,got.y,got.w,got.h,
        exp.x,exp.y,exp.w,exp.h
    );
    return false;
}

void test_ui_compute_case(void(*init_test)(TestCase* tc)){
    TestCase tc = {0};
    PainterCommand exp_commands[MAX_NODE_LEN]={0};
    tc.expected.capacity=MAX_NODE_LEN;
    tc.expected.data=exp_commands;
    STATIC_INIT_TREE(tc.tree,MAX_NODE_LEN);
    init_test(&tc);
    compute(&tc.tree,tc.head);
    if(tc.expected.len != tc.tree.commands.len){
        TEST_ERRORF(tc.name,"failed command len exp %d got %d\n",tc.expected.len , tc.tree.commands.len);
    }
    int min_len = MIN(tc.expected.len , tc.tree.commands.len);
    for(int i=0;i<min_len;i++ ){
    	if(painter_command_match(tc.name,i,tc.expected.data[i],tc.tree.commands.data[i])==false) {
            // break;
    	}
    }
}

void test_no_children_fixed_position(TestCase* tc){
    tc->name =__func__;
    tc->expected.len=1;
    tc->expected.data[0] = (PainterCommand){.x=10,.y=10,.w=100,.h=100,.painter={0}};
    tc->tree.nodes.len=1;
    tc->tree.nodes.data[0]=(Node){
        .painter={0},
        .pos ={10,10},
        .size={
            (Size){.kind=SizeKindFixed,.size=100},
            (Size){.kind=SizeKindFixed,.size=100},
        },
        .first_children=-1,
        .last_children=-1,
        .next=-1,
    };
}

void test_one_children_fixed_position(TestCase* tc){
    tc->name = __func__;
    tc->expected.len=2;
    tc->expected.data[0] = (PainterCommand){.x=10,.y=10,.w=100,.h=100,.painter={0}};
    tc->expected.data[1] = (PainterCommand){.x=20,.y=20,.w=50,.h=50,.painter={0}};
    tc->tree.nodes.len=2;
    tc->tree.nodes.data[0]=(Node){
        .pos ={10,10},
        .layout=LayoutStack,
        .size={
            (Size){.kind=SizeKindFixed,.size=100},
            (Size){.kind=SizeKindFixed,.size=100},
        },
        .first_children = 1,
        .last_children = 1,
        .next=-1,
    };
    tc->tree.nodes.data[1]=(Node){
        .pos ={10,10},
        .size={
            (Size){.kind=SizeKindFixed,.size=50},
            (Size){.kind=SizeKindFixed,.size=50},
        },
        .first_children = -1,
        .last_children = -1,
        .next = -1,
    };
}

void test_root_fitting_to_one_children(TestCase* tc) {
    tc->name = __func__;
    tc->expected.len = 3;
    tc->expected.data[0] = (PainterCommand){.x = 0, .y = 0, .w = 100, .h = 50, .painter = {0}};
    tc->expected.data[1] = (PainterCommand){.x = 0, .y = 0, .w = 50, .h = 50, .painter = {0}};
    tc->expected.data[2] = (PainterCommand){.x = 50, .y = 0, .w = 50, .h = 50, .painter = {0}};
    tc->tree.nodes.len = 3;
    tc->tree.nodes.data[0] = (Node){
        .layout = LayoutHorizontal,
        .size = {
            (Size){.kind = SizeKindFit},
            (Size){.kind = SizeKindFit},
        },
        .pos = {0, 0},
        .first_children = 1,
        .last_children = 2,
        .next = -1,
    };
    tc->tree.nodes.data[1] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 50},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .pos = {0, 0},
        .first_children = -1,
        .last_children = -1,
        .next = 2,
    };
    tc->tree.nodes.data[2] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 50},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .pos = {0, 0},
        .first_children = -1,
        .last_children = -1,
        .next = -1,
    };
}

void test_root_fitting_to_one_children_in_vertical(TestCase* tc) {
    tc->name = __func__;
    tc->expected.len = 3;
    tc->expected.data[0] = (PainterCommand){.x = 0, .y = 0, .w = 50, .h = 100, .painter = {0}};
    tc->expected.data[1] = (PainterCommand){.x = 0, .y = 0, .w = 50, .h = 50, .painter = {0}};
    tc->expected.data[2] = (PainterCommand){.x = 0, .y = 50, .w = 50, .h = 50, .painter = {0}};
    tc->tree.nodes.len = 3;
    tc->tree.nodes.data[0] = (Node){
        .layout = LayoutVertical,
        .size = {
            (Size){.kind = SizeKindFit},
            (Size){.kind = SizeKindFit},
        },
        .pos = {0, 0},
        .first_children = 1,
        .last_children = 2,
        .next = -1,
    };
    tc->tree.nodes.data[1] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 50},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .pos = {0, 0},
        .first_children = -1,
        .last_children = -1,
        .next = 2,
    };
    tc->tree.nodes.data[2] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 50},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .pos = {0, 0},
        .first_children = -1,
        .last_children = -1,
        .next = -1,
    };
}

void test_nested_children_fixed_positions(TestCase* tc) {
    tc->name = __func__;
    tc->expected.len = 3;
    tc->expected.data[0] = (PainterCommand){.x = 10, .y = 15, .w = 200, .h = 100, .painter = {0}};
    tc->expected.data[1] = (PainterCommand){.x = 20, .y = 30, .w = 150, .h = 50, .painter = {0}};
    tc->expected.data[2] = (PainterCommand){.x = 25, .y = 35, .w = 20, .h = 20, .painter = {0}};
    tc->tree.nodes.len = 3;
    tc->tree.nodes.data[0] = (Node){
        .pos = {10, 15},
        .size = {
            (Size){.kind = SizeKindFixed, .size = 200},
            (Size){.kind = SizeKindFixed, .size = 100},
        },
        .first_children = 1,
        .last_children = 1,
        .next = -1,
    };
    tc->tree.nodes.data[1] = (Node){
        .pos = {10, 15},
        .size = {
            (Size){.kind = SizeKindFixed, .size = 150},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .first_children = 2,
        .last_children = 2,
        .next = -1,
    };
    tc->tree.nodes.data[2] = (Node){
        .pos = {5, 5},
        .size = {
            (Size){.kind = SizeKindFixed, .size = 20},
            (Size){.kind = SizeKindFixed, .size = 20},
        },
        .first_children = -1,
        .last_children = -1,
        .next = -1,
    };
}

void test_nested_children_fixed_positions_and_padding(TestCase* tc) {
    tc->name = __func__;
    tc->expected.len = 3;
    tc->expected.data[0] = (PainterCommand){.x = 10, .y = 15, .w = 200, .h = 100, .painter = {0}};
    tc->expected.data[1] = (PainterCommand){.x = 30, .y = 50, .w = 150, .h = 50, .painter = {0}};
    tc->expected.data[2] = (PainterCommand){.x = 35, .y = 55, .w = 20, .h = 20, .painter = {0}};
    tc->tree.nodes.len = 3;
    tc->tree.nodes.data[0] = (Node){
        .pos = {10, 15},
        .size = {
            (Size){.kind = SizeKindFixed, .size = 200},
            (Size){.kind = SizeKindFixed, .size = 100},
        },
        .padding = {
            .left = 10,
            .top = 20,
            .right = 0,
            .bottom = 0,
        },
        .first_children = 1,
        .last_children = 1,
        .next = -1,
    };
    tc->tree.nodes.data[1] = (Node){
        .pos = {10, 15},
        .size = {
            (Size){.kind = SizeKindFixed, .size = 150},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .first_children = 2,
        .last_children = 2,
        .next = -1,
    };
    tc->tree.nodes.data[2] = (Node){
        .pos = {5, 5},
        .size = {
            (Size){.kind = SizeKindFixed, .size = 20},
            (Size){.kind = SizeKindFixed, .size = 20},
        },
        .first_children = -1,
        .last_children = -1,
        .next = -1,
    };
}

void test_horizontal_layout_with_margin(TestCase* tc) {
    tc->name = __func__;
    tc->expected.len = 3;
    tc->expected.data[0] = (PainterCommand){.x = 0, .y = 0, .w = 200, .h = 100, .painter = {0}};
    tc->expected.data[1] = (PainterCommand){.x = 0, .y = 0, .w = 50, .h = 100, .painter = {0}};
    tc->expected.data[2] = (PainterCommand){.x = 60, .y = 0, .w = 100, .h = 50, .painter = {0}};
    tc->tree.nodes.len = 3;
    tc->tree.nodes.data[0] = (Node){
        .layout = LayoutHorizontal,
        .size = {
            (Size){.kind = SizeKindFixed, .size = 200},
            (Size){.kind = SizeKindFixed, .size = 100},
        },
        .margin = 10,
        .first_children = 1,
        .last_children = 2,
        .next = -1,
    };
    tc->tree.nodes.data[1] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 50},
            (Size){.kind = SizeKindFixed, .size = 100},
        },
        .first_children = -1,
        .last_children = -1,
        .next = 2,
    };
    tc->tree.nodes.data[2] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 100},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .first_children = -1,
        .last_children = -1,
        .next = -1,
    };
}

void test_vertical_layout_with_spacing(TestCase* tc) {
    tc->name = __func__;
    tc->expected.len = 3;
    tc->expected.data[0] = (PainterCommand){.x = 0, .y = 0, .w = 200, .h = 300, .painter = {0}};
    tc->expected.data[1] = (PainterCommand){.x = 0, .y = 0, .w = 50, .h = 100, .painter = {0}};
    tc->expected.data[2] = (PainterCommand){.x = 0, .y = 110, .w = 100, .h = 50, .painter = {0}};
    tc->tree.nodes.len = 3;
    tc->tree.nodes.data[0] = (Node){
        .layout = LayoutVertical,
        .size = {
            (Size){.kind = SizeKindFixed, .size = 200},
            (Size){.kind = SizeKindFixed, .size = 300},
        },
        .margin = 10,
        .first_children = 1,
        .last_children = 2,
        .next = -1,
    };
    tc->tree.nodes.data[1] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 50},
            (Size){.kind = SizeKindFixed, .size = 100},
        },
        .first_children = -1,
        .last_children = -1,
        .next = 2,
    };
    tc->tree.nodes.data[2] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 100},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .first_children = -1,
        .last_children = -1,
        .next = -1,
    };
}

void test_horizontal_layout_with_margin_and_root_fitting(TestCase* tc) {
    tc->name = __func__;
    tc->expected.len = 3;
    tc->expected.data[0] = (PainterCommand){.x = 0, .y = 0, .w = 180, .h = 120, .painter = {0}};
    tc->expected.data[1] = (PainterCommand){.x = 10, .y = 10, .w = 50, .h = 100, .painter = {0}};
    tc->expected.data[2] = (PainterCommand){.x = 70, .y = 10, .w = 100, .h = 50, .painter = {0}};
    tc->tree.nodes.len = 3;
    tc->tree.nodes.data[0] = (Node){
        .layout = LayoutHorizontal,
        .size = {
            (Size){.kind = SizeKindFit},
            (Size){.kind = SizeKindFit},
        },
        .padding = {
            .top = 10,
            .bottom = 10,
            .left = 10,
            .right = 10,
        },
        .margin = 10,
        .first_children = 1,
        .last_children = 2,
        .children_count = 2,
        .next = -1,
    };
    tc->tree.nodes.data[1] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 50},
            (Size){.kind = SizeKindFixed, .size = 100},
        },
        .first_children = -1,
        .last_children = -1,
        .next = 2,
    };
    tc->tree.nodes.data[2] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 100},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .first_children = -1,
        .last_children = -1,
        .next = -1,
    };
}

void test_vertical_layout_with_margin_and_root_fitting(TestCase* tc) {
    tc->name = __func__;
    tc->expected.len = 3;
    tc->expected.data[0] = (PainterCommand){.x = 0, .y = 0, .w = 120, .h = 180, .painter = {0}};
    tc->expected.data[1] = (PainterCommand){.x = 10, .y = 10, .w = 50, .h = 100, .painter = {0}};
    tc->expected.data[2] = (PainterCommand){.x = 10, .y = 120, .w = 100, .h = 50, .painter = {0}};
    tc->tree.nodes.len = 3;
    tc->tree.nodes.data[0] = (Node){
        .layout = LayoutVertical,
        .size = {
            (Size){.kind = SizeKindFit},
            (Size){.kind = SizeKindFit},
        },
        .padding = {
            .left = 10,
            .right = 10,
            .top = 10,
            .bottom = 10,
        },
        .margin = 10,
        .first_children = 1,
        .last_children = 2,
        .children_count = 2,
        .next = -1,
    };
    tc->tree.nodes.data[1] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 50},
            (Size){.kind = SizeKindFixed, .size = 100},
        },
        .first_children = -1,
        .last_children = -1,
        .next = 2,
    };
    tc->tree.nodes.data[2] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 100},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .first_children = -1,
        .last_children = -1,
        .next = -1,
    };
}

void test_grow_children_between_two_fixed_size_in_horizontal(TestCase* tc) {
    tc->name = __func__;
    tc->expected.len = 4;
    tc->expected.data[0] = (PainterCommand){.x = 0, .y = 0, .w = 450, .h = 120, .painter = {0}};
    tc->expected.data[1] = (PainterCommand){.x = 10, .y = 10, .w = 50, .h = 100, .painter = {0}};
    tc->expected.data[2] = (PainterCommand){.x = 70, .y = 10, .w = 260, .h = 100, .painter = {0}};
    tc->expected.data[3] = (PainterCommand){.x = 340, .y = 10, .w = 100, .h = 50, .painter = {0}};
    tc->tree.nodes.len = 4;
    tc->tree.nodes.data[0] = (Node){
        .layout = LayoutHorizontal,
        .size = {
            (Size){.kind = SizeKindFixed, .size = 450},
            (Size){.kind = SizeKindFit},
        },
        .padding = {
            .left = 10,
            .right = 10,
            .top = 10,
            .bottom = 10,
        },
        .margin = 10,
        .first_children = 1,
        .last_children = 3,
        .children_count = 3,
        .next = -1,
    };
    tc->tree.nodes.data[1] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 50},
            (Size){.kind = SizeKindFixed, .size = 100},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = 2,
    };
    tc->tree.nodes.data[2] = (Node){
        .size = {
            (Size){.kind = SizeKindGrow},
            (Size){.kind = SizeKindGrow},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = 3,
    };
    tc->tree.nodes.data[3] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 100},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = -1,
    };
}

void test_grow_children_between_two_fixed_size_in_vertical(TestCase* tc) {
    tc->name = __func__;
    tc->expected.len = 4;
    tc->expected.data[0] = (PainterCommand){.x = 0, .y = 0, .w = 120, .h = 450, .painter = {0}};
    tc->expected.data[1] = (PainterCommand){.x = 10, .y = 10, .w = 100, .h = 50, .painter = {0}};
    tc->expected.data[2] = (PainterCommand){.x = 10, .y = 70, .w = 100, .h = 260, .painter = {0}};
    tc->expected.data[3] = (PainterCommand){.x = 10, .y = 340, .w = 50, .h = 100, .painter = {0}};
    tc->tree.nodes.len = 4;
    tc->tree.nodes.data[0] = (Node){
        .layout = LayoutVertical,
        .size = {
            (Size){.kind = SizeKindFit},
            (Size){.kind = SizeKindFixed, .size = 450},
        },
        .padding = {
            .left = 10,
            .right = 10,
            .top = 10,
            .bottom = 10,
        },
        .margin = 10,
        .first_children = 1,
        .last_children = 3,
        .children_count = 3,
        .next = -1,
    };
    tc->tree.nodes.data[1] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 100},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = 2,
    };
    tc->tree.nodes.data[2] = (Node){
        .size = {
            (Size){.kind = SizeKindGrow},
            (Size){.kind = SizeKindGrow},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = 3,
    };
    tc->tree.nodes.data[3] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 50},
            (Size){.kind = SizeKindFixed, .size = 100},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = -1,
    };
}

void test_two_grow_children_between_two_fixed_size_in_horizontal_with_starting_size(TestCase* tc) {
    tc->name = __func__;
    tc->expected.len = 5;
    tc->expected.data[0] = (PainterCommand){.x = 0, .y = 0, .w = 450, .h = 120, .painter = {0}};
    tc->expected.data[1] = (PainterCommand){.x = 10, .y = 10, .w = 50, .h = 100, .painter = {0}};
    tc->expected.data[2] = (PainterCommand){.x = 70, .y = 10, .w = 125, .h = 100,
                                            .painter = (Painter){.kind=PAINTER_IMG,.value={
                                                .img=(PainterImage){.source="10x15"}
                                            }},
    };
    tc->expected.data[3] = (PainterCommand){.x = 205, .y = 10, .w = 125, .h = 100, .painter = {0}};
    tc->expected.data[4] = (PainterCommand){.x = 340, .y = 10, .w = 100, .h = 50, .painter = {0}};
    tc->tree.nodes.len = 4;

    tc->tree.nodes.data[0] = (Node){
        .layout = LayoutHorizontal,
        .size = {
            (Size){.kind = SizeKindFixed, .size = 450},
            (Size){.kind = SizeKindFit},
        },
        .padding = {
            .left = 10,
            .right = 10,
            .top = 10,
            .bottom = 10,
        },
        .margin = 10,
        .first_children = 1,
        .last_children = 4,
        .children_count = 4,
        .next = -1,
    };

    tc->tree.nodes.data[1] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 50},
            (Size){.kind = SizeKindFixed, .size = 100},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = 2,
    };

    tc->tree.nodes.data[2] = (Node){
        .size = {
            (Size){.kind = SizeKindGrow},
            (Size){.kind = SizeKindGrow},
        },
        .painter = (Painter){.kind=PAINTER_IMG,.value={.img=(PainterImage){.source="10x15"}},},
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = 3,
    };

    tc->tree.nodes.data[3] = (Node){
        .size = {
            (Size){.kind = SizeKindGrow},
            (Size){.kind = SizeKindGrow},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = 4,
    };

    tc->tree.nodes.data[4] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 100},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = -1,
    };
}

void test_two_grow_children_between_two_fixed_size_in_vertical_with_starting_size(TestCase* tc) {
    tc->name = __func__;
    tc->expected.len = 5;
    tc->expected.data[0] = (PainterCommand){.x = 0, .y = 0, .w = 120, .h = 450, .painter = {0}};
    tc->expected.data[1] = (PainterCommand){.x = 10, .y = 10, .w = 100, .h = 50, .painter = {0}};
    tc->expected.data[2] = (PainterCommand){.x = 10, .y = 70, .w = 100, .h = 125, .painter = {0}};
    tc->expected.data[3] = (PainterCommand){.x = 10, .y = 205, .w = 100, .h = 125,
                                            .painter = (Painter){.kind=PAINTER_IMG,.value={
                                                .img=(PainterImage){.source="10x15"}
                                            }},
    };
    tc->expected.data[4] = (PainterCommand){.x = 10, .y = 340, .w = 50, .h = 100, .painter = {0}};
    tc->tree.nodes.len = 4;
    tc->tree.nodes.data[0] = (Node){
        .layout = LayoutVertical,
        .size = {
            (Size){.kind = SizeKindFit},
            (Size){.kind = SizeKindFixed, .size = 450},
        },
        .padding = {
            .left = 10,
            .right = 10,
            .top = 10,
            .bottom = 10,
        },
        .margin = 10,
        .first_children = 1,
        .last_children = 4,
        .children_count = 4,
        .next = -1,
    };
    tc->tree.nodes.data[1] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 100},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = 2,
    };
    tc->tree.nodes.data[2] = (Node){
        .size = {
            (Size){.kind = SizeKindGrow},
            (Size){.kind = SizeKindGrow},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = 3,
    };
    tc->tree.nodes.data[3] = (Node){
        .size = {
            (Size){.kind = SizeKindGrow},
            (Size){.kind = SizeKindGrow},
        },
        .painter = (Painter){.kind=PAINTER_IMG,.value={.img=(PainterImage){.source="10x15"}}},
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = 4,
    };
    tc->tree.nodes.data[4] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 50},
            (Size){.kind = SizeKindFixed, .size = 100},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = -1,
    };
}

void test_two_grow_children_between_two_fixed_size_in_horizontal_with_one_shrinking(TestCase* tc) {
    tc->name = __func__;
    tc->expected.len = 5;
    tc->expected.data[0] = (PainterCommand){.x = 0, .y = 0, .w = 450, .h = 410, .painter = {0}};
    tc->expected.data[1] = (PainterCommand){.x = 10, .y = 10, .w = 50, .h = 100, .painter = {0}};
    tc->expected.data[2] = (PainterCommand){.x = 70, .y = 10, .w = 125, .h = 390,
                                            .painter =  (Painter){.kind=PAINTER_IMG,.value={
                                                .img=(PainterImage){.source="500x15"}
                                            }}
    };
    tc->expected.data[3] = (PainterCommand){.x = 205, .y = 10, .w = 125, .h = 390, .painter = {0}};
    tc->expected.data[4] = (PainterCommand){.x = 340, .y = 10, .w = 100, .h = 50, .painter = {0}};
    tc->tree.nodes.len = 4;

    tc->tree.nodes.data[0] = (Node){
        .layout = LayoutHorizontal,
        .size = {
            (Size){.kind = SizeKindFixed, .size = 450},
            (Size){.kind = SizeKindFit},
        },
        .padding = {
            .left = 10,
            .right = 10,
            .top = 10,
            .bottom = 10,
        },
        .margin = 10,
        .first_children = 1,
        .last_children = 4,
        .children_count = 4,
        .next = -1,
    };

    tc->tree.nodes.data[1] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 50},
            (Size){.kind = SizeKindFixed, .size = 100},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = 2,
    };

    tc->tree.nodes.data[2] = (Node){
        .size = {
            (Size){.kind = SizeKindGrow},
            (Size){.kind = SizeKindGrow},
        },
        .painter =  (Painter){.kind=PAINTER_IMG,.value={
            .img=(PainterImage){.source="500x15"}
        }},
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = 3,
    };
    tc->tree.nodes.data[3] = (Node){
        .size = {
            (Size){.kind = SizeKindGrow},
            (Size){.kind = SizeKindGrow},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = 4,
    };
    tc->tree.nodes.data[4] = (Node){
        .size = {
            (Size){.kind = SizeKindFixed, .size = 100},
            (Size){.kind = SizeKindFixed, .size = 50},
        },
        .first_children = -1,
        .last_children = -1,
        .children_count = 0,
        .next = -1,
    };
}


void test_ui_compute(){
    void(*cases[])(TestCase* tc) = {
        test_no_children_fixed_position,
        test_one_children_fixed_position,
        test_root_fitting_to_one_children,
        test_root_fitting_to_one_children_in_vertical,
        test_nested_children_fixed_positions,
        test_nested_children_fixed_positions_and_padding,
        test_horizontal_layout_with_margin,
        test_vertical_layout_with_spacing,
        test_horizontal_layout_with_margin_and_root_fitting,
        test_grow_children_between_two_fixed_size_in_horizontal,
        test_grow_children_between_two_fixed_size_in_vertical,
        test_two_grow_children_between_two_fixed_size_in_horizontal_with_starting_size,
        test_two_grow_children_between_two_fixed_size_in_vertical_with_starting_size,
    };

    int test_count = sizeof(cases) / sizeof(cases[0]);
    for (int i = 0; i < test_count; i++) {
        mt_total++;
        test_ui_compute_case(cases[i]);
    }
}
